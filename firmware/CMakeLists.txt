cmake_minimum_required(VERSION 3.13)

set(PICO_COPY_TO_RAM 1)

include(pico_sdk_import.cmake)

project(remapper)

pico_sdk_init()

add_compile_options(-Wall)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

add_executable(remapper src/remapper.cc src/pio_usb/pio_usb.c src/pio_usb/usb_crc.c src/crc.cc src/descriptor_parser.cc src/pio_usb_stuff.cc src/tinyusb_stuff.cc src/our_descriptor.cc src/globals.cc src/config.cc)

pico_generate_pio_header(remapper ${CMAKE_CURRENT_LIST_DIR}/src/pio_usb/usb_tx.pio)
pico_generate_pio_header(remapper ${CMAKE_CURRENT_LIST_DIR}/src/pio_usb/usb_rx.pio)

target_include_directories(remapper PRIVATE src src/pio_usb)

target_link_libraries(remapper pico_stdlib pico_multicore hardware_pio hardware_dma hardware_flash tinyusb_device tinyusb_board)

pico_add_extra_outputs(remapper)
