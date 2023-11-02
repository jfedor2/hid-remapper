#include "hardware/gpio.h"
#include "hardware/uart.h"

#include "pico/stdio.h"
#include "stdio.h"

#include "crc.h"
#include "serial.h"

#define SERIAL_UART uart1
#define SERIAL_BAUDRATE 4000000

#define BUFFER_SIZE 512
static char outgoing_buffer[BUFFER_SIZE];
static uint16_t buf_head = 0;
static uint16_t buf_tail = 0;
static uint16_t buf_items = 0;

void serial_init() {
    uart_init(SERIAL_UART, SERIAL_BAUDRATE);
    uart_set_hw_flow(SERIAL_UART, true, true);
    uart_set_translate_crlf(SERIAL_UART, false);
    gpio_set_function(SERIAL_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_CTS_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_RTS_PIN, GPIO_FUNC_UART);
}

#define END 0300     /* indicates end of packet */
#define ESC 0333     /* indicates byte stuffing */
#define ESC_END 0334 /* ESC ESC_END means END data byte */
#define ESC_ESC 0335 /* ESC ESC_ESC means ESC data byte */

bool serial_read(msg_recv_cb_t callback) {
    while ((buf_items > 0) && uart_is_writable(SERIAL_UART)) {
        uart_putc_raw(SERIAL_UART, outgoing_buffer[buf_head]);
        buf_head = (buf_head + 1) % BUFFER_SIZE;
        buf_items--;
    }

    static uint8_t buffer[SERIAL_MAX_PAYLOAD_SIZE + 32];
    static uint16_t bytes_read = 0;
    static bool escaped = false;

    while (uart_is_readable(SERIAL_UART)) {
        bytes_read %= sizeof(buffer);

        char c = uart_getc(SERIAL_UART);

        if (escaped) {
            switch (c) {
                case ESC_END:
                    buffer[bytes_read++] = END;
                    break;
                case ESC_ESC:
                    buffer[bytes_read++] = ESC;
                    break;
                default:
                    // this shouldn't happen
                    buffer[bytes_read++] = c;
                    break;
            }
            escaped = false;
        } else {
            switch (c) {
                case END:
                    if (bytes_read > 4) {
                        uint32_t crc = crc32(buffer, bytes_read - 4);
                        uint32_t received_crc = 0;
                        for (int i = 0; i < 4; i++) {
                            received_crc = (received_crc << 8) | buffer[bytes_read - 1 - i];
                        }
                        if (crc == received_crc) {
                            bool ret = callback(buffer, bytes_read - 4);
                            bytes_read = 0;
                            return ret;
                        } else {
                            printf("CRC error\n");
                        }
                    }
                    bytes_read = 0;
                    break;
                case ESC:
                    escaped = true;
                    break;
                default:
                    buffer[bytes_read++] = c;
                    break;
            }
        }
    }

    return false;
}

static void my_putc(char c) {
    if ((buf_items == 0) && uart_is_writable(SERIAL_UART)) {
        uart_putc_raw(SERIAL_UART, c);
    } else {
        if (buf_items < BUFFER_SIZE) {
            outgoing_buffer[buf_tail] = c;
            buf_tail = (buf_tail + 1) % BUFFER_SIZE;
            buf_items++;
        } else {
            uart_putc_raw(SERIAL_UART, outgoing_buffer[buf_head]);  // blocks
            buf_head = (buf_head + 1) % BUFFER_SIZE;
            outgoing_buffer[buf_tail] = c;
            buf_tail = (buf_tail + 1) % BUFFER_SIZE;
        }
    }
}

void send_escaped_byte(uint8_t b) {
    switch (b) {
        case END:
            my_putc(ESC);
            my_putc(ESC_END);
            break;

        case ESC:
            my_putc(ESC);
            my_putc(ESC_ESC);
            break;

        default:
            my_putc(b);
    }
}

void serial_write(const uint8_t* data, uint16_t len) {
    uint32_t crc = crc32(data, len);

    my_putc(END);

    for (int i = 0; i < len; i++) {
        send_escaped_byte(data[i]);
    }

    for (int i = 0; i < 4; i++) {
        send_escaped_byte((crc >> (i * 8)) & 0xFF);
    }

    my_putc(END);
}
