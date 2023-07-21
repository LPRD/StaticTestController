#include "defs.h"

//declare pressure transducers
PressureTransducer pressure_fuel{PRESSURE_FUEL, "fuel", "Fl"};
PressureTransducer pressure_ox{PRESSURE_OX, "oxygen", "Ox"};
PressureTransducer pressure_fuel_injector{PRESSURE_FUEL_INJECTOR, "injector fuel", "FlE"};
PressureTransducer pressure_ox_injector{PRESSURE_OX_INJECTOR, "injector oxygen", "OxE"};

bool global_pressure_zero_ready = false;

//declare load cells
LoadCell loadcell_1{LOAD_CELL_1_DOUT, LOAD_CELL_1_CLK, LOAD_CELL_1_CALIBRATION_FACTOR, "LoadCell1", "LC1"};
LoadCell loadcell_2{LOAD_CELL_2_DOUT, LOAD_CELL_2_CLK, LOAD_CELL_2_CALIBRATION_FACTOR, "LoadCell2", "LC2"};
LoadCell loadcell_3{LOAD_CELL_3_DOUT, LOAD_CELL_3_CLK, LOAD_CELL_3_CALIBRATION_FACTOR, "LoadCell3", "LC3"};
LoadCell loadcell_4{LOAD_CELL_4_DOUT, LOAD_CELL_4_CLK, LOAD_CELL_4_CALIBRATION_FACTOR, "LoadCell4", "LC4"};


//declare thermocouples 
Thermocouple thermocouple_1(THERMOCOUPLE_PIN_1, "Inlet", "In");
Thermocouple thermocouple_2(THERMOCOUPLE_PIN_2, "Outlet", "Out");


bool sensor_status = true;

//declare all valves
Valve valve_fuel_pre{FUEL_PRE_PIN, "fuel pre", "fuel_pre_setting"};
Valve valve_fuel_main{FUEL_MAIN_PIN, "fuel main", "fuel_main_setting"};
Valve valve_ox_pre{OX_PRE_PIN, "oxygen pre", "ox_pre_setting"};
Valve valve_ox_main{OX_MAIN_PIN, "oxygen main", "ox_main_setting"};
Valve valve_n2_choke{N2_CHOKE_PIN, "n2 choke", "nitro_fill_setting"};
Valve valve_n2_drain{N2_DRAIN_PIN, "n2 drain", "nitro_drain_setting"};


//declare servo for the ignition arm
ServoArm servo_arm(SERVO_PIN);

unsigned long last_heartbeat = 0;

// Generally-used variables for parsing commands
char data[10] = "";
char data_name[20] = "";

// Calling this performs a software reset of the board, reinitializing sensors
void (*reset)(void) = 0;

// Autosequence Functions
void set_state(state_t state, state_t * state_var) {
  *state_var = state;
  SEND(status, states[state]);
}

long start_time     = 0;
long shutdown_time  = 0;
long heartbeat_time = 0;

state_t state = STAND_BY;

void blink(int led, long period){
  digitalWrite(led, (int)((millis() % (period * 2)) / period));
}

void heartbeat(){
  heartbeat_time = millis();
}

void init_autosequence(){
  set_state(STAND_BY, &state);
}

void start_countdown(){
  #if CONFIGURATION != DEMO
    if (sensor_status == false){
      Serial.println(F("Countdown aborted due to sensor failure"));
      set_state(STAND_BY, &state);
    }else
  #endif
  if (valve_fuel_pre.m_current_state  ||
      valve_fuel_main.m_current_state ||
      valve_ox_pre.m_current_state    ||
      valve_ox_main.m_current_state   ||
      valve_n2_choke.m_current_state  ||
      valve_n2_drain.m_current_state  ){
        Serial.println(F("Countdown aborted due to unexpected initial valve state."));
        set_state(STAND_BY, &state);
  }
  else{
    Serial.println(F("Countdown has started"));
    set_state(TERMINAL_COUNT, &state);
    start_time = millis();
    heartbeat();
  }
}

void abort_autosequence(){
  Serial.println(F("run aborted"));
  switch (state){
    case STAND_BY:
      servo_arm.retract();
      break;
      
    case TERMINAL_COUNT:
      set_state(STAND_BY, &state);
      break;

    case PRESTAGE_READY:
      valve_n2_choke.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      set_state(STAND_BY, &state);
      break;

    case PRESTAGE:
      valve_n2_choke.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      reset_igniter();
      set_state(COOL_DOWN, &state);
      shutdown_time = millis();
      break;

    case MAINSTAGE:
      valve_n2_choke.set_valve(0);
      valve_fuel_pre.set_valve(0);
      valve_ox_pre.set_valve(0);
      set_state(SHUTDOWN, &state);
      shutdown_time = millis();
      break;
  }
}

