#include "usb_midi_host.h"
#include "xbox.h"

usbh_class_driver_t const* usbh_app_driver_get_cb(uint8_t* driver_count) {
    static usbh_class_driver_t host_driver[] = {
        {
#if CFG_TUSB_DEBUG >= 2
            .name = "XBOXH",
#endif
            .init = xboxh_init,
            .open = xboxh_open,
            .set_config = xboxh_set_config,
            .xfer_cb = xboxh_xfer_cb,
            .close = xboxh_close,
        },
        {
#if CFG_TUSB_DEBUG >= 2
            .name = "MIDIH",
#endif
            .init = midih_init,
            .open = midih_open,
            .set_config = midih_set_config,
            .xfer_cb = midih_xfer_cb,
            .close = midih_close,
        },
    };
    *driver_count = 2;
    return host_driver;
}
