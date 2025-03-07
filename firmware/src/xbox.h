#ifndef _XBOX_H_
#define _XBOX_H_

#include <stdint.h>

#include "host/usbh.h"
#include "host/usbh_pvt.h"

bool xboxh_init(void);
bool xboxh_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_interface_t const* desc_itf, uint16_t max_len);
bool xboxh_set_config(uint8_t dev_addr, uint8_t itf_num);
bool xboxh_xfer_cb(uint8_t dev_addr, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
void xboxh_close(uint8_t dev_addr);

#endif
