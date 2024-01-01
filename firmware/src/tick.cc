#include "tick.h"

#include <pico/critical_section.h>
#include <pico/platform.h>

static critical_section_t crit_sec;

static volatile bool tick_pending = false;

void tick_init() {
    critical_section_init(&crit_sec);
}

void __no_inline_not_in_flash_func(set_tick_pending)() {
    critical_section_enter_blocking(&crit_sec);
    tick_pending = true;
    critical_section_exit(&crit_sec);
}

bool get_and_clear_tick_pending() {
    critical_section_enter_blocking(&crit_sec);
    uint8_t tmp = tick_pending;
    tick_pending = false;
    critical_section_exit(&crit_sec);
    return tmp;
}
