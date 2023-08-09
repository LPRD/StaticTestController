#ifndef SENSOR_H
#define SENSOR_H

#include <string>

#define SENSOR_ERROR_LIMIT 5 // Max number of errors in a row before deciding a sensor is faulty

class Sensor {
 private:
    int num_errors = 0;
    bool& sensors_ok;
    std::string& error_msg;
    std::string sensor_name;
    std::string sensor_shortname;

 protected:
    Sensor(bool&, std::string&, std::string, std::string);
    void error_check(bool working, const std::string &sensor_type);

};
#endif