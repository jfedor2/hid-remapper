#ifndef _OUT_REPORT_H_
#define _OUT_REPORT_H_

#include <stdint.h>

enum class OutType : int8_t {
    OUTPUT = 0,
    GET_FEATURE = 1,
    SET_FEATURE = 2,
};

void do_queue_out_report(const uint8_t* report, uint16_t len, uint8_t report_id, uint8_t dev_addr, uint8_t interface, OutType type);
void do_queue_get_report(uint8_t report_id, uint8_t dev_addr, uint8_t interface, uint8_t len);
void do_send_out_report();

void get_report_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id, uint8_t report_type, uint8_t* report, uint16_t len);
void set_report_complete_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id);

#endif
