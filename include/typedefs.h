#ifndef TYPEDEFS_H
#define TYPEDEFS_H

enum globalState {
    INIT,
    AUTO,
    MANUAL,
    SETUP,
    SETUP_COCKTAIL,
    CLEAN,
    PAUSED
};

enum ingredient_type
{
    ALCOHOL,
    SYRUP,
    JUICE,
    SODA
};

enum alcohol_drinks
{
    VODKA = 20,
    PROSECCO,
    WHISKY
};

enum syrup_drinks
{
    SUGAR_SYRUP = 40,
    GRENADINE,
    BLUE_CURACAO
};

enum juice_drinks
{
    ORANGE_JUICE = 60,
    LIME_JUICE
};

enum soda_drinks
{
    TONIC = 80,
    RED_BULL,
    COLA,
    SPRITE
};

struct ingredient {
    int type;
    int amount_ml;
};

struct cocktail {
    String name;
    ingredient ingredients[NUM_OF_PUMPS];
};


#endif