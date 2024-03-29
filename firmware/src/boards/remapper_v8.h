// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

#ifndef _BOARDS_REMAPPER_V8_H
#define _BOARDS_REMAPPER_V8_H

#define HID_REMAPPER_BOARD_V8

#define ADC_ENABLED
#define NADCS 2

// these settings are here to let this build be used on
// a regular Pico until we have a proper build with analog
#define GPIO_VALID_PINS_BASE 0b00010000011111111111111111111111
#define PICO_DEFAULT_PIO_USB_DP_PIN 0
#define PICO_SMPS_MODE_PIN 23

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
