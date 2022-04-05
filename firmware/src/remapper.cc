#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <bsp/board.h>
#include <tusb.h>

#include "config.h"
#include "crc.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "pio_usb_stuff.h"
#include "remapper.h"

const uint8_t MAPPING_FLAG_STICKY = 0x01;

const uint8_t V_RESOLUTION_BITMASK = (1 << 0);
const uint8_t H_RESOLUTION_BITMASK = (1 << 2);
const uint32_t V_SCROLL_USAGE = 0x00010038;
const uint32_t H_SCROLL_USAGE = 0x000C0238;

const uint8_t NLAYERS = 4;
const uint32_t LAYERS_USAGE_PAGE = 0xFFF10000;

const std::unordered_map<uint32_t, uint8_t> resolution_multiplier_masks = {
    { V_SCROLL_USAGE, V_RESOLUTION_BITMASK },
    { H_SCROLL_USAGE, H_RESOLUTION_BITMASK },
};

std::unordered_map<uint32_t, std::vector<map_source_t>> reverse_mapping;  // target -> sources list

std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>> our_usages;  // report_id -> usage -> usage_def
std::unordered_map<uint32_t, usage_def_t> our_usages_flat;

std::vector<uint32_t> layer_triggering_stickies;
std::vector<uint64_t> sticky_usages;  // non-layer triggering, layer << 32 | usage

// report_id -> ...
std::unordered_map<uint8_t, uint8_t*> reports;
std::unordered_map<uint8_t, uint8_t*> prev_reports;
std::unordered_map<uint8_t, uint8_t*> report_masks_relative;
std::unordered_map<uint8_t, uint8_t*> report_masks_absolute;
std::unordered_map<uint8_t, uint16_t> report_sizes;

std::vector<uint8_t> report_ids;
int report_id_idx = 0;  // we go through the report IDs in a round-robin manner

// usage -> ...
std::unordered_map<uint32_t, int32_t> input_state;
std::unordered_map<uint32_t, int32_t> prev_input_state;
std::unordered_map<uint64_t, int32_t> sticky_state;  // layer << 32 | usage -> state
std::unordered_map<uint32_t, int32_t> accumulated;   // * 1000

std::vector<uint32_t> relative_usages;

std::unordered_map<uint32_t, int32_t> accumulated_scroll;
std::unordered_map<uint32_t, uint64_t> last_scroll_timestamp;

uint64_t next_timestamp;

int32_t handle_scroll(uint32_t source_usage, uint32_t target_usage, int32_t movement) {
    int32_t ret = 0;
    if (resolution_multiplier & resolution_multiplier_masks.at(target_usage)) {  // hi-res
        ret = movement;
    } else {  // lo-res
        if (movement != 0) {
            last_scroll_timestamp[source_usage] = time_us_64();
            accumulated_scroll[source_usage] += movement;
            int ticks = accumulated_scroll[source_usage] / (1000 * RESOLUTION_MULTIPLIER);
            accumulated_scroll[source_usage] -= ticks * (1000 * RESOLUTION_MULTIPLIER);
            ret = ticks * 1000;
        } else {
            if ((accumulated_scroll[source_usage] != 0) &&
                (time_us_64() - last_scroll_timestamp[source_usage] > partial_scroll_timeout)) {
                accumulated_scroll[source_usage] = 0;
            }
        }
    }
    return ret;
}

inline int8_t get_bit(const uint8_t* data, int len, uint16_t bitpos) {
    int byte_no = bitpos / 8;
    int bit_no = bitpos % 8;
    if (byte_no < len) {
        return (data[byte_no] & 1 << bit_no) ? 1 : 0;
    }
    return 0;
}

inline uint32_t get_bits(const uint8_t* data, int len, uint16_t bitpos, uint8_t size) {
    uint32_t value = 0;
    for (int i = 0; i < size; i++) {
        value |= get_bit(data, len, bitpos + i) << i;
    }
    return value;
}

inline void put_bit(uint8_t* data, int len, uint16_t bitpos, uint8_t value) {
    int byte_no = bitpos / 8;
    int bit_no = bitpos % 8;
    if (byte_no < len) {
        data[byte_no] &= ~(1 << bit_no);
        data[byte_no] |= (value & 1) << bit_no;
    }
}

inline void put_bits(uint8_t* data, int len, uint16_t bitpos, uint8_t size, uint32_t value) {
    for (int i = 0; i < size; i++) {
        put_bit(data, len, bitpos + i, (value >> i) & 1);
    }
}

bool needs_to_be_sent(uint8_t report_id) {
    uint8_t* report = reports[report_id];
    uint8_t* prev_report = prev_reports[report_id];
    uint8_t* relative = report_masks_relative[report_id];
    uint8_t* absolute = report_masks_absolute[report_id];

    for (int i = 0; i < report_sizes[report_id]; i++) {
        if ((report[i] & relative[i]) || ((report[i] & absolute[i]) != (prev_report[i] & absolute[i]))) {
            return true;
        }
    }
    return false;
}

