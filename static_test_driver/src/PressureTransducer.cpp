#include "PressureTransducer.h"
#include <fcntl.h>
#include <unistd.h>

float mean(const float *data, unsigned int size) {
  float result = 0;
  for (unsigned int i = 0; i < size; i++)
      result += data[i];
  return result / size;
}

int analogRead(int pin) {
    // Check if pin is valid
    if (pin < 0 || pin > 7) {
        fprintf(stderr, "Analog pin out of range\n");
        return -1;
    }

    // Build filename
    char filename[256];
    sprintf(filename, "/sys/devices/platform/ocp/44c00000.interconnect/44c00000.interconnect:segment@200000/44e0d000.target-module/44e0d000.tscadc/TI-am335x-adc.0.auto/iio:device0/in_voltage%d_raw", pin);
    
    // Open file
    int fd = open(filename, O_EXCL);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // Read file
    int buffer;
    if (read(fd, &buffer, 4) == -1) {
        perror("read");
        return -1;
    }

    if (close(fd) == -1) {
        perror("close");
        return -1;
    }
    
    return buffer;
}

PressureTransducer::PressureTransducer(int pin, std::string name, bool& sensors_ok) :
        Sensor(sensors_ok, name) {
    m_pressurepin = pin;
    m_tare = 0;
    m_current_hist_val = 0;
    m_zero_ready = 0;
}


float PressureTransducer::read_pressure() {
    // /sys/devices/platform/ocp/44e0d000.tscadc/TI-am335x-adc.0.auto/iio:device0/in_voltage0_raw
    float result = (analogRead(m_pressurepin) * 1.8 / 4096.0);// * PRESSURE_CALIBRATION_FACTOR - PRESSURE_OFFSET;
    error_check(result > PRESSURE_MIN_VALID && result < PRESSURE_MAX_VALID);
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
