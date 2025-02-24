#include <Arduino.h>
#include <TimerOne.h>
#include "HX711.h"

#include "../include/recipes.h"
#include "../include/functions.h"
#include "../include/lcd.h"
#include "../include/scale.h"

/* Weight sensor pins */
#define BUTTONS_PIN A0
#define POT_SELECT_PIN A1
#define POT_VALUE_AUTO_MAX 999
#define POT_VALUE_AUTO_MIN 100

/* Shift register pins */
#define SHIFT_REGISTER_DATA_PIN 8
#define SHIFT_REGISTER_LATCH_PIN 9
#define SHIFT_REGISTER_CLOCK_PIN 10

/* Pump definitions */
#define PUMP_1 (1<<0)
#define PUMP_2 (1<<1)
#define PUMP_3 (1<<2)
#define PUMP_4 (1<<3)
#define PUMP_5 (1<<4)
#define PUMP_6 (1<<5)

/* Timers */ // Divide by OStaskMS (12 = 120ms)
#define SCALE_READ_MS 20
#define BUTTONS_READ_MS 2
#define POT_READ_MS 10
#define LCD_UPDATE_MS 10
#define SCALE_EMPTY_ERROR_MS 300
#define PUMP_EMPTY_ERROR_MS 1000
#define POURING_TIMEOUT_COUNTS 7 //Seconds Aprox
#define READ_SCALE_MS 12 

/* Millis counters */
#define MAIN_LOOP_MS 250
unsigned long lastLoopMillis = 0;

/* Configuration */
unsigned int buttonsReadingCounter = 0;
const unsigned int BUTTONS_THRESHOLDS[NO_BUTTON] = {100, 550, 700};
unsigned int potReadingCounter = 0;
const unsigned int POT_THRESHOLDS[NO_BUTTON] = {200, 400, 600};
unsigned int lcdUpdateCounter = 0;
unsigned int scaleEmptyErrorCounter = 0;
unsigned int pumpEmptyErrorCounter = 0;
unsigned int pouringTimeoutCounter = 0;
unsigned int readScaleCounter = 0;

/* Menu arrays */

/* Keep track of global state */
globalState currentGlobalState = INIT;
globalState previousGlobalState = INIT;

CocktailMaker* cocktailMaker;
LCDController* lcd;
ScaleController* scale; 

/* Flags */
bool lcdUpdateFlag = true;

#ifdef DEBUG
bool debug_print_flag = false;
#endif

/* Global Variables */
unsigned int potValue = 0;
unsigned int previousPotValue = 0;
int scaleValue = 0;
unsigned int previousScaleValue = 0;
bool lockSelectModeButton = false;
unsigned int activePumps = 0;

/* AutoPouring state variables */
unsigned int currentPouringIngredientIdx = 0;
int currentPouringQtyTgt = 0;

/* Function prototypes */
buttons readButtons();
void setPump(int data);
void stopPumps();

void setup() {
  pinMode(BUTTONS_PIN, INPUT);

  pinMode(12, OUTPUT); 
  digitalWrite(12, LOW);

  pinMode(SHIFT_REGISTER_DATA_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_LATCH_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_CLOCK_PIN, OUTPUT);

  /* Initialize timer1 to count 10ms and use callback to function */
  Timer1.initialize(OS_TASK_PERIOD_MS * 1000);
  Timer1.attachInterrupt(OS_10mstask);

  Serial.begin(9600);

  lcd = new LCDController();
  lcd->begin();

  scale = new ScaleController();
  scale->begin();

  cocktailMaker = new CocktailMaker(*scale, *lcd);
  #ifdef DEBUG
  cocktailMaker->setRecipe(RecipeName::HUGO);
  #endif

  #ifndef DEBUG
  cocktailMaker->setRecipe(RecipeName::HUGO);
  #endif
}

