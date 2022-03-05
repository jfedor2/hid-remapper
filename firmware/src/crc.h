#ifndef _CRC_H_
#define _CRC_H_

#include <pico/stdlib.h>

uint32_t crc32(const uint8_t* buf, int len);

#endif
