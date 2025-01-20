#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <Hashtable.h>

enum buttons {
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    NO_BUTTON
};

enum potentiometer_options {
    OPTION_0,
    OPTION_1,
    OPTION_2,
    OPTION_3,
    OPTION_4,
};

enum globalState {
    INIT,
    AUTO,
    MANUAL,
    SETUP,
    SETUP_COCKTAIL,
    CLEAN,
    PAUSED
};

enum class IngredientName {
    NO_NAME,
    GIN,
    RUM,
    TEQUILA,
    WHISKEY,
    VERMOUTH,
    LIME_JUICE,
    LEMON_JUICE,
    ORANGE_JUICE,
    CRANBERRY_JUICE,
    SODA,
    TONIC,
    GINGER_ALE,
    COKE
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