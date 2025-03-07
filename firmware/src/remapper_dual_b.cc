#include <bsp/board_api.h>
#include <tusb.h>

#include "usb_midi_host.h"

#include "hardware/watchdog.h"
#include "pico/stdio.h"
#include "pico/time.h"

#include "activity_led.h"
#include "dual.h"
#include "interval_override.h"
#include "out_report.h"
#include "serial.h"

uint8_t buffer[SERIAL_MAX_PAYLOAD_SIZE + sizeof(device_connected_t)];
bool initialized = false;

bool serial_callback(const uint8_t* data, uint16_t len) {
    switch ((DualCommand) data[0]) {
        case DualCommand::B_INIT:
            interval_override = ((b_init_t*) data)->interval_override;
            initialized = true;
            break;
        case DualCommand::RESTART:
            watchdog_reboot(0, 0, 0);
            break;
        case DualCommand::SEND_OUT_REPORT: {
            send_out_report_t* msg = (send_out_report_t*) data;
            do_queue_out_report(msg->report, len - sizeof(send_out_report_t), msg->report_id, msg->dev_addr, msg->interface, OutType::OUTPUT);
            break;
        }
        case DualCommand::SET_FEATURE_REPORT: {
            set_feature_report_t* msg = (set_feature_report_t*) data;
            do_queue_out_report(msg->report, len - sizeof(set_feature_report_t), msg->report_id, msg->dev_addr, msg->interface, OutType::SET_FEATURE);
            break;
        }
        case DualCommand::GET_FEATURE_REPORT: {
            get_feature_report_t* msg = (get_feature_report_t*) data;
            do_queue_get_report(msg->report_id, msg->dev_addr, msg->interface, msg->len);
            break;
        }
        default:
            break;
    }

    return false;
}

void request_b_init() {
    request_b_init_t msg;
    serial_write_nonblocking((uint8_t*) &msg, sizeof(msg));
}

int main() {
    serial_init();
    board_init();
    stdio_init_all();

    while (!initialized) {
        request_b_init();
        serial_read(serial_callback);
    }

    tusb_init();

    while (true) {
        tuh_task();
        serial_read(serial_callback);
        do_send_out_report();
        activity_led_off_maybe();
    }

    return 0;
}

void report_received_callback(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    activity_led_on();

    report_received_t* msg = (report_received_t*) buffer;
    msg->command = DualCommand::REPORT_RECEIVED;
    msg->dev_addr = dev_addr;
    msg->interface = instance;
    memcpy(msg->report, report, len);
    serial_write_nonblocking((uint8_t*) msg, len + sizeof(report_received_t));
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    report_received_callback(dev_addr, instance, report, len);

    tuh_hid_receive_report(dev_addr, instance);
}

void descriptor_received_callback(uint16_t vendor_id, uint16_t product_id, const uint8_t* report_descriptor, int len, uint16_t interface, uint8_t hub_port, uint8_t itf_num) {
    device_connected_t* msg = (device_connected_t*) buffer;
    msg->command = DualCommand::DEVICE_CONNECTED;
    msg->vid = vendor_id;
    msg->pid = product_id;
    msg->dev_addr = (interface >> 8) & 0xFF;
    msg->interface = interface & 0xFF;
    msg->hub_port = hub_port;
    msg->itf_num = itf_num;
    memcpy(msg->report_descriptor, report_descriptor, len);
    serial_write((uint8_t*) msg, len + sizeof(device_connected_t));
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    printf("tuh_hid_mount_cb\n");

    uint8_t hub_addr;
    uint8_t hub_port;
    tuh_get_hub_addr_port(dev_addr, &hub_addr, &hub_port);

    uint16_t vid;
    uint16_t pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    tuh_itf_info_t itf_info;
    tuh_hid_itf_get_info(dev_addr, instance, &itf_info);
    uint8_t itf_num = itf_info.desc.bInterfaceNumber;

    descriptor_received_callback(vid, pid, desc_report, desc_len, (uint16_t) (dev_addr << 8) | instance, hub_port, itf_num);
    tuh_hid_receive_report(dev_addr, instance);
}

void umount_callback(uint8_t dev_addr, uint8_t instance) {
    device_disconnected_t msg;
    msg.dev_addr = dev_addr;
    msg.interface = instance;
    serial_write((uint8_t*) &msg, sizeof(msg));
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("tuh_hid_umount_cb %d %d\n", dev_addr, instance);
    umount_callback(dev_addr, instance);
}

void tuh_sof_cb() {
    start_of_frame_t msg;
    serial_write_nonblocking((uint8_t*) &msg, sizeof(msg));
}

void get_report_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id, uint8_t report_type, uint8_t* report, uint16_t len) {
    get_feature_response_t* msg = (get_feature_response_t*) buffer;
    msg->command = DualCommand::GET_FEATURE_RESPONSE;
    msg->dev_addr = dev_addr;
    msg->interface = interface;
    msg->report_id = report_id;
    memcpy(msg->report, report, len);
    serial_write((uint8_t*) msg, len + sizeof(get_feature_response_t));
}

void set_report_complete_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id) {
    set_feature_complete_t* msg = (set_feature_complete_t*) buffer;
    msg->command = DualCommand::SET_FEATURE_COMPLETE;
    msg->dev_addr = dev_addr;
    msg->interface = interface;
    msg->report_id = report_id;
    serial_write((uint8_t*) msg, sizeof(set_feature_complete_t));
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
    activity_led_on();

    uint8_t hub_addr;
    uint8_t hub_port;
    tuh_get_hub_addr_port(dev_addr, &hub_addr, &hub_port);

    midi_received_t* msg = (midi_received_t*) buffer;
    msg->command = DualCommand::MIDI_RECEIVED;
    msg->hub_port = hub_port;
    while (tuh_midi_packet_read(dev_addr, msg->msg)) {
        serial_write_nonblocking((uint8_t*) msg, sizeof(midi_received_t));
    }
}
