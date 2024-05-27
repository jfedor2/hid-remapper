#include <cstring>

#include "globals.h"
#include "our_descriptor.h"
#include "ps_auth.h"
#include "remapper.h"

const uint8_t REPORT_ID_MOUSE = 1;
const uint8_t REPORT_ID_KEYBOARD = 2;
const uint8_t REPORT_ID_CONSUMER = 3;

const uint8_t our_report_descriptor_kb_mouse[] = {
    0x05, 0x01,                   // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,                   // Usage (Mouse)
    0xA1, 0x01,                   // Collection (Application)
    0x05, 0x01,                   //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,                   //   Usage (Mouse)
    0xA1, 0x02,                   //   Collection (Logical)
    0x85, REPORT_ID_MOUSE,        //     Report ID (REPORT_ID_MOUSE)
    0x09, 0x01,                   //     Usage (Pointer)
    0xA1, 0x00,                   //     Collection (Physical)
    0x05, 0x09,                   //       Usage Page (Button)
    0x19, 0x01,                   //       Usage Minimum (0x01)
    0x29, 0x08,                   //       Usage Maximum (0x08)
    0x95, 0x08,                   //       Report Count (8)
    0x75, 0x01,                   //       Report Size (1)
    0x25, 0x01,                   //       Logical Maximum (1)
    0x81, 0x02,                   //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,                   //       Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,                   //       Usage (X)
    0x09, 0x31,                   //       Usage (Y)
    0x95, 0x02,                   //       Report Count (2)
    0x75, 0x10,                   //       Report Size (16)
    0x16, 0x00, 0x80,             //       Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,             //       Logical Maximum (32767)
    0x81, 0x06,                   //       Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xA1, 0x02,                   //       Collection (Logical)
    0x85, REPORT_ID_MULTIPLIER,   //         Report ID (REPORT_ID_MULTIPLIER)
    0x09, 0x48,                   //         Usage (Resolution Multiplier)
    0x95, 0x01,                   //         Report Count (1)
    0x75, 0x02,                   //         Report Size (2)
    0x15, 0x00,                   //         Logical Minimum (0)
    0x25, 0x01,                   //         Logical Maximum (1)
    0x35, 0x01,                   //         Physical Minimum (1)
    0x45, RESOLUTION_MULTIPLIER,  //         Physical Maximum (RESOLUTION_MULTIPLIER)
    0xB1, 0x02,                   //         Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, REPORT_ID_MOUSE,        //         Report ID (REPORT_ID_MOUSE)
    0x09, 0x38,                   //         Usage (Wheel)
    0x35, 0x00,                   //         Physical Minimum (0)
    0x45, 0x00,                   //         Physical Maximum (0)
    0x16, 0x00, 0x80,             //         Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,             //         Logical Maximum (32767)
    0x75, 0x10,                   //         Report Size (16)
    0x81, 0x06,                   //         Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                         //       End Collection
    0xA1, 0x02,                   //       Collection (Logical)
    0x85, REPORT_ID_MULTIPLIER,   //         Report ID (REPORT_ID_MULTIPLIER)
    0x09, 0x48,                   //         Usage (Resolution Multiplier)
    0x75, 0x02,                   //         Report Size (2)
    0x15, 0x00,                   //         Logical Minimum (0)
    0x25, 0x01,                   //         Logical Maximum (1)
    0x35, 0x01,                   //         Physical Minimum (1)
    0x45, RESOLUTION_MULTIPLIER,  //         Physical Maximum (RESOLUTION_MULTIPLIER)
    0xB1, 0x02,                   //         Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x35, 0x00,                   //         Physical Minimum (0)
    0x45, 0x00,                   //         Physical Maximum (0)
    0x75, 0x04,                   //         Report Size (4)
    0xB1, 0x03,                   //         Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, REPORT_ID_MOUSE,        //         Report ID (REPORT_ID_MOUSE)
    0x05, 0x0C,                   //         Usage Page (Consumer)
    0x16, 0x00, 0x80,             //         Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,             //         Logical Maximum (32767)
    0x75, 0x10,                   //         Report Size (16)
    0x0A, 0x38, 0x02,             //         Usage (AC Pan)
    0x81, 0x06,                   //         Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                         //       End Collection
    0xC0,                         //     End Collection
    0xC0,                         //   End Collection
    0xC0,                         // End Collection

    0x05, 0x01,                // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,                // Usage (Keyboard)
    0xA1, 0x01,                // Collection (Application)
    0x85, REPORT_ID_KEYBOARD,  //   Report ID (REPORT_ID_KEYBOARD)
    0x05, 0x07,                //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,                //   Usage Minimum (0xE0)
    0x29, 0xE7,                //   Usage Maximum (0xE7)
    0x15, 0x00,                //   Logical Minimum (0)
    0x25, 0x01,                //   Logical Maximum (1)
    0x75, 0x01,                //   Report Size (1)
    0x95, 0x08,                //   Report Count (8)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x04,                //   Usage Minimum (0x04)
    0x29, 0x73,                //   Usage Maximum (0x73)
    0x95, 0x70,                //   Report Count (112)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x87,                //   Usage Minimum (0x87)
    0x29, 0x8B,                //   Usage Maximum (0x8B)
    0x95, 0x05,                //   Report Count (5)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x90,                //   Usage (0x90)
    0x09, 0x91,                //   Usage (0x91)
    0x95, 0x02,                //   Report Count (2)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,                //   Report Count (1)
    0x81, 0x03,                //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, REPORT_ID_LEDS,      //   Report ID (REPORT_ID_LEDS)
    0x05, 0x08,                //   Usage Page (LEDs)
    0x95, 0x05,                //   Report Count (5)
    0x19, 0x01,                //   Usage Minimum (Num Lock)
    0x29, 0x05,                //   Usage Maximum (Kana)
    0x91, 0x02,                //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,                //   Report Count (1)
    0x75, 0x03,                //   Report Size (3)
    0x91, 0x03,                //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                      // End Collection

    0x05, 0x0C,                // Usage Page (Consumer)
    0x09, 0x01,                // Usage (Consumer Control)
    0xA1, 0x01,                // Collection (Application)
    0x85, REPORT_ID_CONSUMER,  //   Report ID (REPORT_ID_CONSUMER)
    0x15, 0x00,                //   Logical Minimum (0)
    0x25, 0x01,                //   Logical Maximum (1)
    0x09, 0xB5,                //   Usage (Scan Next Track)
    0x09, 0xB6,                //   Usage (Scan Previous Track)
    0x09, 0xB7,                //   Usage (Stop)
    0x09, 0xCD,                //   Usage (Play/Pause)
    0x09, 0xE2,                //   Usage (Mute)
    0x09, 0xE9,                //   Usage (Volume Increment)
    0x09, 0xEA,                //   Usage (Volume Decrement)
    0x75, 0x01,                //   Report Size (1)
    0x95, 0x07,                //   Report Count (7)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0B,                //   Usage Page (Telephony)
    0x09, 0x2F,                //   Usage (Phone Mute)
    0x95, 0x01,                //   Report Count (1)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                      // End Collection
};

