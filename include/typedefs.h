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
    SETUP,
    SETUP_COCKTAIL,
    CLEAN,
    NO_OF_STATES,
    INIT,
    POURING,
    SCALE_IS_EMPTY,
    PUMP_IS_EMPTY,
    PAUSED,
    SELECT_MODE
};

const char* functionModes[NO_OF_STATES] = {
    "AUTO            ", 
    "MANUAL          ", 
    "SETUP           ", 
    "SETUP_COCKTAIL  ", 
    "CLEAN           "
};

enum RecipeName {
    GIN_TONIC,
    HUGO,
    NO_OF_RECIPES
};

const char* cocktailNames[NO_OF_RECIPES] = {
    "GIN_TONIC       ", 
    "HUGO            "
};
enum IngredientName {
    NO_NAME,
    
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

    NO_OF_INGREDIENTS
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
    "SIMPLE SYRUP    "
};

enum class Category {
    ALCOHOL,
    SYRUP,
    JUICE,
    SODA,
    OTHER
};


// extern Hashtable<Category, IngredientName[]> ingredientCategoryMap;

// void initializeIngredientCategoryMap() {
//     ingredientCategoryMap.put(Category::ALCOHOL, {
//         IngredientName::GIN,
//         IngredientName::RUM,
//         IngredientName::TEQUILA,
//         IngredientName::WHISKEY,
//         IngredientName::VERMOUTH
//     });

//     ingredientCategoryMap.put(Category::SYRUP, {
//         IngredientName::LIME_JUICE,
//         IngredientName::LEMON_JUICE,
//         IngredientName::ORANGE_JUICE,
//         IngredientName::CRANBERRY_JUICE
//     });

//     ingredientCategoryMap.put(Category::SODA, {
//         IngredientName::SODA,
//         IngredientName::TONIC,
//         IngredientName::GINGER_ALE,
//         IngredientName::COKE
//     });
// };

#endif