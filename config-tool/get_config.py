#!/usr/bin/env python3

from common import *

import struct
import json

device = get_device()

data = struct.pack("<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, GET_CONFIG, *([0] * 26))
device.send_feature_report(add_crc(data))

data = device.get_feature_report(REPORT_ID_CONFIG, CONFIG_SIZE + 1)

(
    report_id,
    version,
    flags,
    partial_scroll_timeout,
    mapping_count,
    our_usage_count,
    their_usage_count,
    interval_override,
    *_,
    crc,
) = struct.unpack("<BBBLLLLB9BL", data)
check_crc(data, crc)

config = {
    "version": version,
    "unmapped_passthrough": (flags & UNMAPPED_PASSTHROUGH_FLAG) != 0,
    "partial_scroll_timeout": partial_scroll_timeout,
    "interval_override": interval_override,
    "mappings": [],
}

for i in range(mapping_count):
    data = struct.pack(
        "<BBBL22B", REPORT_ID_CONFIG, CONFIG_VERSION, GET_MAPPING, i, *([0] * 22)
    )
    device.send_feature_report(add_crc(data))
    data = device.get_feature_report(REPORT_ID_CONFIG, CONFIG_SIZE + 1)
    (
        report_id,
        target_usage,
        source_usage,
        scaling,
        layer,
        flags,
        *_,
        crc,
    ) = struct.unpack("<BLLlBB14BL", data)
    check_crc(data, crc)
    config["mappings"].append(
        {
            "target_usage": "{0:#010x}".format(target_usage),
            "source_usage": "{0:#010x}".format(source_usage),
            "scaling": scaling,
            "layer": layer,
            "sticky": (flags & 0x01) != 0,
        }
    )

print(json.dumps(config, indent=2))
