#include <cstdio>
#include <cstring>

#include <tusb.h>

#include "out_report.h"

struct outgoing_out_report_t {
    uint8_t dev_addr;
    uint8_t interface;
    uint8_t report_id;
    uint16_t len;
    uint8_t report[64];  // XXX
};

#define OOR_BUFSIZE 8
static outgoing_out_report_t outgoing_out_reports[OOR_BUFSIZE];
static uint8_t oor_head = 0;
static uint8_t oor_tail = 0;
static uint8_t oor_items = 0;

static bool ready_to_send = true;

void do_queue_out_report(const uint8_t* report, uint16_t len, uint8_t report_id, uint8_t dev_addr, uint8_t interface) {
    if (oor_items == OOR_BUFSIZE) {
        printf("out overflow!\n");
        return;
    }
    if (len > sizeof(outgoing_out_reports[oor_tail].report)) {
        return;
    }
    outgoing_out_reports[oor_tail].dev_addr = dev_addr;
    outgoing_out_reports[oor_tail].interface = interface;
    outgoing_out_reports[oor_tail].report_id = report_id;
    outgoing_out_reports[oor_tail].len = len;
    memcpy(outgoing_out_reports[oor_tail].report, report, len);
    oor_tail = (oor_tail + 1) % OOR_BUFSIZE;
    oor_items++;
}

void do_send_out_report() {
    if ((oor_items > 0) && ready_to_send) {
        outgoing_out_report_t* out = &(outgoing_out_reports[oor_head]);
        if (tuh_hid_set_report(out->dev_addr, out->interface, out->report_id, HID_REPORT_TYPE_OUTPUT, out->report, out->len)) {
            ready_to_send = false;
            oor_head = (oor_head + 1) % OOR_BUFSIZE;
            oor_items--;
        }
    }
}

void tuh_hid_set_report_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len) {
    ready_to_send = true;
}
