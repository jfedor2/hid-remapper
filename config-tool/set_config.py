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
partial_scroll_timeout = config.get("partial_scroll_timeout", 1000000)
unmapped_passthrough = config.get("unmapped_passthrough", True)
interval_override = config.get("interval_override", 0)

flags = UNMAPPED_PASSTHROUGH_FLAG if unmapped_passthrough else 0

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
    layer = mapping.get("layer", 0)
    flags = STICKY_FLAG if mapping.get("sticky", False) else 0
    data = struct.pack(
        "<BBBLLlBB12B",
        REPORT_ID_CONFIG,
        CONFIG_VERSION,
        ADD_MAPPING,
        target_usage,
        source_usage,
        scaling,
        layer,
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
