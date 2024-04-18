#ifndef _QUIRKS_H_
#define _QUIRKS_H_

#include <stdint.h>
#include <unordered_map>
#include "types.h"

void apply_quirks(uint16_t vendor_id, uint16_t product_id, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, const uint8_t* report_descriptor, int len, uint8_t itf_num);

#endif
