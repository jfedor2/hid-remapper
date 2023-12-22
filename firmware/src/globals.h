#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <unordered_map>
#include <vector>

#include "our_descriptor.h"
#include "types.h"

extern std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_usages;          // dev_addr+interface -> report_id -> usage -> usage_def
extern std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_out_usages;      // dev_addr+interface -> report_id -> usage -> usage_def
extern std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_feature_usages;  // dev_addr+interface -> report_id -> usage -> usage_def

extern std::unordered_map<uint16_t, bool> has_report_id_theirs;  // dev_addr+interface -> bool

extern std::unordered_map<uint32_t, uint8_t*> out_reports;                         // dev_addr+interface << 16 | report_id -> buffer
extern std::unordered_map<uint32_t, uint8_t*> prev_out_reports;                    // dev_addr+interface << 16 | report_id -> buffer
extern std::unordered_map<uint32_t, uint8_t> out_report_sizes;                     // dev_addr+interface << 16 | report_id -> size
extern std::unordered_map<uint32_t, std::vector<uint32_t>> their_out_usages_flat;  // usage -> vector of dev_addr+interface << 16 | report_id

extern std::unordered_map<uint16_t, uint8_t> interface_index;  // dev_addr+interface -> unique 0-31 integer
extern uint32_t interface_index_in_use;                        // bit mask

extern std::vector<usage_rle_t> our_usages_rle;
extern std::vector<usage_rle_t> their_usages_rle;

extern volatile bool need_to_persist_config;
extern volatile bool their_descriptor_updated;
extern volatile bool suspended;
extern volatile bool config_updated;

extern uint8_t unmapped_passthrough_layer_mask;
extern uint32_t partial_scroll_timeout;
extern uint32_t tap_hold_threshold;
extern uint64_t gpio_debounce_time;
extern uint8_t our_descriptor_number;
extern bool ignore_auth_dev_inputs;
extern uint8_t macro_entry_duration;
extern uint8_t gpio_output_mode;

extern std::vector<mapping_config11_t> config_mappings;

extern uint8_t resolution_multiplier;

#define NMACROS_8 8
#define NMACROS 32
extern std::vector<std::vector<uint32_t>> macros[NMACROS];

#define NEXPRESSIONS 8
extern std::vector<expr_elem_t> expressions[NEXPRESSIONS];

extern bool monitor_enabled;

extern const our_descriptor_def_t* our_descriptor;

extern uint8_t gpio_out_state[4];

#endif
