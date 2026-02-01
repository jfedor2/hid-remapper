#!/usr/bin/env python3

from common import *

import struct
import json

device = get_device()

data = struct.pack("<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, GET_CONFIG, *([0] * 26))
device.send_feature_report(add_crc(data))

data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)

(
    report_id,
    version,
    flags,
    unmapped_passthrough_layer_mask,
    partial_scroll_timeout,
    mapping_count,
    our_usage_count,
    their_usage_count,
    interval_override,
    tap_hold_threshold,
    gpio_debounce_time_ms,
    our_descriptor_number,
    macro_entry_duration,
    quirk_count,
    imu_angle_clamp_limit,
    imu_filter_buffer_size,
    imu_roll_inverted,
    imu_pitch_inverted,
    *_,
    crc,
) = struct.unpack("<BBBBLHLLBLBBBHBBBBL", data)
check_crc(data, crc)

config = {
    "version": version,
    "unmapped_passthrough_layers": mask_to_layer_list(unmapped_passthrough_layer_mask),
    "partial_scroll_timeout": partial_scroll_timeout,
    "interval_override": interval_override,
    "tap_hold_threshold": tap_hold_threshold,
    "gpio_debounce_time_ms": gpio_debounce_time_ms,
    "our_descriptor_number": our_descriptor_number,
    "ignore_auth_dev_inputs": bool(flags & IGNORE_AUTH_DEV_INPUTS_FLAG),
    "macro_entry_duration": macro_entry_duration + 1,
    "gpio_output_mode": 1 if (flags & GPIO_OUTPUT_MODE_FLAG) else 0,
    "input_labels": 0,
    "normalize_gamepad_inputs": bool(flags & NORMALIZE_GAMEPAD_INPUTS_FLAG),
    "imu_enabled": bool(flags & IMU_ENABLE_FLAG),
    "imu_angle_clamp_limit": min(imu_angle_clamp_limit, 90),
    "imu_filter_buffer_size": imu_filter_buffer_size,
    "imu_roll_inverted": bool(imu_roll_inverted),
    "imu_pitch_inverted": bool(imu_pitch_inverted),
    "mappings": [],
    "macros": [],
    "expressions": [],
    "quirks": [],
}

for i in range(mapping_count):
    data = struct.pack(
        "<BBBL22B", REPORT_ID_CONFIG, CONFIG_VERSION, GET_MAPPING, i, *([0] * 22)
    )
    device.send_feature_report(add_crc(data))
    data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)
    (
        report_id,
        target_usage,
        source_usage,
        scaling,
        layer_mask,
        flags,
        hub_ports,
        *_,
        crc,
    ) = struct.unpack("<BLLlBBB13BL", data)
    check_crc(data, crc)
    config["mappings"].append(
        {
            "target_usage": "{0:#010x}".format(target_usage),
            "source_usage": "{0:#010x}".format(source_usage),
            "scaling": scaling,
            "layers": mask_to_layer_list(layer_mask),
            "sticky": (flags & STICKY_FLAG) != 0,
            "tap": (flags & TAP_FLAG) != 0,
            "hold": (flags & HOLD_FLAG) != 0,
            "source_port": hub_ports & 0x0F,
            "target_port": (hub_ports >> 4) & 0x0F,
        }
    )

for macro_i in range(NMACROS):
    macro = []
    i = 0
    keep_going = True
    while keep_going:
        data = struct.pack(
            "<BBBLL18B",
            REPORT_ID_CONFIG,
            CONFIG_VERSION,
            GET_MACRO,
            macro_i,
            i,
            *([0] * 18)
        )
        device.send_feature_report(add_crc(data))
        data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)
        (
            report_id,
            nitems,
            *usages,
            crc,
        ) = struct.unpack("<BB6L3BL", data)
        check_crc(data, crc)
        if nitems < MACRO_ITEMS_IN_PACKET:
            keep_going = False
        if (len(macro) == 0) and (nitems > 0):
            macro = [[]]
        for usage in usages[0:nitems]:
            if usage == 0:
                macro.append([])
            else:
                macro[-1].append("{0:#010x}".format(usage))
        i += MACRO_ITEMS_IN_PACKET
    config["macros"].append(macro)

for expression_i in range(NEXPRESSIONS):
    expression = []
    i = 0
    while True:
        data = struct.pack(
            "<BBBLL18B",
            REPORT_ID_CONFIG,
            CONFIG_VERSION,
            GET_EXPRESSION,
            expression_i,
            i,
            *([0] * 18)
        )
        device.send_feature_report(add_crc(data))
        data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)
        (
            report_id,
            nelems,
            *elems,
            crc,
        ) = struct.unpack("<BB27BL", data)
        check_crc(data, crc)
        if nelems == 0:
            break
        for _ in range(nelems):
            elem = elems[0]
            elems = elems[1:]
            if elem in (ops["PUSH"], ops["PUSH_USAGE"]):
                val = elems[3] << 24 | elems[2] << 16 | elems[1] << 8 | elems[0]
                elems = elems[4:]
                if elem == ops["PUSH"]:
                    val -= (val & (1 << 31)) << 1
                    expression.append(str(val))
                else:
                    expression.append("0x{:08x}".format(val))
            else:
                expression.append(opcodes[elem].lower())
        i += nelems

    config["expressions"].append(" ".join(expression))

for quirk_i in range(quirk_count):
    data = struct.pack(
        "<BBBL22B", REPORT_ID_CONFIG, CONFIG_VERSION, GET_QUIRK, quirk_i, *([0] * 22)
    )
    device.send_feature_report(add_crc(data))
    data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)
    (
        report_id_,
        vendor_id,
        product_id,
        interface,
        report_id,
        usage,
        bitpos,
        size_flags,
        *_,
        crc,
    ) = struct.unpack("<BHHBBLHB15BL", data)
    check_crc(data, crc)
    config["quirks"].append(
        {
            "vendor_id": "{0:#06x}".format(vendor_id),
            "product_id": "{0:#06x}".format(product_id),
            "interface": interface,
            "report_id": report_id,
            "usage": "{0:#010x}".format(usage),
            "size": size_flags & QUIRK_SIZE_MASK,
            "bitpos": bitpos,
            "relative": (size_flags & QUIRK_FLAG_RELATIVE_MASK) != 0,
            "signed": (size_flags & QUIRK_FLAG_SIGNED_MASK) != 0,
        }
    )

print(json.dumps(config, indent=4))
