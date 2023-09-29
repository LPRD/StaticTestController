#ifndef SENSOR_H
#define SENSOR_H

#include <string>

#define SENSOR_ERROR_LIMIT 5 // Max number of errors in a row before deciding a sensor is faulty

class Sensor {
 protected:
    int num_errors = 0;
    bool& sensors_ok;
    std::string sensor_name;

    Sensor(bool&, std::string);
    void error_check(bool working);

};
#endif