void loop() {
  /* Prevent display from updating earlier than MAIN_LOOP_MS */
  if(millis() - lastLoopMillis > MAIN_LOOP_MS)
  {
    if(lcdUpdateFlag == true) {
      switch (currentGlobalState)
      {
        case INIT:
          lcd->printFirstLine("Cocktail Maker");
          lcd->printSecondLine("Initializing");
          break;
        case AUTO:
          lcd->printFirstLine("AUTO Mode       ");
          lcd->printSecondLine("ml: ");
          lcd->lcdClearAt(7, 1);
          lcd->printAtCursor(4, 1, potValue);
          break;
        case SELECT_MODE:
          lcd->printFirstLine("Select Mode :   ");
          lcd->printSecondLine(functionModes[potValue]);
          break;
        case MANUAL:
          lcd->printFirstLine("Cocktail Maker  ");
          lcd->printSecondLine("Manual         ");
          break;
        case SETUP:
          lcd->printFirstLine("Cocktail Maker  ");
          lcd->printSecondLine("Setup           ");
          break;
        case SETUP_COCKTAIL:
          lcd->printFirstLine("Let's make...   ");
          lcd->printSecondLine(cocktailNames[potValue]);
          break;
        case POURING:
          lcd->printFirstLine("Pouring         ");
          if(scaleValue < 10)
          {
            lcd->printAtCursor(0, 1, scaleValue);
            lcd->printAtCursor(1, 1, "  ");
          }
          else if(scaleValue < 100)
          {
            lcd->printAtCursor(0, 1, scaleValue);
            lcd->printAtCursor(2, 1, " ");
          }
          else{
            lcd->printAtCursor(0, 1, scaleValue);
          }
          
          lcd->printAtCursor(3, 1, "/");
          lcd->printAtCursor(4, 1, potValue);
          break;
        case SCALE_IS_EMPTY:  
          lcd->printFirstLine("Scale is empty! ");
          lcd->printSecondLine("                ");
          break;
        case PUMP_IS_EMPTY:  
          lcd->printFirstLine("Pump is empty!  ");
          lcd->printSecondLine("Refill pump     ");
          lcd->printAtCursor(13, 1, int(cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getPumpIndex()));
          break;
        case PAUSED:
          break;
        case CLEAN:
          lcd->printFirstLine("Cocktail Maker  ");
          lcd->printSecondLine("Cleaning        ");
          break;
        case NO_OF_STATES:
          /* For deleting warning */
        break;
      }
      lcdUpdateFlag = false;
    }

    lastLoopMillis = millis();
  }

#ifdef DEBUG
  if(debug_print_flag == true) {
    debug_print_flag = false;
  }
#endif
    
}

