#include <cmath>
#include <cstdio>
#include <cstdlib>
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

const uint8_t MAPPING_FLAG_STICKY = 1 << 0;
const uint8_t MAPPING_FLAG_TAP = 1 << 1;
const uint8_t MAPPING_FLAG_HOLD = 1 << 2;

const uint8_t V_RESOLUTION_BITMASK = (1 << 0);
const uint8_t H_RESOLUTION_BITMASK = (1 << 2);
const uint32_t V_SCROLL_USAGE = 0x00010038;
const uint32_t H_SCROLL_USAGE = 0x000C0238;

const uint8_t NLAYERS = 4;
const uint32_t LAYERS_USAGE_PAGE = 0xFFF10000;
const uint32_t MACRO_USAGE_PAGE = 0xFFF20000;
const uint32_t EXPR_USAGE_PAGE = 0xFFF30000;

const uint16_t STACK_SIZE = 16;

const std::unordered_map<uint32_t, uint8_t> resolution_multiplier_masks = {
    { V_SCROLL_USAGE, V_RESOLUTION_BITMASK },
    { H_SCROLL_USAGE, H_RESOLUTION_BITMASK },
};

std::unordered_map<uint32_t, std::vector<map_source_t>> reverse_mapping;  // target -> sources list

std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>> our_usages;  // report_id -> usage -> usage_def
std::unordered_map<uint32_t, usage_def_t> our_usages_flat;

// XXX any reason these are int64 and not a struct?
std::vector<uint64_t> sticky_usages;      // layer_mask << 32 | usage
std::vector<uint64_t> tap_sticky_usages;  // layer_mask << 32 | usage
std::vector<uint32_t> tap_hold_usages;
std::vector<uint32_t> macro_usages;

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
std::unordered_map<uint32_t, int8_t> tap_state;
std::unordered_map<uint32_t, int8_t> hold_state;
std::unordered_map<uint32_t, int32_t> accumulated;  // * 1000
std::unordered_map<uint32_t, uint64_t> pressed_at;  // usage -> timestamp
uint8_t layer_state_mask = 1;

std::vector<uint32_t> relative_usages;
std::unordered_set<uint32_t> relative_usage_set;

std::queue<std::vector<uint32_t>> macro_queue;

std::unordered_map<uint32_t, int32_t> accumulated_scroll;
std::unordered_map<uint32_t, uint64_t> last_scroll_timestamp;

uint32_t reports_received;
uint32_t reports_sent;

bool expression_valid[NEXPRESSIONS] = { false };

uint8_t monitor_usages_queued = 0;
monitor_report_t monitor_report = { .report_id = REPORT_ID_MONITOR };

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

bool is_expr_valid(uint8_t expr) {
    int16_t on_stack = 0;
    for (auto const& elem : expressions[expr]) {
        // should we have a data structure with each op's input/output instead?
        switch (elem.op) {
            case Op::DEBUG:
                break;
            case Op::PUSH:
            case Op::PUSH_USAGE:
            case Op::AUTO_REPEAT:
            case Op::TIME:
            case Op::SCALING:
            case Op::LAYER_STATE:
                if (on_stack >= STACK_SIZE) {
                    return false;
                }
                on_stack++;
                break;
            case Op::NOT:
            case Op::INPUT_STATE:
            case Op::INPUT_STATE_BINARY:
            case Op::ABS:
            case Op::SIN:
            case Op::COS:
            case Op::RELU:
            case Op::STICKY_STATE:
            case Op::TAP_STATE:
            case Op::HOLD_STATE:
            case Op::BITWISE_NOT:
                if (on_stack < 1) {
                    return false;
                }
                break;
            case Op::DUP:
                if ((on_stack < 1) || (on_stack >= STACK_SIZE)) {
                    return false;
                }
                on_stack++;
                break;
            case Op::ADD:
            case Op::MUL:
            case Op::EQ:
            case Op::GT:
            case Op::MOD:
            case Op::BITWISE_OR:
            case Op::BITWISE_AND:
                if (on_stack < 2) {
                    return false;
                }
                on_stack--;
                break;
            case Op::CLAMP:
                if (on_stack < 3) {
                    return false;
                }
                on_stack -= 2;
                break;
            default:
                printf("unknown op in expression_valid()\n");
                return false;
        }
    }
    return true;
}

