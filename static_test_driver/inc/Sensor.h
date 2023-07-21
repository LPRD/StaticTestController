#include <string>

#define SENSOR_ERROR_LIMIT 5 // Max number of errors in a row before deciding a sensor is faulty

class Sensor {
 private:
    bool& sensor_status;
    String& sensor_errors;

 protected:
    void error_check(int &error, bool working, const String &sensor_type, const String &sensor_name="", const String &sensor_short_name="");
};
