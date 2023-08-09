#include <string>
#include <HX711.h>
#include "Sensor.h"

#define LOAD_CELL_RETRY_INTERVAL 10
#define LOAD_CELL_MAX_RETRIES 20

#define FORCE_MIN_VALID -50
#define FORCE_MAX_VALID 500

class LoadCell : public Sensor
{
 private:
    HX711 m_scale;
    u_int8_t m_dout; //Digital out pin
    u_int8_t m_clk;  //Clock pin
    double m_calibrationFactor;

 public:
    float m_current_force;

    LoadCell(u_int8_t dout, u_int8_t clk, double cal, int num, bool& sensors_ok, std::string& error_msg);

    void init_loadcell();

    float read_force();

    void updateForces();

    void zeroForces();
};