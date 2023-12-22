#include <cstdio>
#include <cstring>

#include "pico/time.h"

#include "descriptor_parser.h"
#include "dual.h"
#include "interval_override.h"
#include "remapper.h"
#include "serial.h"
#include "tick.h"

#include "dual_b_binary.h"

extern "C" {
#include "adi.h"
#include "flash.h"
#include "swd.h"
}

void send_b_init() {
    b_init_t msg;
    msg.interval_override = interval_override;
    serial_write((uint8_t*) &msg, sizeof(msg));
}

static int64_t tick_timer_callback(alarm_id_t id, void* user_data) {
    set_tick_pending();
    return 0;
}

bool serial_callback(const uint8_t* data, uint16_t len) {
    bool ret = false;
    switch ((DualCommand) data[0]) {
        case DualCommand::DEVICE_CONNECTED: {
            device_connected_t* msg = (device_connected_t*) data;
            parse_descriptor(msg->vid, msg->pid, msg->report_descriptor, len - sizeof(device_connected_t), (uint16_t) (msg->dev_addr << 8) | msg->interface);
            device_connected_callback((uint16_t) (msg->dev_addr << 8) | msg->interface, msg->vid, msg->pid, msg->hub_port);
            break;
        }
        case DualCommand::DEVICE_DISCONNECTED: {
            device_disconnected_t* msg = (device_disconnected_t*) data;
            device_disconnected_callback(msg->dev_addr);
            break;
        }
        case DualCommand::REPORT_RECEIVED: {
            report_received_t* msg = (report_received_t*) data;
            handle_received_report(msg->report, len - sizeof(report_received_t), (uint16_t) (msg->dev_addr << 8) | msg->interface);
            ret = true;
            break;
        }
        case DualCommand::REQUEST_B_INIT:
            send_b_init();
            break;
        case DualCommand::START_OF_FRAME:
            add_alarm_in_us(300, tick_timer_callback, NULL, true);
            break;
        case DualCommand::GET_FEATURE_RESPONSE: {
            get_feature_response_t* msg = (get_feature_response_t*) data;
            handle_get_report_response((uint16_t) (msg->dev_addr << 8) | msg->interface, msg->report_id, msg->report, len - sizeof(get_feature_response_t));
            break;
        }
        case DualCommand::SET_FEATURE_COMPLETE: {
            set_feature_complete_t* msg = (set_feature_complete_t*) data;
            handle_set_report_complete((uint16_t) (msg->dev_addr << 8) | msg->interface, msg->report_id);
            break;
        }
        case DualCommand::MIDI_RECEIVED: {
            midi_received_t* msg = (midi_received_t*) data;
            handle_received_midi(msg->hub_port, msg->msg);
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}

void extra_init() {
    serial_init();
}

uint32_t get_gpio_valid_pins_mask() {
    return GPIO_VALID_PINS_BASE & ~(
#ifdef PICO_DEFAULT_UART_TX_PIN
                                      (1 << PICO_DEFAULT_UART_TX_PIN) |
#endif
#ifdef PICO_DEFAULT_UART_RX_PIN
                                      (1 << PICO_DEFAULT_UART_RX_PIN) |
#endif
                                      (1 << PIN_SWDIO) |
                                      (1 << PIN_SWDCLK) |
                                      (1 << SERIAL_TX_PIN) |
                                      (1 << SERIAL_RX_PIN) |
                                      (1 << SERIAL_CTS_PIN) |
                                      (1 << SERIAL_RTS_PIN));
}

void read_report(bool* new_report, bool* tick) {
    *new_report = serial_read(serial_callback);
    *tick = get_and_clear_tick_pending();
}

void interval_override_updated() {
    restart_t msg;
    serial_write((uint8_t*) &msg, sizeof(msg));
}

bool swd_initialized = false;

void flash_b_side() {
    if (!swd_initialized) {
        printf("swd_init: %d\n", swd_init());
        swd_initialized = true;
    }
    printf("dp_init: %d\n", dp_init());

    core_select(0);
    core_reset_halt();
    core_select(1);
    core_reset_halt();
    core_select(0);

    rp2040_add_flash_bit(0, dual_b_binary, dual_b_binary_length);
    rp2040_add_flash_bit(0xffffffff, NULL, 0);
}

uint8_t buffer[64 + sizeof(send_out_report_t)];

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* report, uint8_t len) {
    // XXX
    // This is called from process_mapping() so we probably shouldn't be writing directly
    // to serial here, as it can block.
    send_out_report_t* msg = (send_out_report_t*) buffer;
    msg->command = DualCommand::SEND_OUT_REPORT;
    msg->dev_addr = interface >> 8;
    msg->interface = interface & 0xFF;
    msg->report_id = report_id;
    memcpy(msg->report, report, len);
    serial_write((uint8_t*) msg, len + sizeof(send_out_report_t));
}

void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* report, uint8_t len) {
    set_feature_report_t* msg = (set_feature_report_t*) buffer;
    msg->command = DualCommand::SET_FEATURE_REPORT;
    msg->dev_addr = interface >> 8;
    msg->interface = interface & 0xFF;
    msg->report_id = report_id;
    memcpy(msg->report, report, len);
    serial_write((uint8_t*) msg, len + sizeof(set_feature_report_t));
}

void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len) {
    get_feature_report_t* msg = (get_feature_report_t*) buffer;
    msg->command = DualCommand::GET_FEATURE_REPORT;
    msg->dev_addr = interface >> 8;
    msg->interface = interface & 0xFF;
    msg->report_id = report_id;
    msg->len = len;
    serial_write((uint8_t*) msg, sizeof(get_feature_report_t));
}

void send_out_report() {
}

void sof_callback() {
}
