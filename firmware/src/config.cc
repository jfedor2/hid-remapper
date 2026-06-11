#include <cstdio>
#include <cstring>
#include <unordered_set>

#include "config.h"
#include "crc.h"
#include "globals.h"
#include "interval_override.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

const uint8_t CONFIG_VERSION = 19;

const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH = 0x01;
const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK = 0b00001111;
const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT = 0;
const uint8_t CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT = 4;
const uint8_t CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT = 5;
const uint8_t CONFIG_FLAG_NORMALIZE_GAMEPAD_INPUTS_BIT = 6;
const uint8_t CONFIG_FLAG_IMU_ENABLE_BIT = 7;

ConfigCommand last_config_command = ConfigCommand::NO_COMMAND;
uint32_t requested_index = 0;
uint32_t requested_secondary_index = 0;

bool checksum_ok(const uint8_t* buffer, uint16_t data_size) {
    return crc32(buffer, data_size - 4) == ((crc32_t*) (buffer + data_size - 4))->crc32;
}

bool persisted_version_ok(const uint8_t* buffer) {
    uint8_t version = ((config_version_t*) buffer)->version;
    return (version >= 3) && (version <= CONFIG_VERSION);
}

bool command_version_ok(const uint8_t* buffer) {
    uint8_t version = ((set_feature_t*) buffer)->version;
    return version == CONFIG_VERSION;
}

static mapping_config11_t mapping_config_10_to_11(mapping_config10_t mapping) {
    return (mapping_config11_t){
        .target_usage = mapping.target_usage,
        .source_usage = mapping.source_usage,
        .scaling = mapping.scaling,
        .layer_mask = mapping.layer_mask,
        .flags = mapping.flags,
        .hub_ports = 0,
    };
}

void load_config_v3_v4(const uint8_t* persisted_config) {
    persist_config_v4_t* config = (persist_config_v4_t*) persisted_config;
    if (config->version == 3) {
        unmapped_passthrough_layer_mask = (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH) ? 1 : 0;
    }
    if (config->version >= 4) {
        unmapped_passthrough_layer_mask =
            (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    }
    partial_scroll_timeout = config->partial_scroll_timeout;
    interval_override = config->interval_override;
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v4_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
        if (config->version == 3) {
            config_mappings.back().layer_mask = 1 << config_mappings.back().layer_mask;
        }
    }

    if (config->version >= 4) {
        const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v4_t) + config->mapping_count * sizeof(mapping_config10_t));
        my_mutex_enter(MutexId::MACROS);
        for (int i = 0; i < NMACROS_8; i++) {
            macros[i].clear();
            uint8_t macro_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].reserve(macro_len);
            for (int j = 0; j < macro_len; j++) {
                uint8_t entry_len = *macros_config_ptr;
                macros_config_ptr++;
                macros[i].push_back({});
                macros[i].back().reserve(entry_len);
                for (int k = 0; k < entry_len; k++) {
                    macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                    macros_config_ptr += sizeof(macro_item_t);
                }
            }
        }
        my_mutex_exit(MutexId::MACROS);
    }
}

void load_config_v5(const uint8_t* persisted_config) {
    persist_config_v5_t* config = (persist_config_v5_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    interval_override = config->interval_override;
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v5_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v5_t) + config->mapping_count * sizeof(mapping_config10_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS_8; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);
}

