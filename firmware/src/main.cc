#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <bsp/board.h>
#include <tusb.h>

#include <hardware/flash.h>
#include <pico/bootrom.h>
#include <pico/mutex.h>
#include <pico/stdio.h>

#include "config.h"
#include "crc.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

#define CONFIG_OFFSET_IN_FLASH (PICO_FLASH_SIZE_BYTES - PERSISTED_CONFIG_SIZE)
#define FLASH_CONFIG_IN_MEMORY (((uint8_t*) XIP_BASE) + CONFIG_OFFSET_IN_FLASH)

// We need a certain part of mapping processing (absolute->relative mappings) to
// happen exactly once per millisecond. This variable keeps track of whether we
// already did it this time around. It is set to true when we receive
// start-of-frame from USB host.
volatile bool tick_pending;

uint64_t next_print = 0;

mutex_t their_usages_mutex;
mutex_t macros_mutex;

void print_stats_maybe() {
    uint64_t now = time_us_64();
    if (now > next_print) {
        print_stats();
        while (next_print < now) {
            next_print += 1000000;
        }
    }
}

inline bool get_and_clear_tick_pending() {
    // atomicity not critical
    uint8_t tmp = tick_pending;
    tick_pending = false;
    return tmp;
}

void sof_handler(uint32_t frame_count) {
    tick_pending = true;
}

bool do_send_report(const uint8_t* report_with_id, uint8_t len) {
    tud_hid_n_report(0, report_with_id[0], report_with_id + 1, len - 1);
    return true;  // XXX?
}

void do_persist_config(uint8_t* buffer) {
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(CONFIG_OFFSET_IN_FLASH, PERSISTED_CONFIG_SIZE);
    flash_range_program(CONFIG_OFFSET_IN_FLASH, buffer, PERSISTED_CONFIG_SIZE);
    restore_interrupts(ints);
}

void reset_to_bootloader() {
    reset_usb_boot(0, 0);
}

void pair_new_device() {
}

void clear_bonds() {
}

void usages_mutex_init() {
    mutex_init(&their_usages_mutex);
}

void usages_mutex_enter() {
    mutex_enter_blocking(&their_usages_mutex);
}

void usages_mutex_exit() {
    mutex_exit(&their_usages_mutex);
}

void macros_mutex_init() {
    mutex_init(&macros_mutex);
}

void macros_mutex_enter() {
    mutex_enter_blocking(&macros_mutex);
}

void macros_mutex_exit() {
    mutex_exit(&macros_mutex);
}

uint64_t get_time() {
    return time_us_64();
}

int main() {
    usages_mutex_init();
    macros_mutex_init();
    extra_init();
    parse_our_descriptor();
    load_config(FLASH_CONFIG_IN_MEMORY);
    set_mapping_from_config();
    board_init();
    tusb_init();
    stdio_init_all();

    tud_sof_isr_set(sof_handler);

    next_print = time_us_64() + 1000000;

    bool led_state = false;
    uint64_t turn_led_off_after = 0;

    while (true) {
        if (read_report()) {
            led_state = true;
            board_led_write(true);
            turn_led_off_after = time_us_64() + 50000;
            process_mapping(get_and_clear_tick_pending());
        }
        tud_task();
        if (tud_hid_n_ready(0)) {
            if (get_and_clear_tick_pending()) {
                process_mapping(true);
            }
            send_report(do_send_report);
        }

        if (their_descriptor_updated) {
            update_their_descriptor_derivates();
            their_descriptor_updated = false;
        }
        if (need_to_persist_config) {
            persist_config();
            need_to_persist_config = false;
        }

        print_stats_maybe();

        if (led_state && (time_us_64() > turn_led_off_after)) {
            led_state = false;
            board_led_write(false);
        }
    }

    return 0;
}
