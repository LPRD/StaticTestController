#include <string>

class Valve
{
 private:
    int m_valvepin;
    std::string m_valvename;
    std::string m_telemetry_id;
    bool m_current_state;

 public:
    Valve(int pin, const std::string& name, const std::string& telemetry) {};

    void init_valve();

    void set_valve(bool setting);
};