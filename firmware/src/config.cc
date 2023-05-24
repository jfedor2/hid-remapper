#include <cstring>
#include <unordered_set>

#include "config.h"
#include "crc.h"
#include "globals.h"
#include "interval_override.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

const uint8_t CONFIG_VERSION = 7;

const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH = 0x01;
const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK = 0b00001111;
const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT = 0;

ConfigCommand last_config_command = ConfigCommand::NO_COMMAND;
uint32_t requested_index = 0;
uint32_t requested_secondary_index = 0;

bool checksum_ok(const uint8_t* buffer, uint16_t data_size) {
    return crc32(buffer, data_size - 4) == ((crc32_t*) (buffer + data_size - 4))->crc32;
}

bool persisted_version_ok(const uint8_t* buffer) {
    uint8_t version = ((config_version_t*) buffer)->version;
    return (version == 3) || (version == 4) || (version == 5) || (version == 6) || (version == 7);
}

bool command_version_ok(const uint8_t* buffer) {
    uint8_t version = ((set_feature_t*) buffer)->version;
    return version == CONFIG_VERSION;
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
    mapping_config_t* buffer_mappings = (mapping_config_t*) (persisted_config + sizeof(persist_config_v4_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
        if (config->version == 3) {
            config_mappings.back().layer_mask = 1 << config_mappings.back().layer_mask;
        }
    }

    if (config->version >= 4) {
        const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v4_t) + config->mapping_count * sizeof(mapping_config_t));
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
    mapping_config_t* buffer_mappings = (mapping_config_t*) (persisted_config + sizeof(persist_config_v5_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v5_t) + config->mapping_count * sizeof(mapping_config_t));
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
    mapping_config_t* buffer_mappings = (mapping_config_t*) (persisted_config + sizeof(persist_config_v6_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v6_t) + config->mapping_count * sizeof(mapping_config_t));
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

void load_config(const uint8_t* persisted_config) {
    if (!checksum_ok(persisted_config, PERSISTED_CONFIG_SIZE) || !persisted_version_ok(persisted_config)) {
        return;
    }

    uint8_t version = ((config_version_t*) persisted_config)->version;

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

    persist_config_v7_t* config = (persist_config_v7_t*) persisted_config;
    unmapped_passthrough_layer_mask =
        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
    partial_scroll_timeout = config->partial_scroll_timeout;
    tap_hold_threshold = config->tap_hold_threshold;
    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
    interval_override = config->interval_override;
    mapping_config_t* buffer_mappings = (mapping_config_t*) (persisted_config + sizeof(persist_config_v7_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        config_mappings.push_back(buffer_mappings[i]);
    }

    const uint8_t* macros_config_ptr = (persisted_config + sizeof(persist_config_v7_t) + config->mapping_count * sizeof(mapping_config_t));
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

void fill_get_config(get_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    config->flags |=
        (unmapped_passthrough_layer_mask << CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT) & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK;
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->tap_hold_threshold = tap_hold_threshold;
    config->gpio_debounce_time_ms = gpio_debounce_time / 1000;
    config->mapping_count = config_mappings.size();
    config->our_usage_count = our_usages_rle.size();
    config->their_usage_count = their_usages_rle.size();
    config->interval_override = interval_override;
}

void fill_persist_config(persist_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    config->flags |=
        (unmapped_passthrough_layer_mask << CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT) & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK;
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->tap_hold_threshold = tap_hold_threshold;
    config->gpio_debounce_time_ms = gpio_debounce_time / 1000;
    config->mapping_count = config_mappings.size();
    config->interval_override = interval_override;
}

void persist_config() {
    // stack size is 2KB
    static uint8_t buffer[PERSISTED_CONFIG_SIZE];
    memset(buffer, 0, sizeof(buffer));

    persist_config_t* config = (persist_config_t*) buffer;
    fill_persist_config(config);
    mapping_config_t* buffer_mappings = (mapping_config_t*) (buffer + sizeof(persist_config_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        buffer_mappings[i] = config_mappings[i];
    }

    uint8_t* macros_config_ptr = (buffer + sizeof(persist_config_t) + config->mapping_count * sizeof(mapping_config_t));
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
        *expr_config_ptr = expressions[i].size();
        expr_config_ptr++;
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

    ((crc32_t*) (buffer + PERSISTED_CONFIG_SIZE - 4))->crc32 = crc32(buffer, PERSISTED_CONFIG_SIZE - 4);

    do_persist_config(buffer);
}

void reset_resolution_multiplier() {
    // reset hi-res scroll on reboots
    resolution_multiplier = 0;
}

uint16_t handle_get_report(uint8_t report_id, uint8_t* buffer, uint16_t reqlen) {
    if (report_id == REPORT_ID_MULTIPLIER && reqlen >= 1) {
        memcpy(buffer, &resolution_multiplier, 1);
        return 1;
    }
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
                mapping_config_t* mapping_config = (mapping_config_t*) config_buffer;
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
                    if (exhausted && (ret_idx > 0)) {
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
                uint8_t i = 0;
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
            default:
                break;
        }
        config_buffer->crc32 = crc32((uint8_t*) config_buffer, CONFIG_SIZE - 4);
        last_config_command = ConfigCommand::NO_COMMAND;
        return CONFIG_SIZE;
    }

    return 0;
}

void handle_set_report(uint8_t report_id, uint8_t const* buffer, uint16_t bufsize) {
    if (report_id == REPORT_ID_MULTIPLIER && bufsize >= 1) {
        memcpy(&resolution_multiplier, buffer, 1);
    }
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
                    unmapped_passthrough_layer_mask =
                        (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH_MASK) >> CONFIG_FLAG_UNMAPPED_PASSTHROUGH_BIT;
                    partial_scroll_timeout = config->partial_scroll_timeout;
                    tap_hold_threshold = config->tap_hold_threshold;
                    gpio_debounce_time = config->gpio_debounce_time_ms * 1000;
                    uint8_t prev_interval_override = interval_override;
                    interval_override = config->interval_override;
                    if (prev_interval_override != interval_override) {
                        interval_override_updated();
                    }
                    config_updated = true;
                    break;
                }
                case ConfigCommand::GET_CONFIG:
                    break;
                case ConfigCommand::CLEAR_MAPPING:
                    config_mappings.clear();
                    config_updated = true;
                    break;
                case ConfigCommand::ADD_MAPPING: {
                    mapping_config_t* mapping_config = (mapping_config_t*) config_buffer->data;
                    config_mappings.push_back(*mapping_config);
                    config_updated = true;
                    break;
                }
                case ConfigCommand::GET_MAPPING:
                case ConfigCommand::GET_OUR_USAGES:
                case ConfigCommand::GET_THEIR_USAGES: {
                    get_indexed_t* get_indexed = (get_indexed_t*) config_buffer->data;
                    requested_index = get_indexed->requested_index;
                    break;
                }
                case ConfigCommand::PERSIST_CONFIG:
                    need_to_persist_config = true;
                    break;
                case ConfigCommand::SUSPEND:
                    suspended = true;
                    break;
                case ConfigCommand::RESUME:
                    suspended = false;
                    // XXX clear input_state, sticky_state, accumulated?
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
                    config_updated = true;
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
                default:
                    last_config_command = ConfigCommand::INVALID_COMMAND;
                    break;
            }
        } else {
            last_config_command = ConfigCommand::INVALID_COMMAND;
        }
    }
}
