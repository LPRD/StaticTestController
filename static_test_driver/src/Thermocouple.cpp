#include "Thermocouple.h"

Thermocouple::Thermocouple(int pin, const std::string& name, const std::string& shortname) {
    m_sensor_name = name;
    m_sensor_short_name = shortname;
    m_error = 0;
    m_current_temp = 0.0;
}

float Thermocouple::read_temp() {
    m_thermocouple.requestTemperatures();

    float result = m_thermocouple.getTempC(m_address);
    error_check(m_error, result > TEMP_MIN_VALID && result < TEMP_MAX_VALID, "Temp", m_sensor_name, m_sensor_short_name);
    return result;
}

void Thermocouple::init_thermocouple() 
{
    m_thermocouple.begin();

    if(!m_thermocouple.getAddress(m_address, 0))
    {
        Serial.print(m_sensor_name);
        Serial.println("Thermocouple cannot be found");
    }
    else{
        Serial.print(m_sensor_name);
        Serial.println(F(" theromocouple connected"));
        m_thermocouple.setResolution(9); //2 decimals 
    }
    delay(100);
}

void Thermocouple::updateTemps() {
    m_current_temp = read_temp();
  }
