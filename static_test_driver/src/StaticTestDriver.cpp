#include "defs.h"
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

bool sensors_ok = true;
std::string error_msg = "";

//declare pressure transducers
PressureTransducer pressure_fuel(PRESSURE_FUEL, "fuel", "Fl", sensors_ok, error_msg);
PressureTransducer pressure_ox(PRESSURE_OX, "oxygen", "Ox", sensors_ok, error_msg);
PressureTransducer pressure_fuel_injector(PRESSURE_FUEL_INJECTOR, "injector fuel", "FlE", sensors_ok, error_msg);
PressureTransducer pressure_ox_injector(PRESSURE_OX_INJECTOR, "injector oxygen", "OxE", sensors_ok, error_msg);

bool global_pressure_zero_ready = false;

//declare load cells
LoadCell loadcell_1(LOAD_CELL_1_DOUT, LOAD_CELL_1_CLK, LOAD_CELL_1_CALIBRATION_FACTOR, 1, sensors_ok, error_msg);
LoadCell loadcell_2(LOAD_CELL_2_DOUT, LOAD_CELL_2_CLK, LOAD_CELL_2_CALIBRATION_FACTOR, 2, sensors_ok, error_msg);
LoadCell loadcell_3(LOAD_CELL_3_DOUT, LOAD_CELL_3_CLK, LOAD_CELL_3_CALIBRATION_FACTOR, 3, sensors_ok, error_msg);
LoadCell loadcell_4(LOAD_CELL_4_DOUT, LOAD_CELL_4_CLK, LOAD_CELL_4_CALIBRATION_FACTOR, 4, sensors_ok, error_msg);


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

unsigned long last_heartbeat = 0;

// Generally-used variables for parsing commands
char data[10];
char data_name[20];

// Calling this performs a software reset of the board, reinitializing sensors
void (*reset)(void) = 0;

// Function to replace Arduino.h millis()
unsigned int millis () {
  struct timeval t ;
  gettimeofday ( & t , NULL ) ;
  return t.tv_sec * 1000 + ( t.tv_usec + 500 ) / 1000 ;
}

// Autosequence Functions
void set_state(state_t state, state_t * state_var) {
  *state_var = state;
  SEND(status, states[state], "%s");
}

long start_time     = 0;
long shutdown_time  = 0;
long heartbeat_time = 0;

state_t state = STAND_BY;

// void blink(int led, long period){
//   digitalWrite(led, (int)((millis() % (period * 2)) / period));
// }

void heartbeat(){
  heartbeat_time = millis();
}

void init_autosequence(){
  set_state(STAND_BY, &state);
}

void start_countdown(){
  #if CONFIGURATION != DEMO
    if (sensors_ok == false){
      std::cout << "Countdown aborted due to sensor failure\n";
      set_state(STAND_BY, &state);
    }else
  #endif
  if (valve_fuel_pre.m_current_state  ||
      valve_fuel_main.m_current_state ||
      valve_ox_pre.m_current_state    ||
      valve_ox_main.m_current_state   ||
      valve_n2_fill.m_current_state  ||
      valve_n2_drain.m_current_state  ){
        std::cout << "Countdown aborted due to unexpected initial valve state.\n";
        set_state(STAND_BY, &state);
  }
  else{
    std::cout << "Countdown has started\n";
    set_state(TERMINAL_COUNT, &state);
    start_time = millis();
    heartbeat();
  }
}

void abort_autosequence(){
  std::cout << "run aborted\n";
  switch (state){
    case STAND_BY:
      break;
      
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
  }
}

