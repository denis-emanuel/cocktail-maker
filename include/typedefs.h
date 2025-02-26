#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <Hashtable.h>

enum buttons {
    BUTTON_OK,
    BUTTON_POUR,
    BUTTON_SELECT_MODE,
    NO_BUTTON
};

enum globalState {
    AUTO,
    MANUAL,
    SETUP_COCKTAIL,
    CLEAN,

    NO_OF_STATES,

    INIT,
    POURING,
    SCALE_IS_EMPTY,
    PUMP_IS_EMPTY,
    PAUSED,
    SELECT_MODE,
    PUMP_PREVIEW,

    NO_OF_GLOBAL_STATES
};

const char* functionModes[NO_OF_STATES] = {
    "AUTO            ", 
    "MANUAL          ", 
    "CHOOSE COCKTAIL ", 
    "CLEAN           "
};

enum RecipeName {
    GIN_TONIC,
    HUGO,
    NO_OF_RECIPES
};

const char* cocktailNames[NO_OF_RECIPES] = {
    "GIN TONIC       ", 
    "HUGO            "
};
enum IngredientName {
    GIN,
    RUM,
    TEQUILA,
    WHISKEY,
    VERMOUTH,
    PROSECCO,

    LIME_JUICE,
    LEMON_JUICE,
    ORANGE_JUICE,
    CRANBERRY_JUICE,

    SODA,
    SPRITE,
    TONIC,
    GINGER_ALE,
    COKE,

    ELDERFLOWER_SYRUP,
    SIMPLE_SYRUP,

    NO_INGREDIENT,
    NO_OF_INGREDIENTS,
};

const char* ingredientNames[NO_OF_INGREDIENTS] = {
    "GIN             ", 
    "RUM             ", 
    "TEQUILA         ", 
    "WHISKEY         ", 
    "VERMOUTH        ", 
    "PROSECCO        ",

    "LIME JUICE      ", 
    "LEMON JUICE     ", 
    "ORANGE JUICE    ", 
    "CRANBERRY JUICE ", 

    "SODA            ", 
    "SPRITE          ", 
    "TONIC           ", 
    "GINGER ALE      ", 
    "COKE            ", 

    "ELDERFLOWERSYRUP", 
    "SIMPLE SYRUP    ",
    "NO INGREDIENT   "
};

enum class Category {
    ALCOHOL,
    SYRUP,
    JUICE,
    SODA,
    OTHER
};

#endif