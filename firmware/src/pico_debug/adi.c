#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lerp/debug.h"

#include "adi.h"
#include "swd.h"

//
// Debug Port Register Addresses
//
#define DP_DPIDR                        0x00U   // IDCODE Register (RD)
#define DP_ABORT                        0x00U   // Abort Register (WR)
#define DP_CTRL_STAT                    0x04U   // Control & Status
#define DP_RESEND                       0x08U   // Resend (RD)
#define DP_SELECT                       0x08U   // Select Register (WR)
#define DP_RDBUFF                       0x0CU   // Read Buffer (RD)
#define DP_TARGETSEL                    0x0CU   // Read Buffer (WR)

#define DP_DLCR                         0x14    // (RW)
#define DP_TARGETID                     0x24    // Target ID (RD)
#define DP_DLPIDR                       0x34    // (RD)
#define DP_EVENTSTAT                    0x44    // (RO)

//
// Control/Status Register Defines
//
#define ORUNDETECT          (1<<0)
#define STICKYORUN          (1<<1)
#define STICKYCMP           (1<<4)
#define STICKYERR           (1<<5)
#define WDATAERR            (1<<7)
#define SWDERRORS           (STICKYORUN|STICKYCMP|STICKYERR)    

#define CDBGRSTREQ          (1<<26)
#define CDBGRSTACK          (1<<27)
#define CDBGPWRUPREQ        (1<<28)
#define CDBGPWRUPACK        (1<<29)
#define CSYSPWRUPREQ        (1<<30)
#define CSYSPWRUPACK        (1<<31)

//
// Abort Register Defines
//
#define DAP_ABORT           (1<<0)
#define STKCMPCLR           (1<<1)          
#define STKERRCLR           (1<<2)
#define WDERRCLR            (1<<3)
#define ORUNERRCLR          (1<<4)
#define ALLERRCLR           (STKCMPCLR|WDERRCLR|WDERRCLR|ORUNERRCLR)


#define TARGET_CORE_0       0x01002927
#define TARGET_CORE_1       0x11002927
#define TARGET_RESCUE       0xf1002927


// ----------------------------------------------------------------------------
// We need a few things on a per-core basis...
// ----------------------------------------------------------------------------

struct reg {
    int valid;
    uint32_t value;
};
struct memcache {
    uint32_t addr;
    uint32_t value;
};

enum {
    STATE_UNKNOWN,
    STATE_RUNNING,
    STATE_HALTED,
};


struct core {
    int                 state;
    int                 reason;

    uint32_t            dp_select_cache;
    uint32_t            ap_mem_csw_cache;

    uint32_t            breakpoints[4];
    struct reg          reg_cache[24];
};


struct core cores[2];

// Core will point at whichever one is current...
struct core *core = &cores[0];


// ----------------------------------------------------------------------------
// Memory cache helper functions
//
// The cache can be invalidated by a single write, for performance, we then take
// the hit when we add a new item.
//
// These functions can be called for all addresses, they will only operate if
// the addr is in the cacheable range (ROM + FLASGH + RAM)
//
// A single memory cache, not one per core since the memory is shared.
// ----------------------------------------------------------------------------
#define MEM_CACHE_SIZE              4
#define IS_MEM_CACHEABLE(addr)      (addr < 0x30000000)

static struct memcache     mem_cache[MEM_CACHE_SIZE];
static int                 mem_cache_pos;
static int                 mem_cache_valid;

static inline int mem_cache_add(uint32_t addr, uint32_t value) {
    if (!IS_MEM_CACHEABLE(addr)) return SWD_ERROR;
    if (!mem_cache_valid) {
        // We were invalidated, so we need to clear everything
        for (int i=0; i < MEM_CACHE_SIZE; i++) {
            mem_cache[i].addr = 0xffffffff;
        }
        mem_cache_pos = 0;
        mem_cache_valid = 1;
    }
    mem_cache[mem_cache_pos].addr = addr;
    mem_cache[mem_cache_pos++].value = value;
    if (mem_cache_pos == MEM_CACHE_SIZE) mem_cache_pos = 0;
    return SWD_OK;
}
static inline int mem_cache_find(uint32_t addr, uint32_t *res) {
    if (!IS_MEM_CACHEABLE(addr)) return SWD_ERROR;
    if (!mem_cache_valid) return SWD_ERROR;
    for (int i=0; i < MEM_CACHE_SIZE; i++) {
        if (mem_cache[i].addr == addr) {
            *res = mem_cache[i].value;
            return SWD_OK;
        }
    }
    return SWD_ERROR;
}
static inline void mem_cache_invalidate() {
    mem_cache_valid = 0;
}

// ----------------------------------------------------------------------------
// Slightly Higher Level Functions
// ----------------------------------------------------------------------------


