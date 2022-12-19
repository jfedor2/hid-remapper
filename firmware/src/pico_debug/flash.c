/**
 * @file flash.c
 * @author Lee Essen (lee.essen@nowonline.co.uk)
 * @brief 
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * Routines that support flash programming on RP2040 based devices.
 * 
 * This includes an algorithm that should optmise the flash programming times
 * by balancing erase and programming block sizes with actual deltas in the
 * code.
 * 
 * Basic approach:
 * 
 * 1. Copy over stuff in up to 64k chunks
 * 2. Go throug the 64k chunk and compare it in 4k blocks with existing image
 * 3. If more than 2 x 4k blocks need programming then do the whole thing
 * 4. Otherwise do the 4k block individually
 * 
 * But this algorithm really needs to run on the remote device so we need
 * to produce some relocatable code to do it.
 * 
 */

#include "flash.h"
#include "swd.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"

#include "adi.h"

#define FOR_TARGET          __attribute__((noinline, section("for_target")))
#define DATA_BUFFER         0x20000000
#define CODE_START          0x20010000
#define BOOT2_START         0x20020000
#define STACK_ADDDR         0x20040800
#define FLASH_BASE          0x10000000

static int flash_code_copied = 0;

/**
 * @brief Called once the block is already on the target
 * 
 * This will copy over the code (if needed) and execute the remote function
 * 
 * @param offset 
 * @param length 
 * @return int 
 */
static int rp2040_program_flash_chunk(int offset, int length) {
    extern char __start_for_target[];
    extern char __stop_for_target[];
    int rc;

    printf("FLASH: Request to flash 0x%08x (len=%d)\r\n", offset, length);

    // Copy the code over ... only needed once per flashing cycle...

    if (!flash_code_copied) {
        int code_len = (__stop_for_target - __start_for_target);
        printf("FLASH: Copying custom flash code to 0x%08x (%d bytes)\r\n", CODE_START, code_len);
        rc = mem_write_block(CODE_START, code_len, (uint8_t *)__start_for_target);
        if (rc != SWD_OK) return rc;
        flash_code_copied = 1;
    }

    uint32_t t = time_us_32();

    uint32_t args[] = { offset, DATA_BUFFER, length };
    rc = rp2040_call_function(CODE_START, args, sizeof(args)/sizeof(uint32_t));
    if (rc != SWD_OK) return rc;

    uint32_t r0, erased, programmed;
    rc = reg_read(0, &r0);

    erased = (r0 >> 24) * 4;
    programmed = r0 & 0x00ffffff;

    printf("FLASH: Erased %luk and programmed %lu bytes in %lums\r\n", erased, programmed, (time_us_32() - t)/1000);

    return 0;
}

/**
 * @brief Add some data to the existing flash chunk
 * 
 * This will add data (and copy it to the target) up to 64k, then it will
 * call rp2040_program_flash_chunk() to actually do the programming.
 * 
 * If we call this with a high offset and zero size then it will cause
 * any residual chunk to be written out.
 * 
 * @param offset 
 * @param src 
 * @param size 
 * @return int 
 */
static uint32_t     chunk_start = 0;        // flash address of the start
static uint32_t     chunk_size = 0;         // how much is already done

int rp2040_add_flash_bit(uint32_t offset, const uint8_t *src, int size) {
    int rc;

    printf("FLASH: writing %d bytes to flash at 0x%08lx\r\n", size, offset);


    // If we are starting outside the range of an existing block...
    if (chunk_size && (offset >= (chunk_start + 65536))) {
        rp2040_program_flash_chunk(chunk_start, chunk_size);
        chunk_size = 0;
    }

    // If size is zero here then we are the last bit...
    if (size == 0) {
        flash_code_copied = 0;
        return 0;
    }

    while (size) {
        // If we are a new chunk...
        if (chunk_size == 0) {
            chunk_start = offset;
        }
        // Now how much can we fit in...
        int space = 65536 - chunk_size;
        int count = MIN(space, size);

        // Let's copy it over...
        uint32_t t = time_us_32();

        rc = mem_write_block(DATA_BUFFER + (offset - chunk_start), count, src);
        if (rc != SWD_OK) {
            printf("COPY FAILED: %d\r\n", rc);
            return 1;;
        }
        printf("FLASH: Copied %d bytes to location 0x%08lx (%lu ms)\r\n", count, DATA_BUFFER+(offset-chunk_start),
                                                                            (time_us_32() - t)/1000);
        chunk_size += count;

        // If we have a full one...
        if (chunk_size == 65536) {
            rp2040_program_flash_chunk(chunk_start, chunk_size);
            chunk_size = 0;
        }

        // Now process the remainder...
        offset += count;
        size -= count;
        src += count;
    }
    return 0;
}

