#ifndef _REMAPPER_H_
#define _REMAPPER_H_

void set_mapping_from_config();
void handle_received_report(const uint8_t* report, int len, uint16_t interface);

void extra_init();
bool read_report();

void interval_override_updated();

#endif