void OS_10mstask() {
  /* Global state machine */
  switch(currentGlobalState)
  {
    case INIT:
      stopPumps();
      currentGlobalState = AUTO;
      lcdUpdateFlag = true;
      break;
      
    case AUTO:
      readScaleCounter++;
      if(readScaleCounter % (READ_SCALE_MS * 10) == 0)
      {
        scaleValue = scale->read();
        if(scaleValue < 0){
          scale->tare();
        }
        readScaleCounter = 0;
      }

      potReadingCounter++;
      if (potReadingCounter >= POT_READ_MS) {
        potValue = map(analogRead(POT_SELECT_PIN), 0, 1023, 4, 39) * 25;

        /* If potentiometer has a new value, update on display */
        if(previousPotValue != potValue)
        {
          lcdUpdateFlag = true;
          previousPotValue = potValue;
        }
        potReadingCounter = 0;
      }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
            scaleValue = scale->read();
            Serial.println(scaleValue);
            if(scaleValue < 10){
              currentGlobalState = SCALE_IS_EMPTY;
            }
            else{
              scale->tare();
              scaleValue = 0;

              currentPouringIngredientIdx = 0;
              currentPouringQtyTgt = cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getAmount() * potValue / 100;
              currentGlobalState = POURING;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
            }
            break;

          case BUTTON_POUR:
            scale->tare();
            scaleValue = 0;
            break;

          case BUTTON_SELECT_MODE:
            previousGlobalState = currentGlobalState;
            if (lockSelectModeButton == false) {
              currentGlobalState = SELECT_MODE;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;

          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }
        
        buttonsReadingCounter = 0;
      }
    
      break;
    case SELECT_MODE:
      /* Select what mode you want with the potentiometer */
      potReadingCounter++;
      if (potReadingCounter >= POT_READ_MS) {
        /* What to do in loop */
        potValue = map(analogRead(POT_SELECT_PIN), 0, 1023, 0, NO_OF_STATES - 1);
        
        /* If potentiometer has a new value, update on display */
        if(previousPotValue != potValue)
        {
          lcdUpdateFlag = true;
          previousPotValue = potValue;
        }

        potReadingCounter = 0;
      }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
            currentGlobalState = static_cast<globalState>(potValue);
            /* Update new state on LCD*/
            lcdUpdateFlag = true;
            break;
          case BUTTON_SELECT_MODE:
            if(lockSelectModeButton == false){
              currentGlobalState = previousGlobalState;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;
          case BUTTON_POUR:
            break;
          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }

        buttonsReadingCounter = 0;
      }
      break;
    case MANUAL:
      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
            break;
          case BUTTON_POUR:
            break;
          case BUTTON_SELECT_MODE:
            previousGlobalState = currentGlobalState;
            if (lockSelectModeButton == false) {
              currentGlobalState = SELECT_MODE;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;
          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }

        buttonsReadingCounter = 0;
      }
      break;
    case SETUP:
      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
          
            break;
          case BUTTON_POUR:
          
            break;
          case BUTTON_SELECT_MODE:
            previousGlobalState = currentGlobalState;
            if (lockSelectModeButton == false) {
              currentGlobalState = SELECT_MODE;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;
          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }

        buttonsReadingCounter = 0;
      }
      break;
    case SETUP_COCKTAIL:

      /* Select what cocktail you want with the potentiometer */
      potReadingCounter++;
      if (potReadingCounter >= POT_READ_MS) {
        /* What to do in loop */
        potValue = map(analogRead(POT_SELECT_PIN), 0, 1023, 0, NO_OF_RECIPES - 1);
        
        /* If potentiometer has a new value, update on display */
        if(previousPotValue != potValue)
        {
          lcdUpdateFlag = true;
          previousPotValue = potValue;
        }

        potReadingCounter = 0;
      }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
            cocktailMaker->setRecipe((RecipeName)potValue);
            
            currentGlobalState = AUTO;
            /* Update new state on LCD*/
            lcdUpdateFlag = true;
          
            break;
          case BUTTON_POUR:
          
            break;
          case BUTTON_SELECT_MODE:
            previousGlobalState = currentGlobalState;
            if (lockSelectModeButton == false) {
              currentGlobalState = SELECT_MODE;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;
          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }

        buttonsReadingCounter = 0;
      }
      break;
    case CLEAN:
      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
          
            break;
          case BUTTON_POUR:
          
            break;
          case BUTTON_SELECT_MODE:
            previousGlobalState = currentGlobalState;
            if (lockSelectModeButton == false) {
              currentGlobalState = SELECT_MODE;
              /* Update new state on LCD*/
              lcdUpdateFlag = true;
              lockSelectModeButton = true;
            }
            break;
          case NO_BUTTON:
            lockSelectModeButton = false;
            break;
        }

        buttonsReadingCounter = 0;
      }
      break;
    case POURING:
      readScaleCounter++;
      if (pouringTimeoutCounter >= POURING_TIMEOUT_COUNTS) {
          Serial.println(millis());
          pouringTimeoutCounter = 0;
          currentGlobalState = PUMP_IS_EMPTY;
          /* Update new state on LCD*/
          lcdUpdateFlag = true;
      }

      if(readScaleCounter % (READ_SCALE_MS ) == 0)
      {
        scaleValue = scale->read();
        if(int(previousScaleValue) == scaleValue)
        {
          pouringTimeoutCounter++;
        }
        else
        {
          pouringTimeoutCounter = 0;
        }
        previousScaleValue = scaleValue;

        #ifdef DEBUG
        debug_print_flag = true;
        currentGlobalState = PAUSED;
        #endif

        #ifndef DEBUG

        if(scaleValue < int(potValue)){
          if(scaleValue <= currentPouringQtyTgt)
          {
            if((activePumps & (1 << cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getPumpIndex())) == 0)
            {
              Serial.println(cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getPumpIndex());
              setPump(1 << cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getPumpIndex());
            }
          }
          else
          {
            stopPumps();
            currentPouringIngredientIdx++;
            currentPouringQtyTgt = currentPouringQtyTgt + (cocktailMaker->getActiveRecipe()->getIngredientByIdx(currentPouringIngredientIdx)->getAmount() * potValue / 100);
          }
        }
        else
        {
          stopPumps();
          currentPouringIngredientIdx = 0;
          currentPouringQtyTgt = 0;
          currentGlobalState = AUTO;
          /* Update new state on LCD*/
          lcdUpdateFlag = true;
        }
        #endif
      }
      
      break;
    case SCALE_IS_EMPTY:
      scaleEmptyErrorCounter++;
      if (scaleEmptyErrorCounter >= SCALE_EMPTY_ERROR_MS) {
        scaleEmptyErrorCounter = 0;
        currentGlobalState = AUTO;
        /* Update new state on LCD*/
        lcdUpdateFlag = true;
      }
      break;
    case PUMP_IS_EMPTY:
      pumpEmptyErrorCounter++;
      if (pumpEmptyErrorCounter >= PUMP_EMPTY_ERROR_MS) {
        pumpEmptyErrorCounter = 0;
        currentGlobalState = AUTO;
        /* Update new state on LCD*/
        lcdUpdateFlag = true;
      }
      break;
    case PAUSED:
      break;
    case NO_OF_STATES:
      break;
    default:
      break;
  }
  
}

buttons readButtons() {
  unsigned int buttonValue = analogRead(BUTTONS_PIN);

  if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_OK]) {
    return BUTTON_OK;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_POUR]) {
    return BUTTON_POUR;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_SELECT_MODE]) {
    return BUTTON_SELECT_MODE;
  } else {
    return NO_BUTTON;
  }
}

void setPump(int data) {
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, LOW);
  shiftOut(SHIFT_REGISTER_DATA_PIN, SHIFT_REGISTER_CLOCK_PIN, MSBFIRST, data);
  activePumps = data;
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, HIGH);
}

void stopPumps() {
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, LOW);
  shiftOut(SHIFT_REGISTER_DATA_PIN, SHIFT_REGISTER_CLOCK_PIN, MSBFIRST, 0x00);
  activePumps = 0;
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, HIGH);
}

