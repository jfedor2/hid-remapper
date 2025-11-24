#include <hardware/regs/psm.h>
#include <hardware/sync.h>
#include <hardware/watchdog.h>

#include "dual_b_binary.h"

extern "C" {
#include "adi.h"
#include "flash.h"
#include "swd.h"
}

void watchdog_reboot_target() {
    // We're using our macros so this will only work if debugger and target are the same platform.

    // hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
    mem_write32(WATCHDOG_BASE + WATCHDOG_CTRL_OFFSET + REG_ALIAS_CLR_BITS, WATCHDOG_CTRL_ENABLE_BITS);

    // watchdog_hw->scratch[4] = 0;
    mem_write32(WATCHDOG_BASE + WATCHDOG_SCRATCH4_OFFSET, 0);

    // hw_set_bits(&psm_hw->wdsel, PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS));
    mem_write32(PSM_BASE + PSM_WDSEL_OFFSET + REG_ALIAS_SET_BITS, PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS));

    // hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_PAUSE_JTAG_BITS | WATCHDOG_CTRL_PAUSE_DBG0_BITS | WATCHDOG_CTRL_PAUSE_DBG1_BITS);
    mem_write32(WATCHDOG_BASE + WATCHDOG_CTRL_OFFSET + REG_ALIAS_CLR_BITS, WATCHDOG_CTRL_PAUSE_JTAG_BITS | WATCHDOG_CTRL_PAUSE_DBG0_BITS | WATCHDOG_CTRL_PAUSE_DBG1_BITS);

    // hw_set_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_TRIGGER_BITS);
    mem_write32(WATCHDOG_BASE + WATCHDOG_CTRL_OFFSET + REG_ALIAS_SET_BITS, WATCHDOG_CTRL_TRIGGER_BITS);
}

int main() {
    swd_init();
    dp_init();

    core_select(0);
    core_reset_halt();
    core_select(1);
    core_reset_halt();
    core_select(0);

    rp2040_add_flash_bit(0, dual_b_binary, dual_b_binary_length);
    rp2040_add_flash_bit(0xffffffff, NULL, 0);

    watchdog_reboot_target();

    watchdog_reboot(0, 0, 0);
    while (true) {
        __wfi();
    }

    return 0;
}
