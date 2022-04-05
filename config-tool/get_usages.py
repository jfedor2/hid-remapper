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
GET_OUR_USAGES = 8
GET_THEIR_USAGES = 9


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

usages = {"our_usages": [], "their_usages": []}

for command, key, count in [
    [GET_OUR_USAGES, "our_usages", our_usage_count],
    [GET_THEIR_USAGES, "their_usages", their_usage_count],
]:
    for i in range(0, count, 3):
        data = struct.pack(
            "<BBBL22B", REPORT_ID_CONFIG, CONFIG_VERSION, command, i, *([0] * 22)
        )
        device.send_feature_report(add_crc(data))
        data = device.get_feature_report(REPORT_ID_CONFIG, CONFIG_SIZE + 1)
        (report_id, *usages_rle, _, crc) = struct.unpack("<B6L4BL", data)
        check_crc(data, crc)
        for u, l in zip(*(iter(usages_rle),) * 2):
            if u != 0:
                usages[key].extend("{0:#010x}".format(u + i) for i in range(l))

print(json.dumps(usages, indent=2))
