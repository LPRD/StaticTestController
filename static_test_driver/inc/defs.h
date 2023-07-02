#include <Telemetry.h>
// #include <Servo.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

#include "HX711.h"
#include <string>

#ifndef DEFINITIONS
#define DEFINITIONS

// SENSOR DEFINES

#define SENSOR_ERROR_LIMIT 5 // Max number of errors in a row before deciding a sensor is faulty

// #define PRESSURE_CALIBRATION_FACTOR 246.58
// #define PRESSURE_OFFSET 118.33
#define PRESSURE_MIN_VALID -100
#define PRESSURE_MAX_VALID 1000

#define TEMP_MIN_VALID -10
#define TEMP_MAX_VALID 120

#define FORCE_MIN_VALID -50
#define FORCE_MAX_VALID 500

#if CONFIGURATION == MK_2
#define LOAD_CELL_1_CALIBRATION_FACTOR  
#define LOAD_CELL_2_CALIBRATION_FACTOR 4760
#define LOAD_CELL_3_CALIBRATION_FACTOR 4690
#define LOAD_CELL_4_CALIBRATION_FACTOR 4650
#else
#define LOAD_CELL_CALIBRATION_FACTOR 20400//This value is obtained using the SparkFun_HX711_Calibration sketch
#endif

#define LOAD_CELL_RETRY_INTERVAL 10
#define LOAD_CELL_MAX_RETRIES 20


// SENSOR DEFINES END 




//  IGNITER DEFINES

#define IGNITER_PIN 15
#define SERVO_PIN 13

#if CONFIGURATION == DEMO
#define IGNITER_DURATION 5000
#else
#define IGNITER_DURATION 500
#endif



//IGNITER DEFINES END 

//STATIC TEST DRIVER DEFINES BEGIN 

// #define INLET_TEMP A5
// #define OUTLET_TEMP A4
// #define NUMBER_OF_TEMP_SENSORS 4
// int temp_error[NUMBER_OF_TEMP_SENSORS] = { 0,0 };

#define PRESSURE_FUEL A0
#define PRESSURE_OX A1
#define PRESSURE_FUEL_INJECTOR A2
#define PRESSURE_OX_INJECTOR A3
#define PRESSURE_NUM_HIST_VALS 10
#define NUMBER_OF_PRESSURE_SENSORS 4


//valves
#define FUEL_PRE_PIN 34
#define FUEL_MAIN_PIN 33
#define OX_PRE_PIN 32
#define OX_MAIN_PIN 31
#define N2_CHOKE_PIN 29
#define N2_DRAIN_PIN 28

// Load cell setup
#define LOAD_CELL_1_DOUT 11
#define LOAD_CELL_2_DOUT 10
#define LOAD_CELL_3_DOUT 9
#define LOAD_CELL_4_DOUT 8
#define LOAD_CELL_1_CLK 7
#define LOAD_CELL_2_CLK 6
#define LOAD_CELL_3_CLK 5
#define LOAD_CELL_4_CLK 4

//thermocouple 
#define THERMOCOUPLE_PIN_1 2
#define THERMOCOUPLE_PIN_2 3

typedef enum{
  STAND_BY,
  TERMINAL_COUNT,
  PRESTAGE_READY,
  PRESTAGE,
  RETRACTION,
  MAINSTAGE,
  OXYGEN_SHUTDOWN,
  SHUTDOWN,
  COOL_DOWN
}  state_t;


void init_autosequence();

#endif 
