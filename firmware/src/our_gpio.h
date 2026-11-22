#ifndef _OUR_GPIO_H_
#define _OUR_GPIO_H_

#include <cstdint>

// GPIO pins as mapping inputs and outputs. Which pins can be used comes from
// get_gpio_valid_pins_mask() (platform.h), which each variant provides.

extern bool set_gpio_dir_pending;

void gpio_pins_init();
void set_gpio_dir();
bool read_gpio(uint64_t now);
void write_gpio();

#endif
