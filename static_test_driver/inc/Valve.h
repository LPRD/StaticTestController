#include <string>

class Valve
{
 private:
    u_int8_t m_valvepin;
    std::string m_valvename;
    std::string m_telemetry_id;
    bool m_current_state;

 public:
    Valve(u_int8_t pin, const std::string& name, const std::string& telemetry):
        m_valvepin{pin},
        m_valvename{name},
        m_telemetry_id{telemetry},
        m_current_state{false}
        {}

    void init_valve();

    void set_valve(bool setting);
};