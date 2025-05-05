#include <cstring>

#include "constants.h"
#include "globals.h"
#include "platform.h"
#include "quirks.h"

const uint16_t VENDOR_ID_ELECOM = 0x056e;
const uint16_t PRODUCT_ID_ELECOM_M_XT3URBK = 0x00fb;
const uint16_t PRODUCT_ID_ELECOM_M_XT3DRBK = 0x00fc;
const uint16_t PRODUCT_ID_ELECOM_M_XT4DRBK = 0x00fd;
const uint16_t PRODUCT_ID_ELECOM_M_DT1URBK = 0x00fe;
const uint16_t PRODUCT_ID_ELECOM_M_DT1DRBK = 0x00ff;
const uint16_t PRODUCT_ID_ELECOM_M_HT1URBK = 0x010c;
const uint16_t PRODUCT_ID_ELECOM_M_HT1DRBK_010D = 0x010d;
const uint16_t PRODUCT_ID_ELECOM_M_HT1DRBK_011C = 0x011c;

const uint16_t VENDOR_ID_KENSINGTON = 0x047d;
const uint16_t PRODUCT_ID_KENSINGTON_SLIMBLADE = 0x2041;

const uint16_t VENDOR_ID_CH_PRODUCTS = 0x068e;
const uint16_t PRODUCT_ID_CH_PRODUCTS_DT225 = 0xf700;

const uint16_t VENDOR_ID_3DCONNEXION = 0x256f;
const uint16_t PRODUCT_ID_3DCONNEXION_SPACEMOUSE_COMPACT = 0xc635;
const uint16_t PRODUCT_ID_3DCONNEXION_SPACEMOUSE_PRO = 0xc62b;

const uint16_t VENDOR_ID_GOOGLE = 0x18d1;
const uint16_t PRODUCT_ID_GOOGLE_STADIA_CONTROLLER = 0x9400;

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

// Elecom Huge wireless, 056e:011c
const uint8_t elecom_huge_descriptor2[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x01,        //     Report ID (1)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x08,        //     Report Count (8)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x00,        //     Report Size (0)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x16, 0x00, 0x80,  //     Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x0C,        //     Usage Page (Consumer)
    0x0A, 0x38, 0x02,  //     Usage (AC Pan)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x08,        //     Report Size (8)
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
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0x3C, 0x02,  //   Logical Maximum (572)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x2A, 0x3C, 0x02,  //   Usage Maximum (AC Format)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
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
    0x95, 0x03,        //   Report Count (3)
    0x75, 0x01,        //   Report Size (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x05,        //   Report Size (5)
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

const uint8_t ch_products_dt225_descriptor[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)
    0x09, 0x01,  //   Usage (Pointer)
    0xA1, 0x00,  //   Collection (Physical)
    0x05, 0x09,  //     Usage Page (Button)
    0x19, 0x01,  //     Usage Minimum (0x01)
    0x29, 0x01,  //     Usage Maximum (0x01)
    0x15, 0x00,  //     Logical Minimum (0)
    0x25, 0x01,  //     Logical Maximum (1)
    0x95, 0x04,  //     Report Count (4)
    0x75, 0x01,  //     Report Size (1)
    0x81, 0x02,  //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,  //     Report Count (1)
    0x75, 0x04,  //     Report Size (4)
    0x81, 0x01,  //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,  //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,  //     Usage (X)
    0x09, 0x31,  //     Usage (Y)
    0x15, 0x81,  //     Logical Minimum (-127)
    0x25, 0x7F,  //     Logical Maximum (127)
    0x75, 0x08,  //     Report Size (8)
    0x95, 0x02,  //     Report Count (2)
    0x81, 0x06,  //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,        //   End Collection
    0xC0,        // End Collection
};

