#include <Arduino.h>
#include "../include/constants.h"
#include "../include/functions.h"

void setup() {
  Serial.begin(9600);
}

void loop() {
  sampleTaskAt(MAIN_TASK_PERIOD_MS, []() {
    Serial.println("Hello, world! at ");
    Serial.print(millis());
  });
}

void sampleTaskAt(int period, void (*function)()) {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= period) {
    lastTime = currentTime;
    function();
  }
}
