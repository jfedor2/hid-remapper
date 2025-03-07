#!/usr/bin/env python3

from common import *

import sys
import struct
import json

config = json.load(sys.stdin)

device = get_device()

data = struct.pack("<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, SUSPEND, *([0] * 26))
device.send_feature_report(add_crc(data))

version = config.get("version", CONFIG_VERSION)
if version < 3:
    raise Exception("Incompatible version.")
partial_scroll_timeout = config.get(
    "partial_scroll_timeout", DEFAULT_PARTIAL_SCROLL_TIMEOUT
)
tap_hold_threshold = config.get("tap_hold_threshold", DEFAULT_TAP_HOLD_THRESHOLD)
gpio_debounce_time_ms = config.get("gpio_debounce_time_ms", DEFAULT_GPIO_DEBOUNCE_TIME)
if version == 3:
    unmapped_passthrough_layer_mask = (
        1 if config.get("unmapped_passthrough", True) else 0
    )
else:
    unmapped_passthrough_layer_mask = layer_list_to_mask(
        config.get("unmapped_passthrough_layers", list(range(NLAYERS)))
    )
interval_override = config.get("interval_override", 0)
our_descriptor_number = config.get("our_descriptor_number", 0)
ignore_auth_dev_inputs = config.get("ignore_auth_dev_inputs", False)
macro_entry_duration = config.get("macro_entry_duration", 1) - 1
gpio_output_mode = config.get("gpio_output_mode", 0)

flags = 0
flags |= IGNORE_AUTH_DEV_INPUTS_FLAG if ignore_auth_dev_inputs else 0
flags |= GPIO_OUTPUT_MODE_FLAG if gpio_output_mode == 1 else 0

data = struct.pack(
    "<BBBBBLBLBBB12B",
    REPORT_ID_CONFIG,
    CONFIG_VERSION,
    SET_CONFIG,
    flags,
    unmapped_passthrough_layer_mask,
    partial_scroll_timeout,
    interval_override,
    tap_hold_threshold,
    gpio_debounce_time_ms,
    our_descriptor_number,
    macro_entry_duration,
    *([0] * 12)
)
device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, CLEAR_MAPPING, *([0] * 26)
)
device.send_feature_report(add_crc(data))

for mapping in config.get("mappings", []):
    target_usage = int(mapping["target_usage"], 16)
    source_usage = int(mapping["source_usage"], 16)
    scaling = mapping.get("scaling", DEFAULT_SCALING)
    if version == 3:
        layer_mask = 1 << mapping.get("layer", 0)
    else:
        layer_mask = layer_list_to_mask(mapping.get("layers", [0]))
    flags = 0
    flags |= STICKY_FLAG if mapping.get("sticky", False) else 0
    if version >= 5:
        flags |= TAP_FLAG if mapping.get("tap", False) else 0
        flags |= HOLD_FLAG if mapping.get("hold", False) else 0
    hub_ports = ((mapping.get("target_port", 0) & 0x0F) << 4) | (
        mapping.get("source_port", 0) & 0x0F
    )
    data = struct.pack(
        "<BBBLLlBBB11B",
        REPORT_ID_CONFIG,
        CONFIG_VERSION,
        ADD_MAPPING,
        target_usage,
        source_usage,
        scaling,
        layer_mask,
        flags,
        hub_ports,
        *([0] * 11)
    )
    device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, CLEAR_MACROS, *([0] * 26)
)
device.send_feature_report(add_crc(data))

for macro_index, macro in enumerate(config.get("macros", [])):
    if macro_index >= NMACROS:
        break
    flat_zero_separated = [
        int(item, 16) for entry in macro for item in entry + ["0x00"]
    ][:-1]
    for chunk in batched(flat_zero_separated, MACRO_ITEMS_IN_PACKET):
        data = struct.pack(
            "<BBBBB6L",
            REPORT_ID_CONFIG,
            CONFIG_VERSION,
            APPEND_TO_MACRO,
            macro_index,
            len(chunk),
            *(chunk + (0,) * (MACRO_ITEMS_IN_PACKET - len(chunk)))
        )
        device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, CLEAR_EXPRESSIONS, *([0] * 26)
)
device.send_feature_report(add_crc(data))

for expr_index, expr in enumerate(config.get("expressions", [])):
    if expr_index >= NEXPRESSIONS:
        break
    elems = expr_to_elems(expr)
    while elems:
        bytes_left = 24
        pack_string = ""
        pack_items = []
        nelems = 0
        while elems and (bytes_left > 0):
            elem = elems[0]
            if elem[0] in (ops["PUSH"], ops["PUSH_USAGE"]):
                if bytes_left >= 5:
                    pack_string += "BL"
                    pack_items.append(elem[0])
                    pack_items.append(elem[1] & 0xFFFFFFFF)
                    bytes_left -= 5
                    nelems += 1
                    elems = elems[1:]
                else:
                    break
            else:
                pack_string += "B"
                pack_items.append(elem[0])
                bytes_left -= 1
                nelems += 1
                elems = elems[1:]
        data = struct.pack(
            "<BBBBB" + pack_string + ("B" * bytes_left),
            REPORT_ID_CONFIG,
            CONFIG_VERSION,
            APPEND_TO_EXPRESSION,
            expr_index,
            nelems,
            *(pack_items + [0] * bytes_left)
        )
        device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, CLEAR_QUIRKS, *([0] * 26)
)
device.send_feature_report(add_crc(data))

for quirk in config.get("quirks", []):
    size_flags = (
        (quirk["size"] & QUIRK_SIZE_MASK)
        | (QUIRK_FLAG_RELATIVE_MASK if quirk["relative"] else 0)
        | (QUIRK_FLAG_SIGNED_MASK if quirk["signed"] else 0)
    )
    data = struct.pack(
        "<BBBHHBBLHB13B",
        REPORT_ID_CONFIG,
        CONFIG_VERSION,
        ADD_QUIRK,
        int(quirk["vendor_id"], 16),
        int(quirk["product_id"], 16),
        quirk["interface"],
        quirk["report_id"],
        int(quirk["usage"], 16),
        quirk["bitpos"],
        size_flags,
        *([0] * 13)
    )
    device.send_feature_report(add_crc(data))


data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, PERSIST_CONFIG, *([0] * 26)
)
device.send_feature_report(add_crc(data))

data = get_feature_report(device, REPORT_ID_CONFIG, CONFIG_SIZE + 1)
(
    report_id,
    persist_config_return_code,
    *_,
    crc,
) = struct.unpack("<BB27BL", data)
check_crc(data, crc)

data = struct.pack("<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, RESUME, *([0] * 26))
device.send_feature_report(add_crc(data))

if persist_config_return_code == PERSIST_CONFIG_SUCCESS:
    pass
elif persist_config_return_code == PERSIST_CONFIG_CONFIG_TOO_BIG:
    raise Exception("Configuration too big to persist.")
else:
    raise Exception(
        "Unknown PERSIST_CONFIG return code ({}).".format(persist_config_return_code)
    )
