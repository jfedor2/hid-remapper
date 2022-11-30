#!/usr/bin/env python3

import sys
import hid
import binascii
import struct

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_VERSION = 3
REPORT_ID_CONFIG = 100

commands = {
    "reset_into_bootsel": 1,
    "pair_new_device": 12,
    "clear_bonds": 13,
}


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


if len(sys.argv) < 2 or sys.argv[1].lower() not in commands:
    print("Available commands: " + ", ".join(commands.keys()))
    sys.exit()

device = hid.Device(VENDOR_ID, PRODUCT_ID)

config_command = commands[sys.argv[1].lower()]

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, config_command, *([0] * 26)
)
device.send_feature_report(add_crc(data))
