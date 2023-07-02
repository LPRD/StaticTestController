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

  public:
  int m_error;

  float m_current_force;

  LoadCell(){}
  LoadCell(u_int8_t dout, u_int8_t clk, double cal, const std::string& name, const std::string& shortname):
   m_calibrationFactor{cal},
   m_dout {dout},
   m_clk {clk},
   m_error{0},
   m_current_force{0},
   m_sensor_name ( name ),
   m_sensor_short_name ( shortname ) {}

  float read_force();
  void init_loadcell();

  void updateForces() {
    m_current_force = read_force();
  }

  void zeroForces(){
    m_scale.tare();
  }
};