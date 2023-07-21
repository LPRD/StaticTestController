// #include "defs.h"
#include "Sensor.h"


Sensor::Sensor(bool& sensors_ok, std::string& error_msg, std::string sensor_name, std::string sensor_shortname) :
        sensors_ok(sensors_ok), error_msg(error_msg) {
    this->sensor_name = sensor_name;
    this->sensor_shortname = sensor_shortname;
}

void Sensor::error_check(bool working, const std::string &sensor_type) {
    if (working) {
        num_errors = 0;
    } else {
        if (error_msg.length() != 0) { 
            error_msg += ',';
        }
        error_msg += sensor_type.substr(0, 4) + sensor_shortname;
        if (num_errors > 0) { 
            // Serial.print(sensor_name);    
            if (sensor_name.length()) {
                // Serial.print(' ');
            }
            // Serial.print(sensor_type);
            // Serial.println(F(" sensor error"));
        }
        num_errors++;
        if (num_errors > SENSOR_ERROR_LIMIT) {     
            sensors_ok = false; //static_test_driver
        }
    }
}
