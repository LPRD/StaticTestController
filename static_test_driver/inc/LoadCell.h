#include <string>
#include <HX711.h>

class LoadCell
{
 private:
    HX711 m_scale;
    u_int8_t m_dout; //Digital out pin
    u_int8_t m_clk;  //Clock pin
    double m_calibrationFactor;
    const std::string m_sensor_name;
    const std::string m_sensor_short_name;
    int m_error;
    float m_current_force;

 public:
    LoadCell(u_int8_t dout, u_int8_t clk, double cal, const std::string& name, const std::string& shortname);

    void init_loadcell();

    float read_force();

    void updateForces();

    void zeroForces();
};