#include <cstdio>
#include <cstring>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "crc.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

#define MAX_REPORT_SIZE 64

const uint8_t MAPPING_FLAG_STICKY = 0x01;

const uint8_t V_RESOLUTION_BITMASK = (1 << 0);
const uint8_t H_RESOLUTION_BITMASK = (1 << 2);
const uint32_t V_SCROLL_USAGE = 0x00010038;
const uint32_t H_SCROLL_USAGE = 0x000C0238;

const uint8_t NLAYERS = 4;
const uint32_t LAYERS_USAGE_PAGE = 0xFFF10000;
const uint32_t MACRO_USAGE_PAGE = 0xFFF20000;

const std::unordered_map<uint32_t, uint8_t> resolution_multiplier_masks = {
    { V_SCROLL_USAGE, V_RESOLUTION_BITMASK },
    { H_SCROLL_USAGE, H_RESOLUTION_BITMASK },
};

std::unordered_map<uint32_t, std::vector<map_source_t>> reverse_mapping;  // target -> sources list

std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>> our_usages;  // report_id -> usage -> usage_def
std::unordered_map<uint32_t, usage_def_t> our_usages_flat;

std::vector<uint64_t> layer_triggering_stickies;  // layer_mask << 32 | usage
std::vector<uint64_t> sticky_usages;              // non-layer triggering, layer_mask << 32 | usage

// report_id -> ...
uint8_t* reports[MAX_INPUT_REPORT_ID + 1];
uint8_t* prev_reports[MAX_INPUT_REPORT_ID + 1];
uint8_t* report_masks_relative[MAX_INPUT_REPORT_ID + 1];
uint8_t* report_masks_absolute[MAX_INPUT_REPORT_ID + 1];
uint16_t report_sizes[MAX_INPUT_REPORT_ID + 1];

#define OR_BUFSIZE 8
uint8_t outgoing_reports[OR_BUFSIZE][MAX_REPORT_SIZE + 1];
uint8_t or_head = 0;
uint8_t or_tail = 0;
uint8_t or_items = 0;

std::vector<uint8_t> report_ids;

// usage -> ...
std::unordered_map<uint32_t, int32_t> input_state;
std::unordered_map<uint32_t, int32_t> prev_input_state;
std::unordered_map<uint32_t, int8_t> sticky_state;  // usage -> state per layer (mask)
std::unordered_map<uint32_t, int32_t> accumulated;  // * 1000
uint8_t layer_state_mask = 1;

std::vector<uint32_t> relative_usages;
std::unordered_set<uint32_t> relative_usage_set;

std::queue<std::vector<uint32_t>> macro_queue;
std::vector<uint64_t> macro_usages;  // layer_mask << 32 | usage

std::unordered_map<uint32_t, int32_t> accumulated_scroll;
std::unordered_map<uint32_t, uint64_t> last_scroll_timestamp;

uint32_t reports_received;
uint32_t reports_sent;

