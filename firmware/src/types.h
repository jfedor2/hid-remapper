#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

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
};

struct usage_def_t {
    uint8_t report_id;
    uint8_t size;
    uint16_t bitpos;
    bool is_relative;
    bool is_array = false;
    int32_t logical_minimum;
    uint32_t index = 0;  // for arrays
    uint32_t count = 0;  // for arrays
    uint32_t usage_maximum; // effective, for arrays/usage ranges
};

struct map_source_t {
    uint32_t usage;
    int32_t scaling = 1000;  // * 1000
    bool sticky = false;
    uint8_t layer = 0;
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
    uint8_t layer;
    uint8_t flags;
};

struct __attribute__((packed)) persist_config_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint8_t interval_override;
};

struct __attribute__((packed)) get_config_t {
    uint8_t version;
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint32_t mapping_count;
    uint32_t our_usage_count;
    uint32_t their_usage_count;
    uint8_t interval_override;
};

struct __attribute__((packed)) set_config_t {
    uint8_t flags;
    uint32_t partial_scroll_timeout;
    uint8_t interval_override;
};

struct __attribute__((packed)) get_indexed_t {
    uint32_t requested_index;
};

struct __attribute__((packed)) crc32_t {
    uint32_t crc32;
};

#define NUSAGES_IN_PACKET 3

struct __attribute__((packed)) usages_list_t {
    usage_rle_t usages[NUSAGES_IN_PACKET];
};

#endif
