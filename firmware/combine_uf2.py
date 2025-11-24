#!/usr/bin/env python3

# Combines two UF2 files, first being a normal flash binary, second a
# RAM-only binary. Updates the "total blocks" field in the first binary
# blocks so that it's different than actual block counts for both binaries.
# This makes the RP2040 bootloader write the first binary to flash, then
# load the second binary to RAM and run it.

import struct
import sys


def read_uf2_blocks(filename):
    blocks = []
    with open(filename, "rb") as f:
        while True:
            block = f.read(512)
            if not block:
                break
            if len(block) != 512:
                raise Exception("block size != 512")
            blocks.append(bytearray(block))
    return blocks


flash_blocks = read_uf2_blocks(sys.argv[1])
ram_blocks = read_uf2_blocks(sys.argv[2])

patched_total_blocks = max(len(flash_blocks), len(ram_blocks)) + 1

with open(sys.argv[3], "wb") as f:
    for block in flash_blocks:
        struct.pack_into("<I", block, 24, patched_total_blocks)
        f.write(block)
    for block in ram_blocks:
        f.write(block)
