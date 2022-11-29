#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

void load_config(const uint8_t* persisted_config);
void persist_config();

uint16_t handle_get_report(uint8_t report_id, uint8_t* buffer, uint16_t reqlen);
void handle_set_report(uint8_t report_id, uint8_t const* buffer, uint16_t bufsize);

void reset_resolution_multiplier();

#endif
