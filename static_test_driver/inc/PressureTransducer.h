#include <string>

#define PRESSURE_NUM_HIST_VALS 10

class PressureTransducer
{
 private:
    int m_current_hist_val;
    float m_pressure_history[PRESSURE_NUM_HIST_VALS];
    bool m_zero_ready;
    int m_pressurepin;
    int m_error;
    const std::string m_sensor_name;
    const std::string m_sensor_short_name;
    float m_tare;
    float m_current_pressure;

 public:
    PressureTransducer(int pin, const std::string& name, const std::string& shortname) {};

    void init_transducer();

    float read_pressure();

    void updatePressures();

    void zeroPressures();
};
