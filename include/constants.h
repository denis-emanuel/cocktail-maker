#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

/* Timers */ // Divide by OStaskMS (12 = 120ms)
#define SCALE_READ_MS 20
#define BUTTONS_READ_MS 2
#define POT_READ_MS 10
#define LCD_UPDATE_MS 10
#define SCALE_EMPTY_ERROR_MS 300
#define PUMP_EMPTY_ERROR_MS 1000
#define POURING_TIMEOUT_COUNTS 7 //Seconds Aprox
#define READ_SCALE_MS 12 
#define CLEAN_ROUTINE_MS 5000
#define OS_TASK_PERIOD_MS 10
#define MAIN_LOOP_MS 100

#define NUM_OF_PUMPS 6
#define NOT_ASSIGNED "N/A"

/* Number of ingredients for recipes */
#define GIN_TONIC_INGREDIENTS 3
#define HUGO_INGREDIENTS 3

/* Weight sensor */
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 3

/* Buttons */
#define BUTTONS_PIN A0
#define POT_SELECT_PIN A1
#define POT_VALUE_AUTO_MAX 999
#define POT_VALUE_AUTO_MIN 100

/* Shift register */
#define SHIFT_REGISTER_DATA_PIN 8
#define SHIFT_REGISTER_LATCH_PIN 9
#define SHIFT_REGISTER_CLOCK_PIN 10

#endif
