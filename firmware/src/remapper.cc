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

std::vector<reverse_mapping_t> reverse_mapping;
std::vector<reverse_mapping_t> reverse_mapping_macros;
std::vector<reverse_mapping_t> reverse_mapping_layers;

std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>> our_usages;  // report_id -> usage -> usage_def
std::unordered_map<uint32_t, usage_def_t> our_usages_flat;

std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::vector<usage_usage_def_t>>> their_used_usages;  // dev_addr+interface -> report_id -> (usage, usage_def)
std::unordered_map<uint16_t, std::unordered_map<uint8_t, std::vector<int32_t*>>> array_range_usages;          // dev_addr+interface -> report_id -> input_state ptr vector

std::vector<usage_layer_mask_input_state_t> sticky_usages;
std::vector<usage_layer_mask_t> tap_sticky_usages;
std::vector<usage_input_state_t> tap_hold_usages;
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

#define MAX_INPUT_STATES 1024
#define PREV_STATE_OFFSET MAX_INPUT_STATES

int32_t input_state[MAX_INPUT_STATES * 2];
std::unordered_map<uint32_t, int32_t*> usage_state_ptr;  // usage -> input_state pointer
uint32_t used_state_slots = 0;

// usage -> ...
std::unordered_map<uint32_t, int8_t> sticky_state;  // usage -> state per layer (mask)
std::unordered_map<uint32_t, int8_t> tap_state;
std::unordered_map<uint32_t, int8_t> hold_state;
std::unordered_map<uint32_t, int32_t> accumulated;  // * 1000
std::unordered_map<uint32_t, uint64_t> pressed_at;  // usage -> timestamp
uint8_t layer_state_mask = 1;

std::vector<int32_t*> relative_usages;  // input_state pointers

std::queue<std::vector<uint32_t>> macro_queue;

std::unordered_map<uint32_t, int32_t> accumulated_scroll;
std::unordered_map<uint32_t, uint64_t> last_scroll_timestamp;

uint32_t reports_received;
uint32_t reports_sent;

bool expression_valid[NEXPRESSIONS] = { false };

std::unordered_map<uint32_t, int32_t> monitor_input_state;
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

bool assign_state_slot(uint32_t usage) {
    if (usage_state_ptr.count(usage) == 0) {
        if (used_state_slots >= MAX_INPUT_STATES) {
            printf("out of input_state slots!");
            return false;
        }

        usage_state_ptr[usage] = input_state + used_state_slots++;
    }
    return true;
}

inline int32_t* get_state_ptr(uint32_t usage) {
    auto search = usage_state_ptr.find(usage);
    if (search != usage_state_ptr.end()) {
        return search->second;
    }

    return NULL;
}

