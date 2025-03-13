#ifndef RECIPES_H
#define RECIPES_H

#include "../include/constants.h"
#include "../include/typedefs.h"
#include "../include/scale.h"
#include "../include/lcd.h"

class Ingredient
{
    IngredientName name;
    int amount_percentage;
    short int pumpIndex; // -1 if not assigned/used

public:
    Ingredient()
    {
        this->name = IngredientName::NO_INGREDIENT;
        this->amount_percentage = 0;
        this->pumpIndex = -1;
    }

    Ingredient(IngredientName name, int amount_ml)
    {
        this->name = name;
        this->amount_percentage = amount_ml;
        this->pumpIndex = -1;
    }

    IngredientName getName()
    {
        return this->name;
    }

    int getAmount()
    {
        return this->amount_percentage;
    }

    int getPumpIndex()
    {
        return this->pumpIndex;
    }

    void setPumpIndex(short int pumpIndex)
    {
        this->pumpIndex = pumpIndex;
    }
};

Ingredient **ginTonic = new Ingredient *[GIN_TONIC_INGREDIENTS]
{
    new Ingredient(IngredientName::GIN, 40),
        new Ingredient(IngredientName::TONIC, 50),
        new Ingredient(IngredientName::LEMON_JUICE, 10),
};

Ingredient **hugo = new Ingredient *[HUGO_INGREDIENTS]
{
    new Ingredient(IngredientName::ELDERFLOWER_SYRUP, 10),
        new Ingredient(IngredientName::PROSECCO, 60),
        new Ingredient(IngredientName::SPRITE, 30),
};

Ingredient **aperolSpritz = new Ingredient *[APEROL_SPRITZ_INGREDIENTS]
{
    new Ingredient(IngredientName::APEROL, 30),
        new Ingredient(IngredientName::PROSECCO, 60),
        new Ingredient(IngredientName::SODA, 10),
};

Ingredient **mojito = new Ingredient *[MOJITO_INGREDIENTS]
{
    new Ingredient(IngredientName::WHITE_RUM, 40),
        new Ingredient(IngredientName::SODA, 20),
        new Ingredient(IngredientName::LIME_JUICE, 20),
};

Ingredient *emptyIngredient = new Ingredient(IngredientName::NO_INGREDIENT, 0);

class Recipe
{
private:
    String name;
    short int noOfIngredients;
    Ingredient *ingredients[NUM_OF_PUMPS];

public:
    Recipe() {}

    Recipe(const char name[], short int noOfIngredients, Ingredient *ingredients[])
    {
        this->name = name;
        this->noOfIngredients = noOfIngredients;
        for (int i = 0; i < noOfIngredients; i++)
        {
            this->ingredients[i] = ingredients[i];
        }
    }

    ~Recipe()
    {
    }

    Ingredient *getIngredientByIdx(int index)
    {
        if (index >= this->noOfIngredients)
        {
            return emptyIngredient;
        }
        return this->ingredients[index];
    }

    short int getNumberOfIngredients()
    {
        return this->noOfIngredients;
    }
};

Recipe *recipes[NO_OF_RECIPES] = {
    new Recipe("Gin Tonic", GIN_TONIC_INGREDIENTS, ginTonic),
    new Recipe("Hugo", HUGO_INGREDIENTS, hugo),
    new Recipe("Aperol Spritz", APEROL_SPRITZ_INGREDIENTS, aperolSpritz),
    new Recipe("Mojito", MOJITO_INGREDIENTS, mojito),
};

class CocktailMaker
{
private:
    Recipe *activeRecipe;
    IngredientName pumpAssignment[NUM_OF_PUMPS];
    ScaleController scale;
    LCDController lcd;

public:
    CocktailMaker() {}

    CocktailMaker(
        ScaleController scale,
        LCDController lcd)
    {
        this->scale = scale;
        this->lcd = lcd;
        for (int i = 0; i < NUM_OF_PUMPS; i++)
        {
            this->pumpAssignment[i] = IngredientName::NO_INGREDIENT;
        }
    }

    ~CocktailMaker()
    {
        // Only delete activeRecipe if we dynamically allocated it
        if (this->activeRecipe != nullptr)
        {
            delete this->activeRecipe;
        }
    }

    void assignPumpsForRecipe()
    {
        IngredientName newPumpAssignment[NUM_OF_PUMPS];
        for (int i = 0; i < NUM_OF_PUMPS; i++)
        {
            newPumpAssignment[i] = this->pumpAssignment[i];
        }

        bool isAlreadyAssignedToPump[this->activeRecipe->getNumberOfIngredients()] = {false};

        // reuse existing pump assignments
        for (int ingredientIdx = 0; ingredientIdx < this->activeRecipe->getNumberOfIngredients(); ingredientIdx++)
        {
            if (this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName() != IngredientName::NO_INGREDIENT)
            {
                // check if ingredient is already assigned to a pump
                // loop through pump assignments
                for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++)
                {
                    if (newPumpAssignment[pumpIdx] == this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName())
                    {
                        this->activeRecipe->getIngredientByIdx(ingredientIdx)->setPumpIndex(pumpIdx);
                        isAlreadyAssignedToPump[ingredientIdx] = true;
                    }
                }
            }
        }

        for (int ingredientIdx = 0; ingredientIdx < this->activeRecipe->getNumberOfIngredients(); ingredientIdx++)
        {
            if ((isAlreadyAssignedToPump[ingredientIdx] == false) && (this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName() != IngredientName::NO_INGREDIENT))
            {
                // loop through pump assignments
                for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++)
                {
                    if (newPumpAssignment[pumpIdx] == IngredientName::NO_INGREDIENT)
                    {
                        newPumpAssignment[pumpIdx] = this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName();
                        this->activeRecipe->getIngredientByIdx(ingredientIdx)->setPumpIndex(pumpIdx);
                        isAlreadyAssignedToPump[ingredientIdx] = true;
                        break;
                    }
                }
            }
        }

        for (int ingredientIdx = 0; ingredientIdx < this->activeRecipe->getNumberOfIngredients(); ingredientIdx++)
        {
            if (!isAlreadyAssignedToPump[ingredientIdx] && this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName() != IngredientName::NO_INGREDIENT)
            {
                for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++)
                {
                    newPumpAssignment[pumpIdx] = this->activeRecipe->getIngredientByIdx(ingredientIdx)->getName();
                    this->activeRecipe->getIngredientByIdx(ingredientIdx)->setPumpIndex(pumpIdx);
                    break;
                }
            }
        }

        for (int pumpIdx = 0; pumpIdx < NUM_OF_PUMPS; pumpIdx++)
        {
            this->pumpAssignment[pumpIdx] = newPumpAssignment[pumpIdx];
        }
    }

    Recipe *getActiveRecipe()
    {
        return this->activeRecipe;
    }

    void setRecipe(RecipeName recipeName)
    {
        if (this->activeRecipe != nullptr)
        {
            delete this->activeRecipe;
        }
        this->activeRecipe = recipes[recipeName];

        for (int i = 0; i < NUM_OF_PUMPS; i++)
        {
            this->activeRecipe->getIngredientByIdx(i)->setPumpIndex(-1);
        }

        this->assignPumpsForRecipe();
    }

    IngredientName *getPumpAssignment()
    {
        return this->pumpAssignment;
    }

    void setPumpAssignment(IngredientName pumpAssignment[NUM_OF_PUMPS])
    {
        for (int i = 0; i < NUM_OF_PUMPS; i++)
        {
            this->pumpAssignment[i] = pumpAssignment[i];
        }
    }
};

#endif