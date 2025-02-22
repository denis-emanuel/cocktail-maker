#ifndef RECIPES_H
#define RECIPES_H

#include "../include/constants.h"
#include "../include/typedefs.h"
#include "../include/scale.h"
#include "../include/lcd.h"

class Ingredient {
    IngredientName name;
    int amount_percentage;
    short int pumpIndex; // -1 if not assigned/used

    public:
        Ingredient() {
            this->name = IngredientName::NO_NAME;
            this->amount_percentage = 0;
            this->pumpIndex = -1;
        }

        Ingredient(IngredientName name, int amount_ml) {
            this->name = name;
            this->amount_percentage = amount_ml;
            this->pumpIndex = -1;
        }

        IngredientName getName() {
            return this->name;
        }

        int getAmount() {
            return this->amount_percentage;
        }

        short int getPumpIndex() {
            return this->pumpIndex;
        }

        void setPumpIndex(short int pumpIndex) {
            this->pumpIndex = pumpIndex;
        }   
};

Ingredient* ginTonic[GIN_TONIC_INGREDIENTS] = {
    new Ingredient(IngredientName::GIN, 40),
    new Ingredient(IngredientName::TONIC, 50),
    new Ingredient(IngredientName::SIMPLE_SYRUP, 5),
    new Ingredient(IngredientName::SODA, 5)
};

Ingredient* hugo[HUGO_INGREDIENTS] = {
    new Ingredient(IngredientName::ELDERFLOWER_SYRUP, 10),
    new Ingredient(IngredientName::PROSECCO, 60),
    new Ingredient(IngredientName::SPRITE, 30)
};

Ingredient* hugoTest[HUGO_TEST_INGREDIENTS] = {
    new Ingredient(IngredientName::SIMPLE_SYRUP, 10),
    new Ingredient(IngredientName::SPRITE, 25),
    new Ingredient(IngredientName::SODA, 5),
    new Ingredient(IngredientName::PROSECCO, 60)
};

Ingredient* megaCocktail[MEGA_COCKTAIL_INGREDIENTS] = {
    new Ingredient(IngredientName::GIN, 10),
    new Ingredient(IngredientName::TONIC, 10),
    new Ingredient(IngredientName::SPRITE, 10),
    new Ingredient(IngredientName::PROSECCO, 10),
    new Ingredient(IngredientName::ELDERFLOWER_SYRUP, 10),
    new Ingredient(IngredientName::SODA, 50)
};

Ingredient* test[TEST_INGREDIENTS] = {
    new Ingredient(IngredientName::GIN, 10),
    new Ingredient(IngredientName::TONIC, 10),
    new Ingredient(IngredientName::SPRITE, 10),
    new Ingredient(IngredientName::PROSECCO, 10),
    new Ingredient(IngredientName::ELDERFLOWER_SYRUP, 10),
    new Ingredient(IngredientName::SODA, 50)
};

class Recipe {
    private:
        String name;
        short int noOfIngredients;
        Ingredient* ingredients[6];

    public:
        Recipe() {}

        Recipe(const char name [], short int noOfIngredients, Ingredient* ingredients[]) {
            this->name = name;
            this->noOfIngredients = noOfIngredients;
            for (int i = 0; i < noOfIngredients; i++) {
                this->ingredients[i] = ingredients[i];
            }
        }

        ~Recipe() {
            delete[] this->ingredients;
        }

        Ingredient* getIngredientByIdx(int index) {
            return this->ingredients[index];
        }

        short int getNumberOfIngredients() {
            return this->noOfIngredients;
        }
};

enum RecipeName {
    GIN_TONIC,
    HUGO,
    HUGO_TEST,
    MEGA_COCKTAIL,
    TEST,
    NO_OF_RECIPES
};

