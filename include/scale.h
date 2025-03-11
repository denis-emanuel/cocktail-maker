#ifndef PROJECT_SCALE_H
#define PROJECT_SCALE_H

#include "HX711.h"
#include "./constants.h"

class ScaleController
{
private:
    HX711 scale;
    static unsigned int readingCounter;
    static int weight;

public:
    ScaleController()
    {
        this->weight = 0;
        this->readingCounter = 0;
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
            this->weight = this->scale.get_units(units);
        }
        else
        {
            this->weight = -1;
        }
    }

    int getWeight()
    {
        return this->weight;
    }

    void tare()
    {
        this->scale.tare();
    }

    void incrementReadingCounter()
    {
        this->readingCounter++;
    }

    int getReadingCounter()
    {
        return this->readingCounter;
    }

    void resetReadingCounter()
    {
        this->readingCounter = 0;
    }
};

#endif