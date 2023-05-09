#ifndef _OUT_REPORT_H_
#define _OUT_REPORT_H_

#include <stdint.h>

void do_queue_out_report(const uint8_t* report, uint16_t len, uint8_t report_id, uint8_t dev_addr, uint8_t interface);
void do_send_out_report();

#endif
