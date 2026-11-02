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
#include <zephyr/drivers/retained_mem.h>
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/types.h>
#include <zephyr/usb/class/usbd_hid.h>
#include <zephyr/usb/usbd.h>

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

static const struct device* hid_dev0 = DEVICE_DT_GET(DT_NODELABEL(hid_dev_0));
static const struct device* hid_dev1 = DEVICE_DT_GET(DT_NODELABEL(hid_dev_1));  // config interface

// usbd_hid names its classes after devicetree instance numbers.
BUILD_ASSERT(DT_SAME_NODE(DT_INST(0, zephyr_hid_device), DT_NODELABEL(hid_dev_0)));
BUILD_ASSERT(DT_SAME_NODE(DT_INST(1, zephyr_hid_device), DT_NODELABEL(hid_dev_1)));

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
    count_connections();
    set_led_mode(LedMode::BLINK);

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (conn_err) {
        LOG_ERR("Failed to connect to %s (conn_err=%u).", addr, conn_err);
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));

        return;
    }

    LOG_INF("%s", addr);

    CHK(bt_conn_set_security(conn, BT_SECURITY_L2));
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
        //        printk("error in k_msg_put(report_q\n");
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

static int set_report_cb(const struct device* dev, const uint8_t type, const uint8_t id, const uint16_t len, const uint8_t* const data) {
    LOG_INF("report_id=%d, report_type=%d, len=%d", id, type, len);
    LOG_HEXDUMP_DBG(data, len, "");

    // If the report has an ID, it's also the first byte of the data.
    struct set_report_type buf;
    if ((id > 0) && (len > 0)) {
        if ((dev == hid_dev0) && set_report0_synchronous(id)) {
            handle_set_report0(id, data + 1, len - 1);
        } else {
            if (dev == hid_dev0) {
                buf.interface = 0;
            } else if (dev == hid_dev1) {
                buf.interface = 1;
                k_mutex_lock(&get_report_mutex, K_FOREVER);
                get_report_response_ready = false;
                k_mutex_unlock(&get_report_mutex);
            }
            buf.report_id = id;
            buf.len = MIN(len - 1, sizeof(buf.data));
            memcpy(buf.data, data + 1, buf.len);
            CHK(k_msgq_put(&set_report_q, &buf, K_NO_WAIT));
        }
    } else {
        LOG_ERR("no report ID?");
    }

    return 0;
}

static int get_report_cb(const struct device* dev, const uint8_t type, const uint8_t id, const uint16_t len, uint8_t* const data) {
    LOG_INF("report_id=%d, report_type=%d, len=%d", id, type, len);

    // data is only as big as the host asked for
    uint8_t buf[CONFIG_SIZE + 1];
    uint16_t buf_len = 0;

    buf[0] = id;
    if (dev == hid_dev0) {
        buf_len = handle_get_report0(id, buf + 1, CONFIG_SIZE);
    } else if (dev == hid_dev1) {
        k_mutex_lock(&get_report_mutex, K_FOREVER);
        if (get_report_response_ready) {
            memcpy(buf + 1, get_report_buf, CONFIG_SIZE);
            buf_len = CONFIG_SIZE;
        } else {
            LOG_INF("response not ready");
            buf_len = 0;
        }
        get_report_response_ready = false;
        k_mutex_unlock(&get_report_mutex);
    }
    buf_len++;

    buf_len = MIN(buf_len, len);
    memcpy(data, buf, buf_len);

    return buf_len;
}

static void output_report_cb0(const struct device* dev, const uint16_t len, const uint8_t* const data) {
    static struct report_type buf;
    buf.interface = OUR_OUT_INTERFACE;
    buf.len = MIN(len, sizeof(buf.data));
    memcpy(buf.data, data, buf.len);
    CHK(k_msgq_put(&report_q, &buf, K_NO_WAIT));
}

static void input_report_done_cb0(const struct device* dev, const uint8_t* const report) {
    k_sem_give(&usb_sem0);
}

static void input_report_done_cb1(const struct device* dev, const uint8_t* const report) {
    k_sem_give(&usb_sem1);
}

static void sof_cb0(const struct device* dev);

static const struct hid_device_ops ops0 = {
    .get_report = get_report_cb,
    .set_report = set_report_cb,
    .input_report_done = input_report_done_cb0,
    .output_report = output_report_cb0,
    .sof = sof_cb0,
};

static const struct hid_device_ops ops1 = {
    .get_report = get_report_cb,
    .set_report = set_report_cb,
    .input_report_done = input_report_done_cb1,
};

// Reports are sent straight from these buffers, they have to stay untouched until
// input_report_done is called. The semaphores make sure there's only one report
// in flight per interface.
UDC_STATIC_BUF_DEFINE(report_buf0, 64);
UDC_STATIC_BUF_DEFINE(report_buf1, 64);

