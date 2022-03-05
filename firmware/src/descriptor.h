#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include <stdint.h>

#define NBUTTONS 8

typedef struct {
    bool active;
    uint8_t report_id;
    uint16_t bitpos;
} button_def_t;

typedef struct {
    bool active;
    uint8_t report_id;
    uint16_t bitpos;
    uint8_t size;
} axis_def_t;

extern button_def_t buttons[NBUTTONS];

extern axis_def_t axes[4];  // X, Y, V, H

extern bool has_report_id;

#ifdef __cplusplus
extern "C" {
#endif

void parse_descriptor(volatile uint8_t* report_descriptor, int len);
void clear_descriptor_data();

#ifdef __cplusplus
}
#endif

#endif