/**
 * @brief A cached copy of whats in the DP_SELECT register so that we can
 *        be efficient about updating it.
 */
//static int dp_select_cache = 0xffffffff;

/**
 * @brief Change the dp bank in SELECT if it needs changing
 * 
 * @param bank 
 * @return int 
 */
static inline int dp_select_bank(int bank) {
    int rc = SWD_OK;

    assert(bank <= 0xf);

    if ((core->dp_select_cache & 0xf) != bank) {
        core->dp_select_cache = (core->dp_select_cache & 0xfffffff0) | bank;
        rc = swd_write(0, DP_SELECT, core->dp_select_cache);
    }
    return rc;
}

/**
 * @brief Select the AP and bank if we need to (note bank is bits 4-7)
 * 
 * @param ap 
 * @param bank 
 * @return int 
 */
static inline int ap_select_with_bank(uint ap, uint bank) {
    int rc = SWD_OK;

    assert((bank & 0x0f) == 0);
    assert(bank <= 255);
    assert(ap <= 255);

    if ((ap != (core->dp_select_cache >> 24)) || (bank != (core->dp_select_cache & 0xf0))) {
        core->dp_select_cache = (ap << 24) | bank | (core->dp_select_cache & 0xf);
        rc = swd_write(0, DP_SELECT, core->dp_select_cache);
    }
    return rc;
}

int dp_read(uint32_t addr, uint32_t *res) {
    int rc;

    // First check to see if we are reading something where we might
    // care about the dp_banksel
    if ((addr & 0x0f) == 4) {
        rc = dp_select_bank((addr & 0xf0) >> 4);
        if (rc != SWD_OK) return rc;
    }
    return swd_read(0, addr & 0xf, res);
}

int dp_write(uint32_t addr, uint32_t value) {
    int rc;

    // First check to see if we are writing something where we might
    // care about the dp_banksel
    if ((addr & 0x0f) == 4) {
        rc = dp_select_bank((addr & 0xf0) >> 4);
        if (rc != SWD_OK) return rc;
    }
    return swd_write(0, addr & 0xf, value);
}




/**
 * @brief Read a value from the given AP
 * 
 * This means setting it as the SELECTED ap, setting up the bank, reading
 * the value and then getting it from the buffer.
 * 
 * Note: this will destroy *res even if it fails, might want to rethink it.
 * 
 * @param apnum 
 * @param addr 
 * @param res 
 * @return int 
 */
int ap_read(int apnum, uint32_t addr, uint32_t *res) {
    int rc;

    // Select the AP and bank (if needed)
    rc = ap_select_with_bank(apnum, addr & 0xf0);
    if (rc != SWD_OK) return rc;

    // Now kick off the read (addr[3:2])...
    rc = swd_read(1, addr&0xc, res);
    if (rc != SWD_OK) return rc;

    // Now read the real result from 
    rc = swd_read(0, DP_RDBUFF, res);
    return rc;
}
/**
 * @brief The defer version is used for streaming type access where
 *        we return the value from the read (which will be the previous
 *        result) ... the should be followed by a ap_read_last()
 * 
 * @param apnum 
 * @param addr 
 * @param res 
 * @return int 
 */
int ap_read_defer(int apnum, uint32_t addr, uint32_t *res) {
    int rc;

    // Select the AP and bank (if needed)
    rc = ap_select_with_bank(apnum, addr & 0xf0);
    if (rc != SWD_OK) return rc;

    // Now kick off the read (addr[3:2])...
    rc = swd_read(1, addr&0xc, res);
    return rc;
}

int ap_read_last(uint32_t *res) {
    return swd_read(0, DP_RDBUFF, res);
}

/**
 * @brief Write a value to a given AP
 * 
 * @param apnum 
 * @param addr 
 * @param value 
 * @return int 
 */
int ap_write(int apnum, uint32_t addr, uint32_t value) {
    int rc;

    // Select the AP and bank (if needed)
    rc = ap_select_with_bank(apnum, addr & 0xf0);
    if (rc != SWD_OK) return rc;

    // Now kick off the write (addr[3:2])...
    rc = swd_write(1, addr&0xc, value);
    return rc;
}



// DBGSWENABLE, AHB_MASTER_DEBUG, HPROT1, no-auto-inc, need to add size...
#define AP_MEM_CSW_SINGLE     (1 << 31) \
                            | (1 << 29) \
                            | (1 << 25) \
                            | (0 << 4) 

#define AP_MEM_CSW_32       0b010
#define AP_MEM_CSW_16       0b001
#define AP_MEM_CSW_8        0b000

#define AP_MEM_CSW          0x00
#define AP_MEM_TAR          0x04
#define AP_MEM_DRW          0x0C

