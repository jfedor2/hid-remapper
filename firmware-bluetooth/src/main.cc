#include <errno.h>
#include <stddef.h>

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
static auto const BT_UUID_HIDS_REPORT_ = (struct bt_uuid_16) BT_UUID_INIT_16(BT_UUID_HIDS_REPORT_VAL);
static auto const BT_UUID_HIDS_REPORT_REF_ = (struct bt_uuid_16) BT_UUID_INIT_16(BT_UUID_HIDS_REPORT_REF_VAL);
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

static void activity_led_off_work_fn(struct k_work* work) {
    gpio_pin_set_dt(&led0, false);
}
static K_WORK_DELAYABLE_DEFINE(activity_led_off_work, activity_led_off_work_fn);

// When connection setup fails, the status LED shows a blink code in place of the
// normal status: slow blinks count the last step that passed, then fast blinks
// count the error code of the step that failed. With REMAPPER_DEBUG_STAGES the
// LED always shows the current step, even if nothing has gone wrong.
enum DebugStage {
    DEBUG_SCANNING = 1,
    DEBUG_CONNECTED = 2,
    DEBUG_ENCRYPTED = 3,
    DEBUG_SERVICES_FOUND = 4,
    DEBUG_HID_READY = 5,
    DEBUG_DESCRIPTOR_READ = 6,
    DEBUG_REPORT_RECEIVED = 7,
};

static const int DEBUG_MAX_ERROR_BLINKS = 30;

static atomic_t debug_stage = ATOMIC_INIT(DEBUG_SCANNING);
static atomic_t debug_error = ATOMIC_INIT(0);

static void led_work_fn(struct k_work* work);
static K_WORK_DELAYABLE_DEFINE(led_work, led_work_fn);

static bool debug_code_active() {
#ifdef REMAPPER_DEBUG_STAGES
    return true;
#else
    return atomic_get(&debug_error) != 0;
#endif
}

static void debug_stage_set(int stage) {
    atomic_set(&debug_stage, stage);
    atomic_set(&debug_error, 0);
}

static void debug_stage_fail(int stage, int err) {
    err = (err < 0) ? -err : err;
    atomic_set(&debug_stage, stage);
    atomic_set(&debug_error, MAX(1, MIN(err, DEBUG_MAX_ERROR_BLINKS)));
    k_work_reschedule(&led_work, K_NO_WAIT);
}

// a report from one device shouldn't hide an error from another one
static void debug_stage_set_if_no_error(int stage) {
    if (atomic_get(&debug_error) == 0) {
        atomic_set(&debug_stage, stage);
    }
}

enum class LedMode {
    OFF = 0,
    ON = 1,
    BLINK = 2,
};

static atomic_t led_blink_count = ATOMIC_INIT(0);
static atomic_t led_mode = (atomic_t) ATOMIC_INIT(LedMode::OFF);
static int led_blinks_left = 0;
static bool next_blink_state = true;

// Blinks out the current code. Returns false if there's no code to show.
static bool debug_code_work() {
    static int slow_left = 0;
    static int fast_left = 0;
    static bool lit = false;
    static bool last_blink_slow = false;
    int next_ms;

    if (lit) {
        gpio_pin_set_dt(&led1, false);
        lit = false;
        if (slow_left > 0) {
            next_ms = 300;
        } else if (fast_left > 0) {
            next_ms = last_blink_slow ? 1000 : 120;
        } else {
            next_ms = 2500;
        }
    } else {
        if ((slow_left == 0) && (fast_left == 0)) {
            if (!debug_code_active()) {
                return false;
            }
            slow_left = atomic_get(&debug_stage);
            fast_left = atomic_get(&debug_error);
        }
        last_blink_slow = slow_left > 0;
        if (last_blink_slow) {
            slow_left--;
            next_ms = 400;
        } else {
            fast_left--;
            next_ms = 100;
        }
        gpio_pin_set_dt(&led1, true);
        lit = true;
    }

    k_work_reschedule(&led_work, K_MSEC(next_ms));
    return true;
}

