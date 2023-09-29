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
    int m_pressurepin;

    float m_tare;

 public:
     // TODO: Could these be private?
     float m_current_pressure;    
     bool m_zero_ready;

    PressureTransducer(int pin, std::string name, bool& sensors_ok);

    float read_pressure();

    void updatePressures();

    void zeroPressures();
};
