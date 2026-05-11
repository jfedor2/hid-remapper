/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2025 Raytac Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <nrfx_power.h>

void board_early_init_hook(void)
{
        /* If powered from USB (high voltage mode), GPIO output voltage
         * defaults to 1.8V which is insufficient to drive the LEDs.
         * Bump REGOUT0 to 3.0V and reset so the change takes effect.
         */
        if ((nrf_power_mainregstatus_get(NRF_POWER) ==
             NRF_POWER_MAINREGSTATUS_HIGH) &&
            ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
             (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))) {

                NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
                while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                        __NOP();
                }

                NRF_UICR->REGOUT0 =
                        (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                        (UICR_REGOUT0_VOUT_3V0 << UICR_REGOUT0_VOUT_Pos);

                NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
                while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                        __NOP();
                }

                /* Reset required for UICR changes to take effect */
                NVIC_SystemReset();
        }
}