static bool do_send_report(uint8_t interface, const uint8_t* report_with_id, uint8_t len) {
    if (report_with_id[0] == 0) {
        report_with_id++;
        len--;
    }
    if (interface == 0) {
        memcpy(report_buf0, report_with_id, MIN(len, sizeof(report_buf0)));
        return CHK(hid_device_submit_report(hid_dev0, MIN(len, sizeof(report_buf0)), report_buf0));
    }
    if (interface == 1) {
        memcpy(report_buf1, report_with_id, MIN(len, sizeof(report_buf1)));
        return CHK(hid_device_submit_report(hid_dev1, MIN(len, sizeof(report_buf1)), report_buf1));
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

#if defined(CONFIG_SOC_NRF52840)
#define USB_MANUFACTURER "nRF52840"
#elif defined(CONFIG_SOC_NRF54LM20A)
#define USB_MANUFACTURER "nRF54LM20A"
#else
#define USB_MANUFACTURER CONFIG_SOC
#endif

USBD_DEVICE_DEFINE(usbd, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 0xCAFE, 0xBAF2);

USBD_DESC_LANG_DEFINE(desc_lang);
USBD_DESC_MANUFACTURER_DEFINE(desc_manufacturer, USB_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(desc_product, "HID Remapper Bluetooth");
USBD_DESC_SERIAL_NUMBER_DEFINE(desc_serial_number);

USBD_CONFIGURATION_DEFINE(fs_config, 0, 50, NULL);  // 50*2 mA = 100 mA
USBD_CONFIGURATION_DEFINE(hs_config, 0, 50, NULL);

static void sof_cb0(const struct device* dev) {
    // On high speed there's a SOF every 125 us microframe, we want a 1 ms tick.
    static uint8_t microframe = 0;
    if (USBD_SUPPORTS_HIGH_SPEED && (usbd_bus_speed(&usbd) == USBD_SPEED_HS) && (++microframe % 8 != 0)) {
        return;
    }
    atomic_set_bit(tick_pending, 0);
}

static void usbd_msg_cb(struct usbd_context* const ctx, const struct usbd_msg* msg) {
    switch (msg->type) {
        case USBD_MSG_VBUS_READY:
            LOG_INF("VBUS ready");
            CHK(usbd_enable(ctx));
            break;
        case USBD_MSG_VBUS_REMOVED:
            LOG_INF("VBUS removed");
            CHK(usbd_disable(ctx));
            break;
        default:
            break;
    }
}

static void descriptor_init() {
    our_descriptor = &our_descriptors[our_descriptor_number];
    if ((our_descriptor->vid != 0) && (our_descriptor->pid != 0)) {
        CHK(usbd_device_set_vid(&usbd, our_descriptor->vid));
        CHK(usbd_device_set_pid(&usbd, our_descriptor->pid));
    }
}

static bool add_configuration(enum usbd_speed speed, struct usbd_config_node* config) {
    return CHK(usbd_add_configuration(&usbd, speed, config)) &&
           CHK(usbd_register_class(&usbd, "hid_0", speed, 1)) &&
           CHK(usbd_register_class(&usbd, "hid_1", speed, 1)) &&
           CHK(usbd_device_set_code_triple(&usbd, speed, 0, 0, 0));
}

static void usb_init() {
    if (!device_is_ready(hid_dev0) || !device_is_ready(hid_dev1)) {
        LOG_ERR("HID devices not ready");
        return;
    }

    if (!CHK(hid_device_register(hid_dev0, our_descriptor->descriptor, our_descriptor->descriptor_length, &ops0)) ||
        !CHK(hid_device_register(hid_dev1, config_report_descriptor, config_report_descriptor_length, &ops1))) {
        return;
    }

    if (!CHK(usbd_add_descriptor(&usbd, &desc_lang)) ||
        !CHK(usbd_add_descriptor(&usbd, &desc_manufacturer)) ||
        !CHK(usbd_add_descriptor(&usbd, &desc_product)) ||
        !CHK(usbd_add_descriptor(&usbd, &desc_serial_number))) {
        return;
    }

    if (USBD_SUPPORTS_HIGH_SPEED && (usbd_caps_speed(&usbd) == USBD_SPEED_HS)) {
        if (!add_configuration(USBD_SPEED_HS, &hs_config)) {
            return;
        }
    }

    if (!add_configuration(USBD_SPEED_FS, &fs_config)) {
        return;
    }

    usbd_self_powered(&usbd, false);

    if (!CHK(usbd_msg_register_cb(&usbd, usbd_msg_cb))) {
        return;
    }

    if (!CHK(usbd_init(&usbd))) {
        return;
    }

    if (!usbd_can_detect_vbus(&usbd)) {
        CHK(usbd_enable(&usbd));
    }
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

void reset_to_bootloader() {
#if defined(CONFIG_BUILD_OUTPUT_UF2) && defined(CONFIG_RETAINED_MEM_NRF_GPREGRET)
    const struct device* gpregret_dev = DEVICE_DT_GET(DT_NODELABEL(gpregret1));

    // https://github.com/adafruit/Adafruit_nRF52_Bootloader/blob/master/src/main.c#L116
    const uint8_t DFU_MAGIC_UF2_RESET = 0x57;

    // The bootloader sees the magic value after the reboot and enters UF2 mode.
    if (CHK(retained_mem_write(gpregret_dev, 0, &DFU_MAGIC_UF2_RESET, 1))) {
        sys_reboot(SYS_REBOOT_WARM);
    }
#else
    LOG_WRN("no UF2 bootloader on this board");
#endif
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
