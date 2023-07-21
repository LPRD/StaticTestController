// #include "defs.h"
#include "Sensor.h"

    void Sensor::error_check(int &error, bool working, const String &sensor_type, const String &sensor_name="", const String &sensor_short_name="") {
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
