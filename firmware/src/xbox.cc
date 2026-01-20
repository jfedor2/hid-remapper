#include "xbox.h"
#include "constants.h"
#include "remapper.h"

#define NXDEVS 8

// Xbox controllers have the vertical axes inverted compared to the standard
// HID convention. We use logical_minimum=32767 and logical_maximum=-32768 for
// those axes in the fake report descriptor below. This happens to give the
// correct results when we scale the values to 0-255 range.

static const uint8_t xbox_one_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x20,        //   Report ID (32)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x03,        //   Report Count (3)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x10,        //   Usage Maximum (0x10)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x02,        //   Report Count (2)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
    0x05, 0x02,        //   Usage Page (Sim Ctrls)
    0x09, 0xC5,        //   Usage (Brake)
    0x09, 0xC4,        //   Usage (Accelerator)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x09, 0x30,        //   Usage (X)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x26, 0x00, 0x80,  //   Logical Maximum (-32768)
    0x16, 0xFF, 0x7F,  //   Logical Minimum (32767)
    0x09, 0x31,        //   Usage (Y)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x09, 0x32,        //   Usage (Z)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x26, 0x00, 0x80,  //   Logical Maximum (-32768)
    0x16, 0xFF, 0x7F,  //   Logical Minimum (32767)
    0x09, 0x35,        //   Usage (Rz)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x10,        //   Report Count (16)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x05, 0x0C,        //   Usage Page (Consumer)
    0x09, 0x85,        //   Usage (Order Movie)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x07,        //   Report ID (7)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x03,        //   Report Count (3)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x01,        //   Report Count (1)
    0x05, 0x09,        //   Usage Page (Button)
    0x09, 0x11,        //   Usage (0x11)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

static const uint8_t xbox_360_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x10,        //   Report Count (16)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x06, 0xF9, 0xFF,  //   Usage Page (Vendor Defined 0xFFF9)
    0x09, 0x03,        //   Usage (0x03)
    0x09, 0x04,        //   Usage (0x04)
    0x09, 0x01,        //   Usage (0x01)
    0x09, 0x02,        //   Usage (0x02)
    0x05, 0x09,        //   Usage Page (Button)
    0x09, 0x0A,        //   Usage (0x0A)
    0x09, 0x09,        //   Usage (0x09)
    0x09, 0x0B,        //   Usage (0x0B)
    0x09, 0x0C,        //   Usage (0x0C)
    0x09, 0x05,        //   Usage (0x05)
    0x09, 0x06,        //   Usage (0x06)
    0x09, 0x0D,        //   Usage (0x0D)
    0x09, 0x0E,        //   Usage (0x0E)
    0x09, 0x02,        //   Usage (0x02)
    0x09, 0x03,        //   Usage (0x03)
    0x09, 0x01,        //   Usage (0x01)
    0x09, 0x04,        //   Usage (0x04)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x33,        //   Usage (Rx)
    0x09, 0x34,        //   Usage (Ry)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x09, 0x30,        //   Usage (X)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x26, 0x00, 0x80,  //   Logical Maximum (-32768)
    0x16, 0xFF, 0x7F,  //   Logical Minimum (32767)
    0x09, 0x31,        //   Usage (Y)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x16, 0x00, 0x80,  //   Logical Minimum (-32768)
    0x26, 0xFF, 0x7F,  //   Logical Maximum (32767)
    0x09, 0x32,        //   Usage (Z)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x26, 0x00, 0x80,  //   Logical Maximum (-32768)
    0x16, 0xFF, 0x7F,  //   Logical Minimum (32767)
    0x09, 0x35,        //   Usage (Rz)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x06,        //   Report Count (6)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

static uint8_t init1[] = { 0x05, 0x20, 0x01, 0x01, 0x00 };
static uint8_t init2[] = { 0x05, 0x20, 0x02, 0x0f, 0x06 };
static uint8_t init3[] = { 0x06, 0x20, 0x03, 0x02, 0x01, 0x00 };

enum class XType : int8_t {
    UNKNOWN = 0,
    XBOX_360 = 1,
    XBOX_ONE = 2,
};

struct xdev_t {
    XType type = XType::UNKNOWN;
    uint8_t dev_addr = 0;
    uint8_t itf_num = 0;
    uint8_t in_ep = 0;
    uint16_t in_ep_size = 0;
    uint8_t out_ep = 0;
    uint16_t out_ep_size = 0;
    int setup_stage = 0;
    uint8_t buf[64] = { 0 };
};