static void led_work_fn(struct k_work* work) {
    if (debug_code_work()) {
        return;
    }

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
        debug_stage_set(DEBUG_SCANNING);
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

static void discovery_finish(struct bt_gatt_dm* dm, struct bt_hogp* hogp) {
    int assign_err = bt_hogp_handles_assign(dm, hogp);
    if (CHK(assign_err)) {  // XXX disconnect if this fails?
        debug_stage_set(DEBUG_SERVICES_FOUND);
    } else {
        debug_stage_fail(DEBUG_ENCRYPTED, assign_err);
    }
    CHK(bt_gatt_dm_data_release(dm));
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

// The HOGP library guesses each report's type from its characteristic
// properties, then rejects the whole device if the Report Reference descriptor
// says otherwise. Some devices have reports with unusual properties (for example
// an output report that lacks Write Without Response), so we read the Report
// References first and make the discovered properties match them.
static struct {
    struct bt_gatt_dm* dm;
    struct bt_hogp* hogp;
    const struct bt_gatt_dm_attr* chrc;
    struct bt_gatt_read_params read_params;
} report_type_fix;

static uint8_t report_ref_read_cb(struct bt_conn* conn, uint8_t err, struct bt_gatt_read_params* params, const void* data, uint16_t length);

static void report_type_fix_next() {
    while (NULL != (report_type_fix.chrc = bt_gatt_dm_char_next(report_type_fix.dm, report_type_fix.chrc))) {
        if (bt_uuid_cmp(bt_gatt_dm_attr_chrc_val(report_type_fix.chrc)->uuid, (struct bt_uuid*) &BT_UUID_HIDS_REPORT_)) {
            continue;
        }

        const struct bt_gatt_dm_attr* ref = bt_gatt_dm_desc_by_uuid(report_type_fix.dm, report_type_fix.chrc, (struct bt_uuid*) &BT_UUID_HIDS_REPORT_REF_);
        if (ref == NULL) {
            continue;
        }

        report_type_fix.read_params.func = report_ref_read_cb;
        report_type_fix.read_params.handle_count = 1;
        report_type_fix.read_params.single.handle = ref->handle;
        report_type_fix.read_params.single.offset = 0;
        if (CHK(bt_gatt_read(bt_gatt_dm_conn_get(report_type_fix.dm), &report_type_fix.read_params))) {
            return;
        }
        break;
    }

    discovery_finish(report_type_fix.dm, report_type_fix.hogp);
}

static uint8_t report_ref_read_cb(struct bt_conn* conn, uint8_t err, struct bt_gatt_read_params* params, const void* data, uint16_t length) {
    if (err || (data == NULL) || (length != 2)) {
        LOG_WRN("can't read report reference (err=%u, length=%u)", err, length);
    } else {
        struct bt_gatt_chrc* chrc_val = bt_gatt_dm_attr_chrc_val(report_type_fix.chrc);
        uint8_t original_properties = chrc_val->properties;

        switch (((const uint8_t*) data)[1]) {
            case BT_HIDS_REPORT_TYPE_INPUT:
                chrc_val->properties |= BT_GATT_CHRC_NOTIFY;
                chrc_val->properties &= ~BT_GATT_CHRC_WRITE_WITHOUT_RESP;
                break;
            case BT_HIDS_REPORT_TYPE_OUTPUT:
                chrc_val->properties |= BT_GATT_CHRC_WRITE_WITHOUT_RESP;
                chrc_val->properties &= ~BT_GATT_CHRC_NOTIFY;
                break;
            case BT_HIDS_REPORT_TYPE_FEATURE:
                chrc_val->properties &= ~(BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE_WITHOUT_RESP);
                break;
            default:
                break;
        }

        if (chrc_val->properties != original_properties) {
            LOG_INF("report 0x%04x properties 0x%02x -> 0x%02x", report_type_fix.chrc->handle, original_properties, chrc_val->properties);
        }
    }

    report_type_fix_next();

    return BT_GATT_ITER_STOP;
}

static void discovery_completed_cb(struct bt_gatt_dm* dm, void* context) {
    LOG_INF("");
    patch_broken_uuids(dm);
    report_type_fix.dm = dm;
    report_type_fix.hogp = (struct bt_hogp*) context;
    report_type_fix.chrc = NULL;
    report_type_fix_next();
}

static void discovery_service_not_found_cb(struct bt_conn* conn, void* context) {
    LOG_WRN("");
    debug_stage_fail(DEBUG_ENCRYPTED, ENOENT);
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static void discovery_error_found_cb(struct bt_conn* conn, int err, void* context) {
    LOG_ERR("err=%d", err);
    debug_stage_fail(DEBUG_ENCRYPTED, err);
    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
}

static const struct bt_gatt_dm_cb discovery_cb = {
    .completed = discovery_completed_cb,
    .service_not_found = discovery_service_not_found_cb,
    .error_found = discovery_error_found_cb,
};

static void gatt_discover(struct bt_conn* conn) {
    uint8_t conn_idx = bt_conn_index(conn);
    int dm_err = bt_gatt_dm_start(conn, (struct bt_uuid*) &BT_UUID_HIDS_, &discovery_cb, &hogps[conn_idx]);
    if (!CHK(dm_err)) {
        debug_stage_fail(DEBUG_ENCRYPTED, dm_err);
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
    count_connections();
    set_led_mode(LedMode::BLINK);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (conn_err) {
        LOG_ERR("Failed to connect to %s (conn_err=%u).", addr, conn_err);
        debug_stage_fail(DEBUG_SCANNING, conn_err);
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));

        return;
    }

    LOG_INF("%s", addr);
    debug_stage_set(DEBUG_CONNECTED);

    int security_err = bt_conn_set_security(conn, BT_SECURITY_L2);
    if (!CHK(security_err)) {
        debug_stage_fail(DEBUG_CONNECTED, security_err);
    }
}

static void disconnected(struct bt_conn* conn, uint8_t reason) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("%s (reason=%u)", addr, reason);

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
        debug_stage_set(DEBUG_ENCRYPTED);
        gatt_discover(conn);
    } else {
        LOG_ERR("security failed: %s, level=%u, err=%d", addr, level, err);
        debug_stage_fail(DEBUG_CONNECTED, err);
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

    debug_stage_set_if_no_error(DEBUG_REPORT_RECEIVED);

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
        //        printk("error in k_msg_put(report_q\n");
    }

    return BT_GATT_ITER_CONTINUE;
}

