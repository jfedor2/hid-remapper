#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>
#include <bluetooth/services/hogp.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/types.h>
#include <zephyr/usb/class/usb_hid.h>
#include <zephyr/usb/usb_device.h>

#include "config.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"

LOG_MODULE_REGISTER(remapper, LOG_LEVEL_DBG);

#define CHK(X) ({ int err = X; if (err != 0) { LOG_ERR("%s returned %d (%s:%d)", #X, err, __FILE__, __LINE__); } err == 0; })

static const int SCAN_DELAY_MS = 1000;
static const int CLEAR_BONDS_BUTTON_PRESS_MS = 3000;

// these macros don't work in C++ when used directly ("taking address of temporary array")
static auto const BT_UUID_HIDS_ = (struct bt_uuid_16) BT_UUID_INIT_16(BT_UUID_HIDS_VAL);
static auto BT_ADDR_LE_ANY_ = BT_ADDR_LE_ANY[0];
static auto BT_CONN_LE_CREATE_CONN_ = BT_CONN_LE_CREATE_CONN[0];

static struct bt_hogp hogps[CONFIG_BT_MAX_CONN];

static K_SEM_DEFINE(usb_sem0, 1, 1);
static K_SEM_DEFINE(usb_sem1, 1, 1);

static struct k_mutex mutexes[(uint8_t) MutexId::N];

static struct k_mutex get_report_mutex;
static uint8_t get_report_buf[64];
static bool get_report_response_ready = false;

static const struct device* hid_dev0;
static const struct device* hid_dev1;  // config interface

struct report_type {
    uint16_t interface;
    uint8_t len;
    uint8_t data[65];
};

struct descriptor_type {
    uint16_t size;
    uint8_t conn_idx;
    uint8_t data[512];
};

struct hogp_ready_type {
    struct bt_hogp* hogp;
};

struct disconnected_type {
    uint8_t conn_idx;
};

struct set_report_type {
    uint8_t report_id;
    uint8_t interface;
    uint16_t len;
    uint8_t data[64];
};

K_MSGQ_DEFINE(report_q, sizeof(struct report_type), 16, 4);
K_MSGQ_DEFINE(descriptor_q, sizeof(struct descriptor_type), 2, 4);
K_MSGQ_DEFINE(hogp_ready_q, sizeof(struct hogp_ready_type), CONFIG_BT_MAX_CONN, 4);
K_MSGQ_DEFINE(disconnected_q, sizeof(struct disconnected_type), CONFIG_BT_MAX_CONN, 4);
K_MSGQ_DEFINE(set_report_q, sizeof(struct set_report_type), 8, 4);
K_MSGQ_DEFINE(switch_pro_response_q, 64, 8, 4);
ATOMIC_DEFINE(tick_pending, 1);

#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

#define LED0_NODE DT_ALIAS(led0)
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#define LED1_NODE DT_ALIAS(led1)
#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

static struct gpio_callback button_cb_data;

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static bool scanning = false;
static bool peers_only = true;

static struct bt_le_conn_param* conn_param = BT_LE_CONN_PARAM(6, 6, 44, 400);

static bool switch_pro_input_enabled = false;
static uint8_t switch_pro_timer = 0;
static int64_t switch_pro_last_input_ms = 0;
static int64_t switch_pro_last_stats_ms = 0;
static int64_t switch_pro_last_button_log_ms = 0;
static uint8_t switch_pro_current_input[64];

static uint32_t switch_pro_ble_reports = 0;
static uint32_t switch_pro_ble_report_drops = 0;
static uint32_t switch_pro_host_reports = 0;
static uint32_t switch_pro_host_report_drops = 0;
static uint32_t switch_pro_set_reports = 0;
static uint32_t switch_pro_translated_reports = 0;
static uint32_t switch_pro_mapped_writes = 0;
static uint32_t switch_pro_mapped_write_fails = 0;
static uint32_t switch_pro_heartbeat_writes = 0;
static uint32_t switch_pro_heartbeat_write_fails = 0;
static uint32_t switch_pro_response_writes = 0;
static uint32_t switch_pro_response_write_fails = 0;
static uint32_t switch_pro_response_drops = 0;
static uint32_t switch_pro_report_q_highwater = 0;
static uint32_t switch_pro_response_q_highwater = 0;
static uint32_t switch_pro_bt_connected_events = 0;
static uint32_t switch_pro_bt_disconnected_events = 0;
static uint32_t switch_pro_hogp_ready_events = 0;
static uint32_t switch_pro_conn_count_highwater = 0;
static uint32_t switch_pro_last_disconnect_reason = 0;

#define SWITCH_PRO_DIAG_PAGES 5
#define SWITCH_PRO_DIAG_VALUES 7

static uint32_t switch_pro_saved_diag[SWITCH_PRO_DIAG_PAGES][SWITCH_PRO_DIAG_VALUES];
static int64_t switch_pro_last_diag_persist_ms = 0;
static uint16_t switch_pro_axis_last[4] = { 0x8000, 0x8000, 0x8000, 0x8000 };
static uint16_t switch_pro_axis_min[4] = { 0xffff, 0xffff, 0xffff, 0xffff };
static uint16_t switch_pro_axis_max[4] = { 0, 0, 0, 0 };

static bool is_switch_pro_mode() {
    return our_descriptor_number == 6;
}

static void switch_pro_reset_input() {
    memset(switch_pro_current_input, 0, sizeof(switch_pro_current_input));
    switch_pro_current_input[0] = 0x30;
    switch_pro_current_input[2] = 0x91;
    switch_pro_current_input[4] = 0x80;
    switch_pro_current_input[6] = 0x00;
    switch_pro_current_input[7] = 0x08;
    switch_pro_current_input[8] = 0x80;
    switch_pro_current_input[9] = 0x00;
    switch_pro_current_input[10] = 0x08;
    switch_pro_current_input[11] = 0x80;
    switch_pro_current_input[12] = 0x0c;
}

static void switch_pro_reset_axis_diagnostics() {
    for (uint8_t axis = 0; axis < 4; axis++) {
        switch_pro_axis_last[axis] = 0x8000;
        switch_pro_axis_min[axis] = 0xffff;
        switch_pro_axis_max[axis] = 0;
    }
}

static void switch_pro_reset_session() {
    switch_pro_input_enabled = false;
    switch_pro_timer = 0;
    switch_pro_last_input_ms = 0;
    switch_pro_last_stats_ms = 0;
    switch_pro_last_button_log_ms = 0;
    k_msgq_purge(&switch_pro_response_q);
    switch_pro_reset_input();
    switch_pro_reset_axis_diagnostics();
}

static uint32_t queue_depth(struct k_msgq* queue) {
    return k_msgq_num_used_get(queue);
}

static void note_switch_pro_report_q_depth() {
    uint32_t depth = queue_depth(&report_q);
    if (depth > switch_pro_report_q_highwater) {
        switch_pro_report_q_highwater = depth;
    }
}

static void note_switch_pro_response_q_depth() {
    uint32_t depth = queue_depth(&switch_pro_response_q);
    if (depth > switch_pro_response_q_highwater) {
        switch_pro_response_q_highwater = depth;
    }
}

static bool report_button_pressed(const uint8_t* report, size_t button_index) {
    const size_t bit = button_index - 1;
    return (report[1 + bit / 8] & BIT(bit % 8)) != 0;
}

