#include <cstdio>
#include <cstring>

#include <tusb.h>

#include "out_report.h"

struct outgoing_out_report_t {
    uint8_t dev_addr;
    uint8_t interface;
    uint8_t report_id;
    uint16_t len;
    OutType type;
    uint8_t report[64];  // XXX
};

#define OOR_BUFSIZE 8
static outgoing_out_report_t outgoing_out_reports[OOR_BUFSIZE];
static uint8_t oor_head = 0;
static uint8_t oor_tail = 0;
static uint8_t oor_items = 0;

static uint8_t get_buffer[64];

static bool ready_to_send = true;

void do_queue_out_report(const uint8_t* report, uint16_t len, uint8_t report_id, uint8_t dev_addr, uint8_t interface, OutType type) {
    if (oor_items == OOR_BUFSIZE) {
        printf("out overflow!\n");
        return;
    }
    if ((len + ((report_id != 0) ? 1 : 0)) > sizeof(outgoing_out_reports[oor_tail].report)) {
        return;
    }
    outgoing_out_reports[oor_tail].dev_addr = dev_addr;
    outgoing_out_reports[oor_tail].interface = interface;
    outgoing_out_reports[oor_tail].report_id = report_id;
    outgoing_out_reports[oor_tail].len = len + ((report_id != 0) ? 1 : 0);
    outgoing_out_reports[oor_tail].type = type;
    if (report_id != 0) {
        outgoing_out_reports[oor_tail].report[0] = report_id;
    }
    memcpy(outgoing_out_reports[oor_tail].report + ((report_id != 0) ? 1 : 0), report, len);
    oor_tail = (oor_tail + 1) % OOR_BUFSIZE;
    oor_items++;
}

void do_queue_get_report(uint8_t report_id, uint8_t dev_addr, uint8_t interface, uint8_t len) {
    if (oor_items == OOR_BUFSIZE) {
        printf("out overflow!\n");
        return;
    }
    outgoing_out_reports[oor_tail].dev_addr = dev_addr;
    outgoing_out_reports[oor_tail].interface = interface;
    outgoing_out_reports[oor_tail].report_id = report_id;
    outgoing_out_reports[oor_tail].type = OutType::GET_FEATURE;
    outgoing_out_reports[oor_tail].len = len;
    oor_tail = (oor_tail + 1) % OOR_BUFSIZE;
    oor_items++;
}

void do_send_out_report() {
    if ((oor_items > 0) && ready_to_send) {
        outgoing_out_report_t* out = &(outgoing_out_reports[oor_head]);
        if ((out->type == OutType::OUTPUT) || (out->type == OutType::SET_FEATURE)) {
            if (tuh_hid_set_report(out->dev_addr, out->interface, out->report_id, (out->type == OutType::OUTPUT) ? HID_REPORT_TYPE_OUTPUT : HID_REPORT_TYPE_FEATURE, out->report, out->len)) {
                ready_to_send = false;
                oor_head = (oor_head + 1) % OOR_BUFSIZE;
                oor_items--;
            }
        } else if (out->type == OutType::GET_FEATURE) {
            if (tuh_hid_get_report(out->dev_addr, out->interface, out->report_id, HID_REPORT_TYPE_FEATURE, get_buffer, out->len)) {
                ready_to_send = false;
                oor_head = (oor_head + 1) % OOR_BUFSIZE;
                oor_items--;
            }
        }
    }
}

void tuh_hid_set_report_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) {
    ready_to_send = true;
    set_report_complete_cb(dev_addr, instance, report_id);
}

void tuh_hid_get_report_complete_cb(uint8_t dev_addr, uint8_t idx, uint8_t report_id, uint8_t report_type, uint16_t len) {
    ready_to_send = true;
    get_report_cb(dev_addr, idx, report_id, report_type, get_buffer, len);
}
