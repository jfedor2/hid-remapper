#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

#define SERIAL_MAX_PAYLOAD_SIZE 512

typedef bool (*msg_recv_cb_t)(const uint8_t* data, uint16_t len);

void serial_init();
bool serial_read(msg_recv_cb_t callback);
void serial_write(const uint8_t* data, uint16_t len);

#endif
