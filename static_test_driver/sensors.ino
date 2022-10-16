
#define SENSOR_ERROR_LIMIT 5 // Max number of errors in a row before deciding a sensor is faulty

#define PRESSURE_CALIBRATION_FACTOR 246.58
#define PRESSURE_OFFSET 118.33
#define PRESSURE_MIN_VALID -100
#define PRESSURE_MAX_VALID 1000

#define TEMP_MIN_VALID -10
#define TEMP_MAX_VALID 120

#define FORCE_MIN_VALID -50
#define FORCE_MAX_VALID 500

#if CONFIGURATION == MK_2
#define LOAD_CELL_CALIBRATION_FACTOR 20400.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#else
#define LOAD_CELL_CALIBRATION_FACTOR 20400
#endif

#define LOAD_CELL_RETRY_INTERVAL 10
#define LOAD_CELL_MAX_RETRIES 20

//Error Checking function revisions.
#define LOAD_CELL_IDENTITY 100
#define THERMOCOUPLE_IDENTITY 200
#define PRESSURE_TRANSDUCER_IDENTITY 300


String sensor_errors = "";

//Number of Sensors 
//Load Cells - 4
//Pressure Transducers - 4
//Thermocouples - n/a (not a priority)
//Accelerometer - probably 1 so no need to work on that too much.

//-------------------------------------------------------------------------------------------
//Utility Functions
//-------------------------------------------------------------------------------------------

float mean(const float *data, unsigned int size) {
  float result = 0;
  for (int i = 0; i < size; i++)
      result += data[i];
  return result / size;
}

void update_sensor_errors() {
  set_lcd_errors(sensor_errors);
  sensor_errors = "";
}

void error_check(int &error, bool working, const String &sensor_type, const String &sensor_name="", const String &sensor_short_name="") {
  if (working) {
    error = 0;
  } else {
    if (sensor_errors.length()) {
      sensor_errors += ',';
    }
    sensor_errors += sensor_type.substring(0, min(sensor_type.length(), 2)) + sensor_short_name;
    if (!error) { //if its not working and error = 0 (so like first error in the run through) then we do this 
      Serial.print(sensor_name);
      if (sensor_name.length()) {
        Serial.print(' ');
      }
      Serial.print(sensor_type);
      Serial.println(F(" sensor error"));
    }
    error++;
    if (error > SENSOR_ERROR_LIMIT) {
      sensor_status = false; //static_test_driver
    }
  }
}


//-------------------------------------------------------------------------------------------
//LoadCell 
//-------------------------------------------------------------------------------------------

//SENSOR DEVICE 1 

class LoadCell
{
  private:
  HX711 m_scale;

  public:
  uint8_t m_dout; //Digital out pin
  uint8_t m_clk;  //Clock pin
  double m_calibrationFactor;

  int m_error;

  LoadCell(){}
  LoadCell(uint8_t dout, uint8_t clk) : m_calibrationFactor{LOAD_CELL_CALIBRATION_FACTOR} , m_dout {dout} , m_clk {clk}, m_error{0} {}

  float read_force();
  void init_force();
};

void LoadCell::init_force() {
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
  error_check(m_error, is_ready && !isnan(result) && result > FORCE_MIN_VALID && result < FORCE_MAX_VALID, "Force");
  return result;
}


//-------------------------------------------------------------------------------------------
//Thermocouple 
//-------------------------------------------------------------------------------------------

//SENSOR DEVICE 2


class Thermocouple 
{
  private:
  Adafruit_MAX31855 m_thermocouple;
  public:
  int m_thermocouplepin;
  int m_error;
  const String m_sensor_name;
  const String m_sensor_short_name;

  Thermocouple(int8_t pin, const String& name, const String& shortname) : m_thermocouplepin {pin}, m_sensor_name { name } , m_sensor_short_name { shortname }, m_error{0} , m_thermocouple{pin} {} 
  
  void init_thermocouple();
  float read_thermocouple();
  float read_temp();

};

float Thermocouple::read_temp() {
  float result = analogRead(m_thermocouplepin) * 5.0 * 100 / 1024;
  error_check(m_error, result > TEMP_MIN_VALID && result < TEMP_MAX_VALID, "temp", m_sensor_name, m_sensor_short_name);
  return result;
}

void Thermocouple::init_thermocouple() {
  int error = 0;
  m_thermocouple.begin();
  read_thermocouple();
  if (!m_error) {
    Serial.print(m_sensor_name);
    Serial.println(F(" theromocouple connected"));
  }
  delay(100);
}

float Thermocouple::read_thermocouple() {
  float result = m_thermocouple.readCelsius();
  error_check(m_error, !isnan(result) && result > 0, "thermocouple", m_sensor_name, m_sensor_short_name);
  return result;
}



//-------------------------------------------------------------------------------------------
//Pressure Transducers
//-------------------------------------------------------------------------------------------

//Sensor Device 3 

class PressureTransducer
{
  public:
  int m_pressurepin;
  int m_error;
  const String m_sensor_name;
  const String m_sensor_short_name;
  PressureTransducer(int pin, const String& name, const String& shortname) : m_pressurepin{pin}, m_sensor_name{name}, m_sensor_short_name{shortname} , m_error{0} {} 

  float read_pressure(int sensor, int &error, const String &sensor_name, const String &sensor_short_name);
  
};

float PressureTransducer::read_pressure(int sensor, int &error, const String &sensor_name, const String &sensor_short_name) {
  float result = (analogRead(sensor) * 5 / 1024.0) * PRESSURE_CALIBRATION_FACTOR - PRESSURE_OFFSET;
  error_check(error, result > PRESSURE_MIN_VALID && result < PRESSURE_MAX_VALID, sensor_name, "pressure");
  return result;
};

//-------------------------------------------------------------------------------------------
//Accelerometer
//-------------------------------------------------------------------------------------------

void init_accelerometer(Adafruit_MMA8451 &mma) {
  bool working = false;
  if (mma.begin()) {
    mma.setRange(MMA8451_RANGE_2_G);  // set acc range (2 4 8)
    Serial.print(F("Accelerometer range ")); 
    Serial.print(2 << mma.getRange()); 
    Serial.println("G");
    working = true;
  }
  int error = 0;
  error_check(error, working, "accelerometer");
  delay(100);
}

sensors_vec_t read_accelerometer(Adafruit_MMA8451 &mma, int &error) {
  sensors_event_t event;
  mma.getEvent(&event);
  sensors_vec_t accel = event.acceleration;
  error_check(error, !(mma.x == -1 && mma.y == -1 && mma.z == -1), "accelerometer");
  return accel;
}
