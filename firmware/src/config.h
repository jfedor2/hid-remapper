#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <types.h>

void load_config(const uint8_t* persisted_config);
PersistConfigReturnCode persist_config();

uint16_t handle_get_report1(uint8_t report_id, uint8_t* buffer, uint16_t reqlen);
void handle_set_report1(uint8_t report_id, uint8_t const* buffer, uint16_t bufsize);

void reset_resolution_multiplier();

#endif
