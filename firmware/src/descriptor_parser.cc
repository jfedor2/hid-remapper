#include <stdio.h>
#include <deque>

#include "descriptor_parser.h"
#include "globals.h"

const uint8_t HID_INPUT = 0x80;
const uint8_t HID_OUTPUT = 0x90;
const uint8_t HID_FEATURE = 0xB0;
const uint8_t HID_COLLECTION = 0xA0;
const uint8_t HID_USAGE_PAGE = 0x04;
const uint8_t HID_REPORT_SIZE = 0x74;
const uint8_t HID_REPORT_ID = 0x84;
const uint8_t HID_REPORT_COUNT = 0x94;
const uint8_t HID_USAGE = 0x08;
const uint8_t HID_USAGE_MINIMUM = 0x18;
const uint8_t HID_USAGE_MAXIMUM = 0x28;
const uint8_t HID_LOGICAL_MINIMUM = 0x14;
const uint8_t HID_LOGICAL_MAXIMUM = 0x24;

void mark_usage(std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, uint32_t usage, uint8_t report_id, uint16_t bitpos, uint8_t size, bool is_relative, int32_t logical_minimum, bool is_array = false, uint32_t index = 0, uint32_t count = 0) {
    usage_map[report_id][usage] = (usage_def_t){
        .report_id = report_id,
        .size = size,
        .bitpos = bitpos,
        .is_relative = is_relative,
        .is_array = is_array,
        .logical_minimum = logical_minimum,
        .index = index,
        .count = count,
    };
}

void parse_descriptor(const volatile uint8_t* report_descriptor, int len, uint8_t interface) {
    parse_descriptor(their_usages[interface], has_report_id_theirs[interface], report_descriptor, len);
    their_descriptor_updated = true;
}

std::unordered_map<uint8_t, uint16_t> parse_descriptor(std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, bool& has_report_id, const volatile uint8_t* report_descriptor, int len) {
    int idx = 0;

    uint8_t report_id = 0;
    std::unordered_map<uint8_t, uint16_t> bitpos;  // report_id -> bitpos
    uint32_t report_size = 0;
    uint32_t report_count = 0;
    uint32_t usage_page = 0;
    std::deque<uint32_t> usages;
    uint32_t usage_minimum = 0;
    uint32_t usage_maximum = 0;
    int32_t logical_minimum = 0;
    int32_t logical_maximum = 0;

    while (idx < len) {
        if (report_descriptor[idx] == 0 && idx == len - 1) {
            continue;
        }

        uint8_t item = report_descriptor[idx] & 0xFC;
        uint8_t item_size = report_descriptor[idx] & 0x03;
        if (item_size == 3) {
            item_size = 4;
        }
        uint32_t value = 0;
        idx++;
        for (int i = 0; i < item_size; i++) {
            value |= report_descriptor[idx++] << (i * 8);
        }

        switch (item) {
            case HID_INPUT: {
                printf("Input %0lx\n", value);

                bool relative = value & (1 << 2);
                if ((value & 0x03) == 0x02) {  // scalar
                    if (usage_minimum && usage_maximum) {
                        uint32_t usage = usage_minimum;
                        for (uint32_t i = 0; i < report_count; i++) {
                            mark_usage(usage_map, usage, report_id, bitpos[report_id], report_size, relative, logical_minimum);
                            if (usage < usage_maximum) {
                                usage++;
                            }
                            bitpos[report_id] += report_size;
                        }
                    } else if (!usages.empty()) {
                        uint32_t usage = 0;
                        for (uint32_t i = 0; i < report_count; i++) {
                            if (!usages.empty()) {
                                usage = usages.front();
                                usages.pop_front();
                            }
                            mark_usage(usage_map, usage, report_id, bitpos[report_id], report_size, relative, logical_minimum);
                            bitpos[report_id] += report_size;
                        }
                    } else {
                        bitpos[report_id] += report_size * report_count;
                    }
                } else if ((value & 0x03) == 0x00) {  // array
                    if (usage_minimum && usage_maximum) {
                        uint32_t usage = usage_minimum;
                        for (int index = logical_minimum; index <= logical_maximum; index++) {
                            mark_usage(usage_map, usage, report_id, bitpos[report_id], report_size, relative, logical_minimum, true, index, report_count);
                            if (usage < usage_maximum) {
                                usage++;
                            }
                        }
                    } else if (!usages.empty()) {
                        uint32_t usage = 0;
                        for (int index = logical_minimum; index <= logical_maximum; index++) {
                            if (!usages.empty()) {
                                usage = usages.front();
                                usages.pop_front();
                            }
                            mark_usage(usage_map, usage, report_id, bitpos[report_id], report_size, relative, logical_minimum, true, index, report_count);
                        }
                    }
                    bitpos[report_id] += report_size * report_count;
                } else {  // constant
                    bitpos[report_id] += report_size * report_count;
                }

                usages.clear();
                usage_minimum = 0;
                usage_maximum = 0;
                break;
            }
            case HID_COLLECTION:
            case HID_OUTPUT:
            case HID_FEATURE:
                usages.clear();
                usage_minimum = 0;
                usage_maximum = 0;
                break;
            case HID_USAGE_PAGE:
                printf("Usage page %0lx\n", value);
                usage_page = value;
                break;
            case HID_REPORT_SIZE:
                printf("Report size %0lx\n", value);
                report_size = value;
                break;
            case HID_REPORT_ID:
                printf("Report ID %0lx\n", value);
                report_id = value;
                has_report_id = true;
                break;
            case HID_REPORT_COUNT:
                printf("Report count %0lx\n", value);
                report_count = value;
                break;
            case HID_USAGE: {
                printf("Usage %0lx\n", value);
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usages.push_back(full_usage);
                break;
            }
            case HID_USAGE_MINIMUM: {
                printf("Usage minimum %0lx\n", value);
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usage_minimum = full_usage;
                break;
            }
            case HID_USAGE_MAXIMUM: {
                printf("Usage maximum %0lx\n", value);
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usage_maximum = full_usage;
                break;
            }
            case HID_LOGICAL_MINIMUM:
                printf("Logical minimum %0lx\n", value);
                logical_minimum = value;
                if (logical_minimum & (1 << (item_size * 8 - 1))) {
                    logical_minimum |= 0xFFFFFFFF << item_size * 8;
                }
                break;
            case HID_LOGICAL_MAXIMUM:
                printf("Logical maximum %0lx\n", value);
                logical_maximum = value;
                break;
        }
    }

    for (auto& [report_id_, position] : bitpos) {
        position /= 8;  // final bit position becomes report size in bytes
    }

    return bitpos;
}

void clear_descriptor_data() {
    need_to_clear_descriptor_data = true;
}