Recipe* recipes[NO_OF_RECIPES] = {
    new Recipe("Gin Tonic", GIN_TONIC_INGREDIENTS, ginTonic),
    new Recipe("Hugo", HUGO_INGREDIENTS, hugo),
    new Recipe("Hugo Test", HUGO_TEST_INGREDIENTS, hugoTest),
    new Recipe("Mega Cocktail", MEGA_COCKTAIL_INGREDIENTS, megaCocktail),
    new Recipe("Test", TEST_INGREDIENTS, test)
};

class CocktailMaker {
    private:
        Recipe* activeRecipe;
        IngredientName pumpAssignment[NUM_OF_PUMPS];
        ScaleController scale;
        LCDController lcd;

        // void setPumpIndexesForRecipe(Recipe& recipe) {
        //     // loop through ingredients
        //     for (int i = 0; i < NUM_OF_PUMPS; i++) {
        //         if (recipe.getIngredientByIdx(i)->getName() != IngredientName::NO_NAME) {
        //             // loop through pump assignments
        //             for (int j = 0; j < NUM_OF_PUMPS; j++) {
        //                 if (this->pumpAssignment[j] == recipe.getIngredientByIdx(i)->getName()) {
        //                     recipe.setPumpIndex(i, j);
        //                     break;
        //                 }
        //             }
        //         }
        //     }
        // }

    public:
        CocktailMaker() {}

        CocktailMaker(
            ScaleController scale,
            LCDController lcd
        ) {
            this->scale = scale;
            this->lcd = lcd;
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->pumpAssignment[i] = IngredientName::NO_NAME;
            }
        }

        void assignPumpsForRecipe() {
            IngredientName newPumpAssignment[NUM_OF_PUMPS];
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                newPumpAssignment[i] = IngredientName::NO_NAME;
            }

            bool isAlreadyAssignedToPump[this->activeRecipe->getNumberOfIngredients()] = {false};

            // reuse existing pump assignments
            // loop through ingredients:
            for (int ingredientIdx = 0; ingredientIdx < this->activeRecipe->getNumberOfIngredients(); ingredientIdx++) {
                if (this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName() != IngredientName::NO_NAME) {
                    // check if ingredient is already assigned to a pump
                    // loop through pump assignments
                    for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++) {
                        if (this->pumpAssignment[pumpIdx] == this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName()) {
                            newPumpAssignment[pumpIdx] = this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName();
                            this->activeRecipe->getIngredientByIdx(ingredientIdx)->setPumpIndex(pumpIdx);
                            isAlreadyAssignedToPump[ingredientIdx] = true;
                        }
                    }
                }
            }

            for (int ingredientIdx = 0; ingredientIdx < this->activeRecipe->getNumberOfIngredients(); ingredientIdx++) {
                if (!isAlreadyAssignedToPump[ingredientIdx] && this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName() != IngredientName::NO_NAME) {
                    // loop through pump assignments
                    for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++) {
                        if (newPumpAssignment[pumpIdx] == IngredientName::NO_NAME) {
                            newPumpAssignment[pumpIdx] = this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName();
                            this->activeRecipe->getIngredientByIdx(ingredientIdx)->setPumpIndex(pumpIdx);
                            break;
                        }
                    }
                }
            }

            for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++) {
                this->pumpAssignment[pumpIdx] = newPumpAssignment[pumpIdx];
            }
        }

        Recipe* getActiveRecipe() {
            return this->activeRecipe;
        }

        void setRecipe(RecipeName recipeName) {
            if(this->activeRecipe != nullptr) {
                delete this->activeRecipe;
            }
            this->activeRecipe = recipes[recipeName];

            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->activeRecipe->getIngredientByIdx(i)->setPumpIndex(-1);
            }
        }

        IngredientName* getPumpAssignment() {
            return this->pumpAssignment;
        }

        void setPumpAssignment(IngredientName pumpAssignment[NUM_OF_PUMPS]) {
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->pumpAssignment[i] = pumpAssignment[i];
            }
        }
};


#endif