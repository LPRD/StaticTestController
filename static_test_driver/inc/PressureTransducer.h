#include <string>
#include "Sensor.h"

#define PRESSURE_NUM_HIST_VALS 10
#define PRESSURE_MIN_VALID -100
#define PRESSURE_MAX_VALID 1000

class PressureTransducer : public Sensor
{
 private:
    int m_current_hist_val;
    float m_pressure_history[PRESSURE_NUM_HIST_VALS];
    bool m_zero_ready;
    int m_pressurepin;

    float m_tare;
    float m_current_pressure;

 public:
    PressureTransducer(int pin, std::string name, std::string shortname, bool& sensors_ok, std::string&error_msg);

    void init_transducer();

    float read_pressure();

    void updatePressures();

    void zeroPressures();
};