static uint16_t report_axis_16(const uint8_t* report, size_t offset) {
    return (uint16_t) report[offset] | ((uint16_t) report[offset + 1] << 8);
}

static uint16_t switch_pro_apply_axis_deadzone(uint16_t value) {
    const uint16_t center = 0x8000;
    const uint16_t threshold = 0x0100;

    if (value >= center - threshold && value <= center + threshold) {
        return center;
    }
    return value;
}

static uint16_t switch_pro_expand_axis(uint16_t value) {
    if (value <= 0x00ff) {
        return value * 0x0101;
    }
    return value;
}

static uint16_t switch_pro_invert_axis(uint16_t value) {
    return 0xffff - value;
}

static uint16_t switch_pro_normalize_axis(uint16_t value, bool invert) {
    value = switch_pro_expand_axis(value);
    if (invert) {
        value = switch_pro_invert_axis(value);
    }
    return switch_pro_apply_axis_deadzone(value);
}

static uint16_t switch_pro_axis_from_state_or_report(uint32_t usage, uint16_t report_value) {
    bool found = false;
    int32_t value = get_input_state_value(usage, 0, false, &found);
    if (!found) {
        return report_value;
    }
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        value = 255;
    }
    return (uint16_t) (value * 0x0101);
}

static void switch_pro_note_axis(uint8_t axis, uint16_t value) {
    switch_pro_axis_last[axis] = value;
    if (value < switch_pro_axis_min[axis]) {
        switch_pro_axis_min[axis] = value;
    }
    if (value > switch_pro_axis_max[axis]) {
        switch_pro_axis_max[axis] = value;
    }
}

