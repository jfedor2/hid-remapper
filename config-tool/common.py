import hid
import binascii
import struct
import itertools
import re
import time

VENDOR_ID = 0xCAFE
PRODUCT_ID = 0xBAF2

CONFIG_USAGE_PAGE = 0xFF00
CONFIG_USAGE = 0x0020

CONFIG_VERSION = 17
CONFIG_SIZE = 32
REPORT_ID_CONFIG = 100

DEFAULT_PARTIAL_SCROLL_TIMEOUT = 1000000
DEFAULT_TAP_HOLD_THRESHOLD = 200000
DEFAULT_GPIO_DEBOUNCE_TIME = 5
DEFAULT_SCALING = 1000
DEFAULT_MACRO_ENTRY_DURATION = 1

NLAYERS = 8

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
FLASH_B_SIDE = 14
CLEAR_MACROS = 15
APPEND_TO_MACRO = 16
GET_MACRO = 17
INVALID_COMMAND = 18
CLEAR_EXPRESSIONS = 19
APPEND_TO_EXPRESSION = 20
GET_EXPRESSION = 21
SET_MONITOR_ENABLED = 22
CLEAR_QUIRKS = 23
ADD_QUIRK = 24
GET_QUIRK = 25

PERSIST_CONFIG_SUCCESS = 1
PERSIST_CONFIG_CONFIG_TOO_BIG = 2


UNMAPPED_PASSTHROUGH_FLAG = 0x01
STICKY_FLAG = 1 << 0
TAP_FLAG = 1 << 1
HOLD_FLAG = 1 << 2

IGNORE_AUTH_DEV_INPUTS_FLAG = 1 << 4
GPIO_OUTPUT_MODE_FLAG = 1 << 5

NMACROS = 32
NEXPRESSIONS = 8
MACRO_ITEMS_IN_PACKET = 6

QUIRK_FLAG_RELATIVE_MASK = 0b10000000
QUIRK_FLAG_SIGNED_MASK = 0b01000000
QUIRK_SIZE_MASK = 0b00111111

ops = {
    "PUSH": 0,
    "PUSH_USAGE": 1,
    "INPUT_STATE": 2,
    "ADD": 3,
    "MUL": 4,
    "EQ": 5,
    "TIME": 6,
    "MOD": 7,
    "GT": 8,
    "NOT": 9,
    "INPUT_STATE_BINARY": 10,
    "ABS": 11,
    "DUP": 12,
    "SIN": 13,
    "COS": 14,
    "DEBUG": 15,
    "AUTO_REPEAT": 16,
    "RELU": 17,
    "CLAMP": 18,
    "SCALING": 19,
    "LAYER_STATE": 20,
    "STICKY_STATE": 21,
    "TAP_STATE": 22,
    "HOLD_STATE": 23,
    "BITWISE_OR": 24,
    "BITWISE_AND": 25,
    "BITWISE_NOT": 26,
    "PREV_INPUT_STATE": 27,
    "PREV_INPUT_STATE_BINARY": 28,
    "STORE": 29,
    "RECALL": 30,
    "SQRT": 31,
    "ATAN2": 32,
    "ROUND": 33,
    "PORT": 34,
    "DPAD": 35,
    "EOL": 36,
    "INPUT_STATE_FP32": 37,
    "PREV_INPUT_STATE_FP32": 38,
    "MIN": 39,
    "MAX": 40,
    "IFTE": 41,
    "DIV": 42,
    "SWAP": 43,
    "MONITOR": 44,
    "SIGN": 45,
    "SUB": 46,
    "PRINT_IF": 47,
    "TIME_SEC": 48,
    "LT": 49,
    "PLUGGED_IN": 50,
    "INPUT_STATE_SCALED": 51,
    "PREV_INPUT_STATE_SCALED": 52,
    "DEADZONE": 53,
    "DEADZONE2": 54,
}

opcodes = {v: k for k, v in ops.items()}


def check_crc(buf, crc_):
    if binascii.crc32(buf[1:29]) != crc_:
        raise Exception("CRC mismatch")


def add_crc(buf):
    return buf + struct.pack("<L", binascii.crc32(buf[1:]))


def get_device():
    devices = [
        d
        for d in hid.enumerate()
        if d["usage_page"] == CONFIG_USAGE_PAGE and d["usage"] == CONFIG_USAGE
    ]
    if len(devices) == 0:
        raise Exception("No HID Remapper devices found.")
    if len(devices) > 1:
        raise Exception("More than one HID Remapper device found.")
    return hid.Device(path=devices[0]["path"])


def mask_to_layer_list(layer_mask):
    return [layer for layer in range(NLAYERS) if (layer_mask & (1 << layer))]


def layer_list_to_mask(layer_list):
    return sum([(1 << layer) for layer in range(NLAYERS) if layer in layer_list])


def batched(iterable, n):
    "Batch data into tuples of length n. The last batch may be shorter."
    # batched('ABCDEFG', 3) --> ABC DEF G
    if n < 1:
        raise ValueError("n must be at least one")
    it = iter(iterable)
    while batch := tuple(itertools.islice(it, n)):
        yield batch


def convert_elem(elem):
    if elem[0:2].lower() == "0x":
        return (ops["PUSH_USAGE"], int(elem, 16))
    if elem[0] in ("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-"):
        return (ops["PUSH"], int(elem))
    if elem.upper() in ops:
        return (ops[elem.upper()],)
    raise Exception("Invalid expression: '{}'".format(elem))


def expr_to_elems(expr):
    return [convert_elem(x) for x in re.sub(r"(?s)/\*.*?\*/", " ", expr).split()]


def get_feature_report(device, report_id, size):
    attempts_left = 10
    delay = 0.002
    while True:
        data = device.get_feature_report(report_id, size)
        if len(data) > 1:
            return data
        attempts_left -= 1
        if attempts_left > 0:
            time.sleep(delay)
            delay *= 2
            continue
        raise Exception("Error in get_feature_report (given up retrying)")
