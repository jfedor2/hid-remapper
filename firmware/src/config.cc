#include <unordered_set>

#include <bsp/board.h>
#include <tusb.h>

#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include <hardware/flash.h>

#include "config.h"
#include "crc.h"
#include "globals.h"
#include "our_descriptor.h"
#include "remapper.h"
extern "C" {
#include "pio_usb.h"
}

const uint8_t CONFIG_VERSION = 3;

const uint32_t PRESUMED_FLASH_SIZE = 2097152;
const uint32_t CONFIG_OFFSET_IN_FLASH = (PRESUMED_FLASH_SIZE - FLASH_SECTOR_SIZE);
const uint8_t* FLASH_CONFIG_IN_MEMORY = (((uint8_t*) XIP_BASE) + CONFIG_OFFSET_IN_FLASH);

const uint8_t CONFIG_FLAG_UNMAPPED_PASSTHROUGH = 0x01;

ConfigCommand last_config_command = ConfigCommand::NO_COMMAND;
uint32_t requested_index = 0;

bool checksum_ok(const uint8_t* buffer, uint16_t data_size) {
    return crc32(buffer, data_size - 4) == ((crc32_t*) (buffer + data_size - 4))->crc32;
}

bool version_ok(const uint8_t* buffer) {
    return ((set_feature_t*) buffer)->version == CONFIG_VERSION;
}

void load_config() {
    if (checksum_ok(FLASH_CONFIG_IN_MEMORY, FLASH_SECTOR_SIZE) && version_ok(FLASH_CONFIG_IN_MEMORY)) {
        persist_config_t* config = (persist_config_t*) FLASH_CONFIG_IN_MEMORY;
        unmapped_passthrough = (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH) != 0;
        partial_scroll_timeout = config->partial_scroll_timeout;
        set_interval_override(config->interval_override);
        mapping_config_t* buffer_mappings = (mapping_config_t*) (FLASH_CONFIG_IN_MEMORY + sizeof(persist_config_t));
        for (uint32_t i = 0; i < config->mapping_count; i++) {
            config_mappings.push_back(buffer_mappings[i]);
        }
    }
    set_mapping_from_config();
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
    config->interval_override = get_interval_override();
}

void fill_persist_config(persist_config_t* config) {
    config->version = CONFIG_VERSION;
    config->flags = 0;
    if (unmapped_passthrough) {
        config->flags |= CONFIG_FLAG_UNMAPPED_PASSTHROUGH;
    }
    config->partial_scroll_timeout = partial_scroll_timeout;
    config->mapping_count = config_mappings.size();
    config->interval_override = get_interval_override();
}

void persist_config() {
    // stack size is 2KB
    static uint8_t buffer[FLASH_SECTOR_SIZE];
    memset(buffer, 0, sizeof(buffer));

    persist_config_t* config = (persist_config_t*) buffer;
    fill_persist_config(config);
    mapping_config_t* buffer_mappings = (mapping_config_t*) (buffer + sizeof(persist_config_t));
    for (uint32_t i = 0; i < config->mapping_count; i++) {
        buffer_mappings[i] = config_mappings[i];
    }

    ((crc32_t*) (buffer + FLASH_SECTOR_SIZE - 4))->crc32 = crc32(buffer, FLASH_SECTOR_SIZE - 4);

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(CONFIG_OFFSET_IN_FLASH, FLASH_SECTOR_SIZE);
    flash_range_program(CONFIG_OFFSET_IN_FLASH, buffer, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);
}

void tud_mount_cb() {
    // reset hi-res scroll for when we reboot from Windows into Linux
    resolution_multiplier = 0;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
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

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    if (report_id == REPORT_ID_MULTIPLIER && bufsize >= 1) {
        memcpy(&resolution_multiplier, buffer, 1);
    }
    if (report_id == REPORT_ID_CONFIG && bufsize >= CONFIG_SIZE) {
        if (checksum_ok(buffer, CONFIG_SIZE) && version_ok(buffer)) {
            set_feature_t* config_buffer = (set_feature_t*) buffer;
            last_config_command = config_buffer->command;
            switch (config_buffer->command) {
                case ConfigCommand::RESET_INTO_BOOTSEL:
                    reset_usb_boot(0, 0);
                    break;
                case ConfigCommand::SET_CONFIG: {
                    set_config_t* config = (set_config_t*) ((set_feature_t*) buffer)->data;
                    unmapped_passthrough = (config->flags & CONFIG_FLAG_UNMAPPED_PASSTHROUGH) != 0;
                    partial_scroll_timeout = config->partial_scroll_timeout;
                    set_interval_override(config->interval_override);
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
                default:
                    break;
            }
        }
    }
}