void run_control(){
  long run_time = millis() - start_time - COUNTDOWN_DURATION;
  SEND(run_time, run_time, "%ld");

  igniter.handle_igniter();

  #if CONFIGURATION == MK_2_FULL || CONFIGURATION == MK_2_LOW
    if (state!=STAND_BY  &&  state!=COOL_DOWN  &&  millis() > heartbeat_time + HEARTBEAT_TIMEOUT){
      std::cout << "Loss of data link\n";
      abort_autosequence();
    }else
  #endif

  switch (state){
    case STAND_BY:
      // if (!sensors_ok){
      //   set_lcd_status("Sensor Failure");
      // }
      break;

    case TERMINAL_COUNT:
      #if CONFIGURATION != DEMO
        if (!sensors_ok){
          std::cout << "Sensor failure\n";
          abort_autosequence();
        }else
      #endif
      if (run_time >= PRESTAGE_PREP_TIME){
        valve_fuel_pre.set_valve(1);
        valve_n2_fill.set_valve(1);
        valve_ox_pre.set_valve(1);
        set_state(PRESTAGE_READY, &state);
        
      }
      break;

    case PRESTAGE_READY:
      if (run_time >= PRESTAGE_TIME){
        igniter.fire_igniter();
        set_state(PRESTAGE, &state);
      }
      break;

//    case PRESTAGE:
//      if (run_time >= MAINSTAGE_TIME){
//        valve_fuel_main.set_valve(1);
//        valve_ox_main.set_valve(1);
//        set_state(MAINSTAGE, &state);
//      }
//      break;

    case PRESTAGE:
      if (run_time >= MAINSTAGE_TIME){
        valve_fuel_main.set_valve(1);
        valve_ox_main.set_valve(1);
        set_state(MAINSTAGE, &state);
      }
      break;

    case MAINSTAGE:
      #if CONFIGURATION != DEMO
        if (!sensors_ok){
          std::cout << "Sensor Failure\n";
          abort_autosequence();
        }else
      #endif
      //Check outlet temperature
      //TODO: Should this be a function call instead of a parameter reference?
      //TODO: Also, rename the thermocouples.
      if (thermocouple_2.m_current_temp >= MAX_COOLANT_TEMP){
        std::cout << "Temperature reached critical level. Shuttung down.\n";
        abort_autosequence();
      }
      else{ 
        // Force will be the sum of the four loadcells for the purpose of error checking
        float force = (loadcell_1.m_current_force + loadcell_2.m_current_force +
                         loadcell_3.m_current_force + loadcell_4.m_current_force);
        if (run_time >= THRUST_CHECK_TIME && force < MIN_THRUST){
          std::cout << "Thrust below critical level. Shutting down.\n";
          abort_autosequence();
        }
      }  

      if (run_time >= RUN_TIME){
        set_state(OXYGEN_SHUTDOWN, &state);
        valve_ox_pre.set_valve(0);
        shutdown_time = millis();
      }
      break;

    case OXYGEN_SHUTDOWN:
      if (millis() >= shutdown_time + OX_LEADTIME){
        valve_n2_fill.set_valve(0);
        valve_fuel_pre.set_valve(0);
        set_state(SHUTDOWN, &state);
      }
      break;

    case SHUTDOWN:
      if (millis() >= shutdown_time + PRE_LEADTIME){
        valve_ox_main.set_valve(0);
        valve_fuel_main.set_valve(0);
        valve_n2_drain.set_valve(1);
        set_state(COOL_DOWN, &state);
      }
      break;

    case COOL_DOWN:
      if (!valve_n2_drain.m_current_state){
        valve_n2_drain.set_valve(1);
      }
      if (millis() - shutdown_time >= COOLDOWN_TIME){
        std::cout << "Run finished\n";
        valve_n2_drain.set_valve(0);
        set_state(STAND_BY, &state);
        start_time = 0;
      }
      break;
  }
}

// Main code
void setup() {
    // Initialize buffers
    memset(data, 0, 10);
    memset(data_name, 0, 20);

    // Initialize connection
    std::cout << "LPRD static test driver\n";
    std::cout << "Waiting for connection\n";
    wait_for_connection();

    std::cout << "Initializing...\n";

    //init forces
    loadcell_1.init_loadcell();
    loadcell_2.init_loadcell();
    loadcell_3.init_loadcell();
    loadcell_4.init_loadcell();

    //thermocouples
    thermocouple_1.init_thermocouple();
    thermocouple_2.init_thermocouple();

    // //init all valves
    // valve_fuel_pre.init_valve();
    // valve_fuel_main.init_valve();
    // valve_ox_pre.init_valve();
    // valve_ox_main.init_valve();
    // valve_n2_fill.init_valve();
    // valve_n2_drain.init_valve();

    // Set initial state
    init_autosequence();

    std::cout << "Setup Complete\n";
}

void loop() {
    // Grab force data
    loadcell_1.updateForces();
    loadcell_2.updateForces();
    loadcell_3.updateForces();
    loadcell_4.updateForces();
    
    // Update pressures
    pressure_fuel.updatePressures();
    pressure_ox.updatePressures();
    pressure_fuel_injector.updatePressures();
    pressure_ox_injector.updatePressures();
    
    // Grab thermocouple data 
    thermocouple_1.updateTemps();
    thermocouple_2.updateTemps();

   
    // Update sensor diagnostic message on GUI
    std::cout << error_msg << std::endl;
    error_msg = "";

    // Run autonomous control
    run_control();

    // Send collected data
    BEGIN_SEND
        SEND_ITEM(force1, loadcell_1.m_current_force, "%f")
        SEND_ITEM(force2, loadcell_2.m_current_force, "%f")
        SEND_ITEM(force3, loadcell_3.m_current_force, "%f")
        SEND_ITEM(force4, loadcell_4.m_current_force, "%f")
        
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
        std::cout << "Zeroing load cells\n";
        loadcell_1.zeroForces(); 
        loadcell_2.zeroForces(); 
        loadcell_3.zeroForces(); 
        loadcell_4.zeroForces(); 
    }

    READ_FLAG(zero_pressure) {
        if (pressure_fuel.m_zero_ready || pressure_ox.m_zero_ready) {
            std::cout << "Zeroing fuel pressure\n";
            pressure_fuel.zeroPressures();
            pressure_fuel.m_zero_ready = false;
            std::cout << "Zeroing oxidizer pressure\n";
            pressure_ox.zeroPressures();
            pressure_ox.m_zero_ready = false;
        }
        else {
            std::cout << "Pressure zero values not ready\n";
        }
    }

    READ_FLAG(heartbeat) {
        heartbeat();
    }
    READ_FLAG(reset) {
        std::cout << "Resetting board\n";
        reset();
    }
    READ_FLAG(start) {
        start_countdown();
    }
    READ_FLAG(stop) {
        std::cout << "Manual abort initiated\n";
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
        std::cout << "Invalid data field recieved: " << data_name << ":" << data << std::endl;
    }
    END_READ
}

int main() {
  setup();
  while (true) {
    loop();
  }
}
