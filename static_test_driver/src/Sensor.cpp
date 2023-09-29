#include "Sensor.h"
#include <iostream>


Sensor::Sensor(bool& sensors_ok, std::string sensor_name) : sensors_ok(sensors_ok) {
    this->sensor_name = sensor_name;
}

void Sensor::error_check(bool working) {
    if (working) {
        num_errors = 0;
    } else {
        if (num_errors > 0) { 
            std::cout << sensor_name <<" error\n";
        }
        num_errors++;
        if (num_errors > SENSOR_ERROR_LIMIT) {     
            sensors_ok = false;
        }
    }
}