static uint16_t switch_pro_clamp_axis_range(uint16_t value) {
    const uint16_t min = 0x2000;
    const uint16_t max = 0xe000;

    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static void pack_switch_pro_stick(uint8_t* out, uint16_t x16, uint16_t y16) {
    /*
     * Report 0x30 uses packed 12-bit stick positions. Values passed here are
     * normalized to the 16-bit HID Remapper output range.
     */
    x16 = switch_pro_clamp_axis_range(x16);
    y16 = switch_pro_clamp_axis_range(y16);
    uint16_t x = x16 >> 4;
    uint16_t y = y16 >> 4;

    out[0] = x & 0xff;
    out[1] = ((x >> 8) & 0x0f) | ((y & 0x0f) << 4);
    out[2] = (y >> 4) & 0xff;
}

static void apply_switch_pro_hat(uint8_t hat, uint8_t* buttons2) {
    if (hat > 7) {
        return;
    }

    if (hat == 3 || hat == 4 || hat == 5) {
        *buttons2 |= BIT(0);  // Down
    }
    if (hat == 7 || hat == 0 || hat == 1) {
        *buttons2 |= BIT(1);  // Up
    }
    if (hat == 1 || hat == 2 || hat == 3) {
        *buttons2 |= BIT(2);  // Right
    }
    if (hat == 5 || hat == 6 || hat == 7) {
        *buttons2 |= BIT(3);  // Left
    }
}

static void switch_pro_translate_report(const uint8_t* report_with_id, uint8_t len) {
    if ((len < 12) || (report_with_id[0] != 0x30)) {
        return;
    }

    uint8_t next[64];
    memcpy(next, switch_pro_current_input, sizeof(next));
    next[0] = 0x30;
    next[2] = 0x91;
    next[3] = 0;
    next[4] = 0x80;
    next[5] = 0;

    if (report_button_pressed(report_with_id, 1)) next[3] |= BIT(0);   // Y
    if (report_button_pressed(report_with_id, 4)) next[3] |= BIT(1);   // X
    if (report_button_pressed(report_with_id, 2)) next[3] |= BIT(2);   // B
    if (report_button_pressed(report_with_id, 3)) next[3] |= BIT(3);   // A
    if (report_button_pressed(report_with_id, 6)) next[3] |= BIT(6);   // R
    if (report_button_pressed(report_with_id, 8)) next[3] |= BIT(7);   // ZR
    if (report_button_pressed(report_with_id, 9)) next[4] |= BIT(0);   // Minus
    if (report_button_pressed(report_with_id, 10)) next[4] |= BIT(1);  // Plus
    if (report_button_pressed(report_with_id, 12)) next[4] |= BIT(2);  // RS
    if (report_button_pressed(report_with_id, 11)) next[4] |= BIT(3);  // LS
    if (report_button_pressed(report_with_id, 13)) next[4] |= BIT(4);  // Home
    if (report_button_pressed(report_with_id, 14)) next[4] |= BIT(5);  // Capture
    if (report_button_pressed(report_with_id, 5)) next[5] |= BIT(6);   // L
    if (report_button_pressed(report_with_id, 7)) next[5] |= BIT(7);   // ZL

    apply_switch_pro_hat(report_with_id[11] & 0x0f, &next[5]);
    uint16_t lx = switch_pro_normalize_axis(
        switch_pro_axis_from_state_or_report(0x00010030, report_axis_16(report_with_id, 3)), false);
    uint16_t ly = switch_pro_normalize_axis(
        switch_pro_axis_from_state_or_report(0x00010031, report_axis_16(report_with_id, 5)), true);
    uint16_t rx = switch_pro_normalize_axis(
        switch_pro_axis_from_state_or_report(0x00010032, report_axis_16(report_with_id, 7)), false);
    uint16_t ry = switch_pro_normalize_axis(
        switch_pro_axis_from_state_or_report(0x00010035, report_axis_16(report_with_id, 9)), true);
    switch_pro_note_axis(0, lx);
    switch_pro_note_axis(1, ly);
    switch_pro_note_axis(2, rx);
    switch_pro_note_axis(3, ry);
    pack_switch_pro_stick(next + 6, lx, ly);
    pack_switch_pro_stick(next + 9, rx, ry);
    next[12] = 0x0c;

    bool buttons_changed = next[3] != switch_pro_current_input[3] || next[4] != switch_pro_current_input[4] || next[5] != switch_pro_current_input[5];
    memcpy(switch_pro_current_input, next, sizeof(switch_pro_current_input));
    switch_pro_translated_reports++;

    int64_t now = k_uptime_get();
    if (buttons_changed && (now - switch_pro_last_button_log_ms >= 20)) {
        switch_pro_last_button_log_ms = now;
        LOG_INF("switch_pro buttons=%02x %02x %02x sticks=%02x %02x %02x %02x %02x %02x out_q=%u report_q=%u",
            switch_pro_current_input[3], switch_pro_current_input[4], switch_pro_current_input[5],
            switch_pro_current_input[6], switch_pro_current_input[7], switch_pro_current_input[8],
            switch_pro_current_input[9], switch_pro_current_input[10], switch_pro_current_input[11],
            debug_outgoing_report_count(), queue_depth(&report_q));
    }
}

static void switch_pro_queue_response(const uint8_t* response, size_t len) {
    uint8_t buf[64] = {};
    if (len > sizeof(buf)) {
        len = sizeof(buf);
    }
    memcpy(buf, response, len);
    if (k_msgq_put(&switch_pro_response_q, buf, K_NO_WAIT)) {
        switch_pro_response_drops++;
    }
    note_switch_pro_response_q_depth();
}

static void switch_pro_queue_81(uint8_t command) {
    uint8_t response[64] = { 0x81, command };

    if (command == 0x01) {
        static const uint8_t mac_response[] = { 0x81, 0x01, 0x00, 0x03, 0x1f, 0x86, 0x1d, 0xd6, 0x03, 0x04 };
        memcpy(response, mac_response, sizeof(mac_response));
    }

    switch_pro_queue_response(response, sizeof(response));
}

static void switch_pro_queue_21(uint8_t subcommand, uint8_t ack, const uint8_t* data, uint8_t data_len) {
    uint8_t response[64] = {};
    response[0] = 0x21;
    response[1] = switch_pro_timer++;
    memcpy(response + 2, switch_pro_current_input + 2, 11);
    response[13] = ack;
    response[14] = subcommand;
    if (data && data_len) {
        if (data_len > sizeof(response) - 15) {
            data_len = sizeof(response) - 15;
        }
        memcpy(response + 15, data, data_len);
    }
    switch_pro_queue_response(response, sizeof(response));
}

static void switch_pro_spi_read(const uint8_t* args, uint8_t args_len) {
    uint8_t data[48];
    memset(data, 0xff, sizeof(data));
    if (args_len < 5) {
        switch_pro_queue_21(0x10, 0x90, data, sizeof(data));
        return;
    }

    uint32_t address = (uint32_t) args[0] | ((uint32_t) args[1] << 8) | ((uint32_t) args[2] << 16) | ((uint32_t) args[3] << 24);
    uint8_t read_len = args[4];
    memcpy(data, args, 5);

    static const uint8_t stick_cal[] = {
        0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
        0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80
    };
    static const uint8_t body_color[] = { 0x46, 0x46, 0x46 };
    static const uint8_t button_color[] = { 0xff, 0xff, 0xff };

    if (address == 0x0000603d) {
        memcpy(data + 5, stick_cal, MIN(read_len, (uint8_t) sizeof(stick_cal)));
    } else if (address == 0x00006050) {
        memcpy(data + 5, stick_cal, MIN(read_len, (uint8_t) sizeof(stick_cal)));
    } else if (address == 0x00006086) {
        memcpy(data + 5, body_color, MIN(read_len, (uint8_t) sizeof(body_color)));
    } else if (address == 0x00006089) {
        memcpy(data + 5, button_color, MIN(read_len, (uint8_t) sizeof(button_color)));
    }

    switch_pro_queue_21(0x10, 0x90, data, MIN((uint8_t) sizeof(data), (uint8_t) (read_len + 5)));
}

static void switch_pro_handle_subcommand(const uint8_t* report, uint8_t len, bool has_report_id) {
    uint8_t base = has_report_id ? 1 : 0;
    if (len <= base + 9) {
        return;
    }

    uint8_t subcommand = report[base + 9];
    const uint8_t* args = report + base + 10;
    uint8_t args_len = len - base - 10;

    switch (subcommand) {
        case 0x01: {
            static const uint8_t pairing_response[] = { 0x03 };
            switch_pro_queue_21(subcommand, 0x81, pairing_response, sizeof(pairing_response));
            break;
        }
        case 0x02: {
            static const uint8_t device_info[] = { 0x03, 0x48, 0x03, 0x02, 0x53, 0x50, 0x00, 0x01, 0x03, 0x04, 0x01, 0x01 };
            switch_pro_queue_21(subcommand, 0x82, device_info, sizeof(device_info));
            break;
        }
        case 0x04:
            switch_pro_queue_21(subcommand, 0x83, NULL, 0);
            break;
        case 0x10:
            switch_pro_spi_read(args, args_len);
            break;
        case 0x21: {
            static const uint8_t imu_status[] = { 0x01, 0x00, 0xff, 0x00, 0x03, 0x00, 0x05, 0x01 };
            switch_pro_queue_21(subcommand, 0xa0, imu_status, sizeof(imu_status));
            break;
        }
        case 0x30:
            switch_pro_input_enabled = args_len == 0 || args[0] != 0;
            switch_pro_last_input_ms = 0;
            switch_pro_queue_21(subcommand, 0x80, NULL, 0);
            break;
        default:
            switch_pro_queue_21(subcommand, 0x80, NULL, 0);
            break;
    }
}

static bool switch_pro_handle_output_report(const uint8_t* report, uint8_t len) {
    if (!is_switch_pro_mode() || len == 0) {
        return false;
    }

    uint8_t report_id = report[0];
    bool has_report_id = true;

    if (report_id != 0x01 && report_id != 0x10 && report_id != 0x80 && report_id != 0x82) {
        has_report_id = false;
        report_id = len == 1 ? 0x80 : report[0];
    }

    if (report_id == 0x80) {
        uint8_t command = has_report_id ? (len > 1 ? report[1] : 0) : report[0];
        switch (command) {
            case 0x04:
                switch_pro_input_enabled = true;
                switch_pro_last_input_ms = 0;
                break;
            case 0x05:
                switch_pro_input_enabled = false;
                break;
            case 0x01:
            case 0x02:
            case 0x03:
            default:
                switch_pro_queue_81(command);
                break;
        }
        return true;
    }

    if (report_id == 0x01) {
        switch_pro_handle_subcommand(report, len, has_report_id);
        return true;
    }

    return true;
}

static bool switch_pro_send_response() {
    uint8_t response[64];
    if (!is_switch_pro_mode() || k_msgq_get(&switch_pro_response_q, response, K_NO_WAIT)) {
        return false;
    }
    bool sent = CHK(hid_int_ep_write(hid_dev0, response, sizeof(response), NULL));
    if (sent) {
        switch_pro_response_writes++;
    } else {
        switch_pro_response_write_fails++;
    }
    return sent;
}

static bool switch_pro_send_input_heartbeat() {
    if (!is_switch_pro_mode() || !switch_pro_input_enabled) {
        return false;
    }

    int64_t now = k_uptime_get();
    if (switch_pro_last_input_ms && (now - switch_pro_last_input_ms < 8)) {
        return false;
    }

    switch_pro_current_input[1] = switch_pro_timer++;
    switch_pro_last_input_ms = now;
    bool sent = CHK(hid_int_ep_write(hid_dev0, switch_pro_current_input, sizeof(switch_pro_current_input), NULL));
    if (sent) {
        switch_pro_heartbeat_writes++;
    } else {
        switch_pro_heartbeat_write_fails++;
    }
    return sent;
}

static void switch_pro_fill_diagnostics(uint32_t page, uint32_t values[SWITCH_PRO_DIAG_VALUES]) {
    memset(values, 0, SWITCH_PRO_DIAG_VALUES * sizeof(uint32_t));

    switch (page) {
        case 0:
            values[0] = 0x44315053;  // SP1D
            values[1] = 1;
            values[2] = switch_pro_input_enabled;
            values[3] = (queue_depth(&report_q) & 0xffff) | (switch_pro_report_q_highwater << 16);
            values[4] = (queue_depth(&switch_pro_response_q) & 0xffff) | (switch_pro_response_q_highwater << 16);
            values[5] = debug_outgoing_report_count();
            values[6] = debug_outgoing_report_overflows();
            break;
        case 1:
            values[0] = switch_pro_ble_reports;
            values[1] = switch_pro_ble_report_drops;
            values[2] = switch_pro_host_reports;
            values[3] = switch_pro_host_report_drops;
            values[4] = switch_pro_set_reports;
            values[5] = switch_pro_translated_reports;
            values[6] = switch_pro_response_drops;
            break;
        case 2:
            values[0] = switch_pro_mapped_writes;
            values[1] = switch_pro_mapped_write_fails;
            values[2] = switch_pro_heartbeat_writes;
            values[3] = switch_pro_heartbeat_write_fails;
            values[4] = switch_pro_response_writes;
            values[5] = switch_pro_response_write_fails;
            values[6] = switch_pro_timer;
            break;
        case 3:
            values[0] = switch_pro_current_input[3] | (switch_pro_current_input[4] << 8) | (switch_pro_current_input[5] << 16);
            values[1] = switch_pro_current_input[6] | (switch_pro_current_input[7] << 8) | (switch_pro_current_input[8] << 16);
            values[2] = switch_pro_current_input[9] | (switch_pro_current_input[10] << 8) | (switch_pro_current_input[11] << 16);
            values[3] = switch_pro_current_input[1];
            values[4] = switch_pro_last_input_ms;
            values[5] = (switch_pro_bt_connected_events & 0xffff) | ((switch_pro_hogp_ready_events & 0xffff) << 16);
            values[6] = (switch_pro_bt_disconnected_events & 0xffff) | ((switch_pro_last_disconnect_reason & 0xff) << 16) | ((switch_pro_conn_count_highwater & 0xff) << 24);
            break;
        case 4:
            values[0] = switch_pro_axis_last[0] | (switch_pro_axis_last[1] << 16);
            values[1] = switch_pro_axis_last[2] | (switch_pro_axis_last[3] << 16);
            values[2] = switch_pro_axis_min[0] | (switch_pro_axis_max[0] << 16);
            values[3] = switch_pro_axis_min[1] | (switch_pro_axis_max[1] << 16);
            values[4] = switch_pro_axis_min[2] | (switch_pro_axis_max[2] << 16);
            values[5] = switch_pro_axis_min[3] | (switch_pro_axis_max[3] << 16);
            break;
        default:
            break;
    }
}

static void switch_pro_persist_diagnostics() {
    if (!is_switch_pro_mode() || !switch_pro_input_enabled) {
        return;
    }
    if (switch_pro_ble_reports == 0 && switch_pro_translated_reports <= 2) {
        return;
    }

    int64_t now = k_uptime_get();
    if (switch_pro_last_diag_persist_ms && (now - switch_pro_last_diag_persist_ms < 2000)) {
        return;
    }
    switch_pro_last_diag_persist_ms = now;

    for (uint32_t page = 0; page < SWITCH_PRO_DIAG_PAGES; page++) {
        switch_pro_fill_diagnostics(page, switch_pro_saved_diag[page]);
    }
    settings_save_one("remapper/switch_pro_diag", switch_pro_saved_diag, sizeof(switch_pro_saved_diag));
}

static void switch_pro_log_stats() {
    if (!is_switch_pro_mode()) {
        return;
    }

    int64_t now = k_uptime_get();
    if (switch_pro_last_stats_ms && (now - switch_pro_last_stats_ms < 1000)) {
        return;
    }
    switch_pro_last_stats_ms = now;

    LOG_INF("switch_pro_stats enabled=%u report_q=%u/%u response_q=%u/%u out_q=%u out_overflows=%u ble=%u ble_drop=%u host=%u host_drop=%u set=%u translated=%u mapped=%u/%u heartbeat=%u/%u response=%u/%u response_drop=%u buttons=%02x %02x %02x",
        switch_pro_input_enabled,
        queue_depth(&report_q), switch_pro_report_q_highwater,
        queue_depth(&switch_pro_response_q), switch_pro_response_q_highwater,
        debug_outgoing_report_count(), debug_outgoing_report_overflows(),
        switch_pro_ble_reports, switch_pro_ble_report_drops,
        switch_pro_host_reports, switch_pro_host_report_drops,
        switch_pro_set_reports,
        switch_pro_translated_reports,
        switch_pro_mapped_writes, switch_pro_mapped_write_fails,
        switch_pro_heartbeat_writes, switch_pro_heartbeat_write_fails,
        switch_pro_response_writes, switch_pro_response_write_fails,
        switch_pro_response_drops,
        switch_pro_current_input[3], switch_pro_current_input[4], switch_pro_current_input[5]);

    switch_pro_persist_diagnostics();
}

static void activity_led_off_work_fn(struct k_work* work) {
    gpio_pin_set_dt(&led0, false);
}
static K_WORK_DELAYABLE_DEFINE(activity_led_off_work, activity_led_off_work_fn);

enum class LedMode {
    OFF = 0,
    ON = 1,
    BLINK = 2,
};

static atomic_t led_blink_count = ATOMIC_INIT(0);
static atomic_t led_mode = (atomic_t) ATOMIC_INIT(LedMode::OFF);
static int led_blinks_left = 0;
static bool next_blink_state = true;

static void led_work_fn(struct k_work* work);
static K_WORK_DELAYABLE_DEFINE(led_work, led_work_fn);

static void led_work_fn(struct k_work* work) {
    LedMode my_led_mode = (LedMode) atomic_get(&led_mode);
    switch (my_led_mode) {
        case LedMode::OFF:
            gpio_pin_set_dt(&led1, false);
            break;
        case LedMode::ON:
            gpio_pin_set_dt(&led1, true);
            break;
        case LedMode::BLINK: {
            int next_work = 0;
            if (next_blink_state) {
                if (led_blinks_left > 0) {
                    led_blinks_left--;
                    gpio_pin_set_dt(&led1, true);
                    next_blink_state = false;
                    next_work = 100;
                } else {
                    led_blinks_left = atomic_get(&led_blink_count);
                    gpio_pin_set_dt(&led1, false);
                    next_work = 1000;
                }
            } else {
                gpio_pin_set_dt(&led1, false);
                next_blink_state = true;
                next_work = 100;
            }
            k_work_reschedule(&led_work, K_MSEC(next_work));
            break;
        }
    }
}

static void set_led_mode(LedMode led_mode_) {
    if (atomic_set(&led_mode, (atomic_val_t) led_mode_) != (atomic_val_t) led_mode_) {
        k_work_reschedule(&led_work, K_NO_WAIT);
    }
}

static void scan_start() {
    if (CHK(bt_scan_start(BT_SCAN_TYPE_SCAN_PASSIVE))) {
        LOG_DBG("Scanning started.");
        scanning = true;
    }
}

static void scan_stop() {
    if (CHK(bt_scan_stop())) {
        LOG_DBG("Scanning stopped.");
        scanning = false;
        set_led_mode(LedMode::BLINK);
    }
}

static void process_bond(const struct bt_bond_info* info, void* user_data) {
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(&info->addr, addr_str, sizeof(addr_str));
    LOG_DBG("%s", addr_str);
    (*((int*) user_data))++;
    CHK(bt_scan_filter_add(BT_SCAN_FILTER_TYPE_ADDR, &info->addr));
}

static void count_conn_cb(struct bt_conn* conn, void* data) {
    (*((int*) data))++;
}

static int count_connections() {
    int conn_count = 0;
    bt_conn_foreach(BT_CONN_TYPE_LE, count_conn_cb, &conn_count);
    atomic_set(&led_blink_count, conn_count);
    return conn_count;
}

static bool scan_setup_filters() {
    bt_scan_filter_remove_all();

    if (!CHK(bt_scan_filter_add(BT_SCAN_FILTER_TYPE_UUID, (struct bt_uuid*) &BT_UUID_HIDS_))) {
        return false;
    }

    int bonded_count = 0;
    bt_foreach_bond(BT_ID_DEFAULT, process_bond, &bonded_count);

    int conn_count = count_connections();

    uint8_t filter_mode = BT_SCAN_UUID_FILTER;

    if (peers_only && (bonded_count > 0)) {
        if (conn_count == bonded_count) {
            LOG_DBG("all bonded peers connected, not scanning");
            return false;
        }
        filter_mode |= BT_SCAN_ADDR_FILTER;
        LOG_DBG("scanning for bonded peers only");
    } else {
        LOG_DBG("scanning for new peers");
        peers_only = false;
    }

    if (!CHK(bt_scan_filter_enable(filter_mode, true))) {
        return false;
    }

    return true;
}

static void scan_start_work_fn(struct k_work* work) {
    if (scanning) {
        scan_stop();
    }
    if (scan_setup_filters()) {
        scan_start();
        set_led_mode(peers_only ? LedMode::BLINK : LedMode::ON);
    } else {
        set_led_mode(LedMode::BLINK);
    }
}
static K_WORK_DELAYABLE_DEFINE(scan_start_work, scan_start_work_fn);

static void scan_stop_work_fn(struct k_work* work) {
    scan_stop();
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}
static K_WORK_DEFINE(scan_stop_work, scan_stop_work_fn);

static void disconnect_conn(struct bt_conn* conn, void* data) {
    CHK(bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN));
}

