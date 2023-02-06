#include "globals.h"

std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_usages;

std::unordered_map<uint16_t, bool> has_report_id_theirs;

std::unordered_map<uint16_t, uint8_t> interface_index;
uint32_t interface_index_in_use = 0;

std::vector<usage_rle_t> our_usages_rle;
std::vector<usage_rle_t> their_usages_rle;

volatile bool need_to_persist_config = false;
volatile bool their_descriptor_updated = false;
volatile bool suspended = false;

uint8_t unmapped_passthrough_layer_mask = 0b00001111;
uint32_t partial_scroll_timeout = 1000000;
uint32_t tap_hold_threshold = 200000;
std::vector<mapping_config_t> config_mappings;

uint8_t resolution_multiplier = 0;

std::vector<std::vector<uint32_t>> macros[NMACROS];

std::vector<expr_elem_t> expressions[NEXPRESSIONS];