void set_mapping_from_config() {
    std::unordered_set<uint32_t> layer_triggering_sticky_set;
    std::unordered_set<uint64_t> sticky_usage_set;
    std::unordered_set<uint32_t> mapped;

    reverse_mapping.clear();

    for (auto const& mapping : config_mappings) {
        reverse_mapping[mapping.target_usage].push_back((map_source_t){
            .usage = mapping.source_usage,
            .scaling = mapping.scaling,
            .sticky = (mapping.flags & MAPPING_FLAG_STICKY) != 0,
            .layer = mapping.layer,
        });
        if (mapping.layer == 0) {
            mapped.insert(mapping.source_usage);
        }
        if ((mapping.flags & MAPPING_FLAG_STICKY) != 0) {
            if ((mapping.target_usage & 0xFFFF0000) == LAYERS_USAGE_PAGE) {
                layer_triggering_sticky_set.insert(mapping.source_usage);
            } else {
                sticky_usage_set.insert(((uint64_t) mapping.layer << 32) | mapping.source_usage);
            }
        }
    }

    layer_triggering_stickies.assign(layer_triggering_sticky_set.begin(), layer_triggering_sticky_set.end());
    sticky_usages.assign(sticky_usage_set.begin(), sticky_usage_set.end());

    if (unmapped_passthrough) {
        for (auto const& [usage, usage_def] : our_usages_flat) {
            if (!mapped.count(usage)) {
                reverse_mapping[usage].push_back((map_source_t){ .usage = usage });
            }
        }
    }
}

void process_mapping() {
    if (suspended) {
        return;
    }

    for (auto const& usage : layer_triggering_stickies) {
        if ((prev_input_state[usage] == 0) && (input_state[usage] != 0)) {
            sticky_state[usage] = !sticky_state[usage];
        }
        prev_input_state[usage] = input_state[usage];
    }

    static std::unordered_map<uint8_t, bool> layer_state;
    // layer triggers work on all layers (no matter what layer they are defined on)
    // they can be sticky
    layer_state[0] = true;
    for (int i = 1; i < NLAYERS; i++) {
        layer_state[i] = false;
        for (auto const& map_source : reverse_mapping[LAYERS_USAGE_PAGE | i]) {
            if (map_source.sticky ? sticky_state[map_source.usage] : input_state[map_source.usage]) {
                layer_state[i] = true;
                layer_state[0] = false;
                break;
            }
        }
    }

    for (auto const& layer_usage : sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        uint32_t layer = layer_usage >> 32;
        if (layer_state[layer]) {
            if ((prev_input_state[usage] == 0) && (input_state[usage] != 0)) {
                sticky_state[layer_usage] = !sticky_state[layer_usage];
            }
        }
        prev_input_state[usage] = input_state[usage];
    }

    for (auto const& [target, sources] : reverse_mapping) {
        auto search = our_usages_flat.find(target);
        if (search == our_usages_flat.end()) {
            continue;
        }
        const usage_def_t& our_usage = search->second;
        if (our_usage.is_relative) {
            for (auto const& map_source : sources) {
                int32_t value = 0;
                if (map_source.sticky) {
                    value = sticky_state[((uint64_t) map_source.layer << 32) | map_source.usage] * map_source.scaling;
                } else {
                    if (layer_state[map_source.layer]) {
                        value = input_state[map_source.usage] * map_source.scaling;
                    }
                }
                if (value != 0) {
                    if (target == V_SCROLL_USAGE || target == H_SCROLL_USAGE) {
                        accumulated[target] += handle_scroll(map_source.usage, target, value * RESOLUTION_MULTIPLIER);
                    } else {
                        accumulated[target] += value;
                    }
                }
            }
        } else {
            int32_t value = 0;
            for (auto const& map_source : sources) {
                if (map_source.sticky && (sticky_state[((uint64_t) map_source.layer << 32) | map_source.usage] != 0)) {
                    value = sticky_state[((uint64_t) map_source.layer << 32) | map_source.usage];
                } else {
                    if ((layer_state[map_source.layer]) &&
                        (input_state[map_source.usage] != 0)) {
                        value = input_state[map_source.usage];
                    }
                }
            }
            put_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size, value);
        }
    }

    for (auto usage : relative_usages) {
        input_state[usage] = 0;
    }

    for (auto& [usage, accumulated_val] : accumulated) {
        if (accumulated_val == 0) {
            continue;
        }
        usage_def_t& our_usage = our_usages_flat[usage];
        int32_t existing_val = get_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size);
        if (our_usage.logical_minimum < 0) {
            if (existing_val & (1 << (our_usage.size - 1))) {
                existing_val |= 0xFFFFFFFF << our_usage.size;
            }
        }
        int32_t truncated = accumulated_val / 1000;
        accumulated_val -= truncated * 1000;
        if (truncated != 0) {
            put_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size, existing_val + truncated);
        }
    }
}

