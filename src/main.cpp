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

/* Timers */
#define SCALE_READ_MS 200
#define BUTTONS_READ_MS 20
#define POT_READ_MS 100

/* Configuration */
unsigned int buttonsReadingCounter = 0;
const unsigned int BUTTONS_THRESHOLDS[NO_BUTTON] = {100, 550, 700, 790, 900};
unsigned int potReadingCounter = 0;
const unsigned int POT_THRESHOLDS[NO_BUTTON] = {200, 400, 600, 800, 1025};

/* Keep track of global state */
globalState currentGlobalState = INIT;

CocktailMaker cocktailMaker;
LCDController* lcd;
ScaleController* scale; 

/* Function prototypes */
buttons readButtons();

void setup() {
  pinMode(BUTTONS_PIN, INPUT);

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
}

void OS_10mstask() {
  /* Global state machine */
  switch(currentGlobalState)
  {
    case INIT:
      Serial.println("Entering AUTO state");
      /* Enter by default into AUTOMATIC state */
      currentGlobalState = AUTO;

      // cocktailMaker.setRecipe(recipes[0]);
      // cocktailMaker.mixCocktail();
      break;
      
    case AUTO:
      // scale.incrementReadingCounter();
      // Serial.println("reading: " + scale.getReadingCounter());
      // if (scale.getReadingCounter() >= SCALE_READ_MS / OS_TASK_PERIOD_MS) {
      //   /* What to do in loop */
      //   Serial.println(scale.read());

      //   scale.resetReadingCounter();
      // }

      buttonsReadingCounter++;
      if (buttonsReadingCounter >= BUTTONS_READ_MS / OS_TASK_PERIOD_MS) {
        /* What to do in loop */
        buttons button = readButtons();
        
        switch(button) {
          case BUTTON_0:
            Serial.println("Button 0 pressed");
            break;
          case BUTTON_1:
            Serial.println("Button 1 pressed");
            break;
          case BUTTON_2:
            Serial.println("Button 2 pressed");
            break;
          case BUTTON_3:
            Serial.println("Button 3 pressed");
            break;
          case BUTTON_4:
            Serial.println("Button 4 pressed");
            break;
          case NO_BUTTON:
            break;
        }

        buttonsReadingCounter = 0;
      }

      // potReadingCounter++;
      // if (potReadingCounter >= POT_READ_MS / OS_TASK_PERIOD_MS) {
      //   /* What to do in loop */
      //   unsigned int potValue = analogRead(POT_SELECT_PIN);

      //   if (potValue < POT_THRESHOLDS[OPTION_0]) {
      //     Serial.println("Potentiometer 0");
      //   } else if (potValue < POT_THRESHOLDS[OPTION_1]) {
      //     Serial.println("Potentiometer 1");
      //   } else if (potValue < POT_THRESHOLDS[OPTION_2]) {
      //     Serial.println("Potentiometer 2");
      //   } else if (potValue < POT_THRESHOLDS[OPTION_3]) {
      //     Serial.println("Potentiometer 3");
      //   } else if (potValue < POT_THRESHOLDS[OPTION_4]) {
      //     Serial.println("Potentiometer 4");
      //   }

      //   potReadingCounter = 0;
      // }

      break;

    case MANUAL:
      break;
    case SETUP:
      break;
    case SETUP_COCKTAIL:
      break;
    case CLEAN:
      break;
    case PAUSED:
      break;
  }
}

buttons readButtons() {
  unsigned int buttonValue = analogRead(BUTTONS_PIN);

  if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_0]) {
    return BUTTON_0;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_1]) {
    return BUTTON_1;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_2]) {
    return BUTTON_2;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_3]) {
    return BUTTON_3;
  } else if (buttonValue < BUTTONS_THRESHOLDS[BUTTON_4]) {
    return BUTTON_4;
  } else {
    return NO_BUTTON;
  }
}