// DBGSWENABLE, AHB_MASTER_DEBUG, HPROT1, auto-inc, 32-bit
#define AP_MEM_CSW_INC        (1 << 31) \
                            | (1 << 29) \
                            | (1 << 25) \
                            | (1 << 4)


/**
 * @brief Update the memory csw if we need to
 * 
 * @param value 
 */
static inline int ap_mem_set_csw(uint32_t value) {
//    static uint32_t ap_mem_csw_cache = 0xffffffff;
    int rc = SWD_OK;

    if (core->ap_mem_csw_cache != value) {
        core->ap_mem_csw_cache = value;
        rc = ap_write(0, AP_MEM_CSW, value);
    }
    return rc;
}


int mem_read32(uint32_t addr, uint32_t *res) {
    // We implement a 4 word cache....    
    if (mem_cache_find(addr, res) == SWD_OK) return SWD_OK;

    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_SINGLE | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
    CHECK_OK(ap_read(0, AP_MEM_DRW, res));

    mem_cache_add(addr, *res);
    return SWD_OK;
}

int mem_read8(uint32_t addr, uint8_t *res) {
    uint32_t v;

    // Actually do a 32 bit read - may save a CSW update...
    CHECK_OK(mem_read32(addr & 0xfffffffc, &v));

    *res = v >> ((addr & 3) << 3);
    return SWD_OK;
}
int mem_read16(uint32_t addr, uint16_t *res) {
    uint32_t v;

    // Actually do a 32 bit read - may save a CSW update...
    CHECK_OK(mem_read32(addr & 0xfffffffc, &v));

    *res = (addr & 2) ? (v >> 16) : (v & 0xffff);
    return SWD_OK;
}

int mem_write8(uint32_t addr, uint8_t value) {
    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_SINGLE | AP_MEM_CSW_8));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
    return ap_write(0, AP_MEM_DRW, value << ((addr & 3) << 3));
}

int mem_write16(uint32_t addr, uint16_t value) {
    assert((addr & 1) == 0);            // Must be 16 bit aligned

    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_SINGLE | AP_MEM_CSW_16));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
    return (ap_write(0, AP_MEM_DRW, (addr & 2) ? value << 16: value));
}
int mem_write32(uint32_t addr, uint32_t value) {
    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_SINGLE | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
    return ap_write(0, AP_MEM_DRW, value);
}

/**
 * @brief Writes memory to the target it 32bit aligned chunks
 * 
 * Both the src and dst need to be aligned on a 32bit boundary and count
 * needs to be a multiple of 4.
 * 
 * @param addr 
 * @param count 
 * @param src 
 * @return int 
 */
static int mem_write_block_aligned(uint32_t addr, uint32_t count, uint32_t *src) {
    // Set auto-increment and the starting address...
    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_INC | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

    // We count in 32bit words...
    count >>= 2;

    while(count--) {
        CHECK_OK(ap_write(0, AP_MEM_DRW, *src++));

        // We need to track the address to deal with the 1k wrap limit
        addr += 4;
        if ((addr & 0x3ff) == 0) {
            CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
        }
    }
    return SWD_OK;
}
/**
 * @brief Writes memory to the target when it's not aligned
 * 
 * The arget addr needs to be aligned on a 32bit boundary but the src
 * doesn't.
 * 
 * @param addr 
 * @param count 
 * @param src 
 * @return int 
 */
static int mem_write_block_unaligned(uint32_t addr, uint32_t count, uint8_t *src) {
    uint32_t v32;

    // Set auto-increment and the starting address...
    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_INC | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

    // We count in 32bit words...
    count >>= 2;

    while(count--) {
        v32 = *src++;
        v32 |= (*src++) << 8;
        v32 |= (*src++) << 16;
        v32 |= (*src++) << 24;
        CHECK_OK(ap_write(0, AP_MEM_DRW, v32));

        // We need to track the address to deal with the 1k limit
        addr += 4;
        if ((addr & 0x3ff) == 0) {
            CHECK_OK(ap_write(0, AP_MEM_TAR, addr));
        }
    }
    return SWD_OK;
}

int mem_write_block(uint32_t addr, uint32_t count, uint8_t *src) {
    uint16_t v16;

    // The first phase is getting to an aligned address if we aren't...
    if (addr & 3) {
        if ((addr & 1) && count) {
            CHECK_OK(mem_write8(addr++, *src++));
            count--;
            if (!count) return SWD_OK;
        }
        if ((addr & 2) && count) {
            if (count == 1) return mem_write8(addr, *src);
            v16 = *src++;
            v16 |= (*src++) << 8;
            CHECK_OK(mem_write16(addr, v16));
            count -= 2;
            if (!count) return SWD_OK;
            addr += 2;
        }
    }

    // At this point we have an aligned addr, see if we can optimise...
    if (count >= 4) {
        if (((uint32_t)src & 3) == 0) {
            CHECK_OK(mem_write_block_aligned(addr, (count & ~3), (uint32_t *)src));
        } else {
            CHECK_OK(mem_write_block_unaligned(addr, (count & ~3), src));
        }
        count = count & 3;
        if (!count) return SWD_OK;
    }

    // If we get here then we have some stragglers to deal with...
    if (count & 2) {
        v16 = *src++;
        v16 |= (*src++) << 8;
        CHECK_OK(mem_write16(addr, v16));
        count -= 2;
        if (!count) return SWD_OK;
        addr += 2;
    }
    // Must be one byte left to do...
    return mem_write8(addr, *src);
}



