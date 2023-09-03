// #include <Telemetry.h>
// #include <Servo.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

#include "PressureTransducer.h"
#include "LoadCell.h"
#include "Thermocouple.h"
#include "Valve.h"
#include "ServoArm.h"
#include "Igniter.h"
#include "Telemetry.h"


#include "HX711.h"
#include <string>

#ifndef DEFINITIONS
#define DEFINITIONS

// #define PRESSURE_CALIBRATION_FACTOR 246.58
// #define PRESSURE_OFFSET 118.33




#if CONFIGURATION == MK_2
#define LOAD_CELL_1_CALIBRATION_FACTOR 4700
#define LOAD_CELL_2_CALIBRATION_FACTOR 4760
#define LOAD_CELL_3_CALIBRATION_FACTOR 4690
#define LOAD_CELL_4_CALIBRATION_FACTOR 4650
#else
#define LOAD_CELL_CALIBRATION_FACTOR 20400//This value is obtained using the SparkFun_HX711_Calibration sketch
#endif


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

#define PRESSURE_FUEL 0//A0
#define PRESSURE_OX 1//A1
#define PRESSURE_FUEL_INJECTOR 2//A2
#define PRESSURE_OX_INJECTOR 3//A3
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

const char *states[9] = {
  "STAND_BY",
  "TERMINAL_COUNT",
  "PRESTAGE_READY",
  "PRESTAGE",
  "RETRACTION",
  "MAINSTAGE",
  "OXYGEN_SHUTDOWN",
  "SHUTDOWN",
  "COOL_DOWN"
};

//Autosequence

#define DEMO       1    // demo run
#define MK_2_FULL  2    // Test fire for Mk.2 full capacity (new ox regulator)
#define MK_2_LOW   3    // Test fire for Mk.2 low capacity  (old ox regulator)

#define CONFIGURATION MK_2_LOW

//BELOW VALUES ARE DEFINED FROM END OF COUNTDOWN
#define PRESTAGE_PREP_TIME 0              // Time at which to open the prestage valves
#define PRESTAGE_TIME      2000           // Time at which the ignitor fires - make sure this is right
#define RETRACTION_TIME    4000           // Time at which the ignitor arm retracts, concluding ignition time - make sure this is right
#define MAINSTAGE_TIME     5000           // Time at which the main valves open - make sure this is right
#define THRUST_CHECK_TIME  6500           // Time at which to start measuring engine output thrust (2 seconds)
// BELOW VALUES ARE DEFINED FROM SHUTDOWN
#define OX_LEADTIME        500            // Delay between closing oxygen prestage valve and closing fuel prestage valve (0.5 seconds)
#define PRE_LEADTIME       1000           // Delay between closing oxygen prestage valve and closing both mainstage valves (1 second)
#define HEARTBEAT_TIMEOUT  1000           // Timeout to abort run after not recieving heartbeat signal (1 second)

#if CONFIGURATION == MK_2_FULL
  #define COUNTDOWN_DURATION  60000       // 1 minute
  #define RUN_TIME            10000       // 10 seconds
  #define COOLDOWN_TIME       60000 * 10  // 10 minutes
#elseif CONFIGURATION == MK_2_LOW
  #define COUNTDOWN_DURATION  60000       // 1 minute
  #define RUN_TIME            2000        // 2 seconds
  #define COOLDOWN_TIME       60000 * 5   // 5 minutes
#else
  #define COUNTDOWN_DURATION  10000       // 10 seconds
  #define RUN_TIME            10000       // 10 seconds
  #define COOLDOWN_TIME       10000       // 10 seconds
#endif

//***Limits***//
#define MAX_COOLANT_TEMP 60               // states max outlet temperature (60°C)
  
#if CONFIGURATION == MK_2_FULL
  #define MIN_THRUST  1000                // Min thrust that must be reached to avoid triggering a no-ignition shutdown (1000 Newtons)
#elseif CONFIGURATION == MK_2_LOW
  #define MIN_THRUST  100                 // Min thrust that must be reached to avoid triggering a no-ignition shutdown (100 Newtons)
#else
  #define MIN_THRUST  -10
#endif

//***LED's***//
#if CONFIGURATION == MK_2_FULL
  #define TERMINAL_COUNT_LED_PERIOD 60000       // 1 minute
  #define COOL_DOWN_LED_PERIOD      60000 * 10  // 10 minutes
#elseif CONFIGURATION == MK_2_LOW
  #define TERMINAL_COUNT_LED_PERIOD 60000       // 1 minute
  #define COOL_DOWN_LED_PERIOD      60000 * 5   // 5 minutes
#else
  #define TERMINAL_COUNT_LED_PERIOD 10000       // 1 minute
  #define COOL_DOWN_LED_PERIOD      10000       // 1 minute
#endif

#endif 
