#ifndef _QUIRKS_H_
#define _QUIRKS_H_

#include <stdint.h>
#include <unordered_map>
#include "types.h"

void apply_quirks(uint16_t vendor_id, uint16_t product_id, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, const uint8_t* report_descriptor, int len, uint8_t itf_num);

// { normalized usage, Stadia controller usage }
// Declared with its size so that sizeof works in other files.
extern const uint32_t stadia_mapping[16][2];

#endif