int mem_read_block_unaligned(uint32_t addr, uint32_t count, uint8_t *dest) {
    uint32_t v32;
    
    // We count in 32bit words...
    count >>= 2;

    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_INC | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

    CHECK_OK(ap_read_defer(0, AP_MEM_DRW, &v32));
    while (--count) {
        addr += 4;
        if ((addr & 0x3ff) == 0) CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

        CHECK_OK(ap_read_defer(0, AP_MEM_DRW, &v32));
        *dest++ = (v32 & 0xff);
        *dest++ = (v32 & 0xff00) >> 8;
        *dest++ = (v32 & 0xff0000) >> 16;
        *dest++ = v32 >> 24;
    }
    CHECK_OK(ap_read_last(&v32));
        *dest++ = (v32 & 0xff);
        *dest++ = (v32 & 0xff00) >> 8;
        *dest++ = (v32 & 0xff0000) >> 16;
        *dest++ = v32 >> 24;

    return SWD_OK;
}

int mem_read_block_aligned(uint32_t addr, uint32_t count, uint32_t *dest) {
    // We count in 32bit words...
    count >>= 2;

    CHECK_OK(ap_mem_set_csw(AP_MEM_CSW_INC | AP_MEM_CSW_32));
    CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

    CHECK_OK(ap_read_defer(0, AP_MEM_DRW, dest));
    while (--count) {
        addr += 4;
        if ((addr & 0x3ff) == 0) CHECK_OK(ap_write(0, AP_MEM_TAR, addr));

        CHECK_OK(ap_read_defer(0, AP_MEM_DRW, dest++));
    }
    return ap_read_last(dest);
}


int mem_read_block(uint32_t addr, uint32_t count, uint8_t *dest) {
    uint32_t v32;

    // If we have an unaligned starting point then let's read a full word
    // and keep the relevant bits so we don't do mulitple reads of the
    // same word.
    if (addr & 3) {
        CHECK_OK(mem_read32(addr & 0xffffffffc, &v32));

        switch (addr&3) {
            case 1:     *dest++ = (v32 & 0xff00) >> 8; addr++;
            case 2:     *dest++ = (v32 & 0xff0000) >> 16; addr++;
            case 3:     *dest++ = v32 >> 24; addr++;
        }
    }

    // If we are just one aligned read... the use mem_read32 so we get the
    // benefit of the cache.
    if ((count & 0xfffffffc) == 4) {
        CHECK_OK(mem_read32(addr, &v32));
        *dest++ = (v32 & 0xff);
        *dest++ = (v32 & 0xff00) >> 8;
        *dest++ = (v32 & 0xff0000) >> 16;
        *dest++ = v32 >> 24;
        addr += 4;
        count -= 4;
    }

    // At this point we have an aligned addr, see if we can optimise...
    if (count >= 4) {
        if (((uint32_t)dest & 3) == 0) {
            CHECK_OK(mem_read_block_aligned(addr, (count & 0xfffffffc), (uint32_t *)dest));
        } else {
            CHECK_OK(mem_read_block_unaligned(addr, (count & 0xfffffffc), dest));
        }
        dest += count & 0xfffffffc;
        addr += count & 0xfffffffc;
        count = count & 3;
    }

    // If we have some stuff left then we can do a single read...
    if (count) {
        CHECK_OK(mem_read32(addr, &v32));
        while (count--) {
            *dest++ = (v32 & 0xff);
            v32 >>= 8;
        }
    }
    return SWD_OK;
}



#define DCB_DHCSR       0xE000EDF0
#define DCB_DCRSR       0xE000EDF4
#define DCB_DCRDR       0xE000EDF8
#define DCB_DEMCR       0xE000EDFC
#define DCB_DSCSR       0xE000EE08
#define DCB_DFSR        0xE000ED30

#define NVIC_AIRCR      0xE000ED0C


//struct reg reg_cache[24];

#define REG_PC          15  

void reg_flush_cache() {
    for (int i=0; i < sizeof(core->reg_cache)/sizeof(struct reg); i++) {
        core->reg_cache[i].valid = 0;
    }
}


/**
 * @brief Read a core register ... we already assume we are in debug state
 * 
 * @param reg 
 * @param res 
 * @return int 
 */
int reg_read(int reg, uint32_t *res) {
    int rc;
    uint32_t value;

    if (0 && core->reg_cache[reg].valid) {
        *res = core->reg_cache[reg].value;
        return SWD_OK;
    }

    rc = mem_write32(DCB_DCRSR, (0 << 16) | (reg & 0x1f));
    if (rc != SWD_OK) return rc;

    // We are supposed to wait for the reg ready flag ... but it seems we don't
    // need it?
    while (1) {
        rc = mem_read32(DCB_DHCSR, &value);
        if (rc != SWD_OK) return rc;
        if (value & 0x00010000) break;
    }
    CHECK_OK(mem_read32(DCB_DCRDR, &value));
    core->reg_cache[reg].value = value;
    core->reg_cache[reg].valid = 1;
    *res = value;
    return SWD_OK;
}
int reg_write(int reg, uint32_t value) {
    int rc;

    core->reg_cache[reg].value = value;
    core->reg_cache[reg].valid = 1;

    // Write the data into the RDR
    rc = mem_write32(DCB_DCRDR, value);
    if (rc != SWD_OK) return rc;

    // Now write the reg number
    rc = mem_write32(DCB_DCRSR, (1 << 16) | (reg & 0x1f));
    if (rc != SWD_OK) return rc;

    // Now wait until it's done
    while (1) {
        rc = mem_read32(DCB_DHCSR, &value);
        if (rc != SWD_OK) return rc;
        if (value & 0x00010000) break;
    }
    return SWD_OK;
}


#define BPCR        0xE0002000
//static uint32_t breakpoints[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
static const uint32_t bp_reg[4] = { 0xE0002008, 0xE000200C, 0xE0002010, 0xE0002014 };

static inline int bp_find(uint32_t addr) {
    for (int i=0; i < 4; i++) {
        if (core->breakpoints[i] == addr) return i;
    }
    return -1;
}

int bp_is_set(uint32_t addr) {
    return (bp_find(addr) != -1);
}

int bp_set(uint32_t addr) {
    int rc;
    uint32_t matchword;
    int bp = bp_find(addr);
    if (bp != -1) return SWD_OK;        // already have it
    bp = bp_find(0xffffffff);
    if (bp == -1) return SWD_ERROR;     // no slots available

    // Set the breakpoint...
    core->breakpoints[bp] = addr;

    matchword = (addr & 2) ? (0b10 << 30) : (0b01 << 30);

    rc = mem_write32(bp_reg[bp], matchword | (addr & 0x1ffffffc) | (1));
    if (rc != SWD_OK) return rc;

    // Turn on the breakpoint system...
    rc = mem_write32(BPCR, (1<<1) | 1);
    return rc;
}

int bp_clr(uint32_t addr) {
    int rc;

    int bp = bp_find(addr);
    if (bp == -1) return SWD_OK;        // we don't have it? Error?
    core->breakpoints[bp] = 0xffffffff;
    rc = mem_write32(bp_reg[bp], 0);      // fully disabled
    return rc;
}



int core_enable_debug() {
    // Enable debug
    CHECK_OK(mem_write32(DCB_DHCSR, (0xA05F << 16) | 1));

    // Clear each of the breakpoints...
    for (int i=0; i < 4; i++) {
        CHECK_OK(mem_write32(bp_reg[i], 0));
    }
    return SWD_OK;
}

int core_halt() {
    int rc;
    uint32_t value;

    reg_flush_cache();
    mem_cache_invalidate();

    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (1<<3) | (1<<1) | 1);
//    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (1<<1) | 1);
    if (rc != SWD_OK) return rc;
    while (1) {
        rc = mem_read32(DCB_DHCSR, &value);
        if (rc != SWD_OK) return rc;
        if (value & 0x00020000) break;
    }
    core->reason = REASON_DBGRQ;
    return SWD_OK;
}

int core_unhalt() {
    int rc;
    
    reg_flush_cache();
    mem_cache_invalidate();

//    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (1 <<3) | (0<<1) | 1);
    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (0<<1) | 1);
    if (rc != SWD_OK) return rc;
    core->state = STATE_RUNNING;
    return SWD_OK;
    // TODO: more?
}

int core_unhalt_with_masked_ints() {
    int rc;

    reg_flush_cache();
    mem_cache_invalidate();

    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (1<<3) | (0<<1) | 1);
    if (rc != SWD_OK) return rc;
    core->state = STATE_RUNNING;
    return SWD_OK;
}


