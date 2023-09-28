#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "defs.h"
#include "Telemetry.h"
#include "Valve.h"
#include "PressureTransducer.h"
#include "Igniter.h"
#include "Thermocouple.h"
#include "LoadCell.h"

bool sensors_ok = true;
std::string error_msg = "";

//declare pressure transducers
PressureTransducer pressure_fuel(PRESSURE_FUEL, "fuel", "Fl", sensors_ok, error_msg);
PressureTransducer pressure_ox(PRESSURE_OX, "oxygen", "Ox", sensors_ok, error_msg);
PressureTransducer pressure_fuel_injector(PRESSURE_FUEL_INJECTOR, "injector fuel", "FlE", sensors_ok, error_msg);
PressureTransducer pressure_ox_injector(PRESSURE_OX_INJECTOR, "injector oxygen", "OxE", sensors_ok, error_msg);

//declare load cells
LoadCell loadcell(LOAD_CELL_1_DOUT, LOAD_CELL_1_CLK, LOAD_CELL_1_CALIBRATION_FACTOR, 1, sensors_ok, error_msg);

//declare thermocouples 
Thermocouple thermocouple_1(THERMOCOUPLE_PIN_1, "Inlet", "In", sensors_ok, error_msg);
Thermocouple thermocouple_2(THERMOCOUPLE_PIN_2, "Outlet", "Out", sensors_ok, error_msg);


//declare all valves
Valve valve_fuel_pre(FUEL_PRE_PIN, "fuel pre", "fuel_pre_setting");
Valve valve_fuel_main(FUEL_MAIN_PIN, "fuel main", "fuel_main_setting");
Valve valve_ox_pre(OX_PRE_PIN, "oxygen pre", "ox_pre_setting");
Valve valve_ox_main(OX_MAIN_PIN, "oxygen main", "ox_main_setting");
Valve valve_n2_fill(N2_FILL_PIN, "n2 fill", "nitro_fill_setting");
Valve valve_n2_drain(N2_DRAIN_PIN, "n2 drain", "nitro_drain_setting");

Igniter igniter(IGNITER_PIN);

// Generally-used variables for parsing commands
char data[10];
char data_name[20];

// Calling this performs a software reset of the board, reinitializing sensors
// TODO: Reset causes a seg fault and kills the program
//       Could maybe just add a process to always restart it
void (*reset)(void) = 0;

// Function to replace Arduino.h millis()
unsigned int millis () {
  struct timeval t ;
  gettimeofday ( & t , NULL ) ;
  return t.tv_sec * 1000 + ( t.tv_usec + 500 ) / 1000 ;
}

// Set the state variable and send it the state to the GUI
void set_state(state_t state, state_t * state_var) {
  *state_var = state;
  SEND(status, states[state], "%s");
}

long start_time     = 0;
long shutdown_time  = 0;
long heartbeat_time = 0;
long ignition_time  = 0;

state_t state = STAND_BY;

// Update hearbeat_time to have the current time
void heartbeat(){
  heartbeat_time = millis();
}

void init_autosequence(){
  set_state(STAND_BY, &state);
}

void start_countdown(){
  #if CONFIGURATION != DEMO
    if (sensors_ok == false){
      printf("Countdown aborted due to sensor failure\n");
      set_state(STAND_BY, &state);
    }else
  #endif
  if (valve_fuel_pre.m_current_state  ||
      valve_fuel_main.m_current_state ||
      valve_ox_pre.m_current_state    ||
      valve_ox_main.m_current_state   ||
      valve_n2_fill.m_current_state   ||
      valve_n2_drain.m_current_state  ){
        printf("Countdown aborted due to unexpected initial valve state.\n");
        set_state(STAND_BY, &state);
  }
  else{
    printf("Countdown has started\n");
    set_state(TERMINAL_COUNT, &state);
    // Save the current time for reference later
    start_time = millis();
    heartbeat();
  }
}

void abort_autosequence(){
  printf("run aborted\n");
  switch (state){ 
    case TERMINAL_COUNT:
      set_state(STAND_BY, &state);
      break;

    case PRESTAGE_READY:
      valve_n2_fill.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      set_state(STAND_BY, &state);
      break;

    case PRESTAGE:
      valve_n2_fill.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      igniter.reset_igniter();
      set_state(COOL_DOWN, &state);
      shutdown_time = millis();
      break;

    case MAINSTAGE:
      valve_n2_fill.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      set_state(SHUTDOWN, &state);
      shutdown_time = millis();
      break;

    default:
      break;
  }
}

