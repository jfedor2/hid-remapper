/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jacek Fedorynski
 * Copyright (c) 2021 sekigon-gonnoc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "pio_usb_stuff.h"

#include <pico/multicore.h>

static usb_device_t* usb_device = NULL;

void core1_main() {
    sleep_ms(10);

    // To run USB SOF interrupt in core1, create alarm pool in core1.
    static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
    config.alarm_pool = (void*) alarm_pool_create(2, 1);
    usb_device = pio_usb_host_init(&config);

    //// Call pio_usb_host_add_port to use multi port
    // const uint8_t pin_dp2 = 8;
    // pio_usb_host_add_port(pin_dp2);

    while (true) {
        pio_usb_host_task();
    }
}

void launch_pio_usb() {
    set_sys_clock_khz(120000, true);
    sleep_ms(10);
    multicore_reset_core1();
    multicore_launch_core1(core1_main);
}

void pio_usb_task(report_handler_t report_handler) {
    if (usb_device != NULL) {
        for (int dev_idx = 0; dev_idx < PIO_USB_DEVICE_CNT; dev_idx++) {
            usb_device_t* device = &usb_device[dev_idx];
            if (!device->connected) {
                continue;
            }

            for (int ep_idx = 0; ep_idx < PIO_USB_DEV_EP_CNT; ep_idx++) {
                endpoint_t* ep = pio_usb_get_endpoint(device, ep_idx);

                if (ep == NULL) {
                    break;
                }

                uint8_t temp[64];
                int len = pio_usb_get_in_data(ep, temp, sizeof(temp));

                if (len > 0) {
                    report_handler(temp, len, (uint16_t) (device->address << 8) | ep->interface);
                }
            }
        }
    }
}