static void clear_bonds_work_fn(struct k_work* work) {
    if (CHK(bt_unpair(BT_ID_DEFAULT, &BT_ADDR_LE_ANY_))) {
        LOG_INF("Bonds cleared.");
    } else {
        return;
    }

    scan_stop();
    bt_conn_foreach(BT_CONN_TYPE_LE, disconnect_conn, NULL);
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}
static K_WORK_DEFINE(clear_bonds_work, clear_bonds_work_fn);

static void scan_filter_match(struct bt_scan_device_info* device_info, struct bt_scan_filter_match* filter_match, bool connectable) {
    char addr[BT_ADDR_LE_STR_LEN];

    if (!filter_match->uuid.match || (filter_match->uuid.count != 1)) {
        LOG_WRN("%s invalid device connected", __func__);
        return;
    }

    bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));

    LOG_INF("%s address: %s connectable: %s", __func__, addr, connectable ? "yes" : "no");
}

static void scan_connecting_error(struct bt_scan_device_info* device_info) {
    LOG_WRN("");
}

static void scan_connecting(struct bt_scan_device_info* device_info, struct bt_conn* conn) {
    LOG_INF("");
}

// XXX this hasn't been tested in practice
static void scan_filter_no_match(struct bt_scan_device_info* device_info, bool connectable) {
    struct bt_conn* conn;
    char addr[BT_ADDR_LE_STR_LEN];

    if (device_info->recv_info->adv_type == BT_GAP_ADV_TYPE_ADV_DIRECT_IND) {
        bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));
        LOG_INF("Direct advertising received from %s", addr);
        scan_stop();  // XXX

        if (CHK(bt_conn_le_create(device_info->recv_info->addr, &BT_CONN_LE_CREATE_CONN_, device_info->conn_param, &conn))) {
            bt_conn_unref(conn);
        }
    }
}