const uint8_t spacemouse_compact_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x08,        // Usage (Multi-axis Controller)
    0xA1, 0x01,        // Collection (Application)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x01,        //     Report ID (1)
    0x16, 0xA2, 0xFE,  //     Logical Minimum (-350)
    0x26, 0x5E, 0x01,  //     Logical Maximum (350)
    0x36, 0x88, 0xFA,  //     Physical Minimum (-1400)
    0x46, 0x78, 0x05,  //     Physical Maximum (1400)
    0x55, 0x0C,        //     Unit Exponent (-4)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x32,        //     Usage (Z)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x02,        //     Report ID (2)
    0x09, 0x33,        //     Usage (Rx)
    0x09, 0x34,        //     Usage (Ry)
    0x09, 0x35,        //     Usage (Rz)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x03,        //     Report ID (3)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x02,        //     Usage Maximum (0x02)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x35, 0x00,        //     Physical Minimum (0)
    0x45, 0x01,        //     Physical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x0E,        //     Report Count (14)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x04,        //     Report ID (4)
    0x05, 0x08,        //     Usage Page (LEDs)
    0x09, 0x4B,        //     Usage (Generic Indicator)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x01,        //     Report Size (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x07,        //     Report Size (7)
    0x91, 0x03,        //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,        //   Usage (0x01)
    0xA1, 0x02,        //   Collection (Logical)
    0x15, 0x80,        //     Logical Minimum (-128)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x09, 0x3A,        //     Usage (0x3A)
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x05,        //       Report ID (5)
    0x09, 0x20,        //       Usage (0x20)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x06,        //       Report ID (6)
    0x09, 0x21,        //       Usage (0x21)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x07,        //       Report ID (7)
    0x09, 0x22,        //       Usage (0x22)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x08,        //       Report ID (8)
    0x09, 0x23,        //       Usage (0x23)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x09,        //       Report ID (9)
    0x09, 0x24,        //       Usage (0x24)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0A,        //       Report ID (10)
    0x09, 0x25,        //       Usage (0x25)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0B,        //       Report ID (11)
    0x09, 0x26,        //       Usage (0x26)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x13,        //       Report ID (19)
    0x09, 0x2E,        //       Usage (0x2E)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x19,        //       Report ID (25)
    0x09, 0x31,        //       Usage (0x31)
    0x95, 0x04,        //       Report Count (4)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x1A,        //       Report ID (26)
    0x09, 0x32,        //       Usage (0x32)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

const uint8_t spacemouse_pro_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x08,        // Usage (Multi-axis Controller)
    0xA1, 0x01,        // Collection (Application)
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x01,        //     Report ID (1)
    0x16, 0xA2, 0xFE,  //     Logical Minimum (-350)
    0x26, 0x5E, 0x01,  //     Logical Maximum (350)
    0x36, 0x88, 0xFA,  //     Physical Minimum (-1400)
    0x46, 0x78, 0x05,  //     Physical Maximum (1400)
    0x55, 0x0C,        //     Unit Exponent (-4)
    0x65, 0x11,        //     Unit (System: SI Linear, Length: Centimeter)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x32,        //     Usage (Z)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x00,        //   Collection (Physical)
    0x85, 0x02,        //     Report ID (2)
    0x09, 0x33,        //     Usage (Rx)
    0x09, 0x34,        //     Usage (Ry)
    0x09, 0x35,        //     Usage (Rz)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x03,        //     Report ID (3)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x03,        //     Usage Maximum (0x03)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x35, 0x00,        //     Physical Minimum (0)
    0x45, 0x01,        //     Physical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x05,        //     Usage Minimum (0x05)
    0x29, 0x06,        //     Usage Maximum (0x06)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x09,        //     Usage (0x09)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x0D,        //     Usage Minimum (0x0D)
    0x29, 0x10,        //     Usage Maximum (0x10)
    0x95, 0x04,        //     Report Count (4)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //     Report Count (6)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x17,        //     Usage Minimum (0x17)
    0x29, 0x1B,        //     Usage Maximum (0x1B)
    0x95, 0x05,        //     Report Count (5)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x15,        //     Report Count (21)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xA1, 0x02,        //   Collection (Logical)
    0x85, 0x04,        //     Report ID (4)
    0x05, 0x08,        //     Usage Page (LEDs)
    0x09, 0x4B,        //     Usage (Generic Indicator)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x01,        //     Report Size (1)
    0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x07,        //     Report Size (7)
    0x91, 0x03,        //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //   End Collection
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,        //   Usage (0x01)
    0xA1, 0x02,        //   Collection (Logical)
    0x15, 0x80,        //     Logical Minimum (-128)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x09, 0x3A,        //     Usage (0x3A)
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x05,        //       Report ID (5)
    0x09, 0x20,        //       Usage (0x20)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x06,        //       Report ID (6)
    0x09, 0x21,        //       Usage (0x21)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x07,        //       Report ID (7)
    0x09, 0x22,        //       Usage (0x22)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x08,        //       Report ID (8)
    0x09, 0x23,        //       Usage (0x23)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x09,        //       Report ID (9)
    0x09, 0x24,        //       Usage (0x24)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0A,        //       Report ID (10)
    0x09, 0x25,        //       Usage (0x25)
    0x95, 0x07,        //       Report Count (7)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x0B,        //       Report ID (11)
    0x09, 0x26,        //       Usage (0x26)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x13,        //       Report ID (19)
    0x09, 0x2E,        //       Usage (0x2E)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x14,        //       Report ID (20)
    0x09, 0x2F,        //       Usage (0x2F)
    0x95, 0x04,        //       Report Count (4)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x15,        //       Report ID (21)
    0x09, 0x30,        //       Usage (0x30)
    0x95, 0x01,        //       Report Count (1)
    0xB1, 0x02,        //       Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              //     End Collection
    0xA1, 0x02,        //     Collection (Logical)
    0x85, 0x16,        //       Report ID (22)
    0x19, 0x01,        //       Usage Minimum (0x01)
    0x29, 0x1F,        //       Usage Maximum (0x1F)
    0x15, 0x00,        //       Logical Minimum (0)
    0x25, 0x01,        //       Logical Maximum (1)
    0x35, 0x00,        //       Physical Minimum (0)
    0x45, 0x01,        //       Physical Maximum (1)
    0x75, 0x01,        //       Report Size (1)
    0x95, 0x1F,        //       Report Count (31)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //       Report Count (1)
    0x81, 0x03,        //       Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //     End Collection
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

