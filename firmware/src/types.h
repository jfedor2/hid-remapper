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
    CLEAR_QUIRKS = 23,
    ADD_QUIRK = 24,
    GET_QUIRK = 25,
};

struct usage_def_t {
    uint8_t report_id;
    uint8_t size;
    uint16_t bitpos;
    bool is_relative;
    bool is_array = false;
    bool should_be_scaled = false;
    int32_t logical_minimum;
    int32_t logical_maximum;
    uint32_t index = 0;      // for arrays
    uint32_t count = 0;      // for arrays
    uint32_t usage_maximum;  // effective, for arrays/usage ranges
    int32_t* input_state_0 = NULL;
    int32_t* input_state_n = NULL;
    uint8_t index_mask = 0;
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
    PORT = 34,
    DPAD = 35,
    EOL = 36,
    INPUT_STATE_FP32 = 37,
    PREV_INPUT_STATE_FP32 = 38,
    MIN = 39,
    MAX = 40,
    IFTE = 41,
    DIV = 42,
    SWAP = 43,
    MONITOR = 44,
    SIGN = 45,
    SUB = 46,
    PRINT_IF = 47,
    TIME_SEC = 48,
    LT = 49,
    PLUGGED_IN = 50,
    INPUT_STATE_SCALED = 51,
    PREV_INPUT_STATE_SCALED = 52,
    DEADZONE = 53,
    DEADZONE2 = 54,
};

struct tap_hold_state_t {
    bool tap : 1;
    bool hold : 1;
    bool prev_hold : 1;
};

struct expr_elem_t {
    Op op;
    uint32_t val = 0;
    union {
        int32_t* state_ptr = NULL;
        uint8_t* sticky_state_ptr;
        tap_hold_state_t* tap_hold_state_ptr;
    };
};

struct map_source_t {
    uint32_t usage;
    int32_t scaling = 1000;  // * 1000
    bool sticky = false;
    bool tap = false;
    bool hold = false;
    bool is_relative = false;
    bool is_binary = false;
    uint8_t orig_source_port = 0;
    uint8_t layer_mask = 1;
    int32_t* input_state;
    tap_hold_state_t* tap_hold_state;
    uint8_t* sticky_state;
    int32_t accumulated_scroll;
    uint64_t last_scroll_timestamp;  // XXX we can make this 32 or less bits
};

struct out_usage_def_t {
    uint8_t* data;
    uint16_t len;
    uint8_t size;
    uint16_t bitpos;
    uint8_t array_count;
    uint32_t array_index;
};

struct reverse_mapping_t {
    uint32_t target;
    uint8_t default_value = 0;  // should be int32_t theoretically, but currently all defaults fit uint8_t
    uint8_t hub_port = 0;
    bool is_relative = false;
    std::vector<out_usage_def_t> our_usages;
    std::vector<map_source_t> sources;
};

struct tap_hold_usage_t {
    int32_t* input_state;
    tap_hold_state_t* tap_hold_state;
    uint64_t pressed_at;
};

struct sticky_usage_t {
    int32_t* input_state;
    uint8_t* sticky_state;
    uint8_t layer_mask;
};

struct tap_hold_sticky_usage_t {
    uint8_t layer_mask;
    tap_hold_state_t* tap_hold_state;
    uint8_t* sticky_state;
};

struct usage_rle_t {
    uint32_t usage;
    uint32_t count;
};

struct register_ptrs_t {
    int32_t* register_ptr;
    int32_t* state_ptr;
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

struct __attribute__((packed)) mapping_config10_t {
    uint32_t target_usage;
    uint32_t source_usage;
    int32_t scaling;  // * 1000
    uint8_t layer_mask;
    uint8_t flags;
};

struct __attribute__((packed)) mapping_config11_t {
    uint32_t target_usage;
    uint32_t source_usage;
    int32_t scaling;  // * 1000
    uint8_t layer_mask;
    uint8_t flags;
    uint8_t hub_ports = 0;
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

struct __attribute__((packed)) persist_config_v10_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
    uint8_t macro_entry_duration;
};

typedef persist_config_v10_t persist_config_v11_t;

#define QUIRK_FLAG_RELATIVE_MASK 0b10000000
#define QUIRK_FLAG_SIGNED_MASK 0b01000000
#define QUIRK_SIZE_MASK 0b00111111

struct __attribute__((packed)) quirk_t {
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t interface;
    uint8_t report_id;
    uint32_t usage;
    uint16_t bitpos;
    uint8_t size_flags;
};

struct __attribute__((packed)) persist_config_v12_t {
    uint8_t version;
    uint8_t flags;
    uint8_t unmapped_passthrough_layer_mask;
    uint32_t partial_scroll_timeout;
    uint16_t mapping_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
    uint8_t macro_entry_duration;
    uint16_t quirk_count;
};

typedef persist_config_v12_t persist_config_v13_t;

typedef persist_config_v13_t persist_config_t;

struct __attribute__((packed)) get_config_t {
    uint8_t version;
    uint8_t flags;
    uint8_t unmapped_passthrough_layer_mask;
    uint32_t partial_scroll_timeout;
    uint16_t mapping_count;
    uint32_t our_usage_count;
    uint32_t their_usage_count;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
    uint8_t macro_entry_duration;
    uint16_t quirk_count;
};

struct __attribute__((packed)) set_config_t {
    uint8_t flags;
    uint8_t unmapped_passthrough_layer_mask;
    uint32_t partial_scroll_timeout;
    uint8_t interval_override;
    uint32_t tap_hold_threshold;
    uint8_t gpio_debounce_time_ms;
    uint8_t our_descriptor_number;
    uint8_t macro_entry_duration;
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
    QUIRKS,
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

enum class PersistConfigReturnCode : int8_t {
    UNKNOWN = 0,
    SUCCESS = 1,
    CONFIG_TOO_BIG = 2,
};

struct __attribute__((packed)) persist_config_response_t {
    PersistConfigReturnCode return_code;
};

struct __attribute__((packed)) monitor_t {
    uint8_t enabled;
};

struct __attribute__((packed)) monitor_report_item_t {
    uint32_t usage;
    int32_t value;
    uint8_t hub_port;
};

struct __attribute__((packed)) monitor_report_t {
    uint8_t report_id;
    monitor_report_item_t items[7];
};

struct __attribute__((packed)) uint16_val_t {
    uint16_t val;
};

#endif
