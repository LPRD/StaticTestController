#include "PressureTransducer.h"

float mean(const float *data, unsigned int size) {
  float result = 0;
  for (unsigned int i = 0; i < size; i++)
      result += data[i];
  return result / size;
}

PressureTransducer::PressureTransducer(int pin, const std::string& name, const std::string& shortname) {
    m_pressurepin = pin;
    m_sensor_name = name;
    m_sensor_short_name = name;
    m_error = 0;
    m_tare = 0;
    m_pressure_history = {0};
    m_current_hist_val = 0;
    m_zero_ready = 0;
}

void PressureTransducer::init_transducer()
{
    pinMode(m_pressurepin, INPUT);
}

float PressureTransducer::read_pressure() {
    float result = (analogRead(m_pressurepin) * 5 / 1024.0);// * PRESSURE_CALIBRATION_FACTOR - PRESSURE_OFFSET;
    error_check(m_error, result > PRESSURE_MIN_VALID && result < PRESSURE_MAX_VALID, m_sensor_name, "pressure");
    return result;
}

void PressureTransducer::updatePressures()
{
    m_current_pressure = read_pressure();

    if(m_current_hist_val >= PRESSURE_NUM_HIST_VALS)
    {
        m_current_hist_val = 0;
        m_zero_ready = true;
    }
    
    m_pressure_history[m_current_hist_val] = m_current_pressure;
    m_current_pressure -= m_tare;
    m_current_hist_val++;
}

void PressureTransducer::zeroPressures(){
    m_tare = mean(&m_pressure_history[0], PRESSURE_NUM_HIST_VALS);
}