// -----------------------------------------------------------------------------------
// THIS CODE IS DESIGNED TO RUN ON THE TARGET AND WILL BE COPIED OVER 
// (hence it has it's own section)
// -----------------------------------------------------------------------------------
//
// Memory Map on target for programming:
//
// 0x2000 0000      64K incoming data buffer
// 0x2001 0000      start of code
// 0x2002 0000      stage2 bootloader copy
// 0x2004 0800      top of stack 
//


#define rom_hword_as_ptr(rom_address) (void *)(uintptr_t)(*(uint16_t *)rom_address)
#define fn(a, b)        (uint32_t)((b << 8) | a)
typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);

typedef void *(*rom_void_fn)(void);
typedef void *(*rom_flash_erase_fn)(uint32_t addr, size_t count, uint32_t block_size, uint8_t block_cmd);
typedef void *(*rom_flash_prog_fn)(uint32_t addr, const uint8_t *data, size_t count);

FOR_TARGET int flash_block(uint32_t offset, uint8_t *src, int length) {
    // Fill in the rom functions...
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)rom_hword_as_ptr(0x18);
    uint16_t            *function_table = (uint16_t *)rom_hword_as_ptr(0x14);

    rom_void_fn         _connect_internal_flash = rom_table_lookup(function_table, fn('I', 'F'));
    rom_void_fn         _flash_exit_xip = rom_table_lookup(function_table, fn('E', 'X'));
    rom_flash_erase_fn  _flash_range_erase = rom_table_lookup(function_table, fn('R', 'E'));
    rom_flash_prog_fn   flash_range_program = rom_table_lookup(function_table, fn('R', 'P'));
    rom_void_fn         _flash_flush_cache = rom_table_lookup(function_table, fn('F', 'C'));
    rom_void_fn         _flash_enter_cmd_xip = rom_table_lookup(function_table, fn('C', 'X'));

    // We want to make sure the flash is connected so that we can compare
    // with it's current contents...

    _connect_internal_flash();
//    _flash_flush_cache();
//    _flash_enter_cmd_xip();     // would be better to call the boot stage2 to speed things up
    _flash_exit_xip();

    // If we are being called with a zero offset, then the block will include the bootloader
    // so we can just copy it to use later...
    if (offset == 0) {
        uint32_t *s = (uint32_t *)src;
        uint32_t *d = (uint32_t *)BOOT2_START;
        for (int i=0; i < 64; i++) {
            *d++ = *s++;
        }
    }
    // Call the second stage bootloader... reconnect XIP
    ((void (*)(void))BOOT2_START+1)();

    // Now lets get started...
    uint8_t     *changed[3];
    int         change_count = 0;

    // Go through in 4k chunks seeing if we have deltas...
    uint8_t     *src_block = src;
    uint8_t     *dst_block = (uint8_t *)(FLASH_BASE + offset);
    int         left = length;


    while(left > 0) {
        // Look at this block...
        uint8_t *s = src_block;
        uint8_t *d = dst_block;
        int size = MIN(4096, left);
        for (int i=0; i < size; i++) {
            if (*s++ != *d++) {
                changed[change_count++] = src_block;
                break;
            }
        }
        if (change_count > 2) break;
        src_block += 4096;
        dst_block += 4096;
        left -= 4096;
    }
    if (!change_count) return 0;        // nothing to do

    // turn off xip so we can do stuff...
    //_connect_internal_flash();      // do we need this?
    _flash_exit_xip();

    // We will return the number of 4k blocks erased, and the size flashed in this...
    uint32_t rc = 0;

    if (change_count <= 2) {

        // Program the individual blocks...
        for (int i = 0; i < change_count; i++) {
            uint8_t *block = changed[i];
            uint32_t faddr = offset + (block - src);
            uint32_t size = MIN(4096, length-offset);
    
            // erase 4k
            // program 4k
            _flash_range_erase(faddr, 4096, 4096, 0x20);      // sector erase (4K)
            flash_range_program(faddr, block, size);
            rc += size;
        }
        // reconnect xip
        _flash_flush_cache();
//        _flash_enter_cmd_xip();
        // Call the second stage bootloader... reconnect XIP
        ((void (*)(void))BOOT2_START+1)();
        return (change_count << 24) | rc;
    }
    
    // Otherwise we program the whole thing... either 32K (if less than that) or all 64k
    if (length <= 32768) {
        _flash_range_erase(offset, 32768, 32768, 0x52); // 32K erase
        rc = 0x08000000 | length;
    } else {
        _flash_range_erase(offset, 65536, 65536, 0xD8); // 64K erase
        rc = 0x10000000 | length;
    }
    // program everything...
    flash_range_program(offset, src, length);
    // reconnect xip
    _flash_flush_cache();
//    _flash_enter_cmd_xip();
    // Call the second stage bootloader... reconnect XIP
    ((void (*)(void))BOOT2_START+1)();

    return rc;
}