BT_SCAN_CB_INIT(scan_cb, scan_filter_match, scan_filter_no_match, scan_connecting_error, scan_connecting);

// This is a workaround for the Xbox Adaptive Controller that sends UUIDs like this:
// 00002a4a-0000-0000-0000-000000000000 in Find Information responses.
// This is not the correct UUID128 representation for UUID16(2a4a), which would be:
// 00002a4a-0000-1000-8000-00805f9b34fb
static void patch_broken_uuids(struct bt_gatt_dm* dm) {
    const struct bt_gatt_dm_attr* attr = NULL;
    char str1[BT_UUID_STR_LEN];
    char str2[BT_UUID_STR_LEN];

    while (NULL != (attr = bt_gatt_dm_attr_next(dm, attr))) {
        if (attr->uuid->type == BT_UUID_TYPE_128) {
            bool needs_fix = true;
            for (int i = 0; i < 16; i++) {
                if ((i != 12) && (i != 13) && (BT_UUID_128(attr->uuid)->val[i] != 0)) {
                    needs_fix = false;
                    break;
                }
            }
            if (needs_fix) {
                bt_uuid_to_str(attr->uuid, str1, sizeof(str2));
                *((bt_uuid_16*) attr->uuid) = {
                    .uuid = { BT_UUID_TYPE_16 },
                    .val = (BT_UUID_128(attr->uuid)->val[13] << 8 | BT_UUID_128(attr->uuid)->val[12])
                };
                bt_uuid_to_str(attr->uuid, str2, sizeof(str2));
                LOG_INF("%s -> %s", str1, str2);
            }
        }
    }
}

static void discovery_completed_cb(struct bt_gatt_dm* dm, void* context) {
    LOG_INF("");
    patch_broken_uuids(dm);
    CHK(bt_hogp_handles_assign(dm, ((struct bt_hogp*) context)));  // XXX disconnect if this fails?
    CHK(bt_gatt_dm_data_release(dm));
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static void discovery_service_not_found_cb(struct bt_conn* conn, void* context) {
    LOG_WRN("");
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static void discovery_error_found_cb(struct bt_conn* conn, int err, void* context) {
    LOG_ERR("err=%d", err);
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static const struct bt_gatt_dm_cb discovery_cb = {
    .completed = discovery_completed_cb,
    .service_not_found = discovery_service_not_found_cb,
    .error_found = discovery_error_found_cb,
};

static void gatt_discover(struct bt_conn* conn) {
    uint8_t conn_idx = bt_conn_index(conn);
    if (!CHK(bt_gatt_dm_start(conn, (struct bt_uuid*) &BT_UUID_HIDS_, &discovery_cb, &hogps[conn_idx]))) {
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
    }
}

static int64_t button_pressed_at;

static void button_cb(const struct device* dev, struct gpio_callback* cb, uint32_t pins) {
    int button_state = gpio_pin_get(dev, button.pin);
    if (button_state) {
        button_pressed_at = k_uptime_get();
    } else {
        if (k_uptime_get() - button_pressed_at > CLEAR_BONDS_BUTTON_PRESS_MS) {
            clear_bonds();
        } else {
            pair_new_device();
        }
    }
}

static void connected(struct bt_conn* conn, uint8_t conn_err) {
    char addr[BT_ADDR_LE_STR_LEN];

    scanning = false;
    int conn_count = count_connections();
    if (is_switch_pro_mode() && conn_count > (int) switch_pro_conn_count_highwater) {
        switch_pro_conn_count_highwater = conn_count;
    }
    set_led_mode(LedMode::BLINK);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (conn_err) {
        LOG_ERR("Failed to connect to %s (conn_err=%u).", addr, conn_err);
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));

        return;
    }

    LOG_INF("%s", addr);

    if (is_switch_pro_mode()) {
        switch_pro_bt_connected_events++;
    }

    CHK(bt_conn_set_security(conn, BT_SECURITY_L2));
}

static void disconnected(struct bt_conn* conn, uint8_t reason) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("%s (reason=%u)", addr, reason);

    if (is_switch_pro_mode()) {
        switch_pro_bt_disconnected_events++;
        switch_pro_last_disconnect_reason = reason;
    }

    uint8_t conn_idx = bt_conn_index(conn);

    if (bt_hogp_assign_check(&hogps[conn_idx])) {
        bt_hogp_release(&hogps[conn_idx]);
    }

    struct disconnected_type disconnected_item = { .conn_idx = conn_idx };
    CHK(k_msgq_put(&disconnected_q, &disconnected_item, K_NO_WAIT));

    count_connections();

    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static void security_changed(struct bt_conn* conn, bt_security_t level, enum bt_security_err err) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_INF("%s, level=%u.", addr, level);
        peers_only = true;
        gatt_discover(conn);
    } else {
        LOG_ERR("security failed: %s, level=%u, err=%d", addr, level, err);
    }
}

