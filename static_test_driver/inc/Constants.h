#ifndef DEFINITIONS
#define DEFINITIONS

#define DEMO       1    // demo run
#define MK_2_FULL  2    // Test fire for Mk.2 full capacity (new ox regulator)
#define MK_2_LOW   3    // Test fire for Mk.2 low capacity  (old ox regulator)

#define CONFIGURATION MK_2_LOW

#define LOAD_CELL_1_CALIBRATION_FACTOR 4700
#define LOAD_CELL_2_CALIBRATION_FACTOR 4760
#define LOAD_CELL_3_CALIBRATION_FACTOR 4690
#define LOAD_CELL_4_CALIBRATION_FACTOR 4650

// PIN CONSTANTS

// igniter
#define IGNITER_PIN 4

// pressure tranducers
#define PRESSURE_FUEL 0
#define PRESSURE_OX 2
#define PRESSURE_FUEL_INJECTOR 1
#define PRESSURE_OX_INJECTOR 3

//valves
#define FUEL_PRE_PIN 67
#define FUEL_MAIN_PIN 68
#define OX_PRE_PIN 44
#define OX_MAIN_PIN 26
#define N2_FILL_PIN 46
#define N2_DRAIN_PIN 65

// Load cell
#define LOAD_CELL_1_DOUT 66
#define LOAD_CELL_2_DOUT 10
#define LOAD_CELL_3_DOUT 9
#define LOAD_CELL_4_DOUT 8
#define LOAD_CELL_1_CLK 50
#define LOAD_CELL_2_CLK 6
#define LOAD_CELL_3_CLK 5
#define LOAD_CELL_4_CLK 4

// Thermocouple 
#define THERMOCOUPLE_PIN_1 2
#define THERMOCOUPLE_PIN_2 3

// TIMING CONSTANTS

//BELOW VALUES ARE DEFINED FROM END OF COUNTDOWN
#define PRESTAGE_PREP_TIME 0              // Time at which to open the prestage valves
#define PRESTAGE_TIME      2000           // Time at which the ignitor fires - make sure this is right
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
#elif CONFIGURATION == MK_2_LOW
  #define COUNTDOWN_DURATION  60000       // 1 minute
  #define RUN_TIME            2000        // 2 seconds
  #define COOLDOWN_TIME       60000 * 5   // 5 minutes
#else
  #define COUNTDOWN_DURATION  10000       // 10 seconds
  #define RUN_TIME            10000       // 10 seconds
  #define COOLDOWN_TIME       10000       // 10 seconds
#endif

#if CONFIGURATION == DEMO
#define IGNITER_DURATION 5000             // Time for which the igniter should stay powered on (5 seconds)
#else
#define IGNITER_DURATION 500              // Time for which the igniter should stay powered on (0.5 seconds)
#endif

//***Limits***//
#define MAX_COOLANT_TEMP 60               // max outlet temperature (60°C)
  
#if CONFIGURATION == MK_2_FULL
  #define MIN_THRUST  1000                // Min thrust that must be reached to avoid triggering a no-ignition shutdown (1000 Newtons)
#elif CONFIGURATION == MK_2_LOW
  #define MIN_THRUST  100                 // Min thrust that must be reached to avoid triggering a no-ignition shutdown (100 Newtons)
#else
  #define MIN_THRUST  -10
#endif

#if CONFIGURATION == DEMO
#define CHECK_SENSORS false
#else
#define CHECK_SENSORS true
#endif

#endif 
