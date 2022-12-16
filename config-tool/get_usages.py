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
