#include "Thermocouple.h"
#include <iostream>
#include <unistd.h>

Thermocouple::Thermocouple(int pin, const std::string& name, bool& sensors_ok):
    Sensor(sensors_ok, name),
    m_onewire((u_int8_t)pin),
    m_thermocouple(&m_onewire)
{
    m_current_temp = 0.0;
}

float Thermocouple::read_temp() {
    m_thermocouple.requestTemperatures();

    float result = m_thermocouple.getTempC(m_address);
    error_check(result > TEMP_MIN_VALID && result < TEMP_MAX_VALID);
    return result;
}

void Thermocouple::init_thermocouple() 
{
    m_thermocouple.begin();

    if(!m_thermocouple.getAddress(m_address, 0))
    {
        std::cout << sensor_name << " cannot be found\n";
    }
    else{
        std::cout << sensor_name << " connected\n";
        m_thermocouple.setResolution(9); //2 decimals 
    }
    usleep(100000);
}

void Thermocouple::updateTemps() {
    m_current_temp = read_temp();
}
