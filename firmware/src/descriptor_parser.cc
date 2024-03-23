#include <cstring>
#include <deque>

#include "descriptor_parser.h"
#include "globals.h"
#include "platform.h"
#include "quirks.h"

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

void mark_usage(
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>* usage_map,
    uint32_t usage,
    uint8_t report_id,
    uint16_t bitpos,
    uint8_t size,
    bool is_relative,
    int32_t logical_minimum,
    bool is_array = false,
    uint32_t index = 0,
    uint32_t count = 0,
    uint32_t usage_maximum = 0) {
    if (bitpos >= (8 * ((report_id == 0) ? 64 : 63))) {
        // We don't currently handle reports longer than 64 bytes so let's save some memory.
        // It's probably a broken descriptor anyway.
        return;
    }

    (*usage_map)[report_id].try_emplace(usage,
        (usage_def_t){
            .report_id = report_id,
            .size = size,
            .bitpos = bitpos,
            .is_relative = is_relative,
            .is_array = is_array,
            .logical_minimum = logical_minimum,
            .index = index,
            .count = count,
            .usage_maximum = usage_maximum,
        });
}

void assign_interface_index(uint16_t interface) {
    if (interface_index.count(interface)) {
        return;
    }

    uint8_t i = 0;
    while (i < 31 && ((1 << i) & interface_index_in_use)) {
        i++;
    }

    // if we have more than 32 interfaces, they end up sharing bit 31

    interface_index[interface] = i;
    interface_index_in_use |= 1 << i;
}

void parse_descriptor(uint16_t vendor_id, uint16_t product_id, const uint8_t* report_descriptor, int len, uint16_t interface) {
    my_mutex_enter(MutexId::THEIR_USAGES);
    auto their_report_sizes_map = parse_descriptor(
        their_usages[interface],
        their_out_usages[interface],
        their_feature_usages[interface],
        has_report_id_theirs[interface],
        report_descriptor,
        len);
    apply_quirks(vendor_id, product_id, their_usages[interface], report_descriptor, len, interface & 0xFF);
    assign_interface_index(interface);

    for (auto const& [report_id, size] : their_report_sizes_map[ReportType::OUTPUT]) {
        out_report_sizes[(interface << 16) | report_id] = size;
        out_reports[(interface << 16) | report_id] = new uint8_t[size];
        memset(out_reports[(interface << 16) | report_id], 0, size);
        prev_out_reports[(interface << 16) | report_id] = new uint8_t[size];
        memset(prev_out_reports[(interface << 16) | report_id], 0, size);
    }
    for (auto const& [report_id, usage_map] : their_out_usages[interface]) {
        for (auto const& [usage, usage_def] : usage_map) {
            their_out_usages_flat[usage].push_back((interface << 16) | report_id);
        }
    }

    my_mutex_exit(MutexId::THEIR_USAGES);
    their_descriptor_updated = true;
}

static ReportType item_to_report_type(uint8_t item) {
    switch (item) {
        default:
        case HID_INPUT:
            return ReportType::INPUT;
        case HID_OUTPUT:
            return ReportType::OUTPUT;
        case HID_FEATURE:
            return ReportType::FEATURE;
    }
}