static void le_param_updated(struct bt_conn* conn, uint16_t interval, uint16_t latency, uint16_t timeout) {
    LOG_INF("interval=%u, latency=%u, timeout=%u", interval, latency, timeout);
}

static bool le_param_req(struct bt_conn* conn, struct bt_le_conn_param* param) {
    LOG_INF("interval_min=%d, interval_max=%d, latency=%d, timeout=%d", param->interval_min, param->interval_max, param->latency, param->timeout);
    param->interval_max = param->interval_min;
    return true;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req,
    .le_param_updated = le_param_updated,
    .security_changed = security_changed,
};

static void scan_init() {
    struct bt_scan_init_param scan_init = {
        .scan_param = NULL,
        .connect_if_match = 1,
        .conn_param = conn_param,
    };

    bt_scan_init(&scan_init);
    bt_scan_cb_register(&scan_cb);
}

static int8_t hogp_index(struct bt_hogp* hogp) {
    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        if (&hogps[i] == hogp) {
            return i;
        }
    }

    LOG_ERR("unknown hogp!");
    return -1;
}

static uint8_t hogp_notify_cb(struct bt_hogp* hogp, struct bt_hogp_rep_info* rep, uint8_t err, const uint8_t* data) {
    k_work_reschedule(&activity_led_off_work, K_MSEC(50));  // XXX what if work_fn is currently running? it might turn the led off after we turn it on
    gpio_pin_set_dt(&led0, true);

    if (!data) {
        return BT_GATT_ITER_STOP;
    }

    if (scanning) {
        scanning = false;  // more reports can come in before we actually stop scanning; there's probably a scenario where this causes trouble though
        k_work_submit(&scan_stop_work);
    } else {
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
    }

    static struct report_type buf;
    buf.interface = hogp_index(hogp) << 8;
    buf.len = bt_hogp_rep_size(rep) + 1;
    buf.data[0] = bt_hogp_rep_id(rep);

    memcpy(buf.data + 1, data, buf.len);
    if (k_msgq_put(&report_q, &buf, K_NO_WAIT)) {
        if (is_switch_pro_mode()) {
            switch_pro_ble_report_drops++;
        }
    } else if (is_switch_pro_mode()) {
        switch_pro_ble_reports++;
        note_switch_pro_report_q_depth();
    }

    return BT_GATT_ITER_CONTINUE;
}

// XXX is this ready for simultaneous connection setup? is discovery ready? do we care?
static struct descriptor_type their_descriptor;

static void hogp_map_read_cb(struct bt_hogp* hogp, uint8_t err, const uint8_t* data, size_t size, size_t offset) {
    if (data == NULL) {
        their_descriptor.size = offset;
        their_descriptor.conn_idx = hogp_index(hogp);
        CHK(k_msgq_put(&descriptor_q, &their_descriptor, K_NO_WAIT));
        return;
    }

    memcpy(their_descriptor.data + offset, data, size);

    bt_hogp_map_read(hogp, hogp_map_read_cb, offset + size, K_NO_WAIT);
}

struct find_bond_t {
    bt_addr_le_t addr;
    uint8_t i;
    uint8_t found_idx;
};

static void find_bond_cb(const struct bt_bond_info* info, void* user_data) {
    struct find_bond_t* find_bond = (struct find_bond_t*) user_data;
    find_bond->i++;
    if (bt_addr_le_eq(&find_bond->addr, &info->addr)) {
        find_bond->found_idx = find_bond->i;
    }
}

static void hogp_ready_work_fn(struct k_work* work) {
    struct bt_hogp_rep_info* rep = NULL;
    struct hogp_ready_type item;

    while (!k_msgq_get(&hogp_ready_q, &item, K_NO_WAIT)) {
        LOG_INF("hogp_ready");
        if (is_switch_pro_mode()) {
            switch_pro_hogp_ready_events++;
        }

        struct find_bond_t find_bond = {
            .i = 0,
            .found_idx = 0,
        };
        bt_addr_le_copy(&find_bond.addr, bt_conn_get_dst(bt_hogp_conn(item.hogp)));
        bt_foreach_bond(BT_ID_DEFAULT, find_bond_cb, &find_bond);
        LOG_DBG("found bond idx: %d", find_bond.found_idx);
        device_connected_callback(bt_conn_index(bt_hogp_conn(item.hogp)) << 8, 1, 1, find_bond.found_idx);

        while (NULL != (rep = bt_hogp_rep_next(item.hogp, rep))) {
            if (bt_hogp_rep_type(rep) == BT_HIDS_REPORT_TYPE_INPUT) {
                LOG_DBG("subscribing to report ID: %u", bt_hogp_rep_id(rep));
                CHK(bt_hogp_rep_subscribe(item.hogp, rep, hogp_notify_cb));
            }
        }

        bt_hogp_map_read(item.hogp, hogp_map_read_cb, 0, K_NO_WAIT);
    }
}
static K_WORK_DEFINE(hogp_ready_work, hogp_ready_work_fn);

static void hogp_ready_cb(struct bt_hogp* hogp) {
    struct hogp_ready_type q_item = { .hogp = hogp };
    CHK(k_msgq_put(&hogp_ready_q, &q_item, K_NO_WAIT));
    k_work_submit(&hogp_ready_work);
}

static void hogp_prep_error_cb(struct bt_hogp* hogp, int err) {
    LOG_ERR("err=%d", err);
}

static const struct bt_hogp_init_params hogp_init_params = {
    .ready_cb = hogp_ready_cb,
    .prep_error_cb = hogp_prep_error_cb,
};

static void auth_cancel(struct bt_conn* conn) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_WRN("%s", addr);
}

static void pairing_complete(struct bt_conn* conn, bool bonded) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_INF("%s, bonded=%d", addr, bonded);
}

static void pairing_failed(struct bt_conn* conn, enum bt_security_err reason) {
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    LOG_ERR("%s, reason %d", addr, reason);
}

static struct bt_conn_auth_cb conn_auth_callbacks = {
    .cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
    .pairing_complete = pairing_complete,
    .pairing_failed = pairing_failed
};

