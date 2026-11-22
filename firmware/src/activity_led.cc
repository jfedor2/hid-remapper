#include "activity_led.h"

#include <hardware/gpio.h>
#include <hardware/timer.h>

// Flashes PICO_DEFAULT_LED_PIN when there's input. board_init() sets the pin up.

#ifndef PICO_DEFAULT_LED_PIN_INVERTED
#define PICO_DEFAULT_LED_PIN_INVERTED 0
#endif

static bool led_state = false;
static uint64_t turn_led_off_after = 0;

static void led_write(bool on) {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_put(PICO_DEFAULT_LED_PIN, on != PICO_DEFAULT_LED_PIN_INVERTED);
#endif
}

void activity_led_on() {
    led_state = true;
    led_write(true);
    turn_led_off_after = time_us_64() + 50000;
}

void activity_led_off_maybe() {
    if (led_state && (time_us_64() > turn_led_off_after)) {
        led_state = false;
        led_write(false);
    }
}
