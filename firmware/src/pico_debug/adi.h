/**
 * @file swd.h
 * @author Lee Essen (lee.essen@nowonline.co.uk)
 * @brief 
 * @version 0.1
 * @date 2022-06-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __ADI_H
#define __ADI_H

#define SWD_OK              0
#define SWD_WAIT            1
#define SWD_FAULT           2
#define SWD_ERROR           3
#define SWD_PARITY          4

enum {
    REASON_DBGRQ = 0,
    REASON_BREAKPOINT = 1,
    REASON_WATCHPOINT = 2,
    REASON_WPTANDBKPT = 3,
    REASON_SINGLESTEP = 4,
    REASON_NOTHALTED = 5,
    REASON_EXIT = 6,
    REASON_EXC_CATCH = 7,
    REASON_UNDEFINED = 8,
};

int swd_init();
int dp_init();
int swd_test();

int mem_read8(uint32_t addr, uint8_t *res);
int mem_read16(uint32_t addr, uint16_t *res);
int mem_read32(uint32_t addr, uint32_t *res);
int mem_read_block(uint32_t addr, uint32_t count, uint8_t *dest);

int mem_write8(uint32_t addr, uint8_t value);
int mem_write16(uint32_t addr, uint16_t value);
int mem_write32(uint32_t addr, uint32_t value);
int mem_write_block(uint32_t addr, uint32_t count, uint8_t *src);

int core_select(int num);
int core_get();
int core_enable_debug();
int core_halt();
int core_unhalt();
int core_step();
int core_step_avoiding_breakpoint();
int core_is_halted();
int core_reset_halt();
int check_cores();
int core_get_reason(int num);

uint32_t rp2040_find_rom_func(char ch1, char ch2);
int rp2040_call_function(uint32_t addr, uint32_t args[], int argc);

int reg_read(int reg, uint32_t *res);
int reg_write(int reg, uint32_t value);

int bp_set(uint32_t addr);
int bp_clr(uint32_t addr);
int bp_is_set(uint32_t addr);

#endif