static int set_report_cb(const struct device* dev, struct usb_setup_packet* setup, int32_t* len, uint8_t** data) {
    uint8_t request_value[2];

    // report_id, report_type
    sys_put_le16(setup->wValue, request_value);

    LOG_INF("report_id=%d, report_type=%d, len=%d", request_value[0], request_value[1], *len);
    LOG_HEXDUMP_DBG((*data), (uint32_t) *len, "");

    struct set_report_type buf;
    if ((request_value[0] > 0) && (*len > 0)) {
        if ((dev == hid_dev0) && set_report0_synchronous(request_value[0])) {
            handle_set_report0(request_value[0], (*data) + 1, (*len) - 1);
        } else {
            if (dev == hid_dev0) {
                buf.interface = 0;
            } else if (dev == hid_dev1) {
                buf.interface = 1;
                k_mutex_lock(&get_report_mutex, K_FOREVER);
                get_report_response_ready = false;
                k_mutex_unlock(&get_report_mutex);
            }
            buf.report_id = request_value[0];
            buf.len = *len - 1;
            memcpy(buf.data, (*data) + 1, (*len) - 1);
            CHK(k_msgq_put(&set_report_q, &buf, K_NO_WAIT));
        }
    } else {
        LOG_ERR("no report ID?");
    }

    return 0;
};

static int get_report_cb(const struct device* dev, struct usb_setup_packet* setup, int32_t* len, uint8_t** data) {
    uint8_t request_value[2];

    sys_put_le16(setup->wValue, request_value);

    LOG_INF("report_id=%d, %d, len=%d", request_value[0], request_value[1], *len);

    *data[0] = request_value[0];
    if (dev == hid_dev0) {
        *len = handle_get_report0(request_value[0], (*data) + 1, CONFIG_SIZE);
    } else if (dev == hid_dev1) {
        k_mutex_lock(&get_report_mutex, K_FOREVER);
        if (get_report_response_ready) {
            memcpy((*data) + 1, get_report_buf, CONFIG_SIZE);
            *len = CONFIG_SIZE;
        } else {
            LOG_INF("response not ready");
            *len = 0;
        }
        get_report_response_ready = false;
        k_mutex_unlock(&get_report_mutex);
    }
    (*len)++;

    return 0;
};

static void int_in_ready_cb0(const struct device* dev) {
    k_sem_give(&usb_sem0);
}

static void int_out_ready_cb0(const struct device* dev) {
    static struct report_type buf;
    uint32_t len;
    if (CHK(hid_int_ep_read(hid_dev0, buf.data, sizeof(buf.data), &len))) {
        if (is_switch_pro_mode() && switch_pro_handle_output_report(buf.data, len)) {
            switch_pro_host_reports++;
            return;
        }

        buf.interface = OUR_OUT_INTERFACE;
        buf.len = len;
        if (k_msgq_put(&report_q, &buf, K_NO_WAIT)) {
            if (is_switch_pro_mode()) {
                switch_pro_host_report_drops++;
            }
        } else if (is_switch_pro_mode()) {
            switch_pro_host_reports++;
            note_switch_pro_report_q_depth();
        }
    }
}

static void int_in_ready_cb1(const struct device* dev) {
    k_sem_give(&usb_sem1);
}

static const struct hid_ops ops0 = {
    .get_report = get_report_cb,
    .set_report = set_report_cb,
    .int_in_ready = int_in_ready_cb0,
    .int_out_ready = int_out_ready_cb0,
};

static const struct hid_ops ops1 = {
    .get_report = get_report_cb,
    .set_report = set_report_cb,
    .int_in_ready = int_in_ready_cb1,
};

static bool do_send_report(uint8_t interface, const uint8_t* report_with_id, uint8_t len) {
    if (is_switch_pro_mode() && interface == 0 && len > 0 && report_with_id[0] == 0x30) {
        switch_pro_translate_report(report_with_id, len);
        switch_pro_current_input[1] = switch_pro_timer++;
        switch_pro_last_input_ms = k_uptime_get();
        bool sent = CHK(hid_int_ep_write(hid_dev0, switch_pro_current_input, sizeof(switch_pro_current_input), NULL));
        if (sent) {
            switch_pro_mapped_writes++;
        } else {
            switch_pro_mapped_write_fails++;
        }
        return sent;
    }

    if (report_with_id[0] == 0) {
        report_with_id++;
        len--;
    }
    if (interface == 0) {
        return CHK(hid_int_ep_write(hid_dev0, report_with_id, len, NULL));
    }
    if (interface == 1) {
        return CHK(hid_int_ep_write(hid_dev1, report_with_id, len, NULL));
    }
    return false;
}

static void button_init() {
    if (!device_is_ready(button.port)) {
        LOG_ERR("button device %s is not ready", button.port->name);
        return;
    }

    if (!CHK(gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_UP | GPIO_ACTIVE_LOW))) {
        return;
    }

    if (!CHK(gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH))) {
        return;
    }

    gpio_init_callback(&button_cb_data, button_cb, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
}

static void leds_init() {
    if (device_is_ready(led0.port)) {
        CHK(gpio_pin_configure_dt(&led0, GPIO_OUTPUT));
        gpio_pin_set_dt(&led0, false);
    } else {
        LOG_ERR("led0 device %s is not ready", led0.port->name);
    }

    if (device_is_ready(led1.port)) {
        CHK(gpio_pin_configure_dt(&led1, GPIO_OUTPUT));
        gpio_pin_set_dt(&led1, false);
    } else {
        LOG_ERR("led1 device %s is not ready", led1.port->name);
    }
}

static void status_cb(enum usb_dc_status_code status, const uint8_t* param) {
    if (status == USB_DC_SOF) {
        atomic_set_bit(tick_pending, 0);
    } else if (status == USB_DC_RESET || status == USB_DC_CONFIGURED || status == USB_DC_SUSPEND) {
        if (is_switch_pro_mode()) {
            switch_pro_reset_session();
        }
    }
}

extern struct usb_device_descriptor __usb_descriptor_start[];

static void descriptor_init() {
    our_descriptor = &our_descriptors[our_descriptor_number];
    if ((our_descriptor->vid != 0) && (our_descriptor->pid != 0)) {
        struct usb_device_descriptor* device_descriptor = __usb_descriptor_start;
        device_descriptor->idVendor = our_descriptor->vid;
        device_descriptor->idProduct = our_descriptor->pid;
    }
}

static void usb_init() {
    hid_dev0 = device_get_binding("HID_0");
    if (hid_dev0 == NULL) {
        LOG_ERR("Cannot get USB HID Device 0.");
        return;
    }

    hid_dev1 = device_get_binding("HID_1");
    if (hid_dev1 == NULL) {
        LOG_ERR("Cannot get USB HID Device 1.");
        return;
    }

    usb_hid_register_device(hid_dev0, our_descriptor->descriptor, our_descriptor->descriptor_length, &ops0);
    usb_hid_register_device(hid_dev1, config_report_descriptor, config_report_descriptor_length, &ops1);
    CHK(usb_hid_init(hid_dev0));
    CHK(usb_hid_init(hid_dev1));
    CHK(usb_enable(status_cb));
}

static void bt_init() {
    for (int i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        bt_hogp_init(&hogps[i], &hogp_init_params);
    }

    if (!CHK(bt_conn_auth_cb_register(&conn_auth_callbacks))) {
        return;
    }

    if (!CHK(bt_conn_auth_info_cb_register(&conn_auth_info_callbacks))) {
        return;
    }

    CHK(bt_enable(NULL));
}

