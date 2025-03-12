#ifndef PROJECT_SCALE_H
#define PROJECT_SCALE_H

#include "HX711.h"
#include "./constants.h"

class ScaleController
{
private:
    HX711 scale;
    int weight;

public:
    ScaleController()
    {
        this->weight = 0;
    }

    void begin()
    {
        this->scale = HX711();
        this->scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
        this->scale.set_scale(397.f);
        this->scale.tare();
    }

    void update(short int units)
    {
        if (this->scale.is_ready())
        {
            int weightAux = this->scale.get_units(units);
            if (weightAux >= 0)
            {
                this->weight = weightAux;
            }
        }
    }

    int getWeight()
    {
        return this->weight;
    }

    void tare()
    {
        this->weight = 0;
        this->scale.tare();
    }
};

#endif