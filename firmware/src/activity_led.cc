#include "activity_led.h"

#include <bsp/board.h>

#include <hardware/timer.h>

static bool led_state = false;
static uint64_t turn_led_off_after = 0;

void activity_led_on() {
    led_state = true;
    board_led_write(true);
    turn_led_off_after = time_us_64() + 50000;
}

void activity_led_off_maybe() {
    if (led_state && (time_us_64() > turn_led_off_after)) {
        led_state = false;
        board_led_write(false);
    }
}
