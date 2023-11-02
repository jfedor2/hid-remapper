#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

#define SERIAL_MAX_PAYLOAD_SIZE 512

#ifdef HID_REMAPPER_BOARD

#define SERIAL_TX_PIN 24
#define SERIAL_RX_PIN 25
#define SERIAL_CTS_PIN 26
#define SERIAL_RTS_PIN 23

#else

#define SERIAL_TX_PIN 20
#define SERIAL_RX_PIN 21
#define SERIAL_CTS_PIN 26
#define SERIAL_RTS_PIN 27

#endif

typedef bool (*msg_recv_cb_t)(const uint8_t* data, uint16_t len);

void serial_init();
bool serial_read(msg_recv_cb_t callback);
void serial_write(const uint8_t* data, uint16_t len);

#endif
