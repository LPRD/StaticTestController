#include <iostream>
#include "gpiod.h"
#include "Igniter.h"

Igniter::Igniter(int pin){
    // ignition_time = 0.0;

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

	//Set the Igniter line to be output with a low default voltage
	int ret = gpiod_line_request_output(line, "HX711", 0);
	if (ret < 0) {
		perror("Request line as output failed\n");
	}
}

Igniter::~Igniter() {
	gpiod_line_release(line);
	
	gpiod_chip_close(chip);
}

void Igniter::fire_igniter() {
    std::cout << "Firing igniter\n";
    gpiod_line_set_value(line, 1);
    // ignition_time = millis();
}

void Igniter::reset_igniter() {
    std::cout << "Reset igniter\n";
    gpiod_line_set_value(line, 0);
    // ignition_time = 0;
}

// void Igniter::handle_igniter() {
//     if (ignition_time != 0 /* && millis() > ignition_time + IGNITER_DURATION */) {
//         reset_igniter();
//     }
// }
