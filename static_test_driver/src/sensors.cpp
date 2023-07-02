#include "defs.h"

String sensor_errors = "";

//Number of Sensors 
//Load Cells - 4
//Pressure Transducers - 4
//Thermocouples - n/a (not a priority)
//Accelerometer - probably 1 so no need to work on that too much.

//-------------------------------------------------------------------------------------------
//Utility Functions
//-------------------------------------------------------------------------------------------


void update_sensor_errors() {
  // set_lcd_errors(sensor_errors);
  //print sensor errors
  Serial.println(sensor_errors);
  sensor_errors = "";
}

void error_check(int &error, bool working, const String &sensor_type, const String &sensor_name="", const String &sensor_short_name="") {
  if (working) {
    error = 0;
  } else {
    if (sensor_errors.length()) { 
      sensor_errors += ',';
    }
    sensor_errors += sensor_type.substring(0, min(sensor_type.length(), 4)) + sensor_short_name;
    if (!error) { 
      Serial.print(sensor_name);    
      if (sensor_name.length()) {
        Serial.print(' ');
      }
      Serial.print(sensor_type);
      Serial.println(F(" sensor error"));
    }
    error++;
    if (error > SENSOR_ERROR_LIMIT) {     
      sensor_status = false; //static_test_driver
    }
  }
}

