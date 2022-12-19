#!/usr/bin/env python3

from common import *

import sys
import struct

commands = {
    "reset_into_bootsel": RESET_INTO_BOOTSEL,
    "pair_new_device": PAIR_NEW_DEVICE,
    "clear_bonds": CLEAR_BONDS,
    "flash_b_side": FLASH_B_SIDE,
}

device = get_device()

config_command = commands[sys.argv[1].lower()]

data = struct.pack(
    "<BBB26B", REPORT_ID_CONFIG, CONFIG_VERSION, config_command, *([0] * 26)
)
device.send_feature_report(add_crc(data))
