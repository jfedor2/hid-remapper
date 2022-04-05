#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <unordered_map>
#include <vector>
#include "types.h"

extern std::unordered_map<uint8_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_usages;  // interface -> report_id -> usage -> usage_def

extern std::unordered_map<uint8_t, bool> has_report_id_theirs;  // interface -> bool

extern std::vector<usage_rle_t> our_usages_rle;
extern std::vector<usage_rle_t> their_usages_rle;

extern volatile bool need_to_persist_config;
extern volatile bool need_to_clear_descriptor_data;
extern volatile bool their_descriptor_updated;
extern volatile bool suspended;

extern bool unmapped_passthrough;
extern uint32_t partial_scroll_timeout;
extern std::vector<mapping_config_t> config_mappings;

extern uint8_t resolution_multiplier;

#endif