void send_report() {
    if (suspended || !tud_hid_ready()) {
        return;
    }

    for (uint i = 0; i < report_ids.size(); i++) {
        uint8_t report_id = report_ids[(report_id_idx + i) % report_ids.size()];
        if (needs_to_be_sent(report_id)) {
            tud_hid_report(report_id, reports[report_id], report_sizes[report_id]);
            memcpy(prev_reports[report_id], reports[report_id], report_sizes[report_id]);
            memset(reports[report_id], 0, report_sizes[report_id]);
            report_id_idx = (report_id_idx + 1) % report_ids.size();
            break;
        }
    }
}

inline void read_input(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage) {
    int32_t value = 0;
    if (their_usage.is_array) {
        for (uint i = 0; i < their_usage.count; i++) {
            if (get_bits(report, len, their_usage.bitpos + i * their_usage.size, their_usage.size) == their_usage.index) {
                value = 1;
                break;
            }
        }
    } else {
        value = get_bits(report, len, their_usage.bitpos, their_usage.size);
        if (their_usage.logical_minimum < 0) {
            if (value & (1 << (their_usage.size - 1))) {
                value |= 0xFFFFFFFF << their_usage.size;
            }
        }
    }

    input_state[source_usage] = value;
}

void handle_received_report(uint8_t* report, int len, uint8_t interface) {
    uint8_t report_id = 0;
    if (has_report_id_theirs[interface]) {
        report_id = report[0];
        report++;
        len--;
    }

    for (auto const& [their_usage, their_usage_def] : their_usages[interface][report_id]) {
        read_input(report, len, their_usage, their_usage_def);
    }
}

void clear_their_descriptor_derivates() {
    their_usages.clear();
    their_usages_rle.clear();
    has_report_id_theirs.clear();
    accumulated.clear();
    input_state.clear();
    prev_input_state.clear();
    sticky_state.clear();
    relative_usages.clear();
}

void rlencode(const std::set<uint32_t>& usages, std::vector<usage_rle_t>& output) {
    uint32_t start_usage = 0;
    uint32_t count = 0;
    for (auto const& usage : usages) {
        if (start_usage == 0) {
            start_usage = usage;
            count = 1;
            continue;
        }
        if (usage == start_usage + count) {
            count++;
        } else {
            output.push_back({ .usage = start_usage, .count = count });
            start_usage = usage;
            count = 1;
        }
    }
    if (start_usage != 0) {
        output.push_back({ .usage = start_usage, .count = count });
    }
}

void update_their_descriptor_derivates() {
    std::set<uint32_t> their_usages_set;
    for (auto const& [interface, report_id_usage_map] : their_usages) {
        for (auto const& [report_id, usage_map] : report_id_usage_map) {
            for (auto const& [usage, usage_def] : usage_map) {
                their_usages_set.insert(usage);
                if (usage_def.is_relative) {
                    relative_usages.push_back(usage);
                }
            }
        }
    }

    rlencode(their_usages_set, their_usages_rle);
}

void parse_our_descriptor() {
    bool has_report_id_ours;
    report_sizes = parse_descriptor(our_usages, has_report_id_ours, our_report_descriptor, our_report_descriptor_length);
    for (auto const& [report_id, size] : report_sizes) {
        reports[report_id] = new uint8_t[size];
        memset(reports[report_id], 0, size);
        prev_reports[report_id] = new uint8_t[size];
        memset(prev_reports[report_id], 0, size);
        report_masks_relative[report_id] = new uint8_t[size];
        memset(report_masks_relative[report_id], 0, size);
        report_masks_absolute[report_id] = new uint8_t[size];
        memset(report_masks_absolute[report_id], 0, size);

        report_ids.push_back(report_id);
    }

    std::set<uint32_t> our_usages_set;
    for (auto const& [report_id, usage_map] : our_usages) {
        for (auto const& [usage, usage_def] : usage_map) {
            our_usages_flat[usage] = usage_def;
            our_usages_set.insert(usage);

            if (usage_def.is_relative) {
                put_bits(report_masks_relative[report_id], report_sizes[report_id], usage_def.bitpos, usage_def.size, 0xFFFFFFFF);
            } else {
                put_bits(report_masks_absolute[report_id], report_sizes[report_id], usage_def.bitpos, usage_def.size, 0xFFFFFFFF);
            }
        }
    }

    rlencode(our_usages_set, our_usages_rle);
}

void sync_rate() {
    uint64_t now = time_us_64();
    while (next_timestamp < now) {
        next_timestamp += 1000;
    }
    while (time_us_64() < next_timestamp) {
    }
    next_timestamp += 1000;
}

int main() {
    launch_pio_usb();
    parse_our_descriptor();
    load_config();
    board_init();
    tusb_init();

    next_timestamp = time_us_64() + 1000;

    while (true) {
        pio_usb_task(handle_received_report);
        process_mapping();
        sync_rate();
        tud_task();
        send_report();

        if (need_to_clear_descriptor_data) {
            clear_their_descriptor_derivates();
            need_to_clear_descriptor_data = false;
        }
        if (their_descriptor_updated) {
            update_their_descriptor_derivates();
            their_descriptor_updated = false;
        }
        if (need_to_persist_config) {
            persist_config();
            need_to_persist_config = false;
        }
    }

    return 0;
}
