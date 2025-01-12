#ifndef RECIPES_H
#define RECIPES_H

#include "../include/constants.h"
#include "../include/typedefs.h"

enum cocktails {
    HUGO,
    /*
    MOJITO,
    GIN_TONIC,
    VODKA_ORANGE,
    VODKA_RED_BULL,
    MARGARITA,
    COSMOPOLITAN,
    */
    NO_OF_COCKTAILS  
};

const cocktail cocktailList[NO_OF_COCKTAILS] = {
    {
        "HUGO",
        {
            {
                PROSECCO, 100
            },
            {
                SPRITE, 100
            },
            {
                SYRUP, 20
            }
        }
    }

};

#endif