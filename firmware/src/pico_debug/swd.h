/**
 * @file swd.h
 * @author Lee Essen (lee.essen@nowonline.co.uk)
 * @brief 
 * @version 0.1
 * @date 2022-06-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __SWD_H
#define __SWD_H

#ifndef PIN_SWDCLK
#define PIN_SWDCLK 2
#endif

#ifndef PIN_SWDIO
#define PIN_SWDIO 3
#endif

#define SWD_OK              0
#define SWD_WAIT            1
#define SWD_FAULT           2
#define SWD_ERROR           3
#define SWD_PARITY          4

#define CHECK_OK(func)      { int rc = func; if (rc != SWD_OK) return rc; }

int swd_init();
void swd_pio_poll();
void swd_targetsel(uint32_t target);
int swd_read(int APnDP, int addr, uint32_t *result);
int swd_write(int APnDP, int addr, uint32_t value);
void swd_send_bits(uint32_t *data, int bitcount);

void swd_line_reset();
void swd_from_dormant();
void swd_to_dormant();

#endif
