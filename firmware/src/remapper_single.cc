#include <tusb.h>

#include "pio_usb.h"
#include "usb_midi_host.h"

#include "pico/time.h"

#include "descriptor_parser.h"
#include "out_report.h"
#include "remapper.h"
#include "tick.h"

void extra_init() {
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = PICO_DEFAULT_PIO_USB_DP_PIN;
    pio_cfg.skip_alarm_pool = true;
    tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
}

uint32_t get_gpio_valid_pins_mask() {
    return GPIO_VALID_PINS_BASE & ~(
#ifdef PICO_DEFAULT_UART_TX_PIN
                                      (1 << PICO_DEFAULT_UART_TX_PIN) |
#endif
#ifdef PICO_DEFAULT_UART_RX_PIN
                                      (1 << PICO_DEFAULT_UART_RX_PIN) |
#endif
                                      (1 << PICO_DEFAULT_PIO_USB_DP_PIN) |
                                      (1 << (PICO_DEFAULT_PIO_USB_DP_PIN + 1)));
}

static bool reports_received;

void read_report(bool* new_report, bool* tick) {
    *tick = get_and_clear_tick_pending();

    reports_received = false;
    tuh_task();
    *new_report = reports_received;
}

void interval_override_updated() {
}

void flash_b_side() {
}

void descriptor_received_callback(uint16_t vendor_id, uint16_t product_id, const uint8_t* report_descriptor, int len, uint16_t interface) {
    parse_descriptor(vendor_id, product_id, report_descriptor, len, interface);
}

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    printf("tuh_hid_mount_cb\n");

    uint16_t vid;
    uint16_t pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    descriptor_received_callback(vid, pid, desc_report, desc_len, (uint16_t) (dev_addr << 8) | instance);

    device_connected_callback((uint16_t) (dev_addr << 8) | instance, vid, pid);

    tuh_hid_receive_report(dev_addr, instance);
}

void umount_callback(uint8_t dev_addr, uint8_t instance) {
    device_disconnected_callback(dev_addr);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("tuh_hid_umount_cb\n");
    umount_callback(dev_addr, instance);
}

void report_received_callback(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    if (len > 0) {
        handle_received_report(report, len, (uint16_t) (dev_addr << 8) | instance);

        reports_received = true;
    }
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    report_received_callback(dev_addr, instance, report, len);

    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets) {
    uint8_t buf[4];
    while (tuh_midi_packet_read(dev_addr, buf)) {
        handle_received_midi(dev_addr, buf);
    }
    reports_received = true;
}

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    do_queue_out_report(buffer, len, report_id, interface >> 8, interface & 0xFF, OutType::OUTPUT);
}

void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    do_queue_out_report(buffer, len, report_id, interface >> 8, interface & 0xFF, OutType::SET_FEATURE);
}

void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len) {
    do_queue_get_report(report_id, interface >> 8, interface & 0xFF, len);
}

void send_out_report() {
    do_send_out_report();
}

static int64_t manual_sof(alarm_id_t id, void* user_data) {
    pio_usb_host_frame();
    set_tick_pending();
    return 0;
}

void sof_callback() {
    add_alarm_in_us(150, manual_sof, NULL, true);
}

void get_report_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id, uint8_t report_type, uint8_t* report, uint16_t len) {
    handle_get_report_response((uint16_t) (dev_addr << 8) | interface, report_id, report, len);
}

void set_report_complete_cb(uint8_t dev_addr, uint8_t interface, uint8_t report_id) {
    handle_set_report_complete((uint16_t) (dev_addr << 8) | interface, report_id);
}
