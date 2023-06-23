#ifndef _TICK_H_
#define _TICK_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void tick_init();
void set_tick_pending();
bool get_and_clear_tick_pending();

#ifdef __cplusplus
}
#endif

#endif