int core_step() {
    int rc;

    // step and !halt...
    rc = mem_write32(DCB_DHCSR, (0xA05F << 16) | (1<<3) | (1<<2) | (0<<1) | 1);
    if (rc != SWD_OK) return rc;

    reg_flush_cache();
    mem_cache_invalidate();

    core->state = STATE_RUNNING;
    core->reason = REASON_SINGLESTEP;
    return SWD_OK;
}

int core_step_avoiding_breakpoint() {
    uint32_t pc;
    int      had_breakpoint = 0;

    CHECK_OK(reg_read(REG_PC,&pc));
    if (bp_is_set(pc)) {
        bp_clr(pc);
        had_breakpoint = 1;
    }

    CHECK_OK(core_step());

    // Now wait for a halt again...
//    while (1) {
//        rc = mem_read32(DCB_DHCSR, &value);
//        if (rc != SWD_OK) return rc;
//        if (value & 0x00020000) break;
//    }

    // And put the breakpoint back...
    if (had_breakpoint) {
        bp_set(pc);
    }
    return SWD_OK;
}

int core_is_halted() {
    int rc;
    uint32_t value;

    rc = mem_read32(DCB_DHCSR, &value);
    if (rc != SWD_OK) return -1;
    if (value & (1<<17)) return 1;
    return 0;
}

int core_update_status() {
    uint32_t dhcsr;
    uint32_t dfsr;

    CHECK_OK(mem_read32(DCB_DHCSR, &dhcsr));
    CHECK_OK(mem_read32(DCB_DFSR, &dfsr));

    // Are we halted or running...
    if (dhcsr & (1<<17)) {
        core->state = STATE_HALTED;
    } else {
        core->state = STATE_RUNNING;
    }

    // Do we know why?
    if (dhcsr & (1<<25)) {
        core->reason = REASON_DBGRQ;
    } else if (dfsr & (1<<1)) {
        debug_printf("dfsr=0x%08x\r\n", dfsr);
        core->reason = REASON_BREAKPOINT;
        CHECK_OK(mem_write32(DCB_DFSR, (1<<1)));    // clear the BKPT bit
    }
    return SWD_OK;
}

int core_get_reason(int num) {
    return cores[num].reason;
}



/**
 * @brief Reset the core but stop it from executing any instructions
 * 
 * @return int 
 */
int core_reset_halt() {
    int rc;
    uint32_t value;

    reg_flush_cache();
    mem_cache_invalidate();

    // First halt the core...
    core_halt();

    // Now set the DWTENA and VC_CORERESET bits...
    rc = mem_write32(DCB_DEMCR, (1<<24) | (1 << 0));
    if (rc != SWD_OK) return rc;

    // Now reset the core (will be caught by the above)
    rc = mem_write32(NVIC_AIRCR, (0x05FA << 16) | (1 << 2));

    // Now make sure we get a reset flag....
    while (1) {
        rc = mem_read32(DCB_DHCSR, &value);
        if (rc != SWD_OK) return rc;
        if (value & (1<<25)) break;
    }

    // Then make sure it clears...
    while (1) {
        rc = mem_read32(DCB_DHCSR, &value);
        if (rc != SWD_OK) return rc;
        if (!(value & (1<<25))) break;
    }

    // Now clear the CORERESET bit...
    rc = mem_write32(DCB_DEMCR, (1<<24) | (0 << 0));
    return rc;
}




// this is 'M' 'u', 1 (version)
#define BOOTROM_MAGIC 0x01754d
#define BOOTROM_MAGIC_ADDR 0x00000010


uint32_t rp2040_find_rom_func(char ch1, char ch2) {
    uint16_t tag = (ch2 << 8) | ch1;

    // First read the bootrom magic value...
    uint32_t magic;
    int rc;

    rc = mem_read32(BOOTROM_MAGIC_ADDR, &magic);
    if (rc != SWD_OK) return 0;
    if ((magic & 0xffffff) != BOOTROM_MAGIC) return 0;

    // Now find the start of the table...
    uint16_t v;
    uint32_t tabaddr;
    rc = mem_read16(BOOTROM_MAGIC_ADDR+4, &v);
    if (rc != SWD_OK) return 0;
    tabaddr = v;

    // Now try to find our function...
    uint16_t value;
    do {
        rc = mem_read16(tabaddr, &value);
        if (rc != SWD_OK) return 0;
        if (value == tag) {
            rc = mem_read16(tabaddr+2, &value);
            if (rc != SWD_OK) return 0;
            return (uint32_t)value;
        }
        tabaddr += 4;
    } while(value);
    return 0;
}