const uint8_t our_report_descriptor_absolute[] = {
    0x05, 0x01,                   // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,                   // Usage (Mouse)
    0xA1, 0x01,                   // Collection (Application)
    0x05, 0x01,                   //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,                   //   Usage (Mouse)
    0xA1, 0x02,                   //   Collection (Logical)
    0x85, REPORT_ID_MOUSE,        //     Report ID (REPORT_ID_MOUSE)
    0x09, 0x01,                   //     Usage (Pointer)
    0xA1, 0x00,                   //     Collection (Physical)
    0x05, 0x09,                   //       Usage Page (Button)
    0x19, 0x01,                   //       Usage Minimum (0x01)
    0x29, 0x08,                   //       Usage Maximum (0x08)
    0x95, 0x08,                   //       Report Count (8)
    0x75, 0x01,                   //       Report Size (1)
    0x25, 0x01,                   //       Logical Maximum (1)
    0x81, 0x02,                   //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,                   //       Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,                   //       Usage (X)
    0x09, 0x31,                   //       Usage (Y)
    0x95, 0x02,                   //       Report Count (2)
    0x75, 0x10,                   //       Report Size (16)
    0x16, 0x00, 0x00,             //       Logical Minimum (0)
    0x26, 0xFF, 0x7F,             //       Logical Maximum (32767)
    0x81, 0x02,                   //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xA1, 0x02,                   //       Collection (Logical)
    0x85, REPORT_ID_MULTIPLIER,   //         Report ID (REPORT_ID_MULTIPLIER)
    0x09, 0x48,                   //         Usage (Resolution Multiplier)
    0x95, 0x01,                   //         Report Count (1)
    0x75, 0x02,                   //         Report Size (2)
    0x15, 0x00,                   //         Logical Minimum (0)
    0x25, 0x01,                   //         Logical Maximum (1)
    0x35, 0x01,                   //         Physical Minimum (1)
    0x45, RESOLUTION_MULTIPLIER,  //         Physical Maximum (RESOLUTION_MULTIPLIER)
    0xB1, 0x02,                   //         Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, REPORT_ID_MOUSE,        //         Report ID (REPORT_ID_MOUSE)
    0x09, 0x38,                   //         Usage (Wheel)
    0x35, 0x00,                   //         Physical Minimum (0)
    0x45, 0x00,                   //         Physical Maximum (0)
    0x16, 0x00, 0x80,             //         Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,             //         Logical Maximum (32767)
    0x75, 0x10,                   //         Report Size (16)
    0x81, 0x06,                   //         Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                         //       End Collection
    0xA1, 0x02,                   //       Collection (Logical)
    0x85, REPORT_ID_MULTIPLIER,   //         Report ID (REPORT_ID_MULTIPLIER)
    0x09, 0x48,                   //         Usage (Resolution Multiplier)
    0x75, 0x02,                   //         Report Size (2)
    0x15, 0x00,                   //         Logical Minimum (0)
    0x25, 0x01,                   //         Logical Maximum (1)
    0x35, 0x01,                   //         Physical Minimum (1)
    0x45, RESOLUTION_MULTIPLIER,  //         Physical Maximum (RESOLUTION_MULTIPLIER)
    0xB1, 0x02,                   //         Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x35, 0x00,                   //         Physical Minimum (0)
    0x45, 0x00,                   //         Physical Maximum (0)
    0x75, 0x04,                   //         Report Size (4)
    0xB1, 0x03,                   //         Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, REPORT_ID_MOUSE,        //         Report ID (REPORT_ID_MOUSE)
    0x05, 0x0C,                   //         Usage Page (Consumer)
    0x16, 0x00, 0x80,             //         Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,             //         Logical Maximum (32767)
    0x75, 0x10,                   //         Report Size (16)
    0x0A, 0x38, 0x02,             //         Usage (AC Pan)
    0x81, 0x06,                   //         Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                         //       End Collection
    0xC0,                         //     End Collection
    0xC0,                         //   End Collection
    0xC0,                         // End Collection

    0x05, 0x01,                // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,                // Usage (Keyboard)
    0xA1, 0x01,                // Collection (Application)
    0x85, REPORT_ID_KEYBOARD,  //   Report ID (REPORT_ID_KEYBOARD)
    0x05, 0x07,                //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,                //   Usage Minimum (0xE0)
    0x29, 0xE7,                //   Usage Maximum (0xE7)
    0x15, 0x00,                //   Logical Minimum (0)
    0x25, 0x01,                //   Logical Maximum (1)
    0x75, 0x01,                //   Report Size (1)
    0x95, 0x08,                //   Report Count (8)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x04,                //   Usage Minimum (0x04)
    0x29, 0x73,                //   Usage Maximum (0x73)
    0x95, 0x70,                //   Report Count (112)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x87,                //   Usage Minimum (0x87)
    0x29, 0x8B,                //   Usage Maximum (0x8B)
    0x95, 0x05,                //   Report Count (5)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x90,                //   Usage (0x90)
    0x09, 0x91,                //   Usage (0x91)
    0x95, 0x02,                //   Report Count (2)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,                //   Report Count (1)
    0x81, 0x03,                //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, REPORT_ID_LEDS,      //   Report ID (REPORT_ID_LEDS)
    0x05, 0x08,                //   Usage Page (LEDs)
    0x95, 0x05,                //   Report Count (5)
    0x19, 0x01,                //   Usage Minimum (Num Lock)
    0x29, 0x05,                //   Usage Maximum (Kana)
    0x91, 0x02,                //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x01,                //   Report Count (1)
    0x75, 0x03,                //   Report Size (3)
    0x91, 0x03,                //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                      // End Collection

    0x05, 0x0C,                // Usage Page (Consumer)
    0x09, 0x01,                // Usage (Consumer Control)
    0xA1, 0x01,                // Collection (Application)
    0x85, REPORT_ID_CONSUMER,  //   Report ID (REPORT_ID_CONSUMER)
    0x15, 0x00,                //   Logical Minimum (0)
    0x25, 0x01,                //   Logical Maximum (1)
    0x09, 0xB5,                //   Usage (Scan Next Track)
    0x09, 0xB6,                //   Usage (Scan Previous Track)
    0x09, 0xB7,                //   Usage (Stop)
    0x09, 0xCD,                //   Usage (Play/Pause)
    0x09, 0xE2,                //   Usage (Mute)
    0x09, 0xE9,                //   Usage (Volume Increment)
    0x09, 0xEA,                //   Usage (Volume Decrement)
    0x75, 0x01,                //   Report Size (1)
    0x95, 0x07,                //   Report Count (7)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0B,                //   Usage Page (Telephony)
    0x09, 0x2F,                //   Usage (Phone Mute)
    0x95, 0x01,                //   Report Count (1)
    0x81, 0x02,                //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                      // End Collection
};

