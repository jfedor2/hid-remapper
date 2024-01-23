#include <hardware/i2c.h>

#include "globals.h"

#include "mcp4651.h"

// XXX this should be configurable on board level
// worry about it later
static uint8_t addrs[NDIGIPOTS][2] = {
    { 0x2d, 0x00 },
    { 0x2d, 0x10 },
    { 0x2b, 0x00 },
    { 0x2b, 0x10 },
    { 0x2f, 0x10 },
    { 0x2f, 0x00 },
};

void mcp4651_write() {
    uint8_t buf[2];
    for (int i = 0; i < NDIGIPOTS; i++) {
        // technically the digipot has a range of 0-256, not 0-255,
        // but we're going to ignore this for now
        buf[0] = addrs[i][1];
        buf[1] = digipot_state[i];
        i2c_write_blocking(I2C_BLOCK, addrs[i][0], buf, sizeof(buf), false);
    }
}
