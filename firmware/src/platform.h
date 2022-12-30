#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdint.h>

void do_persist_config(uint8_t* buffer);

void reset_to_bootloader();
void pair_new_device();
void clear_bonds();
void flash_b_side();

void usages_mutex_init();
void usages_mutex_enter();
void usages_mutex_exit();

void macros_mutex_init();
void macros_mutex_enter();
void macros_mutex_exit();

uint64_t get_time();

#endif