static struct xdev_t xdevs[NXDEVS];

static struct xdev_t* allocate_xdev() {
    for (int i = 0; i < NXDEVS; i++) {
        if (xdevs[i].dev_addr == 0) {
            return &xdevs[i];
        }
    }

    return nullptr;
}

static struct xdev_t* get_xdev_by_itf(uint8_t dev_addr, uint8_t itf_num) {
    for (int i = 0; i < NXDEVS; i++) {
        if ((xdevs[i].dev_addr == dev_addr) && (xdevs[i].itf_num == itf_num)) {
            return &xdevs[i];
        }
    }

    return nullptr;
}

static struct xdev_t* get_xdev_by_ep(uint8_t dev_addr, uint8_t ep) {
    for (int i = 0; i < NXDEVS; i++) {
        if ((xdevs[i].dev_addr == dev_addr) &&
            ((xdevs[i].in_ep == ep) || (xdevs[i].out_ep == ep))) {
            return &xdevs[i];
        }
    }

    return nullptr;
}

bool xboxh_init(void) {
    return true;
}

bool xboxh_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_interface_t const* desc_itf, uint16_t max_len) {
    if (max_len < sizeof(tusb_desc_interface_t) + desc_itf->bNumEndpoints * sizeof(tusb_desc_endpoint_t)) {
        return false;
    }

    struct xdev_t* xdev = allocate_xdev();
    if (xdev == nullptr) {
        return false;
    }

    if ((desc_itf->bNumEndpoints == 2) &&
        (desc_itf->bInterfaceClass == 255) &&
        (desc_itf->bInterfaceSubClass == 71) &&
        (desc_itf->bInterfaceProtocol == 208)) {
        xdev->type = XType::XBOX_ONE;
    } else if ((desc_itf->bNumEndpoints == 2) &&
               (desc_itf->bInterfaceClass == 255) &&
               (desc_itf->bInterfaceSubClass == 93) &&
               (desc_itf->bInterfaceProtocol == 1)) {  // XXX this is 129 for wireless
        xdev->type = XType::XBOX_360;
    } else {
        return false;
    }

    uint8_t in_ep = 0;
    uint16_t in_ep_size = 0;
    uint8_t out_ep = 0;
    uint16_t out_ep_size = 0;

    uint8_t const* p_desc = (uint8_t const*) desc_itf;

    while (true) {
        p_desc = tu_desc_next(p_desc);
        if (p_desc >= ((uint8_t*) desc_itf) + max_len) {
            break;
        }
        tusb_desc_endpoint_t const* desc_ep = (tusb_desc_endpoint_t const*) p_desc;
        if (desc_ep->bDescriptorType != TUSB_DESC_ENDPOINT) {
            continue;
        }
        if (desc_ep->bmAttributes.xfer != TUSB_XFER_INTERRUPT) {
            continue;
        }
        if (!tuh_edpt_open(dev_addr, desc_ep)) {
            return false;
        }
        uint8_t ep_addr = desc_ep->bEndpointAddress;
        if (tu_edpt_dir(ep_addr) == TUSB_DIR_IN) {
            in_ep = ep_addr;
            in_ep_size = tu_edpt_packet_size(desc_ep);
        } else {
            out_ep = ep_addr;
            out_ep_size = tu_edpt_packet_size(desc_ep);
        }
    }

    xdev->dev_addr = dev_addr;
    xdev->itf_num = desc_itf->bInterfaceNumber;
    xdev->in_ep = in_ep;
    xdev->in_ep_size = in_ep_size;
    if (xdev->in_ep_size > sizeof(xdev->buf)) {
        xdev->in_ep_size = sizeof(xdev->buf);
    }
    xdev->out_ep = out_ep;
    xdev->out_ep_size = out_ep_size;

    return true;
}

static bool xxfer_in(struct xdev_t* xdev) {
    if (!usbh_edpt_claim(xdev->dev_addr, xdev->in_ep)) {
        return false;
    }

    if (!usbh_edpt_xfer(xdev->dev_addr, xdev->in_ep, xdev->buf, xdev->in_ep_size)) {
        usbh_edpt_release(xdev->dev_addr, xdev->in_ep);
        return false;
    }

    return true;
}

static bool xxfer_out(struct xdev_t* xdev, uint8_t* buf, uint16_t len) {
    if (!usbh_edpt_claim(xdev->dev_addr, xdev->out_ep)) {
        return false;
    }

    if (!usbh_edpt_xfer(xdev->dev_addr, xdev->out_ep, buf, len)) {
        usbh_edpt_release(xdev->dev_addr, xdev->out_ep);
        return false;
    }

    return true;
}

