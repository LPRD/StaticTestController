#include <DallasTemperature.h>
#include <OneWire.h>
#include <string>


class Thermocouple 
{
  private:
    DallasTemperature m_thermocouple;
    DeviceAddress m_address;
    OneWire m_onewire;
  public:
    int m_error;
    const std::string m_sensor_name;
    const std::string m_sensor_short_name;

    float m_current_temp;
  
    Thermocouple(int pin, const std::string& name, const std::string& shortname) :
        m_sensor_name ( name ),
        m_sensor_short_name ( shortname ),
        m_error(0) ,
        m_current_temp(0.0),
        m_onewire((u_int8_t)pin),
        m_thermocouple(&m_onewire) {}
  
  
  void init_thermocouple();
  float read_temp();

  void updateTemps() {
    m_current_temp = read_temp();
  }

};