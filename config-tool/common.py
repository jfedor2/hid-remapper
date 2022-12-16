import hid
import binascii
import struct

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_USAGE_PAGE = 0xFF00

CONFIG_VERSION = 3
CONFIG_SIZE = 32
REPORT_ID_CONFIG = 100

RESET_INTO_BOOTSEL = 1
SET_CONFIG = 2
CLEAR_MAPPING = 4
ADD_MAPPING = 5
PERSIST_CONFIG = 7
GET_CONFIG = 3
GET_MAPPING = 6
GET_OUR_USAGES = 8
GET_THEIR_USAGES = 9
SUSPEND = 10
RESUME = 11
PAIR_NEW_DEVICE = 12
CLEAR_BONDS = 13

UNMAPPED_PASSTHROUGH_FLAG = 0x01
STICKY_FLAG = 0x01


def check_crc(buf, crc_):
    if binascii.crc32(buf[1:29]) != crc_:
        raise Exception("CRC mismatch")


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


def get_device():
    devices = [
        d
        for d in hid.enumerate(VENDOR_ID, PRODUCT_ID)
        if d["usage_page"] == CONFIG_USAGE_PAGE
    ]
    if len(devices) == 0:
        raise Exception("No HID Remapper devices found.")
    if len(devices) > 1:
        raise Exception("More than one HID Remapper device found.")
    return hid.Device(path=devices[0]["path"])
