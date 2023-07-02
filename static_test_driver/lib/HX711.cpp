/**
 *
 * HX711 library for Arduino
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 * 
 * Modified by Austin Korpi to work with gpiod instead of Arduino
 *
**/

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <gpiod.h>

#include "HX711.h"



unsigned int millis () {
  struct timeval t ;
  gettimeofday ( & t , NULL ) ;
  return t.tv_sec * 1000 + ( t.tv_usec + 500 ) / 1000 ;
}

// Make shiftIn() be aware of clockspeed for
// faster CPUs like ESP32, Teensy 3.x and friends.
// See also:
// - https://github.com/bogde/HX711/issues/75
// - https://github.com/arduino/Arduino/issues/6561
// - https://community.hiveeyes.org/t/using-bogdans-canonical-hx711-library-on-the-esp32/539
u_int8_t shiftIn(struct gpiod_line *dataPin, struct gpiod_line *clockPin, u_int8_t bitOrder) {
    u_int8_t value = 0;
    u_int8_t i;

    for(i = 0; i < 8; ++i) {
    	gpiod_line_set_value(clockPin, 1);

        if(bitOrder == 0) 
            value |= gpiod_line_get_value(dataPin) << i;
        else
            value |= gpiod_line_get_value(dataPin) << (7 - i);
        
    	gpiod_line_set_value(clockPin, 0);
    }
    return value;
}


HX711::HX711() {
}

HX711::~HX711() {
	gpiod_line_release(DOUT);
	gpiod_line_release(PD_SCK);
	
	gpiod_chip_close(chipDat);
	gpiod_chip_close(chipClk);
}

void HX711::begin(int dout, int pd_sck, int gain) {
	// Open the chips for the clock pin and the data pin.
	// They may or may not use the same chip, so we just open twice.
	chipClk = gpiod_chip_open_by_number(dout / 32);
	if (!chipClk){
		perror("Open chip failed\n");
	}
	chipDat = gpiod_chip_open_by_number(pd_sck / 32);
	if (!chipDat){
		perror("Open chip failed\n");
	}
	
	// Open lines for the clock and the data pins
	PD_SCK = gpiod_chip_get_line(chipClk, pd_sck % 32);
	if (!PD_SCK) {
		perror("Get line failed\n");
	}
	DOUT = gpiod_chip_get_line(chipDat, dout % 32);
	if (!DOUT) {
		perror("Get line failed\n");
	}

	//Set the Clock line to be output with a low default voltage
	int ret = gpiod_line_request_output(PD_SCK, "HX711", 0);
	if (ret < 0) {
		perror("Request line as output failed\n");
	}
	// Set the Data line to be input
	ret = gpiod_line_request_input(DOUT, "HX711");
	if (ret < 0) {
		perror("Request line as intput failed\n");
	}
	
	set_gain(gain);
}

bool HX711::is_ready() {
	return gpiod_line_get_value(DOUT) == 0;
}

void HX711::set_gain(int gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

}

long HX711::read() {
	// Wait for the chip to become ready.
	wait_ready();

	// Define structures for reading data into.
	unsigned long value = 0;
	u_int8_t data[3] = { 0 };
	u_int8_t filler = 0x00;

	// Disable interrupts. (Can't stop schedule though)
	sigset_t oldSet, newSet;
	sigfillset(&newSet);
	sigprocmask(SIG_BLOCK, &newSet, &oldSet);


	// Pulse the clock pin 24 times to read the data.
	data[2] = shiftIn(DOUT, PD_SCK, 1);
	data[1] = shiftIn(DOUT, PD_SCK, 1);
	data[0] = shiftIn(DOUT, PD_SCK, 1);

	// Set the channel and the gain factor for the next reading using the clock pin.
	for (int i = 0; i < GAIN; i++) {
		gpiod_line_set_value(PD_SCK, 1);

		gpiod_line_set_value(PD_SCK, 0);
	}

	// Enable interrupts again.
	sigprocmask(SIG_BLOCK, &oldSet, NULL);


	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);
}

void HX711::wait_ready(unsigned long delay_ms) {
	// Wait for the chip to become ready.
	// This is a blocking implementation and will
	// halt the sketch until a load cell is connected.
	while (!is_ready()) {
		// Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
		// https://github.com/bogde/HX711/issues/73
		usleep(delay_ms*1000);
	}
}

bool HX711::wait_ready_retry(int retries, unsigned long delay_ms) {
	// Wait for the chip to become ready by
	// retrying for a specified amount of attempts.
	// https://github.com/bogde/HX711/issues/76
	int count = 0;
	while (count < retries) {
		if (is_ready()) {
			return true;
		}
		usleep(delay_ms*1000);
		count++;
	}
	return false;
}

bool HX711::wait_ready_timeout(unsigned long timeout, unsigned long delay_ms) {
	// Wait for the chip to become ready until timeout.
	// https://github.com/bogde/HX711/pull/96
	unsigned long millisStarted = millis();
	while (millis() - millisStarted < timeout) {
		if (is_ready()) {
			return true;
		}
		usleep(delay_ms*1000);
	}
	return false;
}

long HX711::read_average(int times) {
	long sum = 0;
	for (int i = 0; i < times; i++) {
		sum += read();
		// Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
		// https://github.com/bogde/HX711/issues/73
		usleep(0);
	}
	return sum / times;
}

double HX711::get_value(int times) {
	return read_average(times) - OFFSET;
}

float HX711::get_units(int times) {
	return get_value(times) / SCALE;
}

void HX711::tare(int times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale) {
	SCALE = scale;
}

float HX711::get_scale() {
	return SCALE;
}

void HX711::set_offset(long offset) {
	OFFSET = offset;
}

long HX711::get_offset() {
	return OFFSET;
}

void HX711::power_down() {
	gpiod_line_set_value(PD_SCK, 0);
	gpiod_line_set_value(PD_SCK, 1);
}

void HX711::power_up() {
	gpiod_line_set_value(PD_SCK, 0);
}
