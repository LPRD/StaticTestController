#ifndef VALVE_H
#define VALVE_H

#include <string>

class Valve
{
 private:
    std::string m_valvename;
    std::string m_telemetry_id;

    struct gpiod_chip *chip;
    struct gpiod_line *line;

 public:
    bool m_current_state;

    Valve(int pin, const std::string& name, const std::string& telemetry);

    ~Valve();

    void set_valve(bool setting);
};

#endif