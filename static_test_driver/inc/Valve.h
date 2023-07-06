#include <string>

class Valve
{
 private:
    std::string m_valvename;
    std::string m_telemetry_id;
    bool m_current_state;

    struct gpiod_chip *chip;
    struct gpiod_line *line;

 public:
    Valve(int pin, const std::string& name, const std::string& telemetry);

    ~Valve();

    void set_valve(bool setting);
};