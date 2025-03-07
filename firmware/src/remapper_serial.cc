#include "descriptor_parser.h"
#include "interval_override.h"
#include "remapper.h"
#include "tick.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"

#include <stdio.h>

#define SERIAL_MOUSE_RX_PIN 21
#define SERIAL_MOUSE_UART uart1

#define FAKE_VID 0x0001
#define FAKE_PID 0x0001
#define FAKE_INTERFACE 0x0101

struct report_t {
    uint8_t buttons;
    int8_t dx;
    int8_t dy;
};

report_t report = { .buttons = 0x00, .dx = 0x00, .dy = 0x00 };

uint8_t byte_number = -1;
uint8_t left = 0;
uint8_t right = 0;
uint8_t middle = 0;
uint8_t new_left;
uint8_t new_right;
int8_t dx = 0;
int8_t dy = 0;

const uint8_t fake_descriptor[] = {
    0x05, 0x09,  // Usage Page (Button)
    0x19, 0x01,  // Usage Minimum (0x01)
    0x29, 0x03,  // Usage Maximum (0x03)
    0x25, 0x01,  // Logical Maximum (1)
    0x95, 0x03,  // Report Count (3)
    0x75, 0x01,  // Report Size (1)
    0x81, 0x02,  // Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,  // Report Count (1)
    0x75, 0x05,  // Report Size (5)
    0x81, 0x01,  // Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,  // Usage (X)
    0x09, 0x31,  // Usage (Y)
    0x15, 0x80,  // Logical Minimum (-128)
    0x25, 0x7F,  // Logical Maximum (127)
    0x75, 0x08,  // Report Size (8)
    0x95, 0x02,  // Report Count (2)
    0x81, 0x06,  // Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
};

void extra_init() {
    gpio_set_function(SERIAL_MOUSE_RX_PIN, GPIO_FUNC_UART);
    uart_init(SERIAL_MOUSE_UART, 1200);
    uart_set_hw_flow(SERIAL_MOUSE_UART, false, false);
    uart_set_translate_crlf(SERIAL_MOUSE_UART, false);
    uart_set_format(SERIAL_MOUSE_UART, 7, 1, UART_PARITY_NONE);

    parse_descriptor(FAKE_VID, FAKE_PID, fake_descriptor, sizeof(fake_descriptor), FAKE_INTERFACE, 0);
}

uint32_t get_gpio_valid_pins_mask() {
    return GPIO_VALID_PINS_BASE & ~(
#ifdef PICO_DEFAULT_UART_TX_PIN
                                      (1 << PICO_DEFAULT_UART_TX_PIN) |
#endif
#ifdef PICO_DEFAULT_UART_RX_PIN
                                      (1 << PICO_DEFAULT_UART_RX_PIN) |
#endif
                                      (1 << SERIAL_MOUSE_RX_PIN));
}

void read_report(bool* new_report, bool* tick) {
    *tick = get_and_clear_tick_pending();
    *new_report = false;

    if (!uart_is_readable(SERIAL_MOUSE_UART)) {
        return;
    }

    char c = uart_getc(SERIAL_MOUSE_UART);
    printf("%02x\n", c);
    if (c & 0x40) {
        byte_number = 0;
    } else {
        if (byte_number >= 0) {
            byte_number++;
        }
    }

    switch (byte_number) {
        case 0: {
            new_left = (c >> 5) & 0x01;
            new_right = (c >> 4) & 0x01;
            dx = (c & 0x03) << 6;
            dy = (c & 0x0c) << 4;
            break;
        }
        case 1: {
            dx |= c;
            break;
        }
        case 2: {
            dy |= c;
            // no movement and no change in left/right buttons
            // means that middle button was pressed or released
            if ((left == new_left) &&
                (right == new_right) &&
                (dx == 0) &&
                (dy == 0)) {
                middle ^= 0x01;
            }
            left = new_left;
            right = new_right;
            report.buttons = left | (right << 1) | (middle << 2);
            report.dx = dx;
            report.dy = dy;
            byte_number = -1;
            handle_received_report((const uint8_t*) &report, sizeof(report), FAKE_INTERFACE);
            *new_report = true;
            break;
        }
    }
}

void interval_override_updated() {
}

void flash_b_side() {
}

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
}

void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
}

void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len) {
}

void send_out_report() {
}

void sof_callback() {
    set_tick_pending();
}
