#include "PressureTransducer.h"

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
