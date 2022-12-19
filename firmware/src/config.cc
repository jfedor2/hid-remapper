#include <cstring>
#include <unordered_set>

#include "config.h"
#include "crc.h"
#include "globals.h"
#include "interval_override.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

const uint8_t CONFIG_VERSION = 3;

const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH = 0x01;

ConfigCommand last_config_command = ConfigCommand::NO_COMMAND;
uint32_t requested_index = 0;

bool checksum_ok(const uint8_t* buffer, uint16_t data_size) {
    return crc32(buffer, data_size - 4) == ((crc32_t*) (buffer + data_size - 4))->crc32;
}

bool version_ok(const uint8_t* buffer) {
    return ((set_feature_t*) buffer)->version == CONFIG_VERSION;
}

void load_config(const uint8_t* persisted_config) {
    if (checksum_ok(persisted_config, PERSISTED_CONFIG_SIZE) && version_ok(persisted_config)) {
        persist_config_t* config = (persist_config_t*) persisted_config;
        unmapped_passthrough = (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH) != 0;
        partial_scroll_timeout = config->partial_scroll_timeout;
        interval_override = config->interval_override;
        mapping_config_t* buffer_mappings = (mapping_config_t*) (persisted_config + sizeof(persist_config_t));
        for (uint32_t i = 0; i < config->mapping_count; i++) {
            config_mappings.push_back(buffer_mappings[i]);
        }
    }
}

void fill_get_config(get_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    if (unmapped_passthrough) {
        config->flags |= CONFIG_FLAG_UNMAPPED_PASSTHROUGH;
    }
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->mapping_count = config_mappings.size();
    config->our_usage_count = our_usages_rle.size();
    config->their_usage_count = their_usages_rle.size();
    config->interval_override = interval_override;
}

void fill_persist_config(persist_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    if (unmapped_passthrough) {
        config->flags |= CONFIG_FLAG_UNMAPPED_PASSTHROUGH;
    }
    config->partial_scroll_timeout = partial_scroll_timeout;
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
            default:
                break;
        }
        config_buffer->crc32 = crc32((uint8_t*) config_buffer, CONFIG_SIZE - 4);
        return CONFIG_SIZE;
    }

    return 0;
}

void handle_set_report(uint8_t report_id, uint8_t const* buffer, uint16_t bufsize) {
    if (report_id == REPORT_ID_MULTIPLIER && bufsize >= 1) {
        memcpy(&resolution_multiplier, buffer, 1);
    }
    if (report_id == REPORT_ID_CONFIG && bufsize >= CONFIG_SIZE) {
        if (checksum_ok(buffer, CONFIG_SIZE) && version_ok(buffer)) {
            set_feature_t* config_buffer = (set_feature_t*) buffer;
            last_config_command = config_buffer->command;
            switch (config_buffer->command) {
                case ConfigCommand::RESET_INTO_BOOTSEL:
                    reset_to_bootloader();
                    break;
                case ConfigCommand::SET_CONFIG: {
                    set_config_t* config = (set_config_t*) ((set_feature_t*) buffer)->data;
                    unmapped_passthrough = (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH) != 0;
                    partial_scroll_timeout = config->partial_scroll_timeout;
                    uint8_t prev_interval_override = interval_override;
                    interval_override = config->interval_override;
                    if (prev_interval_override != interval_override) {
                        interval_override_updated();
                    }
                    set_mapping_from_config();
                    break;
                }
                case ConfigCommand::CLEAR_MAPPING:
                    config_mappings.clear();
                    set_mapping_from_config();
                    break;
                case ConfigCommand::ADD_MAPPING: {
                    mapping_config_t* mapping_config = (mapping_config_t*) ((set_feature_t*) buffer)->data;
                    config_mappings.push_back(*mapping_config);
                    set_mapping_from_config();
                    break;
                }
                case ConfigCommand::GET_MAPPING:
                case ConfigCommand::GET_OUR_USAGES:
                case ConfigCommand::GET_THEIR_USAGES: {
                    get_indexed_t* get_indexed = (get_indexed_t*) ((set_feature_t*) buffer)->data;
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
                default:
                    break;
            }
        }
    }
}
