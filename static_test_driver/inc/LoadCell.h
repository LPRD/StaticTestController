#include <string>
#include <HX711.h>

#define LOAD_CELL_RETRY_INTERVAL 10
#define LOAD_CELL_MAX_RETRIES 20

#define FORCE_MIN_VALID -50
#define FORCE_MAX_VALID 500

class LoadCell
{
 private:
    HX711 m_scale;
    u_int8_t m_dout; //Digital out pin
    u_int8_t m_clk;  //Clock pin
    double m_calibrationFactor;
    const std::string m_sensor_name;
    const std::string m_sensor_short_name;
    float m_current_force;

 public:
    LoadCell(u_int8_t dout, u_int8_t clk, double cal, const std::string& name, const std::string& shortname);

    void init_loadcell();

    float read_force();

    void updateForces();

    void zeroForces();
};