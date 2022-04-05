#!/usr/bin/env python3

import hid
import binascii
import struct
import json

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_VERSION = 2
CONFIG_SIZE = 32
REPORT_ID_CONFIG = 100

GET_CONFIG = 3
GET_MAPPING = 6

UNMAPPED_PASSTHROUGH_FLAG = 0x01


def check_crc(buf, crc_):
    if binascii.crc32(buf[1:29]) != crc_:
        raise Exception("CRC mismatch")


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


device = hid.Device(VENDOR_ID, PRODUCT_ID)

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
    *_,
    crc,
) = struct.unpack("<BBBLLLL10BL", data)
check_crc(data, crc)

config = {
    "version": version,
    "unmapped_passthrough": (flags & UNMAPPED_PASSTHROUGH_FLAG) != 0,
    "partial_scroll_timeout": partial_scroll_timeout,
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