int rp2040_call_function(uint32_t addr, uint32_t args[], int argc) {
    static uint32_t trampoline_addr = 0;
    static uint32_t trampoline_end;
    int rc;
    uint32_t r0;

    assert(argc <= 4);

    // First get the trampoline address...
    if (!trampoline_addr) {
        trampoline_addr = rp2040_find_rom_func('D', 'T');
        trampoline_end = rp2040_find_rom_func('D', 'E');
        if (!trampoline_addr || !trampoline_end) return SWD_ERROR;
    }

    // Set the registers for the trampoline call...
    // function in r7, args in r0, r1, r2, and r3, end in lr?
    rc = reg_write(7, addr);
    if (rc != SWD_OK) return rc;
    for (int i=0; i < argc; i++) {
        rc = reg_write(i, args[i]);
        if (rc != SWD_OK) return rc;
    }
    

    // Now set the PC to go to our address
    rc = reg_write(15, trampoline_addr);
    if (rc != SWD_OK) return rc;

    // Put the end address in LR
    rc = reg_write(14, trampoline_end);
    if (rc != SWD_OK) return rc;

    // Set the stack pointer to something sensible... (MSP)
    rc = reg_write(17, 0x20040800);
    if (rc != SWD_OK) return rc;

    // Set xPSR for the thumb thingy...
    rc = reg_write(16, (1 << 24));

    rc = reg_read(0, &r0);
    if (rc != SWD_OK) return rc;


    rc = core_is_halted();
    if (rc == -1) lerp_panic("aaarg!");
    if (!rc) lerp_panic("core not halted");

    // Now can we continue and just wait for a halt?
//    core_unhalt();
    core_unhalt_with_masked_ints();
    while(1) {
        busy_wait_ms(2);
        rc = core_is_halted();
        if (rc == -1) lerp_panic("here");
        if (rc) break;
    }


/*
    // Bloody hell if we get here!
    uint32_t regs[5];

    for (int i=0; i < 5; i++) {
        rc = reg_read(i, &regs[i]);
        if (rc != SWD_OK) return rc;
    }

    // What is our pc
    uint32_t pc;
    rc = reg_read(15, &pc);
    if (rc != SWD_OK) return rc;
    */
   return SWD_OK;
}



/**
 * @brief Use the rescue dp to perform a hardware reset
 * 
 * @return int 
 */
int dp_rescue_reset() {
    int rc;
    uint32_t rv;
    const uint32_t zero[] = { 0 };

    debug_printf("Attempting rescue_dp reset\r\n");

    swd_from_dormant();
    swd_line_reset();
    swd_targetsel(0xf1002927);
    rc = swd_read(0, DP_DPIDR, &rv);
    if (rc != SWD_OK) {
        debug_printf("rescue failed (DP_IDR read rc=%d)\r\n", rc);
        return rc;
    }

    // Now toggle the power request which will cause the reset...
    rc = swd_write(0, DP_CTRL_STAT, CDBGPWRUPREQ);
    debug_printf("RESET rc=%d\r\n", rc, rv);
    rc = swd_write(0, DP_CTRL_STAT, 0);
    debug_printf("RESET rc=%d\r\n", rc, rv);

    // Make sure the write completes...
    swd_send_bits((uint32_t *)zero, 8);

    // And delay a bit... no idea how long we need, but we need something.
    for (int i=0; i < 2; i++) {
        swd_send_bits((uint32_t *)zero, 32);
    }
    return SWD_OK;
}

/**
 * @brief Does the basic core select and then reads DP_DPIDR as required
 * 
 * @param num 
 * @return int 
 */
int dp_core_select(int num) {
    uint32_t rv;

    swd_line_reset();
    swd_targetsel(num == 0 ? TARGET_CORE_0 : TARGET_CORE_1);

    CHECK_OK(swd_read(0, DP_DPIDR, &rv));
    return SWD_OK;
}

/**
 * @brief Select the core, but also make sure we can properly read
 *        from it. Used in the initialisation routine.
 * 
 * @param num 
 * @return int 
 */
int dp_core_select_and_confirm(int num) {
    uint32_t rv;

    CHECK_OK(dp_core_select(num));
    CHECK_OK(swd_write(0, DP_ABORT, ALLERRCLR));
    CHECK_OK(swd_write(0, DP_SELECT, 0));
    CHECK_OK(swd_read(0, DP_CTRL_STAT, &rv));
    return SWD_OK;
}

/**
 * @brief Do everything we need to be able to utilise to the AP's
 * 
 * This powers on the needed subdomains so that we can access the
 * other AP's.
 * 
 * @return int 
 */
int dp_power_on() {
    uint32_t    rv;

    for (int i=0; i < 10; i++) {
        // Attempt to power up...
        if (dp_write(DP_CTRL_STAT, CDBGPWRUPREQ|CSYSPWRUPREQ) != SWD_OK) continue;
        if (dp_read(DP_CTRL_STAT, &rv) != SWD_OK) continue;
        if (rv & SWDERRORS) { dp_write(DP_ABORT, ALLERRCLR); continue; }
        if (!(rv & CDBGPWRUPACK)) continue;
        if (!(rv & CSYSPWRUPACK)) continue;
        return SWD_OK;
    }
    return SWD_ERROR;
}



