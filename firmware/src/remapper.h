#ifndef _REMAPPER_H_
#define _REMAPPER_H_

#define OUR_OUT_INTERFACE 0xFFFF

#define GPIO_USAGE_PAGE 0xFFF40000
#define DIGIPOT_USAGE_PAGE 0xFFF60000
#define DPAD_USAGE_PAGE 0xFFF90000

#define DPAD_USAGE_LEFT (DPAD_USAGE_PAGE | 1)
#define DPAD_USAGE_RIGHT (DPAD_USAGE_PAGE | 2)
#define DPAD_USAGE_UP (DPAD_USAGE_PAGE | 3)
#define DPAD_USAGE_DOWN (DPAD_USAGE_PAGE | 4)

#define DPAD_USAGE 0x00010039

typedef bool (*send_report_t)(uint8_t interface, const uint8_t* report_with_id, uint8_t len);

void set_mapping_from_config();
void handle_received_report(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id = 0);
void do_handle_received_report(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id = 0);
void handle_received_midi(uint8_t hub_port, uint8_t* midi_msg);
void set_input_state(uint32_t usage, int32_t state_raw, int32_t state_scaled, uint8_t hub_port = 0);

void extra_init();
void read_report(bool* new_report, bool* tick);

void interval_override_updated();

void parse_our_descriptor();
void process_mapping(bool auto_repeat);
void update_their_descriptor_derivates();
bool send_report(send_report_t do_send_report);
void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len);
void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len);
void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len);
void send_out_report();
bool send_monitor_report(send_report_t do_send_report);
void print_stats();
void reset_state();

void set_monitor_enabled(bool enabled);
void monitor_usage(uint32_t usage, int32_t value, uint8_t hub_port);

void sof_callback();

void device_connected_callback(uint16_t interface, uint16_t vid, uint16_t pid, uint8_t hub_port);
void device_disconnected_callback(uint8_t interface);
uint16_t handle_get_report0(uint8_t report_id, uint8_t* buffer, uint16_t reqlen);
void handle_set_report0(uint8_t report_id, const uint8_t* buffer, uint16_t reqlen);
void handle_get_report_response(uint16_t interface, uint8_t report_id, uint8_t* report, uint16_t len);
void handle_set_report_complete(uint16_t interface, uint8_t report_id);

void descriptor_received_callback(uint16_t vendor_id, uint16_t product_id, const uint8_t* report_descriptor, int len, uint16_t interface, uint8_t hub_port, uint8_t itf_num);
void report_received_callback(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);
void umount_callback(uint8_t dev_addr, uint8_t instance);

#endif