int32_t handle_scroll(uint32_t source_usage, uint32_t target_usage, int32_t movement) {
    int32_t ret = 0;
    if (resolution_multiplier & resolution_multiplier_masks.at(target_usage)) {  // hi-res
        ret = movement;
    } else {  // lo-res
        if (movement != 0) {
            last_scroll_timestamp[source_usage] = get_time();
            accumulated_scroll[source_usage] += movement;
            int ticks = accumulated_scroll[source_usage] / (1000 * RESOLUTION_MULTIPLIER);
            accumulated_scroll[source_usage] -= ticks * (1000 * RESOLUTION_MULTIPLIER);
            ret = ticks * 1000;
        } else {
            if ((accumulated_scroll[source_usage] != 0) &&
                (get_time() - last_scroll_timestamp[source_usage] > partial_scroll_timeout)) {
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
    std::unordered_set<uint64_t> layer_triggering_sticky_set;
    std::unordered_set<uint64_t> sticky_usage_set;
    std::unordered_set<uint64_t> macro_usage_set;
    std::unordered_map<uint32_t, uint8_t> mapped_on_layers;  // usage -> layer mask

    reverse_mapping.clear();

    for (auto const& mapping : config_mappings) {
        uint8_t layer_mask = mapping.layer_mask;
        if ((mapping.target_usage & 0xFFFF0000) == LAYERS_USAGE_PAGE) {
            // layer-triggering mappings are forced to be present on the layer they trigger
            layer_mask |= (1 << (mapping.target_usage & 0xFFFF)) & ((1 << NLAYERS) - 1);
        }
        reverse_mapping[mapping.target_usage].push_back((map_source_t){
            .usage = mapping.source_usage,
            .scaling = mapping.scaling,
            .sticky = (mapping.flags & MAPPING_FLAG_STICKY) != 0,
            .layer_mask = layer_mask,
        });
        mapped_on_layers[mapping.source_usage] |= layer_mask;
        if ((mapping.target_usage & 0xFFFF0000) == MACRO_USAGE_PAGE) {
            macro_usage_set.insert(((uint64_t) layer_mask << 32) | mapping.source_usage);
        }
        if ((mapping.flags & MAPPING_FLAG_STICKY) != 0) {
            if ((mapping.target_usage & 0xFFFF0000) == LAYERS_USAGE_PAGE) {
                layer_triggering_sticky_set.insert(((uint64_t) layer_mask << 32) | mapping.source_usage);
            } else {
                sticky_usage_set.insert(((uint64_t) layer_mask << 32) | mapping.source_usage);
            }
        }
    }

    layer_triggering_stickies.assign(layer_triggering_sticky_set.begin(), layer_triggering_sticky_set.end());
    sticky_usages.assign(sticky_usage_set.begin(), sticky_usage_set.end());
    macro_usages.assign(macro_usage_set.begin(), macro_usage_set.end());

    if (unmapped_passthrough_layer_mask) {
        for (auto const& [usage, usage_def] : our_usages_flat) {
            uint8_t unmapped_layers = unmapped_passthrough_layer_mask & ~mapped_on_layers[usage];
            if (unmapped_layers) {
                reverse_mapping[usage].push_back((map_source_t){
                    .usage = usage,
                    .layer_mask = unmapped_layers,
                });
            }
        }
    }
}

bool differ_on_absolute(const uint8_t* report1, const uint8_t* report2, uint8_t report_id) {
    uint8_t* absolute = report_masks_absolute[report_id];

    for (int i = 0; i < report_sizes[report_id]; i++) {
        if ((report1[i] & absolute[i]) != (report2[i] & absolute[i])) {
            return true;
        }
    }

    return false;
}

void aggregate_relative(uint8_t* prev_report, const uint8_t* report, uint8_t report_id) {
    for (auto const& [usage, usage_def] : our_usages[report_id]) {
        if (usage_def.is_relative) {
            int32_t val1 = get_bits(report, report_sizes[report_id], usage_def.bitpos, usage_def.size);
            if (usage_def.logical_minimum < 0) {
                if (val1 & (1 << (usage_def.size - 1))) {
                    val1 |= 0xFFFFFFFF << usage_def.size;
                }
            }
            if (val1) {
                int32_t val2 = get_bits(prev_report, report_sizes[report_id], usage_def.bitpos, usage_def.size);
                if (usage_def.logical_minimum < 0) {
                    if (val2 & (1 << (usage_def.size - 1))) {
                        val2 |= 0xFFFFFFFF << usage_def.size;
                    }
                }

                put_bits(prev_report, report_sizes[report_id], usage_def.bitpos, usage_def.size, val1 + val2);
            }
        }
    }
}

void process_mapping(bool auto_repeat) {
    if (suspended) {
        return;
    }

    for (auto const& layer_usage : layer_triggering_stickies) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        uint32_t layer_mask = layer_usage >> 32;
        if ((layer_mask & layer_state_mask) &&
            (prev_input_state[usage] == 0) && (input_state[usage] != 0)) {
            sticky_state[usage] = !sticky_state[usage];
        }
        prev_input_state[usage] = input_state[usage];
    }

    uint8_t new_layer_state_mask = 0;
    for (int i = 1; i < NLAYERS; i++) {
        for (auto const& map_source : reverse_mapping[LAYERS_USAGE_PAGE | i]) {
            if ((map_source.layer_mask & layer_state_mask) &&
                (map_source.sticky ? sticky_state[map_source.usage] : input_state[map_source.usage])) {
                new_layer_state_mask |= 1 << i;
                break;
            }
        }
    }

    // if no layer is active then layer 0 is active
    if (new_layer_state_mask == 0) {
        new_layer_state_mask = 1;
    }

    layer_state_mask = new_layer_state_mask;

    for (auto const& layer_usage : sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        uint32_t layer_mask = layer_usage >> 32;
        if ((layer_state_mask & layer_mask) &&
            ((prev_input_state[usage] == 0) && (input_state[usage] != 0))) {
            sticky_state[usage] ^= (layer_state_mask & layer_mask);
        }
    }
    for (auto const& layer_usage : sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
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
                bool source_is_relative = relative_usage_set.count(map_source.usage);
                if (auto_repeat || source_is_relative) {
                    int32_t value = 0;
                    if (map_source.sticky) {
                        value = !!(sticky_state[map_source.usage] & map_source.layer_mask) * map_source.scaling;
                    } else {
                        if (layer_state_mask & map_source.layer_mask) {
                            value = (source_is_relative
                                            ? input_state[map_source.usage]
                                            : !!input_state[map_source.usage]) *
                                    map_source.scaling;
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
            }
        } else {
            int32_t value = 0;
            for (auto const& map_source : sources) {
                if (map_source.sticky) {
                    if (sticky_state[map_source.usage] & map_source.layer_mask) {
                        value = 1;
                    }
                } else {
                    if ((layer_state_mask & map_source.layer_mask) &&
                        (relative_usage_set.count(map_source.usage)
                                ? (input_state[map_source.usage] * map_source.scaling > 0)
                                : input_state[map_source.usage])) {
                        value = 1;
                    }
                }
            }
            if (value) {
                put_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size, value);
            }
        }
    }

    // queue triggered macros
    for (int macro = 0; macro < NMACROS; macro++) {
        auto search = reverse_mapping.find(MACRO_USAGE_PAGE | (macro + 1));
        if (search == reverse_mapping.end()) {
            continue;
        }
        auto const& sources = search->second;

        for (auto const& map_source : sources) {
            if ((layer_state_mask & map_source.layer_mask) &&
                ((prev_input_state[map_source.usage] == 0) && (input_state[map_source.usage] != 0))) {
                macros_mutex_enter();
                for (auto const& usages : macros[macro]) {
                    printf("8==D %d\n", usages.size());
                    macro_queue.push(usages);
                }
                macros_mutex_exit();
            }
        }
    }

    for (auto const& layer_usage : macro_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        prev_input_state[usage] = input_state[usage];
    }

    // execute queued macros
    if ((or_items == 0) && !macro_queue.empty()) {
        for (uint32_t usage : macro_queue.front()) {
            auto search = our_usages_flat.find(usage);
            if (search != our_usages_flat.end()) {
                const usage_def_t& our_usage = search->second;
                put_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size, 1);
            }
        }
        macro_queue.pop();
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

    for (unsigned int i = 0; i < report_ids.size(); i++) {  // XXX what order should we go in? maybe keyboard first so that mappings to ctrl-left click work as expected?
        uint8_t report_id = report_ids[i];
        if (needs_to_be_sent(report_id)) {
            if (or_items == OR_BUFSIZE) {
                printf("overflow!\n");
                break;
            }
            uint8_t prev = (or_tail + OR_BUFSIZE - 1) % OR_BUFSIZE;
            if ((or_items > 0) &&
                (outgoing_reports[prev][0] == report_id) &&
                !differ_on_absolute(outgoing_reports[prev] + 1, reports[report_id], report_id)) {
                aggregate_relative(outgoing_reports[prev] + 1, reports[report_id], report_id);
            } else {
                outgoing_reports[or_tail][0] = report_id;
                memcpy(outgoing_reports[or_tail] + 1, reports[report_id], report_sizes[report_id]);
                memcpy(prev_reports[report_id], reports[report_id], report_sizes[report_id]);
                or_tail = (or_tail + 1) % OR_BUFSIZE;
                or_items++;
            }
        }
        memset(reports[report_id], 0, report_sizes[report_id]);
    }
}

bool send_report(send_report_t do_send_report) {
    if (suspended || (or_items == 0)) {
        return false;
    }

    uint8_t report_id = outgoing_reports[or_head][0];

    bool sent = do_send_report(outgoing_reports[or_head], report_sizes[report_id] + 1);

    // XXX even if not sent?
    or_head = (or_head + 1) % OR_BUFSIZE;
    or_items--;

    reports_sent++;

    return sent;
}

inline void read_input(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint16_t interface) {
    int32_t value = 0;
    if (their_usage.is_array) {
        for (unsigned int i = 0; i < their_usage.count; i++) {
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

    if (their_usage.is_relative) {
        input_state[source_usage] = value;
    } else {
        if (value) {
            input_state[source_usage] |= 1 << interface_index[interface];
        } else {
            input_state[source_usage] &= ~(1 << interface_index[interface]);
        }
    }
}

inline void read_input_range(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint16_t interface) {
    // is_array and !is_relative is implied
    for (uint32_t actual_usage = source_usage; actual_usage <= their_usage.usage_maximum; actual_usage++) {
        input_state[actual_usage] &= ~(1 << interface_index[interface]);
    }
    for (unsigned int i = 0; i < their_usage.count; i++) {
        uint32_t bits = get_bits(report, len, their_usage.bitpos + i * their_usage.size, their_usage.size);
        // XXX consider negative indexes
        if ((bits >= their_usage.logical_minimum) &&
            (bits <= their_usage.logical_minimum + their_usage.usage_maximum - source_usage)) {
            uint32_t actual_usage = source_usage + bits - their_usage.logical_minimum;
            input_state[actual_usage] |= 1 << interface_index[interface];
        }
    }
}

void handle_received_report(const uint8_t* report, int len, uint16_t interface) {
    reports_received++;

    usages_mutex_enter();

    uint8_t report_id = 0;
    if (has_report_id_theirs[interface]) {
        report_id = report[0];
        report++;
        len--;
    }

    for (auto const& [their_usage, their_usage_def] : their_usages[interface][report_id]) {
        if (their_usage_def.usage_maximum == 0) {
            read_input(report, len, their_usage, their_usage_def, interface);
        } else {
            read_input_range(report, len, their_usage, their_usage_def, interface);
        }
    }

    usages_mutex_exit();
}

void rlencode(const std::set<uint64_t>& usage_ranges, std::vector<usage_rle_t>& output) {
    uint32_t start_usage = 0;
    uint32_t count = 0;
    for (auto const& range : usage_ranges) {
        uint32_t usage_minimum = range >> 32;
        uint32_t usage_maximum = range & 0xFFFFFFFF;

        if (start_usage == 0) {
            start_usage = usage_minimum;
            count = 1 + usage_maximum - usage_minimum;
            continue;
        }
        if (usage_minimum <= start_usage + count) {
            if (usage_maximum < start_usage + count) {
                continue;
            }
            count += 1 + usage_maximum - (start_usage + count);
        } else {
            output.push_back({ .usage = start_usage, .count = count });
            start_usage = usage_minimum;
            count = 1 + usage_maximum - usage_minimum;
        }
    }
    if (start_usage != 0) {
        output.push_back({ .usage = start_usage, .count = count });
    }
}

void update_their_descriptor_derivates() {
    relative_usages.clear();
    relative_usage_set.clear();
    std::set<uint64_t> their_usage_ranges_set;
    for (auto const& [interface, report_id_usage_map] : their_usages) {
        for (auto const& [report_id, usage_map] : report_id_usage_map) {
            for (auto const& [usage, usage_def] : usage_map) {
                their_usage_ranges_set.insert(((uint64_t) usage << 32) | (usage_def.usage_maximum ? usage_def.usage_maximum : usage));
                if (usage_def.is_relative) {
                    relative_usages.push_back(usage);
                    relative_usage_set.insert(usage);
                }
            }
        }
    }

    their_usages_rle.clear();
    rlencode(their_usage_ranges_set, their_usages_rle);
}

void parse_our_descriptor() {
    bool has_report_id_ours;
    std::unordered_map<uint8_t, uint16_t> report_sizes_map = parse_descriptor(our_usages, has_report_id_ours, our_report_descriptor, our_report_descriptor_length);
    for (auto const& [report_id, size] : report_sizes_map) {
        report_sizes[report_id] = size;
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

    std::set<uint64_t> our_usage_ranges_set;
    for (auto const& [report_id, usage_map] : our_usages) {
        for (auto const& [usage, usage_def] : usage_map) {
            our_usages_flat[usage] = usage_def;
            our_usage_ranges_set.insert(((uint64_t) usage << 32) | (usage_def.usage_maximum ? usage_def.usage_maximum : usage));

            if (usage_def.is_relative) {
                put_bits(report_masks_relative[report_id], report_sizes[report_id], usage_def.bitpos, usage_def.size, 0xFFFFFFFF);
            } else {
                put_bits(report_masks_absolute[report_id], report_sizes[report_id], usage_def.bitpos, usage_def.size, 0xFFFFFFFF);
            }
        }
    }

    rlencode(our_usage_ranges_set, our_usages_rle);
}

void print_stats() {
    printf("%lu %lu\n", reports_received, reports_sent);
    reports_received = 0;
    reports_sent = 0;
}
