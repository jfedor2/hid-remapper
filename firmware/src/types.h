#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>
#include <cstddef>
#include <vector>

enum class ConfigCommand : int8_t {
    NO_COMMAND = 0,
    RESET_INTO_BOOTSEL = 1,
    SET_CONFIG = 2,
    GET_CONFIG = 3,
    CLEAR_MAPPING = 4,
    ADD_MAPPING = 5,
    GET_MAPPING = 6,
    PERSIST_CONFIG = 7,
    GET_OUR_USAGES = 8,
    GET_THEIR_USAGES = 9,
    SUSPEND = 10,
    RESUME = 11,
    PAIR_NEW_DEVICE = 12,
    CLEAR_BONDS = 13,
    FLASH_B_SIDE = 14,
    CLEAR_MACROS = 15,
    APPEND_TO_MACRO = 16,
    GET_MACRO = 17,
    INVALID_COMMAND = 18,
    CLEAR_EXPRESSIONS = 19,
    APPEND_TO_EXPRESSION = 20,
    GET_EXPRESSION = 21,
    SET_MONITOR_ENABLED = 22,
};

struct usage_def_t {
    uint8_t report_id;
    uint8_t size;
    uint16_t bitpos;
    bool is_relative;
    bool is_array = false;
    int32_t logical_minimum;
    uint32_t index = 0;      // for arrays
    uint32_t count = 0;      // for arrays
    uint32_t usage_maximum;  // effective, for arrays/usage ranges
    int32_t* input_state = NULL;
};

struct usage_usage_def_t {
    uint32_t usage;
    usage_def_t usage_def;
};

enum class Op : int8_t {
    PUSH = 0,
    PUSH_USAGE = 1,
    INPUT_STATE = 2,
    ADD = 3,
    MUL = 4,
    EQ = 5,
    TIME = 6,
    MOD = 7,
    GT = 8,
    NOT = 9,
    INPUT_STATE_BINARY = 10,
    ABS = 11,
    DUP = 12,
    SIN = 13,
    COS = 14,
    DEBUG = 15,
    AUTO_REPEAT = 16,
    RELU = 17,
    CLAMP = 18,
    SCALING = 19,
    LAYER_STATE = 20,
    STICKY_STATE = 21,
    TAP_STATE = 22,
    HOLD_STATE = 23,
    BITWISE_OR = 24,
    BITWISE_AND = 25,
    BITWISE_NOT = 26,
    PREV_INPUT_STATE = 27,
    PREV_INPUT_STATE_BINARY = 28,
    STORE = 29,
    RECALL = 30,
    SQRT = 31,
    ATAN2 = 32,
    ROUND = 33,
};

struct expr_elem_t {
    Op op;
    uint32_t val = 0;
};

struct map_source_t {
    uint32_t usage;
    int32_t scaling = 1000;  // * 1000
    bool sticky = false;
    uint8_t layer_mask = 1;
    bool tap = false;
    bool hold = false;
    int32_t* input_state;
    bool is_relative = false;
};

struct out_usage_def_t {
    uint8_t* data;
    uint16_t len;
    uint8_t size;
    uint16_t bitpos;
};

struct reverse_mapping_t {
    uint32_t target;
    bool is_relative = false;
    std::vector<out_usage_def_t> our_usages;
    std::vector<map_source_t> sources;
};

struct usage_input_state_t {
    uint32_t usage;
    int32_t* input_state;
};

struct usage_layer_mask_input_state_t {
    uint32_t usage;
    int32_t* input_state;
    uint8_t layer_mask;
};

struct usage_layer_mask_t {
    uint32_t usage;
    uint8_t layer_mask;
};

struct usage_rle_t {
    uint32_t usage;
    uint32_t count;
};

struct __attribute__((packed)) set_feature_t {
    uint8_t version;
    ConfigCommand command;
    uint8_t data[26];
    uint32_t crc32;
};

struct __attribute__((packed)) get_feature_t {
    uint8_t data[28];
    uint32_t crc32;
};

struct __attribute__((packed)) mapping_config_t {
    uint32_t target_usage;
    uint32_t source_usage;
    int32_t scaling;  // * 1000
    uint8_t layer_mask;
    uint8_t flags;
};

struct __attribute__((packed)) config_version_t {
    uint8_t version;
};

struct __attribute__((packed)) persist_config_v4_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
};

struct __attribute__((packed)) persist_config_v5_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
};

typedef persist_config_v5_t persist_config_v6_t;

struct __attribute__((packed)) persist_config_v7_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
};

struct __attribute__((packed)) persist_config_v9_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
};

typedef persist_config_v9_t persist_config_t;

struct __attribute__((packed)) get_config_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint32_t our_usage_count;
    uint32_t their_usage_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
};

struct __attribute__((packed)) set_config_t {
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
};

struct __attribute__((packed)) get_indexed_t {
    uint32_t requested_index;
};

#define MACRO_ITEMS_IN_PACKET 6

struct __attribute__((packed)) append_to_macro_t {
    uint8_t macro;
    uint8_t nitems;
    uint32_t usages[MACRO_ITEMS_IN_PACKET];
};

struct __attribute__((packed)) crc32_t {
    uint32_t crc32;
};

struct __attribute__((packed)) get_macro_t {
    uint32_t requested_macro;
    uint32_t requested_macro_item;
};

struct __attribute__((packed)) get_macro_response_t {
    uint8_t nitems;
    uint32_t usages[MACRO_ITEMS_IN_PACKET];
};

struct __attribute__((packed)) macro_item_t {
    uint32_t usage;
};

#define NUSAGES_IN_PACKET 3

struct __attribute__((packed)) usages_list_t {
    usage_rle_t usages[NUSAGES_IN_PACKET];
};

enum class MutexId : int8_t {
    THEIR_USAGES,
    MACROS,
    EXPRESSIONS,
    N
};

struct __attribute__((packed)) expr_val_t {
    uint32_t val;
};

struct __attribute__((packed)) get_expr_t {
    uint32_t requested_expr;
    uint32_t requested_expr_elem;
};

struct __attribute__((packed)) append_to_expr_t {
    uint8_t expr;
    uint8_t nelems;
    uint8_t elem_data[24];
};

struct __attribute__((packed)) get_expr_response_t {
    uint8_t nelems;
    uint8_t elem_data[27];
};

struct __attribute__((packed)) monitor_t {
    uint8_t enabled;
};

struct __attribute__((packed)) usage_value_t {
    uint32_t usage;
    int32_t value;
};

struct __attribute__((packed)) monitor_report_t {
    uint8_t report_id;
    usage_value_t usage_values[7];
};

#endif
