#include <string>

class PressureTransducer
{
  private:
  int m_current_hist_val;
  float m_pressure_history[PRESSURE_NUM_HIST_VALS];
  
  public:
  bool m_zero_ready;
  int m_pressurepin;
  int m_error;
  const std::string m_sensor_name;
  const std::string m_sensor_short_name;

  float m_tare;

  float m_current_pressure;
  
    PressureTransducer(int pin, const std::string& name, const std::string& shortname):
        m_pressurepin{pin},
        m_sensor_name{name},
        m_sensor_short_name{shortname},
        m_error{0},
        m_tare{0},
        m_pressure_history{},
        m_current_hist_val{0},
        m_zero_ready{false} {} 

  void init_transducer();
  float read_pressure();

  void updatePressures();

  void zeroPressures(){
      m_tare = mean(&m_pressure_history[0], PRESSURE_NUM_HIST_VALS);
  }
  
};