void validate_expressions() {
    for (uint8_t i = 0; i < NEXPRESSIONS; i++) {
        expression_valid[i] = is_expr_valid(i);
    }
}

void set_mapping_from_config() {
    std::unordered_map<uint32_t, uint8_t> sticky_usage_map;
    std::unordered_map<uint32_t, uint8_t> tap_sticky_usage_map;
    std::unordered_set<uint32_t> tap_hold_usage_set;
    std::unordered_set<uint32_t> macro_usage_set;
    std::unordered_map<uint32_t, uint8_t> mapped_on_layers;  // usage -> layer mask

    validate_expressions();

    reverse_mapping.clear();

    for (auto const& mapping : config_mappings) {
        uint8_t layer_mask = mapping.layer_mask;
        if ((mapping.target_usage & 0xFFFF0000) == LAYERS_USAGE_PAGE) {
            uint16_t layer = mapping.target_usage & 0xFFFF;
            if (mapping.flags & MAPPING_FLAG_STICKY) {
                // sticky layer-triggering mappings are forces to NOT be present on the layer they trigger
                layer_mask &= ~(1 << layer);
                // but for unmapped passthrough purposes we pretend they are
                mapped_on_layers[mapping.source_usage] |= (1 << layer) & ((1 << NLAYERS) - 1);
            } else {
                // non-sticky layer-triggering mappings are forced to BE present on the layer they trigger
                layer_mask |= (1 << layer) & ((1 << NLAYERS) - 1);
            }
        }
        reverse_mapping[mapping.target_usage].push_back((map_source_t){
            .usage = mapping.source_usage,
            .scaling = mapping.scaling,
            .sticky = (mapping.flags & MAPPING_FLAG_STICKY) != 0,
            .layer_mask = layer_mask,
            .tap = (mapping.flags & MAPPING_FLAG_TAP) != 0,
            .hold = (mapping.flags & MAPPING_FLAG_HOLD) != 0,
        });
        // if a usage appears in an expression, consider it mapped
        if ((mapping.source_usage & 0xFFFF0000) == EXPR_USAGE_PAGE) {
            uint8_t expr = (mapping.source_usage & 0xFFFF) - 1;
            for (auto const& elem : expressions[expr]) {
                if (elem.op == Op::PUSH_USAGE) {
                    mapped_on_layers[elem.val] |= layer_mask;
                }
            }
        }
        mapped_on_layers[mapping.source_usage] |= layer_mask;
        if ((mapping.flags & MAPPING_FLAG_STICKY) != 0) {
            if (mapping.flags & MAPPING_FLAG_TAP) {
                tap_sticky_usage_map[mapping.source_usage] |= layer_mask;
            } else {
                sticky_usage_map[mapping.source_usage] |= layer_mask;
            }
        }
        if (((mapping.flags & MAPPING_FLAG_TAP) != 0) ||
            ((mapping.flags & MAPPING_FLAG_HOLD) != 0)) {
            tap_hold_usage_set.insert(mapping.source_usage);
        }
        if ((mapping.target_usage & 0xFFFF0000) == MACRO_USAGE_PAGE) {
            macro_usage_set.insert(mapping.source_usage);
        }
    }

    sticky_usages.clear();
    tap_hold_usages.clear();
    tap_sticky_usages.clear();
    macro_usages.clear();

    for (auto const& [usage, layer_mask] : sticky_usage_map) {
        sticky_usages.push_back(((uint64_t) layer_mask << 32) | usage);
    }

    for (auto const& [usage, layer_mask] : tap_sticky_usage_map) {
        tap_sticky_usages.push_back(((uint64_t) layer_mask << 32) | usage);
    }

    for (auto const usage : tap_hold_usage_set) {
        tap_hold_usages.push_back(usage);
    }

    for (auto const usage : macro_usage_set) {
        macro_usages.push_back(usage);
    }

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

int32_t eval_expr(const map_source_t& map_source, uint64_t now, bool auto_repeat) {
    static int32_t stack[STACK_SIZE];
    bool debug = false;
    int16_t ptr = -1;
    uint8_t expr = (map_source.usage & 0xFFFFF) - 1;
    if (expr >= NEXPRESSIONS) {
        return 0;
    }
    if (!expression_valid[expr]) {
        return 0;
    }
    for (auto const& elem : expressions[expr]) {
        switch (elem.op) {
            case Op::PUSH:
            case Op::PUSH_USAGE:
                stack[++ptr] = elem.val;
                break;
            case Op::INPUT_STATE:
                stack[ptr] = input_state[stack[ptr]] * 1000;
                break;
            case Op::ADD:
                stack[ptr - 1] = stack[ptr - 1] + stack[ptr];
                ptr--;
                break;
            case Op::MUL:
                stack[ptr - 1] = stack[ptr - 1] * stack[ptr] / 1000;
                ptr--;
                break;
            case Op::EQ:
                stack[ptr - 1] = (stack[ptr - 1] == stack[ptr]) * 1000;
                ptr--;
                break;
            case Op::TIME:
                stack[++ptr] = now & 0x7fffffff;
                break;
            case Op::MOD:
                stack[ptr - 1] = stack[ptr - 1] % stack[ptr];
                ptr--;
                break;
            case Op::GT:
                stack[ptr - 1] = (stack[ptr - 1] > stack[ptr]) * 1000;
                ptr--;
                break;
            case Op::NOT:
                stack[ptr] = (!stack[ptr]) * 1000;
                break;
            case Op::INPUT_STATE_BINARY:
                stack[ptr] = (!!input_state[stack[ptr]]) * 1000;
                break;
            case Op::ABS:
                stack[ptr] = labs(stack[ptr]);
                break;
            case Op::DUP:
                stack[ptr + 1] = stack[ptr];
                ptr++;
                break;
            case Op::SIN:
                stack[ptr] = sinf((float) stack[ptr] * 3.14159265f / 180000.0f) * 1000;
                break;
            case Op::COS:
                stack[ptr] = cosf((float) stack[ptr] * 3.14159265f / 180000.0f) * 1000;
                break;
            case Op::DEBUG:
                debug = true;
                printf("\nexpr %d\n", expr + 1);
                break;
            case Op::AUTO_REPEAT:
                stack[++ptr] = auto_repeat ? 1000 : 0;
                break;
            case Op::RELU:
                if (stack[ptr] < 0) {
                    stack[ptr] = 0;
                }
                break;
            case Op::SCALING:
                stack[++ptr] = map_source.scaling;
                break;
            case Op::CLAMP:
                if (stack[ptr - 2] < stack[ptr - 1]) {
                    stack[ptr - 2] = stack[ptr - 1];
                }
                if (stack[ptr - 2] > stack[ptr]) {
                    stack[ptr - 2] = stack[ptr];
                }
                ptr -= 2;
                break;
            case Op::LAYER_STATE:
                stack[++ptr] = layer_state_mask;
                break;
            case Op::STICKY_STATE:
                stack[ptr] = sticky_state[stack[ptr]];
                break;
            case Op::TAP_STATE:
                stack[ptr] = tap_state[stack[ptr]] * 1000;
                break;
            case Op::HOLD_STATE:
                stack[ptr] = hold_state[stack[ptr]] * 1000;
                break;
            case Op::BITWISE_OR:
                stack[ptr - 1] = stack[ptr - 1] | stack[ptr];
                ptr--;
                break;
            case Op::BITWISE_AND:
                stack[ptr - 1] = stack[ptr - 1] & stack[ptr];
                ptr--;
                break;
            case Op::BITWISE_NOT:
                stack[ptr] = ~stack[ptr];
                break;
            default:
                printf("unknown op!\n");
                return 0;
        }
        if (debug) {
            for (int i = 0; i <= ptr; i++) {
                printf("0x%08lx ", stack[i]);
            }
            printf("\n");
        }
    }
    if (ptr >= 0) {
        return stack[ptr];
    }
    return 0;
}

void process_mapping(bool auto_repeat) {
    if (suspended) {
        return;
    }

    uint64_t now = get_time();

    for (auto const usage : tap_hold_usages) {
        tap_state[usage] =
            (input_state[usage] == 0) && (prev_input_state[usage] != 0) &&
            (now - pressed_at[usage] < tap_hold_threshold);
        hold_state[usage] =
            (input_state[usage] != 0) && (prev_input_state[usage] != 0) &&
            (now - pressed_at[usage] >= tap_hold_threshold);
        if ((input_state[usage] != 0) && (prev_input_state[usage] == 0)) {
            pressed_at[usage] = now;
        }
    }

    for (auto const& layer_usage : sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        uint32_t layer_mask = layer_usage >> 32;
        if ((layer_state_mask & layer_mask) &&
            ((prev_input_state[usage] == 0) && (input_state[usage] != 0))) {
            sticky_state[usage] ^= (layer_state_mask & layer_mask);
        }
    }

    for (auto const& layer_usage : tap_sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        uint32_t layer_mask = layer_usage >> 32;
        if ((layer_state_mask & layer_mask) && tap_state[usage]) {
            sticky_state[usage] ^= (layer_state_mask & layer_mask);
        }
    }

    uint8_t new_layer_state_mask = 0;
    for (int i = 1; i < NLAYERS; i++) {
        for (auto const& map_source : reverse_mapping[LAYERS_USAGE_PAGE | i]) {
            if (!map_source.sticky) {
                if ((map_source.layer_mask & layer_state_mask) &&
                    (map_source.hold
                            ? hold_state[map_source.usage]
                            : input_state[map_source.usage])) {
                    new_layer_state_mask |= 1 << i;
                }
            } else {  // is sticky
                // This part is responsible for deactivating a layer if it was activated
                // by a sticky mapping and the user pressed the button again.
                // There must be a better way of handling this.
                if (((!map_source.tap && (prev_input_state[map_source.usage] == 0) && (input_state[map_source.usage] != 0)) ||
                        (map_source.tap && tap_state[map_source.usage])) &&
                    (sticky_state[map_source.usage] & map_source.layer_mask) &&
                    (layer_state_mask & (1 << i))) {
                    sticky_state[map_source.usage] &= ~map_source.layer_mask;
                }

                // Sticky mapping works even if it's not present on the currently active layers.
                if (sticky_state[map_source.usage] & map_source.layer_mask) {
                    new_layer_state_mask |= 1 << i;
                }
            }
        }
    }

    // if no layer is active then layer 0 is active
    if (new_layer_state_mask == 0) {
        new_layer_state_mask = 1;
    }

    layer_state_mask = new_layer_state_mask;

    // queue triggered macros
    for (int macro = 0; macro < NMACROS; macro++) {
        auto search = reverse_mapping.find(MACRO_USAGE_PAGE | (macro + 1));
        if (search == reverse_mapping.end()) {
            continue;
        }
        auto const& sources = search->second;

        for (auto const& map_source : sources) {
            if ((layer_state_mask & map_source.layer_mask) &&
                ((!map_source.tap && (prev_input_state[map_source.usage] == 0) && (input_state[map_source.usage] != 0)) ||
                    (map_source.tap && tap_state[map_source.usage]))) {
                my_mutex_enter(MutexId::MACROS);
                for (auto const& usages : macros[macro]) {
                    macro_queue.push(usages);
                }
                my_mutex_exit(MutexId::MACROS);
            }
        }
    }

    for (auto const& layer_usage : sticky_usages) {
        uint32_t usage = layer_usage & 0xFFFFFFFF;
        prev_input_state[usage] = input_state[usage];
    }

    for (auto const usage : tap_hold_usages) {
        prev_input_state[usage] = input_state[usage];
    }

    for (auto const usage : macro_usages) {
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
                if ((map_source.usage & 0xFFFF0000) == EXPR_USAGE_PAGE) {
                    if (layer_state_mask & map_source.layer_mask) {
                        value = eval_expr(map_source, now, auto_repeat);
                    }
                } else {
                    bool source_is_relative = relative_usage_set.count(map_source.usage);
                    if (auto_repeat || source_is_relative) {
                        if (map_source.sticky) {
                            value = !!(sticky_state[map_source.usage] & map_source.layer_mask) * map_source.scaling;
                        } else {
                            if (layer_state_mask & map_source.layer_mask) {
                                int32_t state = map_source.hold ? hold_state[map_source.usage] : input_state[map_source.usage];
                                value = (source_is_relative ? state : !!state) * map_source.scaling;
                            }
                        }
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
        } else {  // our_usage is absolute
            int32_t value = 0;
            for (auto const& map_source : sources) {
                if ((map_source.usage & 0xFFFF0000) == EXPR_USAGE_PAGE) {
                    if (layer_state_mask & map_source.layer_mask) {
                        value = eval_expr(map_source, now, auto_repeat) / 1000;
                    }
                } else {
                    if (map_source.sticky) {
                        if (sticky_state[map_source.usage] & map_source.layer_mask) {
                            value = 1;
                        }
                    } else {
                        if ((layer_state_mask & map_source.layer_mask)) {
                            if ((map_source.tap && tap_state[map_source.usage]) ||
                                (map_source.hold && hold_state[map_source.usage])) {
                                value = 1;
                            }
                            if ((!map_source.tap && !map_source.hold) &&
                                (relative_usage_set.count(map_source.usage)
                                        ? (input_state[map_source.usage] * map_source.scaling > 0)
                                        : input_state[map_source.usage])) {
                                value = 1;
                            }
                        }
                    }
                }
            }
            if (value) {
                put_bits((uint8_t*) reports[our_usage.report_id], report_sizes[our_usage.report_id], our_usage.bitpos, our_usage.size, value);
            }
        }
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

    bool sent = do_send_report(0, outgoing_reports[or_head], report_sizes[report_id] + 1);

    // XXX even if not sent?
    or_head = (or_head + 1) % OR_BUFSIZE;
    or_items--;

    reports_sent++;

    return sent;
}

void send_monitor_report(send_report_t do_send_report) {
    if ((monitor_usages_queued == 0) || suspended) {
        return;
    }

    do_send_report(1, (uint8_t*) &monitor_report, sizeof(monitor_report));

    memset(&(monitor_report.usage_values), 0, sizeof(monitor_report.usage_values));
    monitor_usages_queued = 0;
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
        if (monitor_enabled && (value != 0)) {
            monitor_usage(source_usage, value);
        }
    } else {
        if (their_usage.size == 1) {
            if (monitor_enabled && (value != (1 & (input_state[source_usage] >> interface_index[interface])))) {
                monitor_usage(source_usage, value);
            }
            if (value) {
                input_state[source_usage] |= 1 << interface_index[interface];
            } else {
                input_state[source_usage] &= ~(1 << interface_index[interface]);
            }
        } else {
            if (monitor_enabled && (value != input_state[source_usage])) {
                monitor_usage(source_usage, value);
            }
            input_state[source_usage] = value;
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
            // for array inputs, "key-up" events (value=0) don't show up in the monitor
            if (monitor_enabled && ((actual_usage & 0xFFFF) != 0)) {
                monitor_usage(actual_usage, 1);
            }
        }
    }
}

void handle_received_report(const uint8_t* report, int len, uint16_t interface) {
    reports_received++;

    my_mutex_enter(MutexId::THEIR_USAGES);

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

    my_mutex_exit(MutexId::THEIR_USAGES);
}

void set_input_state(uint32_t usage, int32_t state) {
    input_state[usage] = state;
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

void monitor_usage(uint32_t usage, int32_t value) {
    if (monitor_usages_queued == sizeof(monitor_report.usage_values) / sizeof(monitor_report.usage_values[0])) {
        return;
    }
    monitor_report.usage_values[monitor_usages_queued++] = { .usage = usage, .value = value };
}
