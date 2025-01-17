#include <Arduino.h>
#include <TimerOne.h>
#include "../include/recipes.h"
#include "../include/functions.h"

/* Global Variables */
/* Keep track of global state */
globalState currentGlobalState = INIT;

Ingredient ginTonicIngredients[2] = {
  Ingredient(IngredientName::GIN, 50),
  Ingredient(IngredientName::TONIC, 150)
};

Recipe recipes[1] = {
  Recipe("Gin Tonic", 2, ginTonicIngredients)
};

CocktailMaker cocktailMaker;

void setup() {
  /* Initialize timer1 to count 10ms and use callback to function */
  Timer1.initialize(OS_TASK_PERIOD_MS * 1000);
  Timer1.attachInterrupt(OS_10mstask);

  Serial.begin(9600);
}

void loop() {
}

void OS_10mstask() {
  /* Global state machine */
  switch(currentGlobalState)
  {
    case INIT:
      /* Enter by default into AUTOMATIC state */
      currentGlobalState = AUTO;

      cocktailMaker.setRecipe(recipes[0]);
      cocktailMaker.mixCocktail();
      break;
      
    case AUTO:
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