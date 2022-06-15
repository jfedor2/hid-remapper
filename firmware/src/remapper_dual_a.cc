#include "descriptor_parser.h"
#include "dual.h"
#include "interval_override.h"
#include "remapper.h"
#include "serial.h"

void send_b_init() {
    b_init_t msg;
    msg.interval_override = interval_override;
    serial_write((uint8_t*) &msg, sizeof(msg));
}

void serial_callback(const uint8_t* data, uint16_t len) {
    switch ((DualCommand) data[0]) {
        case DualCommand::DEVICE_CONNECTED: {
            device_connected_t* msg = (device_connected_t*) data;
            parse_descriptor(msg->vid, msg->pid, msg->report_descriptor, len - sizeof(device_connected_t), (uint16_t) (msg->dev_addr << 8) | msg->interface);
            break;
        }
        case DualCommand::DEVICE_DISCONNECTED: {
            device_disconnected_t* msg = (device_disconnected_t*) data;
            clear_descriptor_data(msg->dev_addr);
            break;
        }
        case DualCommand::REPORT_RECEIVED: {
            report_received_t* msg = (report_received_t*) data;
            handle_received_report(msg->report, len - sizeof(report_received_t), (uint16_t) (msg->dev_addr << 8) | msg->interface);
            break;
        }
        case DualCommand::REQUEST_B_INIT:
            send_b_init();
            break;
        default:
            break;
    }
}

void extra_init() {
    serial_init();
}

bool read_report() {
    return serial_read(serial_callback);
}

void interval_override_updated() {
    restart_t msg;
    serial_write((uint8_t*) &msg, sizeof(msg));
}
