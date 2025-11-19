#ifndef _OUR_DESCRIPTOR_H_
#define _OUR_DESCRIPTOR_H_

#include <stdint.h>

#define CONFIG_SIZE 36
#define RESOLUTION_MULTIPLIER 120

#define REPORT_ID_LEDS 98
#define REPORT_ID_MULTIPLIER 99
#define REPORT_ID_CONFIG 100
#define REPORT_ID_MONITOR 101

#define MAX_INPUT_REPORT_ID 3

#define NOUR_DESCRIPTORS 6

typedef void (*device_connected_t)(uint16_t interface, uint16_t vid, uint16_t pid);
typedef void (*device_disconnected_t)(uint8_t dev_addr);
typedef void (*main_loop_task_t)();
typedef void (*handle_received_report_t)(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id);
typedef uint16_t (*handle_get_report_t)(uint8_t report_id, uint8_t* buffer, uint16_t reqlen);
typedef void (*handle_set_report_t)(uint8_t report_id, const uint8_t* buffer, uint16_t reqlen);
typedef bool (*set_report_synchronous_t)(uint8_t report_id);
typedef void (*handle_get_report_response_t)(uint16_t interface, uint8_t report_id, uint8_t* report, uint16_t len);
typedef void (*handle_set_report_complete_t)(uint16_t interface, uint8_t report_id);
typedef void (*clear_report_t)(uint8_t* report, uint8_t report_id, uint16_t len);
typedef int32_t (*default_value_t)(uint32_t usage);
typedef void (*sanitize_report_t)(uint8_t report_id, uint8_t* buffer, uint16_t len);

struct our_descriptor_def_t {
    uint8_t idx;
    const uint8_t* descriptor;
    uint32_t descriptor_length;
    uint16_t vid = 0;
    uint16_t pid = 0;
    device_connected_t device_connected = nullptr;
    device_disconnected_t device_disconnected = nullptr;
    main_loop_task_t main_loop_task = nullptr;
    handle_received_report_t handle_received_report = nullptr;
    handle_get_report_t handle_get_report = nullptr;
    handle_set_report_t handle_set_report = nullptr;
    set_report_synchronous_t set_report_synchronous = nullptr;
    handle_get_report_response_t handle_get_report_response = nullptr;
    handle_set_report_complete_t handle_set_report_complete = nullptr;
    clear_report_t clear_report = nullptr;
    default_value_t default_value = nullptr;
    sanitize_report_t sanitize_report = nullptr;
};

extern const our_descriptor_def_t our_descriptors[];

extern const uint8_t config_report_descriptor[];
extern const uint32_t config_report_descriptor_length;

extern const uint8_t boot_kb_report_descriptor[];
extern const uint32_t boot_kb_report_descriptor_length;

#endif
