#ifndef _OUR_DESCRIPTOR_H_
#define _OUR_DESCRIPTOR_H_

#include <stdint.h>

#define CONFIG_SIZE 32
#define RESOLUTION_MULTIPLIER 120

#define REPORT_ID_MULTIPLIER 99
#define REPORT_ID_CONFIG 100

extern const uint8_t our_report_descriptor[];
extern const uint32_t our_report_descriptor_length;

#endif
