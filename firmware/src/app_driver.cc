#include "xbox.h"

usbh_class_driver_t const* usbh_app_driver_get_cb(uint8_t* driver_count) {
    static usbh_class_driver_t host_driver[] = {
        {
            .name = "XBOXH",
            .init = xboxh_init,
            .open = xboxh_open,
            .set_config = xboxh_set_config,
            .xfer_cb = xboxh_xfer_cb,
            .close = xboxh_close,
        },
    };
    *driver_count = 1;
    return host_driver;
}
