// Panic and HardFault handlers that blink a pattern on PICO_DEFAULT_LED_PIN, so
// that a board that stopped working can tell you why without a debugger or a
// serial console.
//
// Each pattern is one long flash followed by a number of short ones, then a
// pause. The long flash keeps them apart from the normal status blinking.
//
//   1 short: hard fault
//   2 short: panic
//   3 short: out of memory (malloc failed, that's a panic with this message)
//
// Everything here runs from RAM and only touches hardware registers directly,
// because by the time we get here flash access, the heap or the SDK's state may
// be what's broken. It also has to work before anything has set up the LED pin
// (out of memory at boot happens early).

#include <cstdarg>
#include <cstdio>

#include "hardware/gpio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/timer.h"
#include "pico/platform.h"

#define FAULT_BLINKS_HARD_FAULT 1
#define FAULT_BLINKS_PANIC 2
#define FAULT_BLINKS_OUT_OF_MEMORY 3

#ifndef PICO_DEFAULT_LED_PIN_INVERTED
#define PICO_DEFAULT_LED_PIN_INVERTED 0
#endif

// Debug Halting Control and Status Register, C_DEBUGEN is set when a debugger
// is attached.
#define DHCSR (*(volatile uint32_t*) 0xE000EDF0)
#define DHCSR_C_DEBUGEN 1

static void __no_inline_not_in_flash_func(delay_ms)(uint32_t ms) {
    uint32_t start = time_us_32();
    while (time_us_32() - start < ms * 1000) {
    }
}

#ifdef PICO_DEFAULT_LED_PIN
static void __no_inline_not_in_flash_func(led_put)(bool on) {
    gpio_put(PICO_DEFAULT_LED_PIN, on != PICO_DEFAULT_LED_PIN_INVERTED);
}
#endif

static void __attribute__((noreturn)) __no_inline_not_in_flash_func(fault_blink_forever)(int n) {
    __asm volatile("cpsid i");

    // Let an attached debugger stop here, like the SDK's default handlers do.
    // Only works on RP2350: on RP2040 (Cortex-M0+) software reads C_DEBUGEN as 0
    // even with a debugger attached, so it just blinks and you can halt it.
    if (DHCSR & DHCSR_C_DEBUGEN) {
        __asm volatile("bkpt #0");
    }

#ifdef PICO_DEFAULT_LED_PIN
    // Same as gpio_init() + gpio_set_dir(), but without calling into flash.
    hw_write_masked(&pads_bank0_hw->io[PICO_DEFAULT_LED_PIN],
        PADS_BANK0_GPIO0_IE_BITS,
        PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS);
    led_put(false);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    io_bank0_hw->io[PICO_DEFAULT_LED_PIN].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
#if HAS_PADS_BANK0_ISOLATION
    hw_clear_bits(&pads_bank0_hw->io[PICO_DEFAULT_LED_PIN], PADS_BANK0_GPIO0_ISO_BITS);
#endif

    while (true) {
        led_put(true);
        delay_ms(1000);
        led_put(false);
        delay_ms(400);
        for (int i = 0; i < n; i++) {
            led_put(true);
            delay_ms(150);
            led_put(false);
            delay_ms(250);
        }
        delay_ms(1000);
    }
#else
    while (true) {
        __asm volatile("wfi");
    }
#endif
}

static bool __no_inline_not_in_flash_func(is_out_of_memory)(const char* fmt) {
    // pico_malloc's check_alloc() calls panic("Out of memory").
    static const char oom[] = "Out of memory";
    if (fmt == nullptr) {
        return false;
    }
    for (unsigned int i = 0; i < sizeof(oom); i++) {
        if (fmt[i] != oom[i]) {
            return false;
        }
    }
    return true;
}

// The SDK's panic() calls this (PICO_PANIC_FUNCTION, set in CMakeLists.txt).
extern "C" void __attribute__((noreturn)) __printflike(1, 0) __no_inline_not_in_flash_func(fault_led_panic)(const char* fmt, ...) {
    int n = is_out_of_memory(fmt) ? FAULT_BLINKS_OUT_OF_MEMORY : FAULT_BLINKS_PANIC;

    // Same output as the SDK's default panic().
    puts("\n*** PANIC ***\n");
    if (fmt != nullptr) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        puts("");
    }

    fault_blink_forever(n);
}

// Replaces the SDK's default, which is just a breakpoint (and with no debugger
// attached, a breakpoint in the HardFault handler locks up the core).
extern "C" void __attribute__((noreturn)) __no_inline_not_in_flash_func(isr_hardfault)() {
    fault_blink_forever(FAULT_BLINKS_HARD_FAULT);
}
