#include "Valve.h"
#include <gpiod.h>
#include <sys/time.h>


unsigned int millis ();
// {
//   struct timeval t ;
//   gettimeofday ( & t , NULL ) ;
//   return t.tv_sec * 1000 + ( t.tv_usec + 500 ) / 1000 ;
// }

Valve::Valve(int pin, const std::string& name, const std::string& telemetry) {
    m_valvename = name;
    m_telemetry_id = telemetry;
    m_current_state = false;

    // Open the chips for the valve pin.
	chip = gpiod_chip_open_by_number(pin / 32);
	if (!chip){
		perror("Open chip failed\n");
	}

	// Open lines for the clock and the data pins
	line = gpiod_chip_get_line(chip, pin % 32);
	if (!line) {
		perror("Get line failed\n");
	}

	//Set the Valve line to be output with a low default voltage
	int ret = gpiod_line_request_output(line, "HX711", 0);
	if (ret < 0) {
		perror("Request line as output failed\n");
	}
}

Valve::~Valve() {
	gpiod_line_release(line);
	
	gpiod_chip_close(chip);
}


// valve setting function
void Valve::set_valve(bool setting) {
    m_current_state = setting;
    printf("%s to %s\n", m_valvename.c_str(), m_current_state? "open" : "closed");
	//TODO: Move this call out
    // SEND_NAME(m_telemetry_id.c_str(), m_current_state, "%d");

    gpiod_line_set_value(line, m_current_state);
}
