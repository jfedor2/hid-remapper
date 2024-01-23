// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

#ifndef _BOARDS_REMAPPER_H
#define _BOARDS_REMAPPER_H

#define HID_REMAPPER_BOARD_V7

#define PIN_SWDCLK 28
#define PIN_SWDIO 27

#define SERIAL_TX_PIN 24
#define SERIAL_RX_PIN 25
#define SERIAL_CTS_PIN 26
#define SERIAL_RTS_PIN 23

#define I2C_BLOCK i2c0

#define PIN_SDA 8
#define PIN_SCL 9

#define I2C_ENABLED
#define MCP4651_ENABLED

#define GPIO_VALID_PINS_BASE 0b00000000000001110000110001111111

// --- FLASH ---

#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (1 * 1024 * 1024)
#endif

#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 1
#endif

#endif