void run_control(){
  long run_time = millis() - start_time - COUNTDOWN_DURATION;
  SEND(run_time, run_time);

  handle_igniter();

  #if CONFIGURATION == MK_2_FULL || CONFIGURATION == MK_2_LOW
    if (state!=STAND_BY  &&  state!=COOL_DOWN  &&  millis() > heartbeat_time + HEARTBEAT_TIMEOUT){
      Serial.println(F("Loss of data link"));
      abort_autosequence();
    }else
  #endif

  switch (state){
    case STAND_BY:
      // if (!sensor_status){
      //   set_lcd_status("Sensor Failure");
      // }
      if (servo_arm.servo.read() !=0) {
        Serial.println(F("Servo not activated"));
        abort_autosequence();
      }
      break;

    case TERMINAL_COUNT:
      #if CONFIGURATION != DEMO
        if (!sensor_status){
          Serial.println(F("Sensor failure"));
          abort_autosequence();
        }else
      #endif
      if (run_time >= PRESTAGE_PREP_TIME){
        valve_fuel_pre.set_valve(1);
        valve_n2_choke.set_valve(1);
        valve_ox_pre.set_valve(1);
        set_state(PRESTAGE_READY, &state);
        
      }
      break;

    case PRESTAGE_READY:
      if (run_time >= PRESTAGE_TIME){
        fire_igniter();
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
      if (run_time >= RETRACTION_TIME){
        servo_arm.retract();
        set_state(RETRACTION, &state);
      }
      break;

     case RETRACTION:
      if (run_time >= MAINSTAGE_TIME){
        valve_fuel_main.set_valve(1);
        valve_ox_main.set_valve(1);
        set_state(MAINSTAGE, &state);
      }
      break;

    case MAINSTAGE:
      #if CONFIGURATION != DEMO
        if (!sensor_status){
          Serial.println(F("Sensor Failure"));
          abort_autosequence();
        }else
      #endif
      //Check outlet temperature
      //TODO: Should this be a function call instead of a parameter reference?
      //TODO: Also, rename the thermocouples.
      if (thermocouple_2.m_current_temp >= MAX_COOLANT_TEMP){
        Serial.println(F("Temperature reached critical level. Shuttung down."));
        abort_autosequence();
      }
      else{ 
        // Force will be the sum of the four loadcells for the purpose of error checking
        float force = (loadcell_1.m_current_force + loadcell_2.m_current_force +
                         loadcell_3.m_current_force + loadcell_4.m_current_force);
        if (run_time >= THRUST_CHECK_TIME && force < MIN_THRUST){
          Serial.println(F("Thrust below critical level. Shutting down."));
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
        valve_n2_choke.set_valve(0);
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
        Serial.println(F("Run finished"));
        valve_n2_drain.set_valve(0);
        set_state(STAND_BY, &state);
        start_time = 0;
      }
      break;
  }
}

// Main code
void setup() {
    // Initialize serial
    // while (!Serial);
    Serial.begin(115200);
    Serial.println(F("Mk 2 static test driver"));
    Serial.println(F("Initializing..."));
    delay(500); // wait for chips to stabilize

    //init pressure
    pressure_fuel.init_transducer();
    pressure_ox.init_transducer();
    pressure_fuel_injector.init_transducer();
    pressure_ox_injector.init_transducer();

    //init forces
    loadcell_1.init_loadcell();
    loadcell_2.init_loadcell();
    loadcell_3.init_loadcell();
    loadcell_4.init_loadcell();

    //thermocouples
    thermocouple_1.init_thermocouple();
    thermocouple_2.init_thermocouple();

    //init all valves
    valve_fuel_pre.init_valve();
    valve_fuel_main.init_valve();
    valve_ox_pre.init_valve();
    valve_ox_main.init_valve();
    valve_n2_choke.init_valve();
    valve_n2_drain.init_valve();

    // Set initial state
    init_autosequence();

    Serial.println(F("Setup Complete"));
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

   
    // Update sensor diagnostic message on LCD
    update_sensor_errors();

    // Run autonomous control
    run_control();

    // Send collected data
  
    BEGIN_SEND
        SEND_ITEM(force1, loadcell_1.m_current_force)
        SEND_ITEM(force2, loadcell_2.m_current_force)
        SEND_ITEM(force3, loadcell_3.m_current_force)
        SEND_ITEM(force4, loadcell_4.m_current_force)
        
        SEND_ITEM(outlet_temp, thermocouple_1.m_current_temp)
        SEND_ITEM(inlet_temp, thermocouple_2.m_current_temp)
        
        SEND_ITEM(fuel_press, pressure_fuel.m_current_pressure)
        SEND_ITEM(ox_press, pressure_ox.m_current_pressure)
        SEND_ITEM(fuel_inj_press, pressure_fuel_injector.m_current_pressure)
        SEND_ITEM(ox_inj_press, pressure_ox_injector.m_current_pressure)
        
    SEND_ITEM(sensor_status, sensor_status)
        END_SEND

        // Read a command
        bool valve_command;

    BEGIN_READ
        READ_FLAG(zero_force) {
        Serial.println(F("Zeroing load cells"));
        loadcell_1.zeroForces(); 
        loadcell_2.zeroForces(); 
        loadcell_3.zeroForces(); 
        loadcell_4.zeroForces(); 
    }

    READ_FLAG(zero_pressure) {
        if (pressure_fuel.m_zero_ready || pressure_ox.m_zero_ready) {
            Serial.println(F("Zeroing fuel pressure"));
            pressure_fuel.zeroPressures();
            pressure_fuel.m_zero_ready = false;
            Serial.println(F("Zeroing oxidizer pressure"));
            pressure_ox.zeroPressures();
            pressure_ox.m_zero_ready = false;
        }
        else {
            Serial.println(F("Pressure zero values not ready"));
        }
    }

    READ_FLAG(heartbeat) {
        heartbeat();
    }
    READ_FLAG(reset) {
        Serial.println(F("Resetting board"));
        reset();
    }
    READ_FLAG(start) {
        start_countdown();
    }
    READ_FLAG(stop) {
        Serial.println(F("Manual abort initiated"));
        abort_autosequence();
    }
    READ_FLAG(extend_servo) {
      servo_arm.extend();
    }
    READ_FLAG(fire_igniter) {
        fire_igniter();
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
        valve_n2_choke.set_valve(valve_command);
    }
    READ_FIELD(nitro_drain_command, "%d", valve_command) {
        valve_n2_drain.set_valve(valve_command);
    }
    READ_DEFAULT(data_name, data) {
        Serial.print(F("Invalid data field recieved: "));
        Serial.print(data_name);
        Serial.print(":");
        Serial.println(data);
    }
    END_READ
}