void run_control(){
  // Update run time variable
  long run_time = millis() - start_time - COUNTDOWN_DURATION;
  SEND(run_time, run_time, "%ld");

  // Deactivate Igniter if necessary
  if (ignition_time != 0  && millis() > ignition_time + IGNITER_DURATION) {
      igniter.reset_igniter();
      ignition_time = 0;
  }
  
  // Check for recent communication with GUI
  #if CONFIGURATION == MK_2_FULL || CONFIGURATION == MK_2_LOW
    if (state!=STAND_BY  &&  state!=COOL_DOWN  &&  millis() > heartbeat_time + HEARTBEAT_TIMEOUT){
      printf("Loss of data link\n");
      abort_autosequence();
    }
  #endif

  switch (state){
    case TERMINAL_COUNT:
      // Check sensor status
      #if CONFIGURATION != DEMO
        if (!sensors_ok){
          printf("Sensor failure\n");
          abort_autosequence();
        }else
      #endif
      // Open fuel_pre, n2_fill, ox_pre
      if (run_time >= PRESTAGE_PREP_TIME){
        valve_fuel_pre.set_valve(1);
        valve_n2_fill.set_valve(1);
        valve_ox_pre.set_valve(1);
        set_state(PRESTAGE_READY, &state);
      }
      break;

    case PRESTAGE_READY:
      // Fire igniter
      if (run_time >= PRESTAGE_TIME){
        igniter.fire_igniter();
        ignition_time = millis();
        set_state(PRESTAGE, &state);
      }
      break;

    case PRESTAGE:
      // Open fuel_main and ox_main
      if (run_time >= MAINSTAGE_TIME){
        valve_fuel_main.set_valve(1);
        valve_ox_main.set_valve(1);
        set_state(MAINSTAGE, &state);
      }
      break;

    case MAINSTAGE:
      // Check sensor status
      #if CONFIGURATION != DEMO
        if (!sensors_ok){
          printf("Sensor Failure\n");
          abort_autosequence();
        }
      #endif
      //Check outlet temperature
      if (thermocouple_2.m_current_temp >= MAX_COOLANT_TEMP){
        printf("Temperature reached critical level. Shuttung down.\n");
        abort_autosequence();
      }
      // Check Force
      else if (run_time >= THRUST_CHECK_TIME && loadcell.m_current_force < MIN_THRUST){
        printf("Thrust below critical level. Shutting down.\n");
        abort_autosequence();
      }

      // Start shutdown sequence by closing ox_pre valve
      if (run_time >= RUN_TIME){
        valve_ox_pre.set_valve(0);
        shutdown_time = millis();
        set_state(OXYGEN_SHUTDOWN, &state);
      }
      break;

    case OXYGEN_SHUTDOWN:
      // Close n2_fill and fuel_pre
      if (millis() >= shutdown_time + OX_LEADTIME){
        valve_n2_fill.set_valve(0);
        valve_fuel_pre.set_valve(0);
        set_state(SHUTDOWN, &state);
      }
      break;

    case SHUTDOWN:
      // Close ox_main and fuel_main. Open n2_drain
      if (millis() >= shutdown_time + PRE_LEADTIME){
        valve_ox_main.set_valve(0);
        valve_fuel_main.set_valve(0);
        valve_n2_drain.set_valve(1);
        set_state(COOL_DOWN, &state);
      }
      break;

    case COOL_DOWN:
      // TODO: trace logic to see if "if" is necessary
      if (!valve_n2_drain.m_current_state){
        valve_n2_drain.set_valve(1);
      }
      if (millis() - shutdown_time >= COOLDOWN_TIME){
        printf("Run finished\n");
        valve_n2_drain.set_valve(0);
        set_state(STAND_BY, &state);
        start_time = 0;
      }
      break;

    default:
      break;
  }
}

// Main code
void setup() {
    // Initialize buffers
    memset(data, 0, 10);
    memset(data_name, 0, 20);

    // Initialize connection
    printf("LPRD Static Test Driver\n");
    printf("Blocking and waiting for connection...\n");
    wait_for_connection();
    printf("Connected\n");

    // Initialize LC
    loadcell.init_loadcell();

    // Initialize Thermocouples
    thermocouple_1.init_thermocouple();
    thermocouple_2.init_thermocouple();

    // Set initial state
    init_autosequence();

    printf("Setup Complete\n");
}

