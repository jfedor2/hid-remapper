#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <unordered_map>
#include <vector>

#include "types.h"

extern std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_usages;  // dev_addr+interface -> report_id -> usage -> usage_def

extern std::unordered_map<uint16_t, bool> has_report_id_theirs;  // dev_addr+interface -> bool

extern std::unordered_map<uint16_t, uint8_t> interface_index;  // dev_addr+interface -> unique 0-31 integer
extern uint32_t interface_index_in_use;                        // bit mask

extern std::vector<usage_rle_t> our_usages_rle;
extern std::vector<usage_rle_t> their_usages_rle;

extern volatile bool need_to_persist_config;
extern volatile bool their_descriptor_updated;
extern volatile bool suspended;

extern uint8_t unmapped_passthrough_layer_mask;
extern uint32_t partial_scroll_timeout;
extern std::vector<mapping_config_t> config_mappings;

extern uint8_t resolution_multiplier;

#endif
