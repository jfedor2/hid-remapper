
#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>

int rp2040_add_flash_bit(uint32_t offset, uint8_t *src, int size);

#endif
