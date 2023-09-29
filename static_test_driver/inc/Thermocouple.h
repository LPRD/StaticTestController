#include "Sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <string>

#define TEMP_MIN_VALID -10
#define TEMP_MAX_VALID 120

class Thermocouple : public Sensor
{
  private:
    OneWire m_onewire;
    DallasTemperature m_thermocouple;
    DeviceAddress m_address;
  public:
    float m_current_temp;
  
    Thermocouple(int pin, const std::string& name, bool& sensors_ok);
  
    void init_thermocouple();

  float read_temp();

  void updateTemps();
};