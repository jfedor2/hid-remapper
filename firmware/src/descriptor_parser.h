#ifndef _DESCRIPTOR_PARSER_H_
#define _DESCRIPTOR_PARSER_H_

#include <stdint.h>

#ifdef __cplusplus

#include <unordered_map>
#include "types.h"

enum class ReportType : uint8_t {
    INPUT,
    OUTPUT,
    FEATURE,
};

std::unordered_map<ReportType, std::unordered_map<uint8_t, uint16_t>> parse_descriptor(
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& input_usage_map,
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& output_usage_map,
    std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& feature_usage_map,
    bool& has_report_id,
    const uint8_t* report_descriptor,
    int len);

extern "C" {
#endif

void parse_descriptor(uint16_t vendor_id, uint16_t product_id, const uint8_t* report_descriptor, int len, uint16_t interface, uint8_t itf_num);
void clear_descriptor_data(uint8_t dev_addr);

#ifdef __cplusplus
}
#endif

#endif