uint32_t stadia_mapping[][2] = {
    { 0x00090001, 0x00090004 },
    { 0x00090002, 0x00090001 },
    { 0x00090003, 0x00090002 },
    { 0x00090004, 0x00090005 },
    { 0x00090005, 0x00090007 },
    { 0x00090006, 0x00090008 },
    { 0x00090007, 0x00090014 },
    { 0x00090008, 0x00090013 },
    { 0x00090009, 0x0009000b },
    { 0x0009000a, 0x0009000c },
    { 0x0009000b, 0x0009000e },
    { 0x0009000c, 0x0009000f },
    { 0x0009000e, 0x00090012 },
    { 0x0009000f, 0x00090011 },
    { 0x00010033, 0x000200c5 },
    { 0x00010034, 0x000200c4 },
};

uint32_t xbox_mapping32[][2] = {
    { 0x00090001, 0x00090007 },
    { 0x00090002, 0x00090005 },
    { 0x00090003, 0x00090006 },
    { 0x00090004, 0x00090008 },
    { 0x00090005, 0x0009000d },
    { 0x00090006, 0x0009000e },
    { 0x00090009, 0x00090004 },
    { 0x0009000a, 0x00090003 },
    { 0x0009000b, 0x0009000f },
    { 0x0009000c, 0x00090010 },
    { 0x00010033, 0x000200c5 },
    { 0x00010034, 0x000200c4 },
    { 0xfff90001, 0x0009000b },
    { 0xfff90002, 0x0009000c },
    { 0xfff90003, 0x00090009 },
    { 0xfff90004, 0x0009000a },
};

uint32_t xbox_mapping7[][2] = {
    { 0x0009000d, 0x00090011 },
};

void gamepad_normalize(std::unordered_map<uint32_t, usage_def_t>& current_map, uint32_t mapping[][2], uint16_t nentries) {
    auto new_map = current_map;

    for (uint16_t i = 0; i < nentries; i++) {
        new_map.erase(mapping[i][1]);
    }
    for (uint16_t i = 0; i < nentries; i++) {
        new_map[mapping[i][0]] = current_map[mapping[i][1]];
    }

    current_map = new_map;
}

