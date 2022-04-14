#!/usr/bin/env python3

import sys
import hid
import binascii
import struct
import json

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_VERSION = 3
CONFIG_SIZE = 32
REPORT_ID_CONFIG = 100

SET_CONFIG = 2
CLEAR_MAPPING = 4
ADD_MAPPING = 5
PERSIST_CONFIG = 7
SUSPEND = 10
RESUME = 11

UNMAPPED_PASSTHROUGH_FLAG = 0x01
STICKY_FLAG = 0x01


def check_crc(buf, crc_):
    if binascii.crc32(buf[1:29]) != crc_:
        raise Exception("CRC mismatch")


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


config = json.load(sys.stdin)

device = hid.Device(VENDOR_ID, PRODUCT_ID)

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
