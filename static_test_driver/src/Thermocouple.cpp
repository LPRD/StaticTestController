#include "Thermocouple.h"
#include <iostream>
#include <unistd.h>

Thermocouple::Thermocouple(int pin, const std::string& name, const std::string& shortname) {
    m_sensor_name = name;
    m_sensor_short_name = shortname;
    m_current_temp = 0.0;
}

float Thermocouple::read_temp() {
    m_thermocouple.requestTemperatures();

    float result = m_thermocouple.getTempC(m_address);
    error_check(result > TEMP_MIN_VALID && result < TEMP_MAX_VALID, "Temp", m_sensor_name, m_sensor_short_name);
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