const uint8_t our_report_descriptor_horipad[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x01,        //   Physical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x0E,        //   Report Count (14)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x0E,        //   Usage Maximum (0x0E)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x25, 0x07,        //   Logical Maximum (7)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
    0x09, 0x39,        //   Usage (Hat switch)
    0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
    0x65, 0x00,        //   Unit (None)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

uint8_t const our_report_descriptor_ps4[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x39,        //   Usage (Hat switch)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x07,        //   Logical Maximum (7)
    0x35, 0x00,        //   Physical Minimum (0)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
    0x65, 0x00,        //   Unit (None)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x0E,        //   Usage Maximum (0x0E)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x0E,        //   Report Count (14)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x20,        //   Usage (0x20)
    0x75, 0x06,        //   Report Size (6)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x33,        //   Usage (Rx)
    0x09, 0x34,        //   Usage (Ry)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
    0x09, 0x21,        //   Usage (0x21)
    0x95, 0x36,        //   Report Count (54)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x05,        //   Report ID (5)
    0x09, 0x22,        //   Usage (0x22)
    0x95, 0x1F,        //   Report Count (31)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0x03,        //   Report ID (3)
    0x0A, 0x21, 0x27,  //   Usage (0x2721)
    0x95, 0x2F,        //   Report Count (47)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x06, 0x80, 0xFF,  //   Usage Page (Vendor Defined 0xFF80)
    0x85, 0xE0,        //   Report ID (-32)
    0x09, 0x57,        //   Usage (0x57)
    0x95, 0x02,        //   Report Count (2)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x06, 0xF0, 0xFF,  // Usage Page (Vendor Defined 0xFFF0)
    0x09, 0x40,        // Usage (0x40)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0xF0,        //   Report ID (-16)
    0x09, 0x47,        //   Usage (0x47)
    0x95, 0x3F,        //   Report Count (63)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0xF1,        //   Report ID (-15)
    0x09, 0x48,        //   Usage (0x48)
    0x95, 0x3F,        //   Report Count (63)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0xF2,        //   Report ID (-14)
    0x09, 0x49,        //   Usage (0x49)
    0x95, 0x0F,        //   Report Count (15)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x85, 0xF3,        //   Report ID (-13)
    0x0A, 0x01, 0x47,  //   Usage (0x4701)
    0x95, 0x07,        //   Report Count (7)
    0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};

uint8_t const our_report_descriptor_stadia[] = {
    0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,                    // Usage (Game Pad)
    0xA1, 0x01,                    // Collection (Application)
    0x85, 0x03,                    //   Report ID (3)
    0x05, 0x01,                    //   Usage Page (Generic Desktop Ctrls)
    0x75, 0x04,                    //   Report Size (4)
    0x95, 0x01,                    //   Report Count (1)
    0x25, 0x07,                    //   Logical Maximum (7)
    0x46, 0x3B, 0x01,              //   Physical Maximum (315)
    0x65, 0x14,                    //   Unit (System: English Rotation, Length: Centimeter)
    0x09, 0x39,                    //   Usage (Hat switch)
    0x81, 0x42,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
    0x45, 0x00,                    //   Physical Maximum (0)
    0x65, 0x00,                    //   Unit (None)
    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x04,                    //   Report Count (4)
    0x81, 0x01,                    //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x09,                    //   Usage Page (Button)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0x01,                    //   Logical Maximum (1)
    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x0F,                    //   Report Count (15)
    0x09, 0x12,                    //   Usage (0x12)
    0x09, 0x11,                    //   Usage (0x11)
    0x09, 0x14,                    //   Usage (0x14)
    0x09, 0x13,                    //   Usage (0x13)
    0x09, 0x0D,                    //   Usage (0x0D)
    0x09, 0x0C,                    //   Usage (0x0C)
    0x09, 0x0B,                    //   Usage (0x0B)
    0x09, 0x0F,                    //   Usage (0x0F)
    0x09, 0x0E,                    //   Usage (0x0E)
    0x09, 0x08,                    //   Usage (0x08)
    0x09, 0x07,                    //   Usage (0x07)
    0x09, 0x05,                    //   Usage (0x05)
    0x09, 0x04,                    //   Usage (0x04)
    0x09, 0x02,                    //   Usage (0x02)
    0x09, 0x01,                    //   Usage (0x01)
    0x81, 0x02,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x01,                    //   Report Count (1)
    0x81, 0x01,                    //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,                    //   Usage Page (Generic Desktop Ctrls)
    0x15, 0x01,                    //   Logical Minimum (1)
    0x26, 0xFF, 0x00,              //   Logical Maximum (255)
    0x09, 0x01,                    //   Usage (Pointer)
    0xA1, 0x00,                    //   Collection (Physical)
    0x09, 0x30,                    //     Usage (X)
    0x09, 0x31,                    //     Usage (Y)
    0x75, 0x08,                    //     Report Size (8)
    0x95, 0x02,                    //     Report Count (2)
    0x81, 0x02,                    //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                          //   End Collection
    0x09, 0x01,                    //   Usage (Pointer)
    0xA1, 0x00,                    //   Collection (Physical)
    0x09, 0x32,                    //     Usage (Z)
    0x09, 0x35,                    //     Usage (Rz)
    0x75, 0x08,                    //     Report Size (8)
    0x95, 0x02,                    //     Report Count (2)
    0x81, 0x02,                    //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                          //   End Collection
    0x05, 0x02,                    //   Usage Page (Sim Ctrls)
    0x75, 0x08,                    //   Report Size (8)
    0x95, 0x02,                    //   Report Count (2)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x26, 0xFF, 0x00,              //   Logical Maximum (255)
    0x09, 0xC5,                    //   Usage (Brake)
    0x09, 0xC4,                    //   Usage (Accelerator)
    0x81, 0x02,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0C,                    //   Usage Page (Consumer)
    0x15, 0x00,                    //   Logical Minimum (0)
    0x25, 0x01,                    //   Logical Maximum (1)
    0x09, 0xE9,                    //   Usage (Volume Increment)
    0x09, 0xEA,                    //   Usage (Volume Decrement)
    0x75, 0x01,                    //   Report Size (1)
    0x95, 0x02,                    //   Report Count (2)
    0x81, 0x02,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0xCD,                    //   Usage (Play/Pause)
    0x95, 0x01,                    //   Report Count (1)
    0x81, 0x02,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x05,                    //   Report Count (5)
    0x81, 0x01,                    //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x05,                    //   Report ID (5)
    0x06, 0x0F, 0x00,              //   Usage Page (PID Page)
    0x09, 0x97,                    //   Usage (0x97)
    0x75, 0x10,                    //   Report Size (16)
    0x95, 0x02,                    //   Report Count (2)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65535)
    0x91, 0x02,                    //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                          // End Collection
};

void kb_mouse_handle_set_report(uint8_t report_id, const uint8_t* buffer, uint16_t reqlen) {
    if (report_id == REPORT_ID_MULTIPLIER && reqlen >= 1) {
        memcpy(&resolution_multiplier, buffer, 1);
    } else if (boot_protocol_keyboard || (report_id == REPORT_ID_LEDS)) {
        handle_received_report(buffer, reqlen, OUR_OUT_INTERFACE, report_id);
    }
}

uint16_t kb_mouse_handle_get_report(uint8_t report_id, uint8_t* buffer, uint16_t reqlen) {
    if (report_id == REPORT_ID_MULTIPLIER && reqlen >= 1) {
        memcpy(buffer, &resolution_multiplier, 1);
        return 1;
    }
    return 0;
}

static const uint8_t horipad_neutral[] = { 0x00, 0x00, 0x0F, 0x80, 0x80, 0x80, 0x80, 0x00 };

void horipad_clear_report(uint8_t* report, uint8_t report_id, uint16_t len) {
    memcpy(report, horipad_neutral, sizeof(horipad_neutral));
}

void ps4_clear_report(uint8_t* report, uint8_t report_id, uint16_t len) {
    memset(report, 0, len);
    report[0] = report[1] = report[2] = report[3] = 0x80;
    report[4] = 0x08;
}

static const uint8_t stadia_neutral[] = { 0x08, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00 };

void stadia_clear_report(uint8_t* report, uint8_t report_id, uint16_t len) {
    memcpy(report, stadia_neutral, sizeof(stadia_neutral));
}

int32_t horipad_default_value(uint32_t usage) {
    switch (usage) {
        case 0x00010039:
            return 15;
        case 0x00010030:
        case 0x00010031:
        case 0x00010032:
        case 0x00010035:
            return 0x80;
        default:
            return 0;
    }
}

int32_t ps4_stadia_default_value(uint32_t usage) {
    switch (usage) {
        case 0x00010039:
            return 8;
        case 0x00010030:
        case 0x00010031:
        case 0x00010032:
        case 0x00010035:
            return 0x80;
        default:
            return 0;
    }
}

const our_descriptor_def_t our_descriptors[] = {
    {
        .idx = 0,
        .descriptor = our_report_descriptor_kb_mouse,
        .descriptor_length = sizeof(our_report_descriptor_kb_mouse),
        .handle_received_report = do_handle_received_report,
        .handle_get_report = kb_mouse_handle_get_report,
        .handle_set_report = kb_mouse_handle_set_report,
    },
    {
        .idx = 1,
        .descriptor = our_report_descriptor_absolute,
        .descriptor_length = sizeof(our_report_descriptor_absolute),
        .handle_received_report = do_handle_received_report,
        .handle_get_report = kb_mouse_handle_get_report,
        .handle_set_report = kb_mouse_handle_set_report,
    },
    {
        .idx = 2,
        .descriptor = our_report_descriptor_horipad,
        .descriptor_length = sizeof(our_report_descriptor_horipad),
        .vid = 0x0F0D,
        .pid = 0x00C1,
        .handle_received_report = do_handle_received_report,
        .clear_report = horipad_clear_report,
        .default_value = horipad_default_value,
    },
    {
        .idx = 3,
        .descriptor = our_report_descriptor_ps4,
        .descriptor_length = sizeof(our_report_descriptor_ps4),
        .device_connected = ps4_device_connected,
        .device_disconnected = ps4_device_disconnected,
        .main_loop_task = ps4_main_loop_task,
        .handle_received_report = ps4_handle_received_report,
        .handle_get_report = ps4_handle_get_report,
        .handle_set_report = ps4_handle_set_report,
        .handle_get_report_response = ps4_handle_get_report_response,
        .handle_set_report_complete = ps4_handle_set_report_complete,
        .clear_report = ps4_clear_report,
        .default_value = ps4_stadia_default_value,
    },
    {
        .idx = 4,
        .descriptor = our_report_descriptor_stadia,
        .descriptor_length = sizeof(our_report_descriptor_stadia),
        .vid = 0x18D1,
        .pid = 0x9400,
        .handle_received_report = do_handle_received_report,
        .clear_report = stadia_clear_report,
        .default_value = ps4_stadia_default_value,
    },
};

const uint8_t config_report_descriptor[] = {
    0x06, 0x00, 0xFF,        // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x20,              // Usage (0x20)
    0xA1, 0x01,              // Collection (Application)
    0x09, 0x20,              //   Usage (0x20)
    0x85, REPORT_ID_CONFIG,  //   Report ID (REPORT_ID_CONFIG)
    0x75, 0x08,              //   Report Size (8)
    0x95, CONFIG_SIZE,       //   Report Count (CONFIG_SIZE)
    0xB1, 0x02,              //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                    // End Collection

    0x09, 0x21,               // Usage (0x21)
    0xA1, 0x01,               // Collection (Application)
    0x09, 0x21,               //   Usage (0x21)
    0x85, REPORT_ID_MONITOR,  //   Report ID (REPORT_ID_MONITOR)
    0x75, 0x08,               //   Report Size (8)
    0x95, 0x3F,               //   Report Count (63)
    0x81, 0x02,               //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,                     // End Collection
};

const uint32_t config_report_descriptor_length = sizeof(config_report_descriptor);

// This isn't sent to the host.
uint8_t const boot_kb_report_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x2A, 0x91, 0x00,  //   Usage Maximum (0x91)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x03,        //   Usage Maximum (Scroll Lock)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x03,        //   Report Count (3)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x05,        //   Report Count (5)
    0x91, 0x03,        //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
};

const uint32_t boot_kb_report_descriptor_length = sizeof(boot_kb_report_descriptor);