// XXX is this ready for simultaneous connection setup? is discovery ready? do we care?
static struct descriptor_type their_descriptor;

static void hogp_map_read_cb(struct bt_hogp* hogp, uint8_t err, const uint8_t* data, size_t size, size_t offset) {
    if (data == NULL) {
        if (err) {
            debug_stage_fail(DEBUG_HID_READY, err);
        }
        their_descriptor.size = offset;
        their_descriptor.conn_idx = hogp_index(hogp);
        CHK(k_msgq_put(&descriptor_q, &their_descriptor, K_NO_WAIT));
        return;
    }

    if (offset + size > sizeof(their_descriptor.data)) {
        LOG_ERR("report descriptor too long");
        debug_stage_fail(DEBUG_HID_READY, DEBUG_MAX_ERROR_BLINKS);
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

        struct find_bond_t find_bond = {
            .i = 0,
            .found_idx = 0,
        };
        bt_addr_le_copy(&find_bond.addr, bt_conn_get_dst(bt_hogp_conn(item.hogp)));
        bt_foreach_bond(BT_ID_DEFAULT, find_bond_cb, &find_bond);
        LOG_DBG("found bond idx: %d", find_bond.found_idx);
        device_connected_callback(bt_conn_index(bt_hogp_conn(item.hogp)) << 8, 1, 1, find_bond.found_idx);
        debug_stage_set(DEBUG_HID_READY);

        while (NULL != (rep = bt_hogp_rep_next(item.hogp, rep))) {
            if (bt_hogp_rep_type(rep) == BT_HIDS_REPORT_TYPE_INPUT) {
                LOG_DBG("subscribing to report ID: %u", bt_hogp_rep_id(rep));
                int subscribe_err = bt_hogp_rep_subscribe(item.hogp, rep, hogp_notify_cb);
                if (!CHK(subscribe_err)) {
                    debug_stage_fail(DEBUG_HID_READY, subscribe_err);
                }
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
    debug_stage_fail(DEBUG_SERVICES_FOUND, err);
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
        buf.interface = OUR_OUT_INTERFACE;
        buf.len = len;
        CHK(k_msgq_put(&report_q, &buf, K_NO_WAIT));
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
    LOG_INF("len=%d", len);

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
    usb_init();
    scan_init();
    parse_our_descriptor();
    set_mapping_from_config();

    k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
#ifdef REMAPPER_DEBUG_STAGES
    k_work_reschedule(&led_work, K_NO_WAIT);
#endif

    struct report_type incoming_report;
    struct descriptor_type incoming_descriptor;
    struct disconnected_type disconnected_item;
    static struct set_report_type set_report_item;
    static uint8_t get_report_tmp_buf[64];
    bool process_pending = false;
    bool get_report_response_pending = false;

    while (true) {
        if (!process_pending && !k_msgq_get(&report_q, &incoming_report, K_NO_WAIT)) {
            handle_received_report(incoming_report.data, incoming_report.len, (uint16_t) incoming_report.interface);
            process_pending = true;
        }
        if (atomic_test_and_clear_bit(tick_pending, 0)) {
            process_mapping(true);
            process_pending = false;
        }
        if (!k_sem_take(&usb_sem0, K_NO_WAIT)) {
            if (!send_report(do_send_report)) {
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
                handle_set_report0(set_report_item.report_id, set_report_item.data, set_report_item.len);
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
            if (atomic_get(&debug_stage) == DEBUG_HID_READY) {
                debug_stage_set_if_no_error(DEBUG_DESCRIPTOR_READ);
            }
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
