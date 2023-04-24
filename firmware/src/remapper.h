#ifndef _REMAPPER_H_
#define _REMAPPER_H_

typedef bool (*send_report_t)(uint8_t interface, const uint8_t* report_with_id, uint8_t len);

void set_mapping_from_config();
void handle_received_report(const uint8_t* report, int len, uint16_t interface);

void extra_init();
bool read_report();

void interval_override_updated();

void parse_our_descriptor();
void process_mapping(bool auto_repeat);
void update_their_descriptor_derivates();
bool send_report(send_report_t do_send_report);
void send_monitor_report(send_report_t do_send_report);
void print_stats();

void monitor_usage(uint32_t usage, int32_t value);

#endif