static void process_setup(struct xdev_t* xdev) {
    switch (xdev->setup_stage) {
        case 1:
            xxfer_out(xdev, init1, sizeof(init1));
            break;
        case 2:
            xxfer_out(xdev, init2, sizeof(init2));
            break;
        case 3:
            xxfer_out(xdev, init3, sizeof(init3));
            break;
        case 4:
            xdev->setup_stage = 0;
            xxfer_in(xdev);

            uint8_t hub_addr;
            uint8_t hub_port;
            tuh_get_hub_addr_port(xdev->dev_addr, &hub_addr, &hub_port);
            // We use the same VID/PID for all Xbox controllers to be able to apply quirks.
            // We should probably find another way to signal that it's an Xbox controller and pass the real ones.
            descriptor_received_callback(
                VENDOR_ID_MICROSOFT,
                PRODUCT_ID_MICROSOFT_XBOX_WIRELESS_CONTROLLER,
                xbox_one_descriptor,
                sizeof(xbox_one_descriptor),
                (uint16_t) (xdev->dev_addr << 8) | xdev->itf_num,
                hub_port, xdev->itf_num);
            usbh_driver_set_config_complete(xdev->dev_addr, xdev->itf_num);
            break;
    }
}

bool xboxh_set_config(uint8_t dev_addr, uint8_t itf_num) {
    struct xdev_t* xdev = get_xdev_by_itf(dev_addr, itf_num);
    if (xdev == nullptr) {
        return false;
    }

    switch (xdev->type) {
        case XType::XBOX_ONE:
            xdev->setup_stage = 1;
            process_setup(xdev);
            break;
        case XType::XBOX_360:
            xxfer_in(xdev);

            uint8_t hub_addr;
            uint8_t hub_port;
            tuh_get_hub_addr_port(xdev->dev_addr, &hub_addr, &hub_port);
            // We use the same VID/PID for all Xbox controllers to be able to apply quirks.
            // We should probably find another way to signal that it's an Xbox controller and pass the real ones.
            descriptor_received_callback(
                VENDOR_ID_MICROSOFT,
                PRODUCT_ID_MICROSOFT_XBOX_360_CONTROLLER,
                xbox_360_descriptor,
                sizeof(xbox_360_descriptor),
                (uint16_t) (xdev->dev_addr << 8) | xdev->itf_num,
                hub_port, xdev->itf_num);
            usbh_driver_set_config_complete(xdev->dev_addr, xdev->itf_num);
            break;
        default:
            break;
    }

    return true;
}

bool xboxh_xfer_cb(uint8_t dev_addr, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
    struct xdev_t* xdev = get_xdev_by_ep(dev_addr, ep_addr);
    if (xdev == nullptr) {
        return false;
    }

    switch (xdev->type) {
        case XType::XBOX_ONE:
            if (ep_addr == xdev->in_ep) {
                if (xferred_bytes > 0) {
                    if ((xdev->buf[0] == 0x07) && (xdev->buf[1] == 0x30)) {
                        // this kind of packet requires ack
                        uint8_t ack[] = { 0x01, 0x20, xdev->buf[2], 9, 0x00, 0x07, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
                        xxfer_out(xdev, ack, sizeof(ack));  // we should have a queue or something
                    }
                    if ((xdev->buf[0] == 0x20) || (xdev->buf[0] == 0x07)) {  // only pass stuff we know
                        report_received_callback(xdev->dev_addr, xdev->itf_num, xdev->buf, xferred_bytes);
                    }
                }
                xxfer_in(xdev);
            }
            if (ep_addr == xdev->out_ep) {
                if (xdev->setup_stage != 0) {
                    xdev->setup_stage++;
                    process_setup(xdev);
                }
            }
            break;
        case XType::XBOX_360:
            if (ep_addr == xdev->in_ep) {
                if ((xferred_bytes > 0) && (xdev->buf[0] == 0x00)) {
                    report_received_callback(xdev->dev_addr, xdev->itf_num, xdev->buf, xferred_bytes);
                }
                xxfer_in(xdev);
            }
            break;
        default:
            break;
    }
    return true;
}

void xboxh_close(uint8_t dev_addr) {
    for (int i = 0; i < NXDEVS; i++) {
        if (xdevs[i].dev_addr == dev_addr) {
            umount_callback(dev_addr, xdevs[i].itf_num);
            xdevs[i] = {};
        }
    }
}
