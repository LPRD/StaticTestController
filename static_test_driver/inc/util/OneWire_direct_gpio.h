#ifndef OneWire_Direct_GPIO_h
#define OneWire_Direct_GPIO_h

// This header should ONLY be included by OneWire.cpp.  These defines are
// meant to be private, used within OneWire.cpp, but not exposed to Arduino
// sketches or other libraries which may include OneWire.h.

#include <stdint.h>
#include <gpiod.h>
#include <stdio.h>

int digitalRead(int pin) {
    struct gpiod_chip* chip = gpiod_chip_open_by_number(pin / 32);
	if (!chip){ perror("Open chip failed\n"); }
	
	struct gpiod_line* line = gpiod_chip_get_line(chip, pin % 32);
	if (!pin) {	perror("Get line failed\n"); }

    int ret = gpiod_line_request_input(line, "OW");
	if (ret < 0) { perror("Request line as intput failed\n"); }

    int value = gpiod_line_get_value(line);
    if (value == -1) { perror("Line read value failed"); }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return value;
}
int digitalWrite(int pin, int value) {
    struct gpiod_chip* chip = gpiod_chip_open_by_number(pin / 32);
	if (!chip){ perror("Open chip failed\n"); }
	
	struct gpiod_line* line = gpiod_chip_get_line(chip, pin % 32);
	if (!pin) {	perror("Get line failed\n"); }

    int ret = gpiod_line_request_output(line, "OW", value);
	if (ret < 0) { perror("Request line as output failed\n"); }

    ret = gpiod_line_set_value(line, value);
    if (ret == -1) { perror("Line set value failed"); }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}

int pinMode(int pin, int direction) {
    struct gpiod_chip* chip = gpiod_chip_open_by_number(pin / 32);
	if (!chip){ perror("Open chip failed\n"); }
	
	struct gpiod_line* line = gpiod_chip_get_line(chip, pin % 32);
	if (!pin) {	perror("Get line failed\n"); }

    if (direction == 0) {
        int ret = gpiod_line_request_input(line, "OW");
        if (ret < 0) { perror("Request line as intput failed\n"); }
    } else {
        int ret = gpiod_line_request_output(line, "OW", 0);
	    if (ret < 0) { perror("Request line as output failed\n"); }
    }


    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}


#define PIN_TO_BASEREG(pin)             (0)
#define PIN_TO_BITMASK(pin)             (pin)
#define IO_REG_TYPE unsigned int
#define IO_REG_BASE_ATTR
#define IO_REG_MASK_ATTR
#define DIRECT_READ(base, pin)          digitalRead(pin)
#define DIRECT_WRITE_LOW(base, pin)     digitalWrite(pin, 0)
#define DIRECT_WRITE_HIGH(base, pin)    digitalWrite(pin, 1)
#define DIRECT_MODE_INPUT(base, pin)    pinMode(pin,0)
#define DIRECT_MODE_OUTPUT(base, pin)   pinMode(pin,1)

#define pgm_read_byte(addr) (*(const uint8_t *)(addr))


#endif
