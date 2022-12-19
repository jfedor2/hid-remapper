#include "interval_override.h"
#include "pio_usb_stuff.h"
#include "remapper.h"

void extra_init() {
    launch_pio_usb();
}

bool read_report() {
    return pio_usb_task(handle_received_report);
}

void interval_override_updated() {
}

void flash_b_side() {
}
