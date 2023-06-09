#include <tusb.h>

#include "descriptor_parser.h"
#include "out_report.h"
#include "remapper.h"

void extra_init() {
}

static bool reports_received = false;

bool read_report() {
    tuh_task();

    bool ret = reports_received;
    reports_received = false;
    return ret;
}

void interval_override_updated() {
}

void flash_b_side() {
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    printf("tuh_hid_mount_cb\n");

    uint16_t vid;
    uint16_t pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    parse_descriptor(vid, pid, desc_report, desc_len, (uint16_t) (dev_addr << 8) | instance);

    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("tuh_hid_umount_cb\n");
    clear_descriptor_data(dev_addr);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    if (len > 0) {
        handle_received_report(report, len, (uint16_t) (dev_addr << 8) | instance);

        reports_received = true;
    }

    tuh_hid_receive_report(dev_addr, instance);
}

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    do_queue_out_report(buffer, len, report_id, interface >> 8, interface & 0xFF);
}

void send_out_report() {
    do_send_out_report();
}
