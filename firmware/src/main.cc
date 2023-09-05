#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <bsp/board_api.h>
#include <tusb.h>

#include <hardware/flash.h>
#include <hardware/gpio.h>
#include <pico/bootrom.h>
#include <pico/mutex.h>
#include <pico/stdio.h>
#include <pico/unique_id.h>

#include "activity_led.h"
#include "config.h"
#include "crc.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"
#include "tick.h"

#define CONFIG_OFFSET_IN_FLASH (PICO_FLASH_SIZE_BYTES - PERSISTED_CONFIG_SIZE)
#define FLASH_CONFIG_IN_MEMORY (((uint8_t*) XIP_BASE) + CONFIG_OFFSET_IN_FLASH)

#define GPIO_PIN_FIRST 2
#define GPIO_PIN_LAST 9
#define GPIO_PIN_MASK ((1 << (GPIO_PIN_LAST + 1)) - (1 << GPIO_PIN_FIRST))

#define GPIO_USAGE_PAGE 0xFFF40000

uint64_t next_print = 0;

mutex_t mutexes[(uint8_t) MutexId::N];

uint32_t prev_gpio_state = 0;
uint64_t last_gpio_change[32] = { 0 };

void print_stats_maybe() {
    uint64_t now = time_us_64();
    if (now > next_print) {
        print_stats();
        while (next_print < now) {
            next_print += 1000000;
        }
    }
}

void sof_handler(uint32_t frame_count) {
    sof_callback();
}

bool do_send_report(uint8_t interface, const uint8_t* report_with_id, uint8_t len) {
    tud_hid_n_report(interface, report_with_id[0], report_with_id + 1, len - 1);
    return true;  // XXX?
}

void gpio_pins_init() {
    for (uint8_t i = GPIO_PIN_FIRST; i <= GPIO_PIN_LAST; i++) {
        gpio_init(i);
        gpio_pull_up(i);
    }
}

bool read_gpio(uint64_t now) {
    uint32_t gpio_state = gpio_get_all() & GPIO_PIN_MASK;
    uint32_t changed = prev_gpio_state ^ gpio_state;
    if (changed != 0) {
        for (uint8_t i = GPIO_PIN_FIRST; i <= GPIO_PIN_LAST; i++) {
            uint32_t bit = 1 << i;
            if (changed & bit) {
                if (last_gpio_change[i] + gpio_debounce_time <= now) {
                    uint32_t usage = GPIO_USAGE_PAGE | i;
                    int32_t state = !(gpio_state & bit);  // active low
                    set_input_state(usage, state);
                    if (monitor_enabled) {
                        monitor_usage(usage, state);
                    }
                    last_gpio_change[i] = now;
                } else {
                    // ignore this change
                    gpio_state ^= bit;
                    changed ^= bit;
                }
            }
        }
        prev_gpio_state = gpio_state;
    }
    return changed != 0;
}

void do_persist_config(uint8_t* buffer) {
#if !PICO_COPY_TO_RAM
    uint32_t ints = save_and_disable_interrupts();
#endif
    flash_range_erase(CONFIG_OFFSET_IN_FLASH, PERSISTED_CONFIG_SIZE);
    flash_range_program(CONFIG_OFFSET_IN_FLASH, buffer, PERSISTED_CONFIG_SIZE);
#if !PICO_COPY_TO_RAM
    restore_interrupts(ints);
#endif
}

void reset_to_bootloader() {
    reset_usb_boot(0, 0);
}

void pair_new_device() {
}

void clear_bonds() {
}

void my_mutexes_init() {
    for (int i = 0; i < (int8_t) MutexId::N; i++) {
        mutex_init(&mutexes[i]);
    }
}

void my_mutex_enter(MutexId id) {
    mutex_enter_blocking(&mutexes[(uint8_t) id]);
}

void my_mutex_exit(MutexId id) {
    mutex_exit(&mutexes[(uint8_t) id]);
}

uint64_t get_time() {
    return time_us_64();
}

uint64_t get_unique_id() {
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);
    uint64_t ret = 0;
    for (int i = 0; i < 8; i++) {
        ret |= (uint64_t) unique_id.id[7 - i] << (8 * i);
    }
    return ret;
}

int main() {
    my_mutexes_init();
    gpio_pins_init();
    tick_init();
    parse_our_descriptor();
    load_config(FLASH_CONFIG_IN_MEMORY);
    set_mapping_from_config();
    board_init();
    extra_init();
    tusb_init();
    stdio_init_all();

    tud_sof_isr_set(sof_handler);

    next_print = time_us_64() + 1000000;

    while (true) {
        bool tick;
        bool new_report;
        read_report(&new_report, &tick);
        if (new_report) {
            activity_led_on();
        }
        if (their_descriptor_updated) {
            update_their_descriptor_derivates();
            their_descriptor_updated = false;
        }
        if (tick) {
            bool gpio_state_changed = read_gpio(time_us_64());
            if (gpio_state_changed) {
                activity_led_on();
            }
            process_mapping(true);
        }
        tud_task();
        if (config_updated) {
            set_mapping_from_config();
            config_updated = false;
        }
        if (tud_hid_n_ready(0)) {
            send_report(do_send_report);
        }
        if (monitor_enabled && tud_hid_n_ready(1)) {
            send_monitor_report(do_send_report);
        }
        send_out_report();
        if (need_to_persist_config) {
            persist_config();
            need_to_persist_config = false;
        }

        print_stats_maybe();

        activity_led_off_maybe();
    }

    return 0;
}