void apply_quirks(uint16_t vendor_id, uint16_t product_id, std::unordered_map<uint8_t, std::unordered_map<uint32_t, usage_def_t>>& usage_map, const uint8_t* report_descriptor, int len, uint8_t itf_num) {
    // Button Fn1 is described as a constant (padding) in the descriptor.
    // We add it as button 6.
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
            product_id == PRODUCT_ID_ELECOM_M_HT1DRBK_010D) &&
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

    // Buttons Fn1, Fn2, Fn3 don't work because Usage Maximum=5 when it should be 8
    if (vendor_id == VENDOR_ID_ELECOM &&
        product_id == PRODUCT_ID_ELECOM_M_HT1DRBK_011C &&
        len == sizeof(elecom_huge_descriptor2) &&
        !memcmp(report_descriptor, elecom_huge_descriptor2, len)) {
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

    // Buttons 2-4 don't work because Usage Maximum=1 when it should be 4
    if (vendor_id == VENDOR_ID_CH_PRODUCTS &&
        product_id == PRODUCT_ID_CH_PRODUCTS_DT225 &&
        len == sizeof(ch_products_dt225_descriptor) &&
        !memcmp(report_descriptor, ch_products_dt225_descriptor, len)) {
        usage_map[0][0x00090002] = (usage_def_t){
            .report_id = 0,
            .size = 1,
            .bitpos = 1,
            .is_relative = false,
            .logical_minimum = 0,
        };
        usage_map[0][0x00090003] = (usage_def_t){
            .report_id = 0,
            .size = 1,
            .bitpos = 2,
            .is_relative = false,
            .logical_minimum = 0,
        };
        usage_map[0][0x00090004] = (usage_def_t){
            .report_id = 0,
            .size = 1,
            .bitpos = 3,
            .is_relative = false,
            .logical_minimum = 0,
        };
    }

    // SpaceMouse says its usages are relative, but they're not.
    if (vendor_id == VENDOR_ID_3DCONNEXION &&
        ((product_id == PRODUCT_ID_3DCONNEXION_SPACEMOUSE_COMPACT &&
             len == sizeof(spacemouse_compact_descriptor) &&
             !memcmp(report_descriptor, spacemouse_compact_descriptor, len)) ||
            (product_id == PRODUCT_ID_3DCONNEXION_SPACEMOUSE_PRO &&
                len == sizeof(spacemouse_pro_descriptor) &&
                !memcmp(report_descriptor, spacemouse_pro_descriptor, len)))) {
        usage_map[1][0x00010030].is_relative = false;
        usage_map[1][0x00010031].is_relative = false;
        usage_map[1][0x00010032].is_relative = false;
        usage_map[2][0x00010033].is_relative = false;
        usage_map[2][0x00010034].is_relative = false;
        usage_map[2][0x00010035].is_relative = false;
    }

    // apply user-defined quirks
    my_mutex_enter(MutexId::QUIRKS);
    for (uint16_t i = 0; i < quirks.size(); i++) {
        quirk_t* quirk = &quirks[i];
        if (((quirk->vendor_id == vendor_id) && (quirk->product_id == product_id) && (quirk->interface == itf_num)) ||
            ((quirk->vendor_id == 0) && (quirk->product_id == 0))) {
            uint8_t quirk_size = quirk->size_flags & QUIRK_SIZE_MASK;
            if (quirk_size != 0) {
                usage_map[quirk->report_id][quirk->usage] = (usage_def_t){
                    .report_id = quirk->report_id,
                    .size = quirk_size,
                    .bitpos = quirk->bitpos,
                    .is_relative = (quirk->size_flags & QUIRK_FLAG_RELATIVE_MASK) != 0,
                    .logical_minimum = ((quirk->size_flags & QUIRK_FLAG_SIGNED_MASK) != 0) ? -1 : 0,
                };
            } else {
                usage_map[quirk->report_id].erase(quirk->usage);
            }
        }
    }
    my_mutex_exit(MutexId::QUIRKS);

    if (normalize_gamepad_inputs) {
        if (vendor_id == VENDOR_ID_GOOGLE &&
            product_id == PRODUCT_ID_GOOGLE_STADIA_CONTROLLER) {
            gamepad_normalize(usage_map[3], stadia_mapping, sizeof(stadia_mapping) / sizeof(stadia_mapping[0]));
        }
        if (vendor_id == VENDOR_ID_MICROSOFT &&
            product_id == PRODUCT_ID_MICROSOFT_XBOX_WIRELESS_CONTROLLER) {
            gamepad_normalize(usage_map[32], xbox_mapping32, sizeof(xbox_mapping32) / sizeof(xbox_mapping32[0]));
            gamepad_normalize(usage_map[7], xbox_mapping7, sizeof(xbox_mapping7) / sizeof(xbox_mapping7[0]));
        }
    }
}
