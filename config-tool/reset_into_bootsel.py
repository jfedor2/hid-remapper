#!/usr/bin/env python3

import hid
import binascii
import struct

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_VERSION = 3
REPORT_ID_CONFIG = 100

RESET_INTO_BOOTSEL = 1


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


device = hid.Device(VENDOR_ID, PRODUCT_ID)

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, RESET_INTO_BOOTSEL, *([0] * 26)
)
device.send_feature_report(add_crc(data))
