#ifndef _OUR_DESCRIPTOR_H_
#define _OUR_DESCRIPTOR_H_

#include <stdint.h>

#define CONFIG_SIZE 32
#define RESOLUTION_MULTIPLIER 120

#define REPORT_ID_LEDS 98
#define REPORT_ID_MULTIPLIER 99
#define REPORT_ID_CONFIG 100
#define REPORT_ID_MONITOR 101

#define MAX_INPUT_REPORT_ID 3

extern const uint8_t our_report_descriptor[];
extern const uint32_t our_report_descriptor_length;

extern const uint8_t config_report_descriptor[];
extern const uint32_t config_report_descriptor_length;

#endif