void load_config_v6(const uint8_t* persisted_config) {
    persist_config_v6_t* config = (persist_config_v6_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    interval_override = config->interval_override;
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v6_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v6_t) + config->mapping_count * sizeof(mapping_config10_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS_8; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
}

void load_config_v7_v8(const uint8_t* persisted_config) {
    // v8 is same as v7, it just introduces some new expression ops

    persist_config_v7_t* config = (persist_config_v7_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v7_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v7_t) + config->mapping_count * sizeof(mapping_config10_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
}

void load_config_v9(const uint8_t* persisted_config) {
    persist_config_v9_t* config = (persist_config_v9_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v9_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v9_t) + config->mapping_count * sizeof(mapping_config10_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
}

void load_config_v10(const uint8_t* persisted_config) {
    persist_config_v10_t* config = (persist_config_v10_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    macro_entry_duration = config->macro_entry_duration;
    mapping_config10_t* buffer_mappings = (mapping_config10_t*) (persisted_config + sizeof(persist_config_v10_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(mapping_config_10_to_11(buffer_mappings[i]));
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v10_t) + config->mapping_count * sizeof(mapping_config10_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
}

void load_config_v11(const uint8_t* persisted_config) {
    persist_config_v11_t* config = (persist_config_v11_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    macro_entry_duration = config->macro_entry_duration;
    mapping_config11_t* buffer_mappings = (mapping_config11_t*) (persisted_config + sizeof(persist_config_v11_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v11_t) + config->mapping_count * sizeof(mapping_config11_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
}

void load_config_v12(const uint8_t* persisted_config) {
    persist_config_v12_t* config = (persist_config_v12_t*) persisted_config;
    unmapped_passthrough_layer_mask = config->unmapped_passthrough_layer_mask;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    macro_entry_duration = config->macro_entry_duration;
    mapping_config11_t* buffer_mappings = (mapping_config11_t*) (persisted_config + sizeof(persist_config_v12_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v12_t) + config->mapping_count * sizeof(mapping_config11_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint8_t expr_len = *expr_config_ptr;
        expr_config_ptr++;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);

    my_mutex_enter(MutexId::QUIRKS);
    quirk_t* quirk_config_ptr = (quirk_t*) expr_config_ptr;
    for (int i = 0; i < config->quirk_count; i++) {
        quirks.push_back(*quirk_config_ptr);
        quirk_config_ptr++;
    }
    my_mutex_exit(MutexId::QUIRKS);
}

void load_config_v13(const uint8_t* persisted_config) {
    persist_config_v13_t* config = (persist_config_v13_t*) persisted_config;
    unmapped_passthrough_layer_mask = config->unmapped_passthrough_layer_mask;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    macro_entry_duration = config->macro_entry_duration;
    mapping_config11_t* buffer_mappings = (mapping_config11_t*) (persisted_config + sizeof(persist_config_v13_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v13_t) + config->mapping_count * sizeof(mapping_config11_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint16_t expr_len = ((uint16_val_t*) expr_config_ptr)->val;
        expr_config_ptr += 2;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);

    my_mutex_enter(MutexId::QUIRKS);
    quirk_t* quirk_config_ptr = (quirk_t*) expr_config_ptr;
    for (int i = 0; i < config->quirk_count; i++) {
        quirks.push_back(*quirk_config_ptr);
        quirk_config_ptr++;
    }
    my_mutex_exit(MutexId::QUIRKS);
}



void load_config(const uint8_t* persisted_config) {
    if (!checksum_ok(persisted_config, PERSISTED_CONFIG_SIZE) || !persisted_version_ok(persisted_config)) {
        return;
    }

    uint8_t version = ((config_version_t*) persisted_config)->version;

    if (version < 18) {
        // Normalize gamepad inputs defaults to true, but if we're loading a <18 config,
        // set it to false to preserve previous behavior.
        normalize_gamepad_inputs = false;
    }

    if (version < 19) {
        imu_angle_clamp_limit = 45;
        imu_filter_buffer_size = 10;
        imu_enabled = false;
        imu_roll_inverted = false;
        imu_pitch_inverted = false;
    }

    if ((version == 3) || (version == 4)) {
        load_config_v3_v4(persisted_config);
        return;
    }

    if (version == 5) {
        load_config_v5(persisted_config);
        return;
    }

    if (version == 6) {
        load_config_v6(persisted_config);
        return;
    }

    if ((version == 7) || (version == 8)) {
        load_config_v7_v8(persisted_config);
        return;
    }

    if (version == 9) {
        load_config_v9(persisted_config);
        return;
    }

    if (version == 10) {
        load_config_v10(persisted_config);
        return;
    }

    if (version == 11) {
        load_config_v11(persisted_config);
        return;
    }

    if (version == 12) {
        load_config_v12(persisted_config);
        return;
    }

    // v14 is same as v13, it just introduces a new emulated device type
    // v15 is same as v14, it just introduces some new expression ops
    // v16 is same as v15, it just introduces a new expression op
    // v17 is same as v16, it introduces new expression ops and GET_FEATURE retry behavior

    if ((version == 13) ||
        (version == 14) ||
        (version == 15) ||
        (version == 16) ||
        (version == 17)) {
        load_config_v13(persisted_config);
        return;
    }


    persist_config_v19_t* config = (persist_config_v19_t*) persisted_config;
    unmapped_passthrough_layer_mask = config->unmapped_passthrough_layer_mask;
    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
    normalize_gamepad_inputs = !!(config->flags & (1 << CONFIG_FLAG_NORMALIZE_GAMEPAD_INPUTS_BIT));
    imu_enabled = !!(config->flags & (1 << CONFIG_FLAG_IMU_ENABLE_BIT));
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    our_descriptor_number = config->our_descriptor_number;
    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
        our_descriptor_number = 0;
    }
    macro_entry_duration = config->macro_entry_duration;
    if (version >= 19) {
        imu_angle_clamp_limit = config->imu_angle_clamp_limit;
        if (imu_angle_clamp_limit > 90) {
            imu_angle_clamp_limit = 90;
        }
        imu_filter_buffer_size = config->imu_filter_buffer_size;
        if (imu_filter_buffer_size < 1) {
            imu_filter_buffer_size = 1;
        }
        if (imu_filter_buffer_size > 16) {
            imu_filter_buffer_size = 16;
        }
        imu_roll_inverted = config->imu_roll_inverted;
        imu_pitch_inverted = config->imu_pitch_inverted;
    }
    mapping_config11_t* buffer_mappings = (mapping_config11_t*) (persisted_config + sizeof(persist_config_v19_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v19_t) + config->mapping_count * sizeof(mapping_config11_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        macros[i].clear();
        uint8_t macro_len = *macros_config_ptr;
        macros_config_ptr++;
        macros[i].reserve(macro_len);
        for (int j = 0; j < macro_len; j++) {
            uint8_t entry_len = *macros_config_ptr;
            macros_config_ptr++;
            macros[i].push_back({});
            macros[i].back().reserve(entry_len);
            for (int k = 0; k < entry_len; k++) {
                macros[i].back().push_back(((macro_item_t*) macros_config_ptr)->usage);
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    const uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        expressions[i].clear();
        uint16_t expr_len = ((uint16_val_t*) expr_config_ptr)->val;
        expr_config_ptr += 2;
        expressions[i].reserve(expr_len);
        for (int j = 0; j < expr_len; j++) {
            uint8_t op = *expr_config_ptr;
            expr_config_ptr++;
            uint32_t val = 0;
            if ((op == (uint8_t) Op::PUSH) || (op == (uint8_t) Op::PUSH_USAGE)) {
                val = ((expr_val_t*) expr_config_ptr)->val;
                expr_config_ptr += sizeof(expr_val_t);
            }
            expressions[i].push_back((expr_elem_t){ .op = (Op) op, .val = val });
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);

    my_mutex_enter(MutexId::QUIRKS);
    quirk_t* quirk_config_ptr = (quirk_t*) expr_config_ptr;
    for (int i = 0; i < config->quirk_count; i++) {
        quirks.push_back(*quirk_config_ptr);
        quirk_config_ptr++;
    }
    my_mutex_exit(MutexId::QUIRKS);
}

void fill_get_config(get_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    config->flags |= ignore_auth_dev_inputs << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT;
    config->flags |= gpio_output_mode << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT;
    config->flags |= normalize_gamepad_inputs << CONFIG_FLAG_NORMALIZE_GAMEPAD_INPUTS_BIT;
    config->flags |= imu_enabled << CONFIG_FLAG_IMU_ENABLE_BIT;
    config->unmapped_passthrough_layer_mask = unmapped_passthrough_layer_mask;
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->tap_hold_threshold = tap_hold_threshold;
    config->gpio_debounce_time_ms = gpio_debounce_time / 1000;
    config->mapping_count = config_mappings.size();
    config->our_usage_count = our_usages_rle.size();
    config->their_usage_count = their_usages_rle.size();
    config->interval_override = interval_override;
    config->our_descriptor_number = our_descriptor_number;
    config->macro_entry_duration = macro_entry_duration;
    config->imu_angle_clamp_limit = imu_angle_clamp_limit;
    config->imu_filter_buffer_size = imu_filter_buffer_size;
    config->imu_roll_inverted = imu_roll_inverted;
    config->imu_pitch_inverted = imu_pitch_inverted;
    my_mutex_enter(MutexId::QUIRKS);
    config->quirk_count = quirks.size();
    my_mutex_exit(MutexId::QUIRKS);
}

void fill_persist_config(persist_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    config->flags |= ignore_auth_dev_inputs << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT;
    config->flags |= gpio_output_mode << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT;
    config->flags |= normalize_gamepad_inputs << CONFIG_FLAG_NORMALIZE_GAMEPAD_INPUTS_BIT;
    config->flags |= imu_enabled << CONFIG_FLAG_IMU_ENABLE_BIT;
    config->unmapped_passthrough_layer_mask = unmapped_passthrough_layer_mask;
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->tap_hold_threshold = tap_hold_threshold;
    config->gpio_debounce_time_ms = gpio_debounce_time / 1000;
    config->mapping_count = config_mappings.size();
    config->interval_override = interval_override;
    config->our_descriptor_number = our_descriptor_number;
    config->macro_entry_duration = macro_entry_duration;
    config->imu_angle_clamp_limit = imu_angle_clamp_limit;
    config->imu_filter_buffer_size = imu_filter_buffer_size;
    config->imu_roll_inverted = imu_roll_inverted;
    config->imu_pitch_inverted = imu_pitch_inverted;
    my_mutex_enter(MutexId::QUIRKS);
    config->quirk_count = quirks.size();
    my_mutex_exit(MutexId::QUIRKS);
}

PersistConfigReturnCode persist_config() {
    // stack size is 2KB
    static uint8_t buffer[PERSISTED_CONFIG_SIZE];
    memset(buffer, 0, sizeof(buffer));

    persist_config_t* config = (persist_config_t*) buffer;
    fill_persist_config(config);

    // check if persisted config will fit in the space we have reserved for it in flash
    int32_t real_persisted_config_size = 0;
    real_persisted_config_size += sizeof(persist_config_t);
    real_persisted_config_size += config->mapping_count * sizeof(mapping_config11_t);
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        real_persisted_config_size += 1;
        for (auto const& entries : macros[i]) {
            real_persisted_config_size += 1 + entries.size() * sizeof(macro_item_t);
        }
    }
    my_mutex_exit(MutexId::MACROS);
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        real_persisted_config_size += 2;
        for (auto const& elem : expressions[i]) {
            real_persisted_config_size += 1;
            if ((elem.op == Op::PUSH) || (elem.op == Op::PUSH_USAGE)) {
                real_persisted_config_size += sizeof(expr_val_t);
            }
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);
    my_mutex_enter(MutexId::QUIRKS);
    real_persisted_config_size += quirks.size() * sizeof(quirk_t);
    my_mutex_exit(MutexId::QUIRKS);
    real_persisted_config_size += 4;  // CRC32
    if (real_persisted_config_size > PERSISTED_CONFIG_SIZE) {
        printf("config too large to be persisted!\n");
        return PersistConfigReturnCode::CONFIG_TOO_BIG;
    }

    mapping_config11_t* buffer_mappings = (mapping_config11_t*) (buffer + sizeof(persist_config_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        buffer_mappings[i] = config_mappings[i];
    }

    uint8_t* macros_config_ptr = (buffer + sizeof(persist_config_t) + config->mapping_count * sizeof(mapping_config11_t));
    my_mutex_enter(MutexId::MACROS);
    for (int i = 0; i < NMACROS; i++) {
        *macros_config_ptr = macros[i].size();
        macros_config_ptr++;
        for (auto const& entries : macros[i]) {
            *macros_config_ptr = entries.size();
            macros_config_ptr++;
            for (uint32_t usage : entries) {
                ((macro_item_t*) macros_config_ptr)->usage = usage;
                macros_config_ptr += sizeof(macro_item_t);
            }
        }
    }
    my_mutex_exit(MutexId::MACROS);

    uint8_t* expr_config_ptr = macros_config_ptr;
    my_mutex_enter(MutexId::EXPRESSIONS);
    for (int i = 0; i < NEXPRESSIONS; i++) {
        ((uint16_val_t*) expr_config_ptr)->val = expressions[i].size();
        expr_config_ptr += 2;
        for (auto const& elem : expressions[i]) {
            *expr_config_ptr = (uint8_t) elem.op;
            expr_config_ptr++;
            if ((elem.op == Op::PUSH) || (elem.op == Op::PUSH_USAGE)) {
                ((expr_val_t*) expr_config_ptr)->val = elem.val;
                expr_config_ptr += sizeof(expr_val_t);
            }
        }
    }
    my_mutex_exit(MutexId::EXPRESSIONS);

    my_mutex_enter(MutexId::QUIRKS);
    quirk_t* quirk_config_ptr = (quirk_t*) expr_config_ptr;
    for (size_t i = 0; i < quirks.size(); i++) {
        *quirk_config_ptr = quirks[i];
        quirk_config_ptr++;
    }
    my_mutex_exit(MutexId::QUIRKS);

    ((crc32_t*) (buffer + PERSISTED_CONFIG_SIZE - 4))->crc32 = crc32(buffer, PERSISTED_CONFIG_SIZE - 4);

    if (real_persisted_config_size != 4 + ((uint8_t*) quirk_config_ptr) - buffer) {
        printf("we calculated real persisted config size wrong!\n");
    }

    do_persist_config(buffer);

    return PersistConfigReturnCode::SUCCESS;
}

void reset_resolution_multiplier() {
    // reset hi-res scroll on reboots
    resolution_multiplier = 0;
}

uint16_t handle_get_report1(uint8_t report_id, uint8_t* buffer, uint16_t reqlen) {
    if (report_id == REPORT_ID_CONFIG && reqlen >= CONFIG_SIZE) {
        get_feature_t* config_buffer = (get_feature_t*) buffer;
        memset(config_buffer, 0, sizeof(get_feature_t));
        switch (last_config_command) {
            case ConfigCommand::INVALID_COMMAND: {
                memset(config_buffer, 0xFF, sizeof(get_feature_t));
                break;
            }
            case ConfigCommand::GET_CONFIG: {
                fill_get_config((get_config_t*) config_buffer);
                break;
            }
            case ConfigCommand::GET_MAPPING: {
                mapping_config11_t* mapping_config = (mapping_config11_t*) config_buffer;
                if (requested_index < config_mappings.size()) {
                    *mapping_config = config_mappings[requested_index];
                }
                break;
            }
            case ConfigCommand::GET_OUR_USAGES: {
                usages_list_t* returned_usages = (usages_list_t*) config_buffer;
                for (uint32_t i = 0; (i < NUSAGES_IN_PACKET) && (requested_index + i < our_usages_rle.size()); i++) {
                    returned_usages->usages[i] = our_usages_rle[requested_index + i];
                }
                break;
            }
            case ConfigCommand::GET_THEIR_USAGES: {
                usages_list_t* returned_usages = (usages_list_t*) config_buffer;
                for (uint32_t i = 0; (i < NUSAGES_IN_PACKET) && (requested_index + i < their_usages_rle.size()); i++) {
                    returned_usages->usages[i] = their_usages_rle[requested_index + i];
                }
                break;
            }
            case ConfigCommand::GET_MACRO: {
                if (requested_index < NMACROS) {
                    get_macro_response_t* returned = (get_macro_response_t*) config_buffer;
                    uint16_t i = 0;
                    uint8_t ret_idx = 0;
                    bool exhausted = true;
                    my_mutex_enter(MutexId::MACROS);
                    for (auto const& entries : macros[requested_index]) {
                        if (ret_idx >= MACRO_ITEMS_IN_PACKET) {
                            exhausted = false;
                            break;
                        }
                        for (uint32_t usage : entries) {
                            if (i >= requested_secondary_index) {
                                returned->usages[ret_idx++] = usage;
                                if (ret_idx >= MACRO_ITEMS_IN_PACKET) {
                                    break;
                                }
                            }
                            i++;
                        }
                        if ((ret_idx < MACRO_ITEMS_IN_PACKET) && (i >= requested_secondary_index)) {
                            returned->usages[ret_idx++] = 0;
                        }
                        i++;
                    }
                    my_mutex_exit(MutexId::MACROS);
                    if (exhausted && (ret_idx > 0) && (returned->usages[ret_idx - 1] == 0)) {
                        ret_idx--;
                    }
                    returned->nitems = ret_idx;
                }
                break;
            }
            case ConfigCommand::GET_EXPRESSION: {
                if (requested_index >= NEXPRESSIONS) {
                    break;
                }
                get_expr_response_t* returned = (get_expr_response_t*) config_buffer;
                uint8_t* ptr = returned->elem_data;
                uint32_t i = 0;
                for (auto const& elem : expressions[requested_index]) {
                    if (i >= requested_secondary_index) {
                        if ((elem.op == Op::PUSH) || (elem.op == Op::PUSH_USAGE)) {
                            if (ptr <= returned->elem_data + sizeof(returned->elem_data) - 5) {
                                *ptr = (uint8_t) elem.op;
                                ptr++;
                                ((expr_val_t*) ptr)->val = elem.val;
                                ptr += sizeof(expr_val_t);
                                returned->nelems++;
                            } else {
                                break;
                            }
                        } else {
                            *ptr = (uint8_t) elem.op;
                            ptr++;
                            returned->nelems++;
                        }
                    }
                    if (ptr > returned->elem_data + sizeof(returned->elem_data) - 1) {
                        break;
                    }
                    i++;
                }
                break;
            }
            case ConfigCommand::GET_QUIRK: {
                quirk_t* quirk = (quirk_t*) config_buffer;
                my_mutex_enter(MutexId::QUIRKS);
                if (requested_index < quirks.size()) {
                    *quirk = quirks[requested_index];
                }
                my_mutex_exit(MutexId::QUIRKS);
                break;
            }
            case ConfigCommand::PERSIST_CONFIG: {
                persist_config_response_t* returned = (persist_config_response_t*) config_buffer;
                if (persist_config_return_code == PersistConfigReturnCode::UNKNOWN) {
                    // persist_config() wasn't called yet
                    return 0;
                }
                returned->return_code = persist_config_return_code;
                break;
            }
            default:
                return 0;
        }
        config_buffer->crc32 = crc32((uint8_t*) config_buffer, CONFIG_SIZE - 4);
        last_config_command = ConfigCommand::NO_COMMAND;
        return CONFIG_SIZE;
    }

    return 0;
}

void handle_set_report1(uint8_t report_id, uint8_t const* buffer, uint16_t bufsize) {
    if (report_id == REPORT_ID_CONFIG && bufsize >= CONFIG_SIZE) {
        if (checksum_ok(buffer, CONFIG_SIZE) && command_version_ok(buffer)) {
            set_feature_t* config_buffer = (set_feature_t*) buffer;
            last_config_command = config_buffer->command;
            switch (config_buffer->command) {
                case ConfigCommand::NO_COMMAND:
                    break;
                case ConfigCommand::RESET_INTO_BOOTSEL:
                    reset_to_bootloader();
                    break;
                case ConfigCommand::SET_CONFIG: {
                    set_config_t* config = (set_config_t*) config_buffer->data;
                    unmapped_passthrough_layer_mask = config->unmapped_passthrough_layer_mask;
                    ignore_auth_dev_inputs = config->flags & (1 << CONFIG_FLAG_IGNORE_AUTH_DEV_INPUTS_BIT);
                    gpio_output_mode = !!(config->flags & (1 << CONFIG_FLAG_GPIO_OUTPUT_MODE_BIT));
                    normalize_gamepad_inputs = !!(config->flags & (1 << CONFIG_FLAG_NORMALIZE_GAMEPAD_INPUTS_BIT));
                    imu_enabled = !!(config->flags & (1 << CONFIG_FLAG_IMU_ENABLE_BIT));
                    partial_scroll_timeout = config->partial_scroll_timeout;
                    tap_hold_threshold = config->tap_hold_threshold;
                    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
                    uint8_t prev_interval_override = interval_override;
                    interval_override = config->interval_override;
                    if (prev_interval_override != interval_override) {
                        interval_override_updated();
                    }
                    our_descriptor_number = config->our_descriptor_number;
                    if (our_descriptor_number >= NOUR_DESCRIPTORS) {
                        our_descriptor_number = 0;
                    }
                    macro_entry_duration = config->macro_entry_duration;
                    imu_angle_clamp_limit = config->imu_angle_clamp_limit;
                    if (imu_angle_clamp_limit > 90) {
                        imu_angle_clamp_limit = 90;
                    }
                    imu_filter_buffer_size = config->imu_filter_buffer_size;
                    if (imu_filter_buffer_size < 1) {
                        imu_filter_buffer_size = 1;
                    }
                    if (imu_filter_buffer_size > 16) {
                        imu_filter_buffer_size = 16;
                    }
                    imu_roll_inverted = config->imu_roll_inverted;
                    imu_pitch_inverted = config->imu_pitch_inverted;
                    break;
                }
                case ConfigCommand::GET_CONFIG:
                    break;
                case ConfigCommand::CLEAR_MAPPING:
                    config_mappings.clear();
                    break;
                case ConfigCommand::ADD_MAPPING: {
                    mapping_config11_t* mapping_config = (mapping_config11_t*) config_buffer->data;
                    config_mappings.push_back(*mapping_config);
                    break;
                }
                case ConfigCommand::GET_MAPPING:
                case ConfigCommand::GET_OUR_USAGES:
                case ConfigCommand::GET_THEIR_USAGES:
                case ConfigCommand::GET_QUIRK: {
                    get_indexed_t* get_indexed = (get_indexed_t*) config_buffer->data;
                    requested_index = get_indexed->requested_index;
                    break;
                }
                case ConfigCommand::PERSIST_CONFIG:
                    need_to_persist_config = true;
                    persist_config_return_code = PersistConfigReturnCode::UNKNOWN;
                    break;
                case ConfigCommand::SUSPEND:
                    suspended = true;
                    break;
                case ConfigCommand::RESUME:
                    resume_pending = true;
                    config_updated = true;
                    reset_state();
                    break;
                case ConfigCommand::PAIR_NEW_DEVICE:
                    pair_new_device();
                    break;
                case ConfigCommand::CLEAR_BONDS:
                    clear_bonds();
                    break;
                case ConfigCommand::FLASH_B_SIDE:
                    flash_b_side();
                    break;
                case ConfigCommand::CLEAR_MACROS:
                    my_mutex_enter(MutexId::MACROS);
                    for (int i = 0; i < NMACROS; i++) {
                        macros[i].clear();
                    }
                    my_mutex_exit(MutexId::MACROS);
                    break;
                case ConfigCommand::APPEND_TO_MACRO: {
                    append_to_macro_t* append_to_macro = (append_to_macro_t*) config_buffer->data;
                    my_mutex_enter(MutexId::MACROS);
                    if (macros[append_to_macro->macro].empty()) {
                        macros[append_to_macro->macro].push_back({});
                    }
                    for (int i = 0; (i < MACRO_ITEMS_IN_PACKET) && (i < append_to_macro->nitems); i++) {
                        if (append_to_macro->usages[i] == 0) {
                            macros[append_to_macro->macro].push_back({});
                        } else {
                            macros[append_to_macro->macro].back().push_back(append_to_macro->usages[i]);
                        }
                    }
                    my_mutex_exit(MutexId::MACROS);
                    break;
                }
                case ConfigCommand::GET_MACRO: {
                    get_macro_t* get_macro = (get_macro_t*) config_buffer->data;
                    requested_index = get_macro->requested_macro;
                    requested_secondary_index = get_macro->requested_macro_item;
                    break;
                }
                case ConfigCommand::CLEAR_EXPRESSIONS:
                    my_mutex_enter(MutexId::EXPRESSIONS);
                    for (int i = 0; i < NEXPRESSIONS; i++) {
                        expressions[i].clear();
                    }
                    my_mutex_exit(MutexId::EXPRESSIONS);
                    break;
                case ConfigCommand::APPEND_TO_EXPRESSION: {
                    append_to_expr_t* append_to_expr = (append_to_expr_t*) config_buffer->data;
                    if (append_to_expr->expr >= NEXPRESSIONS) {
                        break;
                    }
                    my_mutex_enter(MutexId::EXPRESSIONS);
                    uint8_t* ptr = append_to_expr->elem_data;
                    for (int i = 0; i < append_to_expr->nelems; i++) {
                        if (ptr > append_to_expr->elem_data + sizeof(append_to_expr->elem_data) - 1) {
                            break;
                        }
                        Op op = (Op) *ptr;
                        ptr++;
                        if ((op == Op::PUSH) || (op == Op::PUSH_USAGE)) {
                            if (ptr > append_to_expr->elem_data + sizeof(append_to_expr->elem_data) - sizeof(uint32_t)) {
                                break;
                            }
                            uint32_t val = ((expr_val_t*) ptr)->val;
                            ptr += sizeof(expr_val_t);
                            expressions[append_to_expr->expr].push_back((expr_elem_t){
                                .op = op,
                                .val = val });
                        } else {
                            expressions[append_to_expr->expr].push_back((expr_elem_t){ .op = op });
                        }
                    }
                    my_mutex_exit(MutexId::EXPRESSIONS);
                    break;
                }
                case ConfigCommand::GET_EXPRESSION: {
                    get_expr_t* get_expr = (get_expr_t*) config_buffer->data;
                    requested_index = get_expr->requested_expr;
                    requested_secondary_index = get_expr->requested_expr_elem;
                    break;
                }
                case ConfigCommand::SET_MONITOR_ENABLED: {
                    monitor_t* monitor = (monitor_t*) config_buffer->data;
                    set_monitor_enabled(monitor->enabled);
                    break;
                }
                case ConfigCommand::CLEAR_QUIRKS:
                    my_mutex_enter(MutexId::QUIRKS);
                    quirks.clear();
                    my_mutex_exit(MutexId::QUIRKS);
                    break;
                case ConfigCommand::ADD_QUIRK: {
                    quirk_t* quirk = (quirk_t*) config_buffer->data;
                    my_mutex_enter(MutexId::QUIRKS);
                    quirks.push_back(*quirk);
                    my_mutex_exit(MutexId::QUIRKS);
                    break;
                }
                default:
                    last_config_command = ConfigCommand::INVALID_COMMAND;
                    break;
            }
        } else {
            last_config_command = ConfigCommand::INVALID_COMMAND;
        }
    }
}
