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
partial_scroll_timeout = config.get("partial_scroll_timeout", 1000000)
if version == 3:
    unmapped_passthrough_layer_mask = (
        1 if config.get("unmapped_passthrough", True) else 0
    )
else:
    unmapped_passthrough_layer_mask = layer_list_to_mask(
        config.get("unmapped_passthrough_layers", list(range(NLAYERS)))
    )
interval_override = config.get("interval_override", 0)

flags = unmapped_passthrough_layer_mask

data = struct.pack(
    "<BBBBLB20B",
    REPORT_ID_CONFIG,
    CONFIG_VERSION,
    SET_CONFIG,
    flags,
    partial_scroll_timeout,
    interval_override,
    *([0] * 20)
)
device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, CLEAR_MAPPING, *([0] * 26)
)
device.send_feature_report(add_crc(data))

for mapping in config.get("mappings", []):
    target_usage = int(mapping["target_usage"], 16)
    source_usage = int(mapping["source_usage"], 16)
    scaling = mapping.get("scaling", 1000)
    if version == 3:
        layer_mask = 1 << mapping.get("layer", 0)
    else:
        layer_mask = layer_list_to_mask(mapping.get("layers", [0]))
    flags = STICKY_FLAG if mapping.get("sticky", False) else 0
    data = struct.pack(
        "<BBBLLlBB12B",
        REPORT_ID_CONFIG,
        CONFIG_VERSION,
        ADD_MAPPING,
        target_usage,
        source_usage,
        scaling,
        layer_mask,
        flags,
        *([0] * 12)
    )
    device.send_feature_report(add_crc(data))

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, PERSIST_CONFIG, *([0] * 26)
)
device.send_feature_report(add_crc(data))

data = struct.pack("<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, RESUME, *([0] * 26))
device.send_feature_report(add_crc(data))