static int remapper_settings_set(const char* name, size_t len, settings_read_cb read_cb, void* cb_arg) {
    LOG_INF("%s len=%d", name, len);

    if (!strcmp(name, "switch_pro_diag")) {
        if (len != sizeof(switch_pro_saved_diag)) {
            return -EINVAL;
        }

        int bytes_read = read_cb(cb_arg, switch_pro_saved_diag, len);
        if (bytes_read < 0) {
            return bytes_read;
        }
        return bytes_read == sizeof(switch_pro_saved_diag) ? 0 : -EINVAL;
    }

    if (strcmp(name, "config")) {
        return -ENOENT;
    }

    static uint8_t buffer[PERSISTED_CONFIG_SIZE];

    if (len != PERSISTED_CONFIG_SIZE) {
        return -EINVAL;
    }

    int bytes_read = read_cb(cb_arg, buffer, len);

    if (bytes_read < 0) {
        return bytes_read;
    }

    if (bytes_read != PERSISTED_CONFIG_SIZE) {
        return -EINVAL;
    }

    //    LOG_HEXDUMP_DBG(buffer, len, "");

    load_config(buffer);

    return 0;
}

static struct settings_handler our_settings_handlers = {
    .name = "remapper",
    .h_set = remapper_settings_set,
};

void do_persist_config(uint8_t* buffer) {
    LOG_INF("");
    CHK(settings_save_one("remapper/config", buffer, PERSISTED_CONFIG_SIZE));
}

// https://github.com/adafruit/Adafruit_nRF52_Bootloader/blob/master/src/main.c#L116
const int DFU_MAGIC_UF2_RESET = 0x57;

void reset_to_bootloader() {
    sys_reboot(DFU_MAGIC_UF2_RESET);
}

void flash_b_side() {
}

void get_switch_pro_diagnostics(uint32_t page, uint32_t values[7]) {
    if (page < SWITCH_PRO_DIAG_PAGES) {
        switch_pro_fill_diagnostics(page, values);
        return;
    }

    page -= SWITCH_PRO_DIAG_PAGES;
    if (page < SWITCH_PRO_DIAG_PAGES) {
        memcpy(values, switch_pro_saved_diag[page], SWITCH_PRO_DIAG_VALUES * sizeof(uint32_t));
        return;
    }

    memset(values, 0, SWITCH_PRO_DIAG_VALUES * sizeof(uint32_t));
}

void pair_new_device() {
    peers_only = false;
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

void clear_bonds() {
    k_work_submit(&clear_bonds_work);
}

void my_mutexes_init() {
    for (int i = 0; i < (int8_t) MutexId::N; i++) {
        k_mutex_init(&mutexes[i]);
    }
    k_mutex_init(&get_report_mutex);
}

void my_mutex_enter(MutexId id) {
    k_mutex_lock(&mutexes[(uint8_t) id], K_FOREVER);
}

void my_mutex_exit(MutexId id) {
    k_mutex_unlock(&mutexes[(uint8_t) id]);
}

uint64_t get_time() {
    return k_uptime_get() * 1000;  // XXX precision?
}

void interval_override_updated() {
}

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    // TODO
}

void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    // TODO
}

void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len) {
    // TODO
}

void set_gpio_inout_masks(uint32_t in_mask, uint32_t out_mask) {
}

int main() {
    LOG_INF("HID Remapper Bluetooth");

    my_mutexes_init();
    button_init();
    leds_init();
    bt_init();
    CHK(settings_subsys_init());
    CHK(settings_register(&our_settings_handlers));
    settings_load();
    descriptor_init();
    if (is_switch_pro_mode()) {
        switch_pro_reset_session();
    }
    usb_init();
    scan_init();
    parse_our_descriptor();
    set_mapping_from_config();

    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));

    struct report_type incoming_report;
    struct descriptor_type incoming_descriptor;
    struct disconnected_type disconnected_item;
    static struct set_report_type set_report_item;
    static uint8_t get_report_tmp_buf[64];
    bool process_pending = false;
    bool get_report_response_pending = false;

    while (true) {
        if (!process_pending) {
            uint8_t reports_to_drain = is_switch_pro_mode() ? 6 : 1;
            while (reports_to_drain-- && !k_msgq_get(&report_q, &incoming_report, K_NO_WAIT)) {
                if (incoming_report.interface == OUR_OUT_INTERFACE && switch_pro_handle_output_report(incoming_report.data, incoming_report.len)) {
                    // Switch Pro output reports are host commands, not remapper inputs.
                } else {
                    handle_received_report(incoming_report.data, incoming_report.len, (uint16_t) incoming_report.interface);
                }
                process_pending = true;
            }
        }
        if (atomic_test_and_clear_bit(tick_pending, 0)) {
            process_mapping(true);
            process_pending = false;
            switch_pro_log_stats();
        }
        if (!k_sem_take(&usb_sem0, K_NO_WAIT)) {
            if (!switch_pro_send_response() && !send_report(do_send_report) && !switch_pro_send_input_heartbeat()) {
                k_sem_give(&usb_sem0);
            }
        }
        if (!k_sem_take(&usb_sem1, K_NO_WAIT)) {
            if (!send_monitor_report(do_send_report)) {
                k_sem_give(&usb_sem1);
            }
        }

        if (!k_msgq_get(&set_report_q, &set_report_item, K_NO_WAIT)) {
            if (set_report_item.interface == 0) {
                uint8_t switch_report[65];
                switch_report[0] = set_report_item.report_id;
                memcpy(switch_report + 1, set_report_item.data, set_report_item.len);
                if (is_switch_pro_mode()) {
                    switch_pro_set_reports++;
                }
                if (!switch_pro_handle_output_report(switch_report, set_report_item.len + 1)) {
                    handle_set_report0(set_report_item.report_id, set_report_item.data, set_report_item.len);
                }
            }
            if (set_report_item.interface == 1) {
                handle_set_report1(set_report_item.report_id, set_report_item.data, set_report_item.len);
                get_report_response_pending = true;
            }
        }
        if (get_report_response_pending) {
            get_report_response_pending = false;
            uint16_t ret = handle_get_report1(REPORT_ID_CONFIG, get_report_tmp_buf, sizeof(get_report_tmp_buf));
            if (ret > 0) {
                k_mutex_lock(&get_report_mutex, K_FOREVER);
                get_report_response_ready = true;
                memcpy(get_report_buf, get_report_tmp_buf, sizeof(get_report_buf));
                k_mutex_unlock(&get_report_mutex);
            }
        }

        while (!k_msgq_get(&disconnected_q, &disconnected_item, K_NO_WAIT)) {
            LOG_INF("device_disconnected_callback conn_idx=%d", disconnected_item.conn_idx);
            device_disconnected_callback(disconnected_item.conn_idx);
        }

        while (!k_msgq_get(&descriptor_q, &incoming_descriptor, K_NO_WAIT)) {
            LOG_HEXDUMP_DBG(incoming_descriptor.data, incoming_descriptor.size, "incoming_descriptor");
            parse_descriptor(1, 1, incoming_descriptor.data, incoming_descriptor.size, incoming_descriptor.conn_idx << 8, 0);
        }

        if (resume_pending) {
            resume_pending = false;
            suspended = false;
        }
        if (config_updated) {
            set_mapping_from_config();
            config_updated = false;
        }

        if (their_descriptor_updated) {
            update_their_descriptor_derivates();
            their_descriptor_updated = false;
        }

        if (need_to_persist_config) {
            int64_t t0 = k_uptime_get();
            persist_config_return_code = persist_config();
            LOG_INF("persist_config took %lld ms\n", k_uptime_get() - t0);
            need_to_persist_config = false;
            get_report_response_pending = true;
        }

        // without this sleep, some devices won't pair; some thread priority issue?
        k_sleep(K_USEC(1));  // XXX
    }

    return 0;
}
