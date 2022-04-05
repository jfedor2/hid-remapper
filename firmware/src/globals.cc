#include "globals.h"

std::unordered_map<uint8_t, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>> their_usages;

std::unordered_map<uint8_t, bool> has_report_id_theirs;

std::vector<usage_rle_t> our_usages_rle;
std::vector<usage_rle_t> their_usages_rle;

volatile bool need_to_persist_config = false;
volatile bool need_to_clear_descriptor_data = false;
volatile bool their_descriptor_updated = false;
volatile bool suspended = false;

bool unmapped_passthrough = true;
uint32_t partial_scroll_timeout = 1000000;
std::vector<mapping_config_t> config_mappings;

uint8_t resolution_multiplier = 0;