void loop() {
    // Grab force data
    loadcell.updateForces();
    
    // Update pressures
    pressure_fuel.updatePressures();
    pressure_ox.updatePressures();
    pressure_fuel_injector.updatePressures();
    pressure_ox_injector.updatePressures();
    
    // Grab thermocouple data 
    thermocouple_1.updateTemps();
    thermocouple_2.updateTemps();

    // Update sensor diagnostic message on GUI
    printf("%s\n", error_msg.c_str());
    error_msg = "";

    // Run autonomous control
    run_control();

    // Send collected data
    BEGIN_SEND
        SEND_ITEM(force, loadcell.m_current_force, "%f")
        
        SEND_ITEM(outlet_temp, thermocouple_1.m_current_temp, "%f")
        SEND_ITEM(inlet_temp, thermocouple_2.m_current_temp, "%f")
        
        SEND_ITEM(fuel_press, pressure_fuel.m_current_pressure, "%f")
        SEND_ITEM(ox_press, pressure_ox.m_current_pressure, "%f")
        SEND_ITEM(fuel_inj_press, pressure_fuel_injector.m_current_pressure, "%f")
        SEND_ITEM(ox_inj_press, pressure_ox_injector.m_current_pressure, "%f")
        
        SEND_ITEM(fuel_pre_setting, valve_fuel_pre.m_current_state, "%d");
        SEND_ITEM(fuel_main_setting, valve_fuel_main.m_current_state, "%d");
        SEND_ITEM(ox_pre_setting, valve_ox_pre.m_current_state, "%d");
        SEND_ITEM(ox_main_setting, valve_ox_main.m_current_state, "%d");
        SEND_ITEM(nitro_fill_setting, valve_n2_fill.m_current_state, "%d");
        SEND_ITEM(nitro_drain_setting, valve_n2_drain.m_current_state, "%d");
        
        SEND_ITEM(sensors_ok, sensors_ok, "%d")
    END_SEND

    // Read a command
    int valve_command;

    BEGIN_READ
    READ_FLAG(zero_force) {
        printf("Zeroing load cells\n");
        loadcell.zeroForces(); 
    }

    READ_FLAG(zero_pressure) {
        if (pressure_fuel.m_zero_ready || pressure_ox.m_zero_ready) {
            printf("Zeroing fuel pressure\n");
            pressure_fuel.zeroPressures();
            pressure_fuel.m_zero_ready = false;
            printf("Zeroing oxidizer pressure\n");
            pressure_ox.zeroPressures();
            pressure_ox.m_zero_ready = false;
        }
        else {
            printf("Pressure zero values not ready\n");
        }
    }

    READ_FLAG(heartbeat) {
        heartbeat();
    }
    READ_FLAG(reset) {
        printf("Resetting board\n");
        reset();
    }
    READ_FLAG(start) {
        start_countdown();
    }
    READ_FLAG(stop) {
        printf("Manual abort initiated\n");
        abort_autosequence();
    }
    READ_FLAG(fire_igniter) {
        igniter.fire_igniter();
    }
    READ_FIELD(fuel_pre_command, "%d", valve_command) {
       valve_fuel_pre.set_valve(valve_command);
    }
    READ_FIELD(fuel_main_command, "%d", valve_command) {
        valve_fuel_main.set_valve(valve_command);
    }
    READ_FIELD(ox_pre_command, "%d", valve_command) {
        valve_ox_pre.set_valve(valve_command);
    }
    READ_FIELD(ox_main_command, "%d", valve_command) {
        valve_ox_main.set_valve(valve_command);
    }
    READ_FIELD(nitro_fill_command, "%d", valve_command) {
        valve_n2_fill.set_valve(valve_command);
    }
    READ_FIELD(nitro_drain_command, "%d", valve_command) {
        valve_n2_drain.set_valve(valve_command);
    }
    READ_DEFAULT(data_name, data) {
        printf("Invalid data field recieved: %s: %s\n", data_name, data);
    }
    END_READ
}

int main() {
  setup();
  while (true) {
    loop();
  }
}