void set_mapping_from_config() {
    std::unordered_map<uint32_t, std::vector<map_source_t>> reverse_mapping_map;  // target -> sources list
    std::unordered_map<uint32_t, uint8_t> sticky_usage_map;
    std::unordered_map<uint32_t, uint8_t> tap_sticky_usage_map;
    std::unordered_set<uint32_t> tap_hold_usage_set;
    std::unordered_set<uint32_t> macro_usage_set;
    std::unordered_map<uint32_t, uint8_t> mapped_on_layers;  // usage -> layer mask

    validate_expressions();

    reverse_mapping.clear();
    reverse_mapping_macros.clear();
    reverse_mapping_layers.clear();
    used_state_slots = 0;
    usage_state_ptr.clear();
    memset(input_state, 0, sizeof(input_state));

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

        if (assign_state_slot(mapping.source_usage)) {
            reverse_mapping_map[mapping.target_usage].push_back((map_source_t){
                .usage = mapping.source_usage,
                .scaling = mapping.scaling,
                .sticky = (mapping.flags & MAPPING_FLAG_STICKY) != 0,
                .layer_mask = layer_mask,
                .tap = (mapping.flags & MAPPING_FLAG_TAP) != 0,
                .hold = (mapping.flags & MAPPING_FLAG_HOLD) != 0,
                .input_state = usage_state_ptr[mapping.source_usage],
            });
        }
        // if a usage appears in an expression, consider it mapped
        if ((mapping.source_usage & 0xFFFF0000) == EXPR_USAGE_PAGE) {
            uint8_t expr = (mapping.source_usage & 0xFFFF) - 1;
            for (auto const& elem : expressions[expr]) {
                if (elem.op == Op::PUSH_USAGE) {
                    mapped_on_layers[elem.val] |= layer_mask;
                    assign_state_slot(elem.val);
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
        int32_t* state_ptr = get_state_ptr(usage);
        if (state_ptr != NULL) {
            sticky_usages.push_back((usage_layer_mask_input_state_t){
                .usage = usage,
                .input_state = state_ptr,
                .layer_mask = layer_mask,
            });
        }
    }

    for (auto const& [usage, layer_mask] : tap_sticky_usage_map) {
        tap_sticky_usages.push_back((usage_layer_mask_t){
            .usage = usage,
            .layer_mask = layer_mask,
        });
    }

    for (auto const usage : tap_hold_usage_set) {
        int32_t* state_ptr = get_state_ptr(usage);
        if (state_ptr != NULL) {
            tap_hold_usages.push_back((usage_input_state_t){
                .usage = usage,
                .input_state = state_ptr,
            });
        }
    }

    for (auto const usage : macro_usage_set) {
        macro_usages.push_back(usage);
    }

    if (unmapped_passthrough_layer_mask) {
        for (auto const& [usage, usage_def] : our_usages_flat) {
            uint8_t unmapped_layers = unmapped_passthrough_layer_mask & ~mapped_on_layers[usage];
            if (unmapped_layers) {
                if (assign_state_slot(usage)) {
                    reverse_mapping_map[usage].push_back((map_source_t){
                        .usage = usage,
                        .layer_mask = unmapped_layers,
                        .input_state = usage_state_ptr[usage],
                    });
                }
            }
        }

        for (auto const& [report_id, usage_map] : their_usages[OUR_OUT_INTERFACE]) {
            for (auto const& [usage, usage_def] : usage_map) {
                uint8_t unmapped_layers = unmapped_passthrough_layer_mask & ~mapped_on_layers[usage];
                if (unmapped_layers) {
                    assign_state_slot(usage);
                    reverse_mapping_map[usage].push_back((map_source_t){
                        .usage = usage,
                        .layer_mask = unmapped_layers,
                        .input_state = usage_state_ptr[usage],
                    });
                }
            }
        }
    }

    for (auto const& [target, sources] : reverse_mapping_map) {
        reverse_mapping_t rev_map = {
            .target = target,
            .sources = sources,
        };
        auto search = our_usages_flat.find(target);
        if (search != our_usages_flat.end()) {
            const usage_def_t& our_usage = search->second;
            rev_map.our_usages.push_back((out_usage_def_t){
                .data = reports[our_usage.report_id],
                .len = report_sizes[our_usage.report_id],
                .size = our_usage.size,
                .bitpos = our_usage.bitpos,
            });
            rev_map.is_relative = our_usage.is_relative;
        }
        if ((target & 0xFFFF0000) == MACRO_USAGE_PAGE) {
            reverse_mapping_macros.push_back(rev_map);
        } else if ((target & 0xFFFF0000) == LAYERS_USAGE_PAGE) {
            reverse_mapping_layers.push_back(rev_map);
        } else {
            reverse_mapping.push_back(rev_map);
        }
    }

    update_their_descriptor_derivates();
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
            case Op::INPUT_STATE: {
                int32_t* state_ptr = get_state_ptr(stack[ptr]);
                stack[ptr] = (state_ptr != NULL) ? *state_ptr * 1000 : 0;
                break;
            }
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
            case Op::INPUT_STATE_BINARY: {
                int32_t* state_ptr = get_state_ptr(stack[ptr]);
                stack[ptr] = (state_ptr != NULL) ? !!(*state_ptr) * 1000 : 0;
                break;
            }
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

    for (auto const tap_hold : tap_hold_usages) {
        tap_state[tap_hold.usage] =
            (*tap_hold.input_state == 0) && (*(tap_hold.input_state + PREV_STATE_OFFSET) != 0) &&
            (now - pressed_at[tap_hold.usage] < tap_hold_threshold);
        hold_state[tap_hold.usage] =
            (*tap_hold.input_state != 0) && (*(tap_hold.input_state + PREV_STATE_OFFSET) != 0) &&
            (now - pressed_at[tap_hold.usage] >= tap_hold_threshold);
        if ((*tap_hold.input_state != 0) && (*(tap_hold.input_state + PREV_STATE_OFFSET) == 0)) {
            pressed_at[tap_hold.usage] = now;
        }
    }

    for (auto const& sticky : sticky_usages) {
        if ((layer_state_mask & sticky.layer_mask) &&
            ((*(sticky.input_state + PREV_STATE_OFFSET) == 0) && (*sticky.input_state != 0))) {
            sticky_state[sticky.usage] ^= (layer_state_mask & sticky.layer_mask);
        }
    }

    for (auto const& tap_sticky : tap_sticky_usages) {
        if ((layer_state_mask & tap_sticky.layer_mask) && tap_state[tap_sticky.usage]) {
            sticky_state[tap_sticky.usage] ^= (layer_state_mask & tap_sticky.layer_mask);
        }
    }

    uint8_t new_layer_state_mask = 0;
    for (auto const& rev_map : reverse_mapping_layers) {
        uint16_t i = rev_map.target & 0xFFFF;
        for (auto const& map_source : rev_map.sources) {
            if (!map_source.sticky) {
                if ((map_source.layer_mask & layer_state_mask) &&
                    (map_source.hold
                            ? hold_state[map_source.usage]
                            : *map_source.input_state)) {
                    new_layer_state_mask |= 1 << i;
                }
            } else {  // is sticky
                // This part is responsible for deactivating a layer if it was activated
                // by a sticky mapping and the user pressed the button again.
                // There must be a better way of handling this.
                if (((!map_source.tap && (*(map_source.input_state + PREV_STATE_OFFSET) == 0) && (*map_source.input_state != 0)) ||
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
    for (auto const& rev_map : reverse_mapping_macros) {
        uint16_t macro = (rev_map.target & 0xFFFF) - 1;
        if (macro >= NMACROS) {
            continue;
        }
        for (auto const& map_source : rev_map.sources) {
            if ((layer_state_mask & map_source.layer_mask) &&
                ((!map_source.tap && (*(map_source.input_state + PREV_STATE_OFFSET) == 0) && (*map_source.input_state != 0)) ||
                    (map_source.tap && tap_state[map_source.usage]))) {
                my_mutex_enter(MutexId::MACROS);
                for (auto const& usages : macros[macro]) {
                    macro_queue.push(usages);
                }
                my_mutex_exit(MutexId::MACROS);
            }
        }
    }

    memcpy(input_state + PREV_STATE_OFFSET, input_state, used_state_slots * sizeof(input_state[0]));

    for (auto const& rev_map : reverse_mapping) {
        uint32_t target = rev_map.target;
        if (rev_map.is_relative) {
            for (auto const& map_source : rev_map.sources) {
                int32_t value = 0;
                if ((map_source.usage & 0xFFFF0000) == EXPR_USAGE_PAGE) {
                    if (layer_state_mask & map_source.layer_mask) {
                        value = eval_expr(map_source, now, auto_repeat);
                    }
                } else {
                    if (auto_repeat || map_source.is_relative) {
                        if (map_source.sticky) {
                            value = !!(sticky_state[map_source.usage] & map_source.layer_mask) * map_source.scaling;
                        } else {
                            if (layer_state_mask & map_source.layer_mask) {
                                int32_t state = map_source.hold ? hold_state[map_source.usage] : *map_source.input_state;
                                value = (map_source.is_relative ? state : !!state) * map_source.scaling;
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
            for (auto const& map_source : rev_map.sources) {
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
                                (map_source.is_relative
                                        ? (*map_source.input_state * map_source.scaling > 0)
                                        : *map_source.input_state)) {
                                value = 1;
                            }
                        }
                    }
                }
            }
            if (value) {
                for (auto const& out_usage_def : rev_map.our_usages) {
                    put_bits(out_usage_def.data, out_usage_def.len, out_usage_def.bitpos, out_usage_def.size, value);
                }
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

    for (auto state : relative_usages) {
        *state = 0;
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

    for (auto const [interface_report_id, report] : out_reports) {
        // XXX we assume everything is absolute
        if (memcmp(report, prev_out_reports[interface_report_id], out_report_sizes[interface_report_id])) {
            queue_out_report(interface_report_id >> 16, interface_report_id & 0xFF, report, out_report_sizes[interface_report_id]);
            memcpy(prev_out_reports[interface_report_id], report, out_report_sizes[interface_report_id]);
        }
        memset(report, 0, out_report_sizes[interface_report_id]);
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

void monitor_usage(uint32_t usage, int32_t value) {
    if (monitor_usages_queued == sizeof(monitor_report.usage_values) / sizeof(monitor_report.usage_values[0])) {
        return;
    }
    monitor_report.usage_values[monitor_usages_queued++] = { .usage = usage, .value = value };
}

inline void read_input(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint8_t interface_idx) {
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

    if (!their_usage.is_relative && (their_usage.size == 1)) {
        if (value) {
            *(their_usage.input_state) |= 1 << interface_idx;
        } else {
            *(their_usage.input_state) &= ~(1 << interface_idx);
        }
    } else {
        *(their_usage.input_state) = value;
    }
}

inline void read_input_range(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint8_t interface_idx) {
    // is_array and !is_relative is implied
    for (unsigned int i = 0; i < their_usage.count; i++) {
        uint32_t bits = get_bits(report, len, their_usage.bitpos + i * their_usage.size, their_usage.size);
        // XXX consider negative indexes
        if ((bits >= their_usage.logical_minimum) &&
            (bits <= their_usage.logical_minimum + their_usage.usage_maximum - source_usage)) {
            uint32_t actual_usage = source_usage + bits - their_usage.logical_minimum;
            int32_t* state_ptr = get_state_ptr(actual_usage);
            if (state_ptr != NULL) {
                *state_ptr |= 1 << interface_idx;
            }
        }
    }
}

inline void monitor_read_input(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint8_t interface_idx) {
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
        if (value != 0) {
            monitor_usage(source_usage, value);
        }
    } else {
        if (their_usage.size == 1) {
            if (value != (1 & (monitor_input_state[source_usage] >> interface_idx))) {
                monitor_usage(source_usage, value);
            }
            if (value) {
                monitor_input_state[source_usage] |= 1 << interface_idx;
            } else {
                monitor_input_state[source_usage] &= ~(1 << interface_idx);
            }
        } else {
            if (value != monitor_input_state[source_usage]) {
                monitor_usage(source_usage, value);
            }
            monitor_input_state[source_usage] = value;
        }
    }
}

inline void monitor_read_input_range(const uint8_t* report, int len, uint32_t source_usage, const usage_def_t& their_usage, uint8_t interface_idx) {
    // is_array and !is_relative is implied
    for (unsigned int i = 0; i < their_usage.count; i++) {
        uint32_t bits = get_bits(report, len, their_usage.bitpos + i * their_usage.size, their_usage.size);
        // XXX consider negative indexes
        if ((bits >= their_usage.logical_minimum) &&
            (bits <= their_usage.logical_minimum + their_usage.usage_maximum - source_usage)) {
            uint32_t actual_usage = source_usage + bits - their_usage.logical_minimum;
            // for array inputs, "key-up" events (value=0) don't show up in the monitor
            if (monitor_enabled && ((actual_usage & 0xFFFF) != 0)) {
                monitor_usage(actual_usage, 1);
            }
        }
    }
}

void handle_received_report(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id) {
    reports_received++;

    my_mutex_enter(MutexId::THEIR_USAGES);

    uint8_t report_id = 0;
    if (has_report_id_theirs[interface]) {
        if (external_report_id != 0) {
            report_id = external_report_id;
        } else {
            report_id = report[0];
            report++;
            len--;
        }
    }

    uint8_t interface_idx = interface_index[interface];
    for (int32_t* state_ptr : array_range_usages[interface][report_id]) {
        *state_ptr &= ~(1 << interface_idx);
    }

    for (auto const& their : their_used_usages[interface][report_id]) {
        if (their.usage_def.usage_maximum == 0) {
            read_input(report, len, their.usage, their.usage_def, interface_idx);
        } else {
            read_input_range(report, len, their.usage, their.usage_def, interface_idx);
        }
    }

    if (monitor_enabled) {
        for (auto const& [their_usage, their_usage_def] : their_usages[interface][report_id]) {
            if (their_usage_def.usage_maximum == 0) {
                monitor_read_input(report, len, their_usage, their_usage_def, interface_idx);
            } else {
                monitor_read_input_range(report, len, their_usage, their_usage_def, interface_idx);
            }
        }
    }

    my_mutex_exit(MutexId::THEIR_USAGES);
}

void set_input_state(uint32_t usage, int32_t state) {
    int32_t* state_ptr = get_state_ptr(usage);
    if (state_ptr != NULL) {
        *state_ptr = state;
    }
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
    std::unordered_set<uint32_t> relative_usage_set;
    std::set<uint64_t> their_usage_ranges_set;

    relative_usages.clear();
    their_used_usages.clear();
    array_range_usages.clear();

    for (auto& [interface, report_id_usage_map] : their_usages) {
        for (auto& [report_id, usage_map] : report_id_usage_map) {
            for (auto& [usage, usage_def] : usage_map) {
                if (usage_def.usage_maximum == 0) {
                    int32_t* state_ptr = get_state_ptr(usage);
                    their_usage_ranges_set.insert(((uint64_t) usage << 32) | usage);
                    if (usage_def.is_relative) {
                        if (state_ptr != NULL) {
                            relative_usages.push_back(state_ptr);
                        }
                        relative_usage_set.insert(usage);
                    }
                    if (state_ptr != NULL) {
                        usage_def.input_state = state_ptr;
                        their_used_usages[interface][report_id].push_back((usage_usage_def_t){
                            .usage = usage,
                            .usage_def = usage_def,
                        });
                    }
                } else {  // usage_maximum != 0, array range usage
                    their_usage_ranges_set.insert(((uint64_t) usage << 32) | usage_def.usage_maximum);
                    bool any_used = false;
                    for (uint32_t actual_usage = usage; actual_usage <= usage_def.usage_maximum; actual_usage++) {
                        int32_t* state_ptr = get_state_ptr(actual_usage);
                        if (state_ptr != NULL) {
                            any_used = true;
                            array_range_usages[interface][report_id].push_back(state_ptr);
                        }
                    }
                    if (any_used) {
                        their_used_usages[interface][report_id].push_back((usage_usage_def_t){
                            .usage = usage,
                            .usage_def = usage_def,
                        });
                    }
                }
            }
        }
    }

    their_usages_rle.clear();
    rlencode(their_usage_ranges_set, their_usages_rle);

    for (auto& rev_map : reverse_mapping) {
        for (auto& map_source : rev_map.sources) {
            map_source.is_relative = relative_usage_set.count(map_source.usage) > 0;
        }
        auto search = their_out_usages_flat.find(rev_map.target);
        if (search != their_out_usages_flat.end()) {
            rev_map.our_usages.clear();
            for (auto dev_addr_int_rep_id : search->second) {
                auto const& our_usage2 = their_out_usages[dev_addr_int_rep_id >> 16][dev_addr_int_rep_id & 0xFFFF][rev_map.target];
                rev_map.our_usages.push_back((out_usage_def_t){
                    .data = out_reports[dev_addr_int_rep_id],
                    .len = out_report_sizes[dev_addr_int_rep_id],
                    .size = our_usage2.size,
                    .bitpos = our_usage2.bitpos,
                });
            }
        }
    }
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

    parse_descriptor(their_usages[OUR_OUT_INTERFACE], has_report_id_theirs[OUR_OUT_INTERFACE], our_report_descriptor, our_report_descriptor_length, true);
}

void print_stats() {
    printf("%lu %lu\n", reports_received, reports_sent);
    reports_received = 0;
    reports_sent = 0;
}

void set_monitor_enabled(bool enabled) {
    if (monitor_enabled != enabled) {
        monitor_input_state.clear();
        monitor_enabled = enabled;
    }
}
