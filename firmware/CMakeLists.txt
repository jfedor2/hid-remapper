cmake_minimum_required(VERSION 3.13)

add_compile_definitions(PICO_DEFAULT_UART_BAUD_RATE=921600)

set(PICO_COPY_TO_RAM 1)

set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")
set(PICO_TINYUSB_PATH "${CMAKE_CURRENT_LIST_DIR}/tinyusb")
set(OUR_PIO_USB_PATH "${CMAKE_CURRENT_LIST_DIR}/src/pio_usb")
set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/boards")

include(pico_sdk_import.cmake)

project(remapper)

pico_sdk_init()

add_compile_options(-Wall)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

add_executable(remapper src/remapper.cc src/remapper_single.cc ${OUR_PIO_USB_PATH}/pio_usb.c ${OUR_PIO_USB_PATH}/usb_crc.c src/crc.cc src/descriptor_parser.cc src/pio_usb_stuff.cc src/tinyusb_stuff.cc src/our_descriptor.cc src/globals.cc src/config.cc src/quirks.cc src/interval_override.cc)

target_compile_definitions(remapper PUBLIC PICO_DEFAULT_UART_TX_PIN=16)
target_compile_definitions(remapper PUBLIC PICO_DEFAULT_UART_RX_PIN=17)

pico_generate_pio_header(remapper ${OUR_PIO_USB_PATH}/usb_tx.pio)
pico_generate_pio_header(remapper ${OUR_PIO_USB_PATH}/usb_rx.pio)

target_include_directories(remapper PRIVATE src src/tusb_config_device ${OUR_PIO_USB_PATH})

target_link_libraries(remapper pico_stdlib pico_multicore hardware_pio hardware_dma hardware_flash tinyusb_device tinyusb_board)

pico_add_extra_outputs(remapper)


add_executable(remapper_dual_a src/remapper.cc src/remapper_dual_a.cc src/crc.cc src/descriptor_parser.cc src/tinyusb_stuff.cc src/our_descriptor.cc src/globals.cc src/config.cc src/quirks.cc src/interval_override.cc src/serial.cc)
target_include_directories(remapper_dual_a PRIVATE src src/tusb_config_device)
target_link_libraries(remapper_dual_a pico_stdlib hardware_flash tinyusb_device tinyusb_board)
pico_add_extra_outputs(remapper_dual_a)

add_executable(remapper_dual_b src/remapper_dual_b.cc src/crc.cc src/interval_override.cc src/serial.cc)
target_include_directories(remapper_dual_b PRIVATE src src/tusb_config_host)
target_link_libraries(remapper_dual_b pico_stdlib tinyusb_host tinyusb_board)
pico_add_extra_outputs(remapper_dual_b)

add_executable(remapper_serial src/remapper.cc src/remapper_serial.cc src/crc.cc src/descriptor_parser.cc src/tinyusb_stuff.cc src/our_descriptor.cc src/globals.cc src/config.cc src/quirks.cc src/interval_override.cc src/serial.cc)
target_include_directories(remapper_serial PRIVATE src src/tusb_config_device)
target_link_libraries(remapper_serial pico_stdlib hardware_flash tinyusb_device tinyusb_board)
pico_add_extra_outputs(remapper_serial)
