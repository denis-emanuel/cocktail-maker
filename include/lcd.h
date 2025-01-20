#ifndef PROJECT_LCD_H
#define PROJECT_LCD_H

#include <LiquidCrystal_I2C.h>

class LCDController {
    private:
        LiquidCrystal_I2C lcd;
    
    public:
        LCDController() : lcd(0x27, 20, 4) {}

        void begin() {
            this->lcd.init();
            this->lcd.backlight();
        }
        
        void clear() {
            this->lcd.clear();
            this->lcd.home();
        }

        void printFirstLine(const char* text) {
            this->lcd.setCursor(0, 0);
            this->lcd.print(text);
        }

        void printSecondLine(const char* text) {
            this->lcd.setCursor(0, 1);
            this->lcd.print(text);
        }
};

#endif