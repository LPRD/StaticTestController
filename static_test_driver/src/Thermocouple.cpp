#include "Thermocouple.h"
#include <iostream>
#include <unistd.h>

Thermocouple::Thermocouple(int pin, const std::string& name, const std::string& shortname, bool& sensors_ok, std::string& error_msg):
    Sensor(sensors_ok, error_msg, m_sensor_name, m_sensor_short_name),
    m_onewire((u_int8_t)pin),
    m_thermocouple(&m_onewire)
{
    m_current_temp = 0.0;
}

float Thermocouple::read_temp() {
    m_thermocouple.requestTemperatures();

    float result = m_thermocouple.getTempC(m_address);
    error_check(result > TEMP_MIN_VALID && result < TEMP_MAX_VALID, "Temp");
    return result;
}

void Thermocouple::init_thermocouple() 
{
    m_thermocouple.begin();

    if(!m_thermocouple.getAddress(m_address, 0))
    {
        std::cout << m_sensor_name << " cannot be found\n";
    }
    else{
        std::cout << m_sensor_name << " connected\n";
        m_thermocouple.setResolution(9); //2 decimals 
    }
    usleep(100000);
}

void Thermocouple::updateTemps() {
    m_current_temp = read_temp();
  }
