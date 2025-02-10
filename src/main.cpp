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

/* Timers */
#define SCALE_READ_MS 200
#define BUTTONS_READ_MS 20
#define POT_READ_MS 100
#define LCD_UPDATE_MS 100
#define SCALE_EMPTY_ERROR_MS 3000
#define PUMP_EMPTY_ERROR_MS 3000
#define POURING_TIMEOUT_MS 10000
#define READ_SCALE_MS 120

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

CocktailMaker cocktailMaker;
LCDController* lcd;
ScaleController* scale; 

/* Flags */
bool lcdUpdateFlag = true;

/* Global Variables */
unsigned int potValue = 0;
int scaleValue = 0;
unsigned int previousScaleValue = 0;
bool lockSelectModeButton = false;
unsigned int activePumps = 0;

/* Function prototypes */
buttons readButtons();
void setPump(int data);
void resetPumps();

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
}

void loop() {
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
        lcd->printSecondLine("Manual          ");
        break;
      case SETUP:
        lcd->printFirstLine("Cocktail Maker  ");
        lcd->printSecondLine("Setup           ");
        break;
      case SETUP_COCKTAIL:
        lcd->printFirstLine("Cocktail Maker  ");
        lcd->printSecondLine("Setup Cocktail  ");
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
        lcd->printSecondLine("                ");
        break;
      case CLEAN:
        lcd->printFirstLine("Cocktail Maker  ");
        lcd->printSecondLine("Cleaning       ");
        break;
    }
    lcdUpdateFlag = false;
  }
    
}

void OS_10mstask() {
  /* Global state machine */
  switch(currentGlobalState)
  {
    case INIT:
      resetPumps();
      lcdUpdateFlag = true;
      currentGlobalState = AUTO;
      break;
      
    case AUTO:
      readScaleCounter++;
      if(readScaleCounter % (READ_SCALE_MS * 10/ OS_TASK_PERIOD_MS) == 0)
      {
        scaleValue = scale->read();
        if(scaleValue < 0){
          scale->tare();
        }
        readScaleCounter = 0;
      }

      potReadingCounter++;
      if (potReadingCounter >= POT_READ_MS / OS_TASK_PERIOD_MS) {
        /* What to do in loop */
        potValue = map(analogRead(POT_SELECT_PIN), 0, 1023, 4, 39) * 25;

        potReadingCounter = 0;
      }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
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
              currentGlobalState = POURING;
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
      if (potReadingCounter >= POT_READ_MS / OS_TASK_PERIOD_MS) {
        /* What to do in loop */
        potValue = map(analogRead(POT_SELECT_PIN), 0, 1023, 0, NO_OF_STATES - 1);

        potReadingCounter = 0;
      }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_OK:
            currentGlobalState = static_cast<globalState>(potValue);
            break;
          case BUTTON_SELECT_MODE:
            if(lockSelectModeButton == false){
              currentGlobalState = previousGlobalState;
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
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
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
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
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
      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
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
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
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
      // if (pouringTimeoutCounter >= POURING_TIMEOUT_MS / READ_SCALE_MS) {
      //     pouringTimeoutCounter = 0;
      //     currentGlobalState = PUMP_IS_EMPTY;
      // }

      if(readScaleCounter % (READ_SCALE_MS / OS_TASK_PERIOD_MS) == 0)
      {
        scaleValue = scale->read();
        if(previousScaleValue == scaleValue)
        {
          pouringTimeoutCounter++;
        }
        previousScaleValue = scaleValue;

        Serial.println(scaleValue);
        Serial.println(potValue);

        if(scaleValue < int(potValue)){
          if((activePumps & (PUMP_1)) == 0)
          {
            setPump(PUMP_1);
            Serial.println("Pump 1 ON");
          }
        }
        else
        {
          resetPumps();
          currentGlobalState = AUTO;
        }
      }
      
      break;
    case SCALE_IS_EMPTY:
      scaleEmptyErrorCounter++;
      if (scaleEmptyErrorCounter >= SCALE_EMPTY_ERROR_MS / OS_TASK_PERIOD_MS) {
        scaleEmptyErrorCounter = 0;
        currentGlobalState = AUTO;
      }
      break;
    case PUMP_IS_EMPTY:
      pumpEmptyErrorCounter++;
      if (pumpEmptyErrorCounter >= PUMP_EMPTY_ERROR_MS / OS_TASK_PERIOD_MS) {
        pumpEmptyErrorCounter = 0;
        currentGlobalState = AUTO;
      }
      break;
    case PAUSED:
      break;
    case NO_OF_STATES:
      break;
    default:
      break;
  }

  /* Update LCD */
  lcdUpdateCounter++;
  if (lcdUpdateCounter >= LCD_UPDATE_MS / OS_TASK_PERIOD_MS) {
    lcdUpdateFlag = true;
    lcdUpdateCounter = 0;
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

void resetPumps() {
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, LOW);
  shiftOut(SHIFT_REGISTER_DATA_PIN, SHIFT_REGISTER_CLOCK_PIN, MSBFIRST, 0x00);
  activePumps = 0;
  digitalWrite(SHIFT_REGISTER_LATCH_PIN, HIGH);
}

