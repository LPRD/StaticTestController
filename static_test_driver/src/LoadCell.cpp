#include "LoadCell.h"

LoadCell::LoadCell(u_int8_t dout, u_int8_t clk, double cal, const std::string& name, const std::string& shortname){
    m_dout = dout;
    m_clk = clk;
    m_calibrationFactor = cal;
    m_sensor_name = name;
    m_sensor_short_name = shortname;
    m_error = 0;
    m_current_force = 0;
}

void LoadCell::init_loadcell() {
    m_scale.begin(m_dout, m_clk);
    
    // Calibrate load cell
    m_scale.set_scale(m_calibrationFactor); // This value is obtained by using the SparkFun_HX711_Calibration sketch
    //Calibration is just the slope of the data function we get from the cell, we use it to find force at the values we get.
    m_scale.tare(); // Load Cell, Assuming there is no weight on the scale, reset to 0 mark intial valeue of cell as 0

    // Try reading a value from the load cell
    read_force();
    
    if (!m_error) {
        Serial.println(F("Load cell amp connected"));
    }
    delay(100);
}


float LoadCell::read_force() {
    // Wait for load cell data to become ready
    bool is_ready = false;
    for (unsigned int i = 0; i < LOAD_CELL_MAX_RETRIES; i++) {
    if (m_scale.is_ready()) {
        is_ready = true;
        break;
    }
    delay(LOAD_CELL_RETRY_INTERVAL);
    }

    // Read a value from the load cell
    float result = 0;
    if (is_ready) {
    result = m_scale.get_units();
    }
    error_check(m_error, is_ready && !isnan(result) && result > FORCE_MIN_VALID && result < FORCE_MAX_VALID,
    "Force", m_sensor_name, m_sensor_short_name);
    return result;
}

void LoadCell::updateForces() {
    m_current_force = read_force();
}

void LoadCell::zeroForces(){
    m_scale.tare();
}