#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#pragma GCC optimize ("O0")


#include "hardware/pio.h"
#include "swd.pio.h"

#include "swd.h"

#ifndef PIN_SWDCLK
#define PIN_SWDCLK 2
#endif

#ifndef PIN_SWDIO
#define PIN_SWDIO 3
#endif

#define OUT                 1
#define IN                  0

static PIO                  swd_pio = pio0;
static uint                 swd_sm;

#define CHECK_OK(func)      { int rc = func; if (rc != SWD_OK) return rc; }



/**
 * @brief Return parity for a given 4 bit number
 * 
 * @param value 
 * @return int 
 */
static int parity4(uint32_t value) {
    static const uint32_t plook[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
    return plook[value];
}

/**
 * @brief Return parity for a given 32 bit number
 * 
 * @param value 
 * @return int 
 */
static inline int parity32(uint32_t value) {
    uint32_t p16 = ((value & 0xffff0000) >> 16) ^ (value & 0xffff);
    uint32_t p8 = ((p16 & 0xff00) >> 8) ^ (p16 & 0xff);
    uint32_t p4 = ((p8 & 0xf0) >> 4) ^ (p8 & 0x0f);
    return parity4(p4);
}


/**
 * @brief Load the PIO with the SWD code and start it
 * 
 * @param pio 
 * @return int 
 */
int swd_init() {
    pio_sm_config   c;
    PIO             pio = swd_pio;

    // Load the program at offset zero so the jumps are constant...
    pio_add_program_at_offset(pio, &swd_program, 0);
    c = swd_program_get_default_config(0);      // 0=offset

    // Map the appropriate pins...
    sm_config_set_out_pins(&c, PIN_SWDIO, 1);
    sm_config_set_set_pins(&c, PIN_SWDIO, 1);
    sm_config_set_in_pins(&c, PIN_SWDIO);
    sm_config_set_sideset_pins(&c, PIN_SWDCLK);

    // Setup PIO to GPIO
    pio_gpio_init(pio, PIN_SWDCLK);
    pio_gpio_init(pio, PIN_SWDIO);
    gpio_pull_up(PIN_SWDIO);

    // Get a state machine...
    swd_sm = pio_claim_unused_sm(pio, true);

    // Setup direction... we want lsb first (so shift right)
    sm_config_set_out_shift(&c, true, true, 32);
    sm_config_set_in_shift(&c, true, true, 32);

    // Set directions...
    pio_sm_set_consecutive_pindirs(pio, swd_sm, PIN_SWDCLK, 1, true);
    pio_sm_set_consecutive_pindirs(pio, swd_sm, PIN_SWDIO, 1, false);

    // And initialise
    pio_sm_init(pio, swd_sm, 0, &c);    // 0=offset

    // Go slow for the minute...
    //pio_sm_set_clkdiv_int_frac(pio, swd_sm, 128, 0);
    pio_sm_set_clkdiv_int_frac(pio, swd_sm, 3, 0);

    // 125MHz / 2 / 3 = ~ 20Mhz --> divider of 3
    // 150Mhz / 2 / 3 = 25Mhz --> divider of 3 (and it works!)

    pio_sm_set_enabled(swd_pio, swd_sm, true);
    return SWD_OK;
}

/**
 * @brief Sends up to 21 bits of data using a single control word
 * 
 * @param count 
 * @param data 
 */
static inline void swd_short_output(int count, uint32_t data) {
    assert(count > 0);
    assert(count <= 21);

    pio_sm_put_blocking(swd_pio, swd_sm, (data << 10) | ((count-1) << 5) | swd_offset_short_output);
}

/**
 * @brief Receive up to 32 bits (a single return value)
 * 
 * Note we can ask for more but will need to read the rest outselves
 * 
 * @param count 
 * @return uint32_t 
 */
static inline uint32_t swd_short_input(int count) {
    assert(count > 0);

    pio_sm_put_blocking(swd_pio, swd_sm, ((count-1) << 5) | swd_offset_input);
    return pio_sm_get_blocking(swd_pio, swd_sm);
}

/**
 * @brief Select a specific target
 * 
 * This is a custom routine because we can't check the ACK bits so we can't use
 * the main conditional function. 
 * 
 * @param target 
 * @return int 
 */
void swd_targetsel(uint32_t target) {
    uint32_t parity = parity32(target);

    // First we send the 8 bits out real output (inc park)...
    swd_short_output(8, 0b10011001);

    // Now we read 5 bits (trn, ack0, ack1, ack2, trn) ... (will send it back)
    pio_sm_put_blocking(swd_pio, swd_sm, ((5-1) << 5) | swd_offset_input);

    // Now we can write the target id (lsb) and a parity bit
    pio_sm_put_blocking(swd_pio, swd_sm, ((33-1) << 5) | swd_offset_output);
    pio_sm_put_blocking(swd_pio, swd_sm, target);               // lsb first
    pio_sm_put_blocking(swd_pio, swd_sm, parity);

    // Now we can read back the three bits (well 6) that should be waiting for us
    // and discard them
    pio_sm_get_blocking(swd_pio, swd_sm);
}

/**
 * @brief Perform an SWD read operation
 * 
 * We pull out bits A[2:3] from the address field
 * 
 * @param APnDP 
 * @param addr 
 * @param result 
 * @return int 
 */
static int _swd_read(int APnDP, int addr, uint32_t *result) {
    uint32_t ack;
    uint32_t res;
    uint32_t parity;

    // We care about 4 bits for parity, 1=RD, APnDP, A3/2
    uint32_t packpar = parity4((addr & 0xc) | 2 | APnDP);

    // First we send the 7 bits out real output... (will send one value back)
    // LSB -> start, (APnDP), 1=RD, A2, A3, parity, stop, 1=park)
    uint32_t data =     (1 << 7)                // park bit
                    |   (0 << 6)                // stop bit
                    |   (packpar << 5)          // parity
                    |   (addr & 0xc) << 1       // A3/A2
                    |   (1 << 2)                // Read
                    |   (APnDP << 1)
                    |   (1);                    // start bit

    swd_short_output(8, data);

    // Now we do a conditional read...
    //
    // 5 bits -- location for conditional
    // 8 bits -- how many bits (value of x) for read
    // 5 bits -- location to jump to if good
    // 14 bits -- locatiom to jump to if we fail
    //
    pio_sm_put_blocking(swd_pio, swd_sm, swd_offset_start << (5+8+5)        // go to start on failure
                            | swd_offset_in_jmp << (5+8)                    // to in_jump on success
                            | (33-1) << 5                                   // 32 + 1 parity
                            | swd_offset_conditional);                      // conditional
    ack = pio_sm_get_blocking(swd_pio, swd_sm) >> 1;

    //
    // We expect an OK response, otherwise decode the error... there will be no
    // further output if we have an error.
    //
    if (ack != 1) {
        // We need a trn if we fail...
        swd_short_output(1, 0);
        if (ack == 2) return SWD_WAIT;
        if (ack == 4) return SWD_FAULT;
        return SWD_ERROR;
    }

    res = pio_sm_get_blocking(swd_pio, swd_sm);
    parity = (pio_sm_get_blocking(swd_pio, swd_sm) & 0x80000000) == 0x80000000;
    if (parity != parity32(res)) return SWD_PARITY;
    *result = res;

    // We always need a trn after a read...
    swd_short_output(1, 0);
    return SWD_OK;    
}
int swd_read(int APnDP, int addr, uint32_t *result) {
    int rc;

    do {
        rc = _swd_read(APnDP, addr, result);
    } while (rc == SWD_WAIT);
    return rc;
}

/**
 * @brief Perform an SWD write operation
 * 
 * We pull out bits A[2:3] from the address field
 * 
 * @param APnDP 
 * @param addr 
 * @param result 
 * @return int 
 */
static int _swd_write(int APnDP, int addr, uint32_t value) {
    uint32_t ack;

    // We care about 4 bits for parity, 0=WR, APnDP, A3/2
    uint32_t packpar = parity4((addr & 0xc) | 0 | APnDP);
    uint32_t parity = parity32(value);

    // First we send the 7 bits out real output... (will send one value back)
    // LSB -> start, (APnDP), 1=RD, A2, A3, parity, stop, 1=park)
    uint32_t data =     (1 << 7)                // park
                    |   (0 << 6)                // stop bit
                    |   (packpar << 5)          // parity
                    |   ((addr & 0xc) << 1)     // A3/A2
                    |   (0 << 2)                // Write
                    |   (APnDP << 1)
                    |   (1);                    // start bit

    swd_short_output(8, data);

    // Now we do a conditional rite...
    //
    // 5 bits -- location for conditional
    // 8 bits -- how many bits (value of x) for write
    // 5 bits -- location to jump to if good
    // 14 bits -- locatiom to jump to if we fail
    //
    pio_sm_put_blocking(swd_pio, swd_sm, swd_offset_cond_write_fail << (5+8+5)  // for failure
                            | swd_offset_cond_write_ok << (5+8)                 // to in_jump on success
                            | (33-1) << 5                                       // 32 + 1 parity
                            | swd_offset_conditional);                          // conditional
    pio_sm_put_blocking(swd_pio, swd_sm, value);
    pio_sm_put_blocking(swd_pio, swd_sm, parity);

    ack = pio_sm_get_blocking(swd_pio, swd_sm) >> 1;

    //
    // We expect an OK response, otherwise decode the error... the system will throw
    // away our other output if there's an error
    //
    if (ack != 1) {
        // We need a trn if we fail...
        swd_short_output(1, 0);
        if (ack == 2) return SWD_WAIT;
        if (ack == 4) return SWD_FAULT;
        return SWD_ERROR;
    }
    return SWD_OK;    
}
int swd_write(int APnDP, int addr, uint32_t value) {
    int rc;

    do {
        rc = _swd_write(APnDP, addr, value);
    } while (rc == SWD_WAIT);
    return rc;
}

/**
 * @brief This sends an arbitary number of bits to the target
 * 
 * It sends each 32bit word in turn LSB first, so it's a little
 * counter intuitive, but keeps the code simple for other parts.
 * 
 * @param data 
 * @param bitcount 
 */
void swd_send_bits(uint32_t *data, int bitcount) {
    // Jump to the output function...
    pio_sm_put_blocking(swd_pio, swd_sm, ((bitcount-1) << 5) | swd_offset_output);

    // And write them...
    while (bitcount > 0) {
        pio_sm_put_blocking(swd_pio, swd_sm, *data++);
        bitcount -= 32;
    }
    // Do we need a sacrificial word?
    if (bitcount == 0) pio_sm_put_blocking(swd_pio, swd_sm, 0);
}

void swd_line_reset() {
    // Reset is at least 50 bits of 1 followed by at least 2 zero's
    static const uint32_t reset_seq[] = { 0xffffffff, 0x0003ffff };

    swd_send_bits((uint32_t *)reset_seq, 52);
}

void swd_from_dormant() {
    // Selection alert sequence... 128bits
    static const uint32_t selection_alert_seq[] = { 0x6209F392, 0x86852D95, 0xE3DDAFE9, 0x19BC0EA2 };

    // Zeros and ones sequences...
    static const uint32_t zero_seq[] = { 0x00000000 };
    static const uint32_t ones_seq[] = { 0xffffffff };

    // Activation sequence (8 bits)
    static const uint32_t act_seq[] = { 0x1a };

    swd_send_bits((uint32_t *)ones_seq, 8);
    swd_send_bits((uint32_t *)selection_alert_seq, 128);
    swd_send_bits((uint32_t *)zero_seq, 4);
    swd_send_bits((uint32_t *)act_seq, 8);

    // At this point SWD will be in protocol error state, so needs a reset
}

void swd_to_dormant() {
    static const uint32_t to_dorm_seq[] = { 0xe3bc };

    swd_line_reset();
    swd_send_bits((uint32_t *)to_dorm_seq, 16);
}
