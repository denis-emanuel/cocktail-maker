#ifndef RECIPES_H
#define RECIPES_H

#include "../include/constants.h"
#include "../include/typedefs.h"

class Ingredient {
    IngredientName name;
    int amount_ml;
    short int pumpIndex; // -1 if not assigned/used

public:
    Ingredient() {
        this->name = IngredientName::NO_NAME;
        this->amount_ml = 0;
        this->pumpIndex = -1;
    }

    Ingredient(IngredientName name, int amount_ml) {
        this->name = name;
        this->amount_ml = amount_ml;
        this->pumpIndex = -1;
    }

    IngredientName getName() {
        return this->name;
    }

    int getAmount() {
        return this->amount_ml;
    }

    short int getPumpIndex() {
        return this->pumpIndex;
    }

    void setPumpIndex(short int pumpIndex) {
        this->pumpIndex = pumpIndex;
    }   
};

class Recipe {
    private:
        String name;
        short int noOfIngredients;
        Ingredient* ingredients;

    public:
        Recipe(const char name [], short int noOfIngredients, Ingredient ingredients[6]) {
            this->name = name;
            this->noOfIngredients = noOfIngredients;
            this->ingredients = new Ingredient[noOfIngredients];
            for (int i = 0; i < noOfIngredients; i++) {
                this->ingredients[i] = ingredients[i];
            }
        }

        ~Recipe() {
            delete[] this->ingredients;
        }

        IngredientName getIngredientNameByIdx(int index) {
            return this->ingredients[index].getName();
        }

        void setPumpIndex(int ingredientIdx, short int pumpIndex) {
            this->ingredients[ingredientIdx].setPumpIndex(pumpIndex);
        }

        Ingredient* getIngredients() {
            return this->ingredients;
        }

        Ingredient getIngredientByIdx(int index) {
            return this->ingredients[index];
        }

        void resetPumpIndexes() {
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->ingredients[i].setPumpIndex(-1);
            }
        }

        short int getNoOfIngredients() {
            return this->noOfIngredients;
        }
};

class CocktailMaker {
    private:
        Recipe* activeRecipe;
        IngredientName pumpAssignment[NUM_OF_PUMPS];

        void assignPumpsForRecipe(Recipe& recipe) {
            IngredientName newPumpAssignment[NUM_OF_PUMPS];
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                newPumpAssignment[i] = IngredientName::NO_NAME;
            }

            // reuse existing pump assignments
            // loop through ingredients:
            for (int i = 0; i < recipe.getNoOfIngredients(); i++) {
                bool isAlreadyAssigned = false;

                if (recipe.getIngredientNameByIdx(i) != IngredientName::NO_NAME) {
                    // check if ingredient is already assigned to a pump
                    // loop through pump assignments
                    for (int j = 0; j < NUM_OF_PUMPS; j++) {
                        if (this->pumpAssignment[j] == recipe.getIngredientNameByIdx(i)) {
                            newPumpAssignment[i] = recipe.getIngredientNameByIdx(i);
                            isAlreadyAssigned = true;
                        }
                    }

                    if (!isAlreadyAssigned) {
                        // loop through pump assignments
                        for (int j = 0; j < NUM_OF_PUMPS; j++) {
                            if (this->pumpAssignment[j] == IngredientName::NO_NAME) {
                                newPumpAssignment[j] = recipe.getIngredientNameByIdx(i);
                                break;
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->pumpAssignment[i] = newPumpAssignment[i];
            }
        }

        void setPumpIndexesForRecipe(Recipe& recipe) {
            // loop through ingredients
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                if (recipe.getIngredientNameByIdx(i) != IngredientName::NO_NAME) {
                    // loop through pump assignments
                    for (int j = 0; j < NUM_OF_PUMPS; j++) {
                        if (this->pumpAssignment[j] == recipe.getIngredientNameByIdx(i)) {
                            recipe.setPumpIndex(i, j);
                            break;
                        }
                    }
                }
            }
        }

    public:
        CocktailMaker() {
            for (int i = 0; i < NUM_OF_PUMPS; i++) {
                this->pumpAssignment[i] = IngredientName::NO_NAME;
            }
        }

        void setRecipe(Recipe& recipe) {
            this->activeRecipe = &recipe;
            this->activeRecipe->resetPumpIndexes();
            assignPumpsForRecipe(recipe);
        }

        IngredientName* getPumpAssignment() {
            return this->pumpAssignment;
        }

        void mixCocktail() {
            setPumpIndexesForRecipe(*this->activeRecipe);

            Ingredient* ingredients = this->activeRecipe->getIngredients();

            for (int i = 0; i < this->activeRecipe->getNoOfIngredients(); i++) {
                if (this->activeRecipe->getIngredientByIdx(i).getPumpIndex() != -1) {
                    // start pump and fill ingredient.amount_ml
                    // wait for pump to finish
                    // stop pump
                }
            }
        }
};


#endif