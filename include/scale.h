#ifndef PROJECT_SCALE_H
#define PROJECT_SCALE_H

#include "HX711.h"
#include "./constants.h"

class ScaleController {
    private:
        HX711 scale;
        unsigned int readingCounter = 0;

    public:
        ScaleController() {}
        
        void begin() {
            this->scale = HX711();
            this->scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
            this->scale.set_scale(397.f);
            this->scale.tare();
        }

        int read() {
            if (this->scale.is_ready()) {
                return this->scale.get_units(10);
            } else {
                return -1;
            }
        }

        void incrementReadingCounter() {
            this->readingCounter++;
            Serial.println("Reading counter incremented");
        }

        int getReadingCounter() {
            return this->readingCounter;
        }   

        void resetReadingCounter() {
            this->readingCounter = 0;
        }     
};

#endif