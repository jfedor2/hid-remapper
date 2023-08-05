#ifndef _PS_AUTH_H_
#define _PS_AUTH_H_

void ps4_device_connected(uint16_t interface, uint16_t vid, uint16_t pid);
void ps4_device_disconnected(uint8_t dev_addr);
void ps4_main_loop_task();
void ps4_handle_received_report(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id);
uint16_t ps4_handle_get_report(uint8_t report_id, uint8_t* buffer, uint16_t reqlen);
void ps4_handle_set_report(uint8_t report_id, const uint8_t* buffer, uint16_t reqlen);
void ps4_handle_get_report_response(uint16_t interface, uint8_t report_id, uint8_t* report, uint16_t len);
void ps4_handle_set_report_complete(uint16_t interface, uint8_t report_id);

#endif
