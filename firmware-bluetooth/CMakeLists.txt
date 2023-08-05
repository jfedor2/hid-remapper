cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})

add_compile_definitions(PERSISTED_CONFIG_SIZE=2048)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

set(REMAPPER_SRC ../firmware/src)

target_include_directories(app PRIVATE ${REMAPPER_SRC})

target_sources(app PRIVATE
    src/main.cc
    ${REMAPPER_SRC}/config.cc
    ${REMAPPER_SRC}/crc.cc
    ${REMAPPER_SRC}/descriptor_parser.cc
    ${REMAPPER_SRC}/globals.cc
    ${REMAPPER_SRC}/interval_override.cc
    ${REMAPPER_SRC}/our_descriptor.cc
    ${REMAPPER_SRC}/quirks.cc
    ${REMAPPER_SRC}/remapper.cc
    ${REMAPPER_SRC}/ps_auth.cc
)
