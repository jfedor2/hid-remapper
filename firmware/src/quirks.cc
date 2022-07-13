#include <cstring>

#include "quirks.h"

const uint16_t VENDOR_ID_ELECOM = 0x056e;
const uint16_t PRODUCT_ID_ELECOM_M_XT3URBK = 0x00fb;
const uint16_t PRODUCT_ID_ELECOM_M_XT3DRBK = 0x00fc;
const uint16_t PRODUCT_ID_ELECOM_M_XT4DRBK = 0x00fd;
const uint16_t PRODUCT_ID_ELECOM_M_DT1URBK = 0x00fe;
const uint16_t PRODUCT_ID_ELECOM_M_DT1DRBK = 0x00ff;
const uint16_t PRODUCT_ID_ELECOM_M_HT1URBK = 0x010c;
const uint16_t PRODUCT_ID_ELECOM_M_HT1DRBK = 0x010d;

const uint16_t VENDOR_ID_KENSINGTON = 0x047d;
const uint16_t PRODUCT_ID_KENSINGTON_SLIMBLADE = 0x2041;

const uint8_t elecom_huge_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x01,        //     Report ID (1)
    0x95, 0x05,        //     Report Count (5)
    0x75, 0x01,        //     Report Size (1)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x03,        //     Report Size (3)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x02,        //     Report Count (2)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x16, 0x00, 0x80,  //     Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x08,        //     Report Size (8)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x08,        //     Report Size (8)
    0x05, 0x0C,        //     Usage Page (Consumer)
    0x0A, 0x38, 0x02,  //     Usage (AC Pan)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
    0x09, 0x00,        // Usage (0x00)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x00,        //   Usage (0x00)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x05,        //   Report ID (5)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x2A, 0x3C, 0x02,  //   Usage Maximum (AC Format)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x3C, 0x02,  //   Logical Maximum (572)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x80,        // Usage (Sys Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x19, 0x81,        //   Usage Minimum (Sys Power Down)
    0x29, 0x83,        //   Usage Maximum (Sys Wake Up)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x03,        //   Report Count (3)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x05,        //   Report Count (5)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0xBC, 0xFF,  // Usage Page (Vendor Defined 0xFFBC)
    0x09, 0x88,        // Usage (0x88)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x04,        //   Report ID (4)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x2A, 0xFF, 0x00,  //   Usage Maximum (0xFF)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0x02, 0xFF,  // Usage Page (Vendor Defined 0xFF02)
    0x09, 0x02,        // Usage (0x02)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x06,        //   Report ID (6)
    0x09, 0x02,        //   Usage (0x02)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x07,        //   Report Count (7)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};

const uint8_t kensington_slimblade_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x02,        //     Usage Maximum (0x02)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x02,        //     Report Count (2)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x06,        //     Report Size (6)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x02,        //     Usage Maximum (0x02)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x02,        //     Report Count (2)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x06,        //     Report Size (6)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

void apply_quirks(uint16_t vendor_id, uint16_t product_id, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, const uint8_t* report_descriptor, int len) {
    // Button Fn1 is described as constants (padding) in the descriptor.
    // We add them as buttons 6.
    if (vendor_id == VENDOR_ID_ELECOM &&
        (product_id == PRODUCT_ID_ELECOM_M_XT3URBK ||
            product_id == PRODUCT_ID_ELECOM_M_XT3DRBK ||
            product_id == PRODUCT_ID_ELECOM_M_XT4DRBK) &&
        len == sizeof(elecom_huge_descriptor) &&
        !memcmp(report_descriptor, elecom_huge_descriptor, len)) {
        usage_map[1][0x00090006] = (usage_def_t){
            .report_id = 1,
            .size = 1,
            .bitpos = 5,
            .is_relative = false,
            .logical_minimum = 0,
        };
    }

    // Buttons Fn1, Fn2, Fn3 are described as constants (padding) in the descriptor.
    // We add them as buttons 6, 7, 8.
    if (vendor_id == VENDOR_ID_ELECOM &&
        (product_id == PRODUCT_ID_ELECOM_M_DT1URBK ||
            product_id == PRODUCT_ID_ELECOM_M_DT1DRBK ||
            product_id == PRODUCT_ID_ELECOM_M_HT1URBK ||
            product_id == PRODUCT_ID_ELECOM_M_HT1DRBK) &&
        len == sizeof(elecom_huge_descriptor) &&
        !memcmp(report_descriptor, elecom_huge_descriptor, len)) {
        usage_map[1][0x00090006] = (usage_def_t){
            .report_id = 1,
            .size = 1,
            .bitpos = 5,
            .is_relative = false,
            .logical_minimum = 0,
        };
        usage_map[1][0x00090007] = (usage_def_t){
            .report_id = 1,
            .size = 1,
            .bitpos = 6,
            .is_relative = false,
            .logical_minimum = 0,
        };
        usage_map[1][0x00090008] = (usage_def_t){
            .report_id = 1,
            .size = 1,
            .bitpos = 7,
            .is_relative = false,
            .logical_minimum = 0,
        };
    }

    // Top left and top right buttons use vendor-specific usages.
    // They can be remapped as is, but we also add them as buttons 3 and 4.
    if (vendor_id == VENDOR_ID_KENSINGTON &&
        product_id == PRODUCT_ID_KENSINGTON_SLIMBLADE &&
        len == sizeof(kensington_slimblade_descriptor) &&
        !memcmp(report_descriptor, kensington_slimblade_descriptor, len)) {
        usage_map[0][0x00090003] = (usage_def_t){
            .report_id = 0,
            .size = 1,
            .bitpos = 32,
            .is_relative = false,
            .logical_minimum = 0,
        };
        usage_map[0][0x00090004] = (usage_def_t){
            .report_id = 0,
            .size = 1,
            .bitpos = 33,
            .is_relative = false,
            .logical_minimum = 0,
        };
    }
}
