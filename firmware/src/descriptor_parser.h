#ifndef _DESCRIPTOR_PARSER_H_
#define _DESCRIPTOR_PARSER_H_

#include <stdint.h>

#ifdef __cplusplus

#include <unordered_map>
#include "types.h"

std::unordered_map<uint8_t, uint16_t> parse_descriptor(std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, bool& has_report_id, const volatile uint8_t* report_descriptor, int len);

extern "C" {
#endif

void parse_descriptor(const volatile uint8_t* report_descriptor, int len, uint8_t interface);
void clear_descriptor_data();

#ifdef __cplusplus
}
#endif

#endif