std::unordered_map<ReportType, std::unordered_map<uint8_t, uint16_t>> parse_descriptor(
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& input_usage_map,
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& output_usage_map,
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& feature_usage_map,
    bool& has_report_id,
    const uint8_t* report_descriptor,
    int len) {
    int idx = 0;

    uint8_t report_id = 0;
    std::unordered_map<ReportType, std::unordered_map<uint8_t, uint16_t>> bitpos;  // in/out/feature -> report_id -> bitpos
    uint32_t report_size = 0;
    uint32_t report_count = 0;
    uint32_t usage_page = 0;
    std::deque<uint32_t> usages;
    uint32_t usage_minimum = 0;
    uint32_t usage_maximum = 0;
    int32_t logical_minimum = 0;
    int32_t logical_maximum = 0;

    std::unordered_map<ReportType, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>*> usage_map;
    usage_map[ReportType::INPUT] = &input_usage_map;
    usage_map[ReportType::OUTPUT] = &output_usage_map;
    usage_map[ReportType::FEATURE] = &feature_usage_map;

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
            case HID_INPUT:
            case HID_OUTPUT:
            case HID_FEATURE: {
                ReportType report_type = item_to_report_type(item);
                bool relative = value & (1 << 2);
                if ((value & 0x03) == 0x02) {  // scalar
                    if (usage_minimum && usage_maximum) {
                        uint32_t usage = usage_minimum;
                        for (uint32_t i = 0; i < report_count; i++) {
                            mark_usage(
                                usage_map[report_type],
                                usage,
                                report_id,
                                bitpos[report_type][report_id],
                                report_size,
                                relative,
                                logical_minimum);

                            if (usage < usage_maximum) {
                                usage++;
                            }

                            bitpos[report_type][report_id] += report_size;
                        }
                    } else if (!usages.empty()) {
                        uint32_t usage = 0;
                        for (uint32_t i = 0; i < report_count; i++) {
                            if (!usages.empty()) {
                                usage = usages.front();
                                usages.pop_front();
                            }

                            mark_usage(
                                usage_map[report_type],
                                usage,
                                report_id,
                                bitpos[report_type][report_id],
                                report_size,
                                relative,
                                logical_minimum);

                            bitpos[report_type][report_id] += report_size;
                        }
                    } else {
                        bitpos[report_type][report_id] += report_size * report_count;
                    }
                } else if ((value & 0x03) == 0x00) {  // array
                    if (usage_minimum && usage_maximum) {
                        uint32_t effective_usage_maximum =
                            std::min(usage_maximum, usage_minimum + logical_maximum - logical_minimum);

                        mark_usage(
                            usage_map[report_type],
                            usage_minimum,
                            report_id,
                            bitpos[report_type][report_id],
                            report_size,
                            relative,
                            logical_minimum,
                            true,
                            logical_minimum,
                            report_count,
                            effective_usage_maximum);
                    } else if (!usages.empty()) {
                        uint32_t usage = 0;
                        for (int index = logical_minimum; index <= logical_maximum; index++) {
                            if (!usages.empty()) {
                                usage = usages.front();
                                usages.pop_front();
                            }

                            mark_usage(
                                usage_map[report_type],
                                usage,
                                report_id,
                                bitpos[report_type][report_id],
                                report_size,
                                relative,
                                logical_minimum,
                                true,
                                index,
                                report_count);
                        }
                    }
                    bitpos[report_type][report_id] += report_size * report_count;
                } else {  // constant
                    bitpos[report_type][report_id] += report_size * report_count;
                }

                usages.clear();
                usage_minimum = 0;
                usage_maximum = 0;
                break;
            }
            case HID_COLLECTION:
                usages.clear();
                usage_minimum = 0;
                usage_maximum = 0;
                break;
            case HID_USAGE_PAGE:
                usage_page = value;
                break;
            case HID_REPORT_SIZE:
                report_size = value;
                break;
            case HID_REPORT_ID:
                report_id = value;
                has_report_id = true;
                break;
            case HID_REPORT_COUNT:
                report_count = value;
                break;
            case HID_USAGE: {
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usages.push_back(full_usage);
                break;
            }
            case HID_USAGE_MINIMUM: {
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usage_minimum = full_usage;
                break;
            }
            case HID_USAGE_MAXIMUM: {
                uint32_t full_usage = item_size <= 2 ? usage_page << 16 | value : value;
                usage_maximum = full_usage;
                break;
            }
            case HID_LOGICAL_MINIMUM:
                logical_minimum = value;
                if (logical_minimum & (1 << (item_size * 8 - 1))) {
                    logical_minimum |= 0xFFFFFFFF << item_size * 8;
                }
                break;
            case HID_LOGICAL_MAXIMUM:
                logical_maximum = value;
                break;
        }
    }

    for (auto& [report_type, rep_id_bitpos] : bitpos) {
        for (auto& [report_id_, position] : rep_id_bitpos) {
            position /= 8;  // final bit position becomes report size in bytes
        }
    }

    return bitpos;
}

void clear_descriptor_data(uint8_t dev_addr) {
    my_mutex_enter(MutexId::THEIR_USAGES);

    for (auto it = their_usages.cbegin(); it != their_usages.cend();) {
        uint16_t dev_addr_interface = it->first;
        if (dev_addr_interface >> 8 == dev_addr) {
            has_report_id_theirs.erase(dev_addr_interface);

            uint8_t index = interface_index[dev_addr_interface];
            interface_index.erase(dev_addr_interface);
            interface_index_in_use &= ~(1 << index);

            it = their_usages.erase(it);
        } else {
            it++;
        }
    }

    for (auto it = their_feature_usages.cbegin(); it != their_feature_usages.cend();) {
        uint16_t dev_addr_interface = it->first;
        if (dev_addr_interface >> 8 == dev_addr) {
            it = their_feature_usages.erase(it);
        } else {
            it++;
        }
    }

    for (auto it = their_out_usages.cbegin(); it != their_out_usages.cend();) {
        uint16_t dev_addr_interface = it->first;
        if (dev_addr_interface >> 8 == dev_addr) {
            it = their_out_usages.erase(it);
        } else {
            it++;
        }
    }

    for (auto it = out_reports.cbegin(); it != out_reports.cend();) {
        uint32_t dev_addr_int_rep_id = it->first;
        if (dev_addr_int_rep_id >> 24 == dev_addr) {
            out_report_sizes.erase(dev_addr_int_rep_id);
            delete[] prev_out_reports[dev_addr_int_rep_id];
            prev_out_reports.erase(dev_addr_int_rep_id);
            delete[] out_reports[dev_addr_int_rep_id];
            it = out_reports.erase(it);
        } else {
            it++;
        }
    }

    for (auto& [usage, addr_int_rep_id_vector] : their_out_usages_flat) {
        for (auto it = addr_int_rep_id_vector.cbegin(); it != addr_int_rep_id_vector.cend();) {
            if ((*it) >> 24 == dev_addr) {
                it = addr_int_rep_id_vector.erase(it);
            } else {
                it++;
            }
        }
    }

    my_mutex_exit(MutexId::THEIR_USAGES);
    their_descriptor_updated = true;
}