/**
 * @brief Send the required sequence to reset the line and start SWD ops
 *  
 * This routine needs to try to connect to each core and make sure it
 * responds, it also powers up the relevant bits and sets debug enabled.
 */
int dp_initialise() {
    // Initialise the core structures...
    for (int i=0; i < 2; i++) {
        cores[i].state = STATE_UNKNOWN;
        cores[i].reason = REASON_UNDEFINED;
        cores[i].dp_select_cache = 0xffffffff;
        cores[i].ap_mem_csw_cache = 0xffffffff;
        for (int j=0; j < 4; j++) {
            cores[i].breakpoints[j] = 0xffffffff;
        }
        for (int j=0; j < sizeof(cores[i].reg_cache)/sizeof(struct reg); j++) {
            cores[i].reg_cache[j].valid = 0;
        }
    }
    mem_cache_invalidate();
    core = NULL;

    swd_from_dormant();
    int have_reset = 0;

    // Now try to connect to each core and setup power and debug status...
    for (int c = 0; c < 2; c++) {
        while (1) {
            if (dp_core_select_and_confirm(c) != SWD_OK) {
                if (dp_core_select_and_confirm(c) != SWD_OK) {
                    // If we've already reset, then this is fatal...
                    if (have_reset) return SWD_ERROR;
                    dp_rescue_reset();
                    swd_from_dormant();     // seem to need this?
                    have_reset = 1;
                    continue;
                }
            }
            // Make sure we can use dp_xxx calls...
            core = &cores[c];
            core->dp_select_cache = 0;
            if (dp_power_on() != SWD_OK) continue;

            // Now we can enable debugging... (and remove breakpoints)
            if (core_enable_debug() != SWD_OK) continue;

            // If we get here, then this core is fine...
            break;
        }
    }
    // And lets make sure we end on core 0
    if (dp_core_select(0) != SWD_OK) {
        return SWD_ERROR;
    }
    core = &cores[0];
/*
    // Now try to read DP_DLIDR (bank 3)
    rc = swd_write(0, DP_SELECT, 0x3);
    if (rc != SWD_OK) {
        debug_printf("rc=%d\r\n", rc);
    }
    rc = swd_read(0, 0x4, &rv);
    debug_printf("DP_DLIDR rc=%d val=0x%08x\r\n", rc, rv);
*/

    return SWD_OK;
}

int core_get() {
    return (core == &cores[0]) ? 0 : 1;
}

int core_select(int num) {
    uint32_t dpidr = 0;
    uint32_t dlpidr = 0;
    
    // See if we are already selected...
    if (core == &cores[num]) return SWD_OK;

    CHECK_OK(dp_core_select(num));
    
    // Need to switch the core here for dp_read to work...
    core = &cores[num];

    // The core_select above will have set some of the SELECT bits to zero
    //core->dp_select_cache &= 0xfffffff0;

    // If that was ok we can validate the switch by checking the TINSTANCE part of
    // DLPIDR
    CHECK_OK(dp_read(DP_DLPIDR, &dlpidr));

    // TODO: shouldn't we validate DPIDR with DLPIDR?
    return SWD_OK;
}


// If one core stops we need to stop the other one....
// we return the one that stopped (or -1 if neither did)
int check_cores() {
    int cur = core_get();
    int other = 1-cur;
    int old_state = core->state;
    int to_halt = -1;
    int rc = -1;

    // The first phase is just gathering some info...
    core_update_status();
    if ((core->state == STATE_HALTED) && core->state != old_state) {
        // We must have stopped... so we should stop the other one
        debug_printf("looks like core %d has halted\r\n", cur);
        to_halt = other;
        rc = cur;
    }
    core_select(other);
    old_state = core->state;
    core_update_status();
    if ((core->state == STATE_HALTED) && core->state != old_state) {
        debug_printf("looks like core %d has halted\r\n", other);
        to_halt = cur;
        rc = other;
    }

    // At this point we have other selected, so we can halt it if needed...
    if (to_halt == other) {
        if (core->state != STATE_HALTED) {
            debug_printf("Halting core: %d\r\n", other);
            core_halt();
        }
    }

    // Go back to the orginal one and see if we needed to stop that...
    core_select(cur);
    if (to_halt == cur) {
        if (core->state != STATE_HALTED) {
            debug_printf("Halting core: %d\r\n", cur);
            core_halt();
        }
    }
    return rc;
}




int dp_init() {
    CHECK_OK(dp_initialise());
    CHECK_OK(core_select(0));
    return SWD_OK;
}

