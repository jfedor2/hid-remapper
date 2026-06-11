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
#include <zephyr/devicetree.h>
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
#include "imu.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"
#include "crc.h"

LOG_MODULE_REGISTER(remapper, LOG_LEVEL_DBG);

#define CHK(X) ({ int err = X; if (err != 0) { LOG_ERR("%s returned %d (%s:%d)", #X, err, __FILE__, __LINE__); } err == 0; })

static const int SCAN_DELAY_MS = 1000;
static const int CLEAR_BONDS_BUTTON_PRESS_MS = 3000;
static const uint16_t NUS_VIRTUAL_INTERFACE = 0x7f00;
static const uint16_t NUS_VIRTUAL_VID = 0x0f0d;
static const uint16_t NUS_VIRTUAL_PID = 0x00c1;

static struct bt_uuid_128 nus_service_uuid = BT_UUID_INIT_128(
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

static struct bt_uuid_128 nus_rx_uuid = BT_UUID_INIT_128(
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);

static struct bt_uuid_128 nus_tx_uuid = BT_UUID_INIT_128(
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

#define NUS_PROTOCOL_VERSION 1
#define NUS_PACKET_BUFFER_SIZE 512
#define END 0300
#define ESC 0333
#define ESC_END 0334
#define ESC_ESC 0335

struct __attribute__((packed)) nus_packet_t {
    uint8_t protocol_version;
    uint8_t our_descriptor_number;
    uint8_t len;
    uint8_t report_id;
    uint8_t data[0];
};

static uint8_t nus_packet_buffer[NUS_PACKET_BUFFER_SIZE];
static uint16_t nus_bytes_read = 0;
static bool nus_escaped = false;
static bool nus_overflowed = false;
static struct bt_conn* nus_conn;

#define NUS_LATENCY_INSTRUMENTATION 1
#if NUS_LATENCY_INSTRUMENTATION
static uint32_t nus_rx_cycles = 0;
static bool nus_latency_pending = false;
static uint32_t nus_latency_samples = 0;
#endif

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

// Forward declarations
static bool do_send_report(uint8_t interface, const uint8_t* report_with_id, uint8_t len);

struct report_type {
    uint16_t interface;
    uint8_t external_report_id;
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

static void nus_start_advertising(void);
static void nus_init_virtual_device(void);
static void nus_process_bytes(const uint8_t* data, uint16_t len);

static ssize_t nus_rx_write_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                               const void* buf, uint16_t len, uint16_t offset, uint8_t flags) {
    if (conn == nus_conn) {
        nus_process_bytes((const uint8_t*) buf, len);
    }
    return len;
}

static void nus_ccc_changed(const struct bt_gatt_attr* attr, uint16_t value) {
    LOG_DBG("NUS CCC changed: %d", value);
}

BT_GATT_SERVICE_DEFINE(nus_service,
    BT_GATT_PRIMARY_SERVICE(&nus_service_uuid),
    BT_GATT_CHARACTERISTIC(&nus_rx_uuid.uuid,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE_ENCRYPT, NULL, nus_rx_write_cb, NULL),
    BT_GATT_CHARACTERISTIC(&nus_tx_uuid.uuid,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(nus_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

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

const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static bool scanning = false;
static bool peers_only = true;

static struct bt_le_conn_param* conn_param = BT_LE_CONN_PARAM(6, 6, 44, 400);

static void activity_led_off_work_fn(struct k_work* work) {
    gpio_pin_set_dt(&led0, false);
}
K_WORK_DELAYABLE_DEFINE(activity_led_off_work, activity_led_off_work_fn);

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

static const uint8_t nus_virtual_gamepad_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Game Pad)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x01,        //   Physical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x0E,        //   Report Count (14)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (1)
    0x29, 0x0E,        //   Usage Maximum (14)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x01,        //   Input (Const)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x25, 0x0F,        //   Logical Maximum (15)
    0x46, 0x3B, 0x01,  //   Physical Maximum (315)
    0x75, 0x04,        //   Report Size (4)
    0x95, 0x01,        //   Report Count (1)
    0x65, 0x14,        //   Unit (English Rotation)
    0x09, 0x39,        //   Usage (Hat switch)
    0x81, 0x42,        //   Input (Data,Var,Abs,Null State)
    0x65, 0x00,        //   Unit (None)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x01,        //   Input (Const)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x09, 0x32,        //   Usage (Z)
    0x09, 0x35,        //   Usage (Rz)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x04,        //   Report Count (4)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x01,        //   Input (Const)
    0xC0,              // End Collection
};

static void nus_init_virtual_device(void) {
    parse_descriptor(NUS_VIRTUAL_VID, NUS_VIRTUAL_PID,
                     nus_virtual_gamepad_descriptor,
                     sizeof(nus_virtual_gamepad_descriptor),
                     NUS_VIRTUAL_INTERFACE, 0);
    device_connected_callback(NUS_VIRTUAL_INTERFACE, NUS_VIRTUAL_VID, NUS_VIRTUAL_PID, 0);
    their_descriptor_updated = true;
}

static void nus_handle_packet(const uint8_t* data, uint16_t len) {
    static uint8_t last_descriptor_warning = 0xff;

    if (len < sizeof(nus_packet_t)) {
        LOG_WRN("NUS packet too small: %d", len);
        return;
    }

    const nus_packet_t* msg = (const nus_packet_t*) data;
    uint16_t payload_len = len - sizeof(nus_packet_t);

    if ((msg->protocol_version != NUS_PROTOCOL_VERSION) ||
        (msg->len != payload_len) ||
        (payload_len > 64) ||
        (msg->our_descriptor_number >= NOUR_DESCRIPTORS) ||
        ((msg->report_id == 0) && (payload_len >= 64))) {
        LOG_WRN("Invalid NUS packet: proto=%d len=%d payload=%d desc=%d report_id=%d",
                msg->protocol_version, msg->len, payload_len,
                msg->our_descriptor_number, msg->report_id);
        return;
    }

    if ((msg->our_descriptor_number != our_descriptor_number) &&
        (msg->our_descriptor_number != last_descriptor_warning)) {
        last_descriptor_warning = msg->our_descriptor_number;
        LOG_WRN("NUS descriptor %d does not match active USB descriptor %d",
                msg->our_descriptor_number, our_descriptor_number);
    }

    struct report_type report = {
        .interface = NUS_VIRTUAL_INTERFACE,
        .external_report_id = msg->report_id,
        .len = (uint8_t) payload_len,
    };
    memcpy(report.data, msg->data, payload_len);
    if (k_msgq_put(&report_q, &report, K_NO_WAIT)) {
        LOG_WRN("Dropped NUS report: report queue full");
    } else {
#if NUS_LATENCY_INSTRUMENTATION
        nus_rx_cycles = k_cycle_get_32();
        nus_latency_pending = true;
#endif
    }
}

static void nus_packet_append(uint8_t c) {
    if (nus_bytes_read >= sizeof(nus_packet_buffer)) {
        if (!nus_overflowed) {
            LOG_WRN("NUS packet too large; dropping until frame end");
        }
        nus_overflowed = true;
        return;
    }
    nus_packet_buffer[nus_bytes_read++] = c;
}

static void nus_process_byte(uint8_t c) {
    if (nus_escaped) {
        switch (c) {
            case ESC_END:
                nus_packet_append(END);
                break;
            case ESC_ESC:
                nus_packet_append(ESC);
                break;
            default:
                nus_packet_append(c);
                break;
        }
        nus_escaped = false;
        return;
    }

    switch (c) {
        case END:
            if (!nus_overflowed && (nus_bytes_read > 4)) {
                uint32_t crc = crc32(nus_packet_buffer, nus_bytes_read - 4);
                uint32_t received_crc =
                    (nus_packet_buffer[nus_bytes_read - 4] << 0) |
                    (nus_packet_buffer[nus_bytes_read - 3] << 8) |
                    (nus_packet_buffer[nus_bytes_read - 2] << 16) |
                    (nus_packet_buffer[nus_bytes_read - 1] << 24);
                if (crc == received_crc) {
                    nus_handle_packet(nus_packet_buffer, nus_bytes_read - 4);
                } else {
                    LOG_WRN("NUS CRC error: expected 0x%08X, got 0x%08X", crc, received_crc);
                }
            }
            nus_bytes_read = 0;
            nus_escaped = false;
            nus_overflowed = false;
            break;
        case ESC:
            if (!nus_overflowed) {
                nus_escaped = true;
            }
            break;
        default:
            nus_packet_append(c);
            break;
    }
}

static void nus_process_bytes(const uint8_t* data, uint16_t len) {
    gpio_pin_set_dt(&led0, true);
    k_work_reschedule(&activity_led_off_work, K_MSEC(50));

    for (uint16_t i = 0; i < len; i++) {
        nus_process_byte(data[i]);
    }
}

static void nus_start_advertising(void) {
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .sid = 0,
        .secondary_max_skip = 0,
        .options = BT_LE_ADV_OPT_CONNECTABLE,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .peer = NULL,
    };

    static const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA_BYTES(BT_DATA_UUID128_ALL,
            0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
            0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e),
    };

    static const struct bt_data sd[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, "HID Remapper", sizeof("HID Remapper") - 1),
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err && err != -EALREADY) {
        LOG_ERR("bt_le_adv_start returned %d", err);
        return;
    }
    LOG_INF("NUS advertising started.");
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
    struct bt_conn_info info;

    if (bt_conn_get_info(conn, &info) || (info.role != BT_CONN_ROLE_CENTRAL)) {
        return;
    }

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

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (conn_err) {
        LOG_ERR("Failed to connect to %s (conn_err=%u).", addr, conn_err);
        k_work_reschedule(&scan_start_work, K_MSEC(SCAN_DELAY_MS));
        return;
    }

    struct bt_conn_info info;
    if (!CHK(bt_conn_get_info(conn, &info))) {
        return;
    }

    if (info.role == BT_CONN_ROLE_PERIPHERAL) {
        if (!nus_conn) {
            nus_conn = bt_conn_ref(conn);
            LOG_INF("NUS client connected: %s", addr);
            CHK(bt_conn_set_security(conn, BT_SECURITY_L2));
        } else {
            LOG_WRN("Rejecting extra NUS client: %s", addr);
            CHK(bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN));
        }
        return;
    }

    scanning = false;
    count_connections();
    set_led_mode(LedMode::BLINK);

    LOG_INF("%s", addr);

    CHK(bt_conn_set_security(conn, BT_SECURITY_L2));
}

static void disconnected(struct bt_conn* conn, uint8_t reason) {
    char addr[BT_ADDR_LE_STR_LEN];
    struct bt_conn_info info;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (conn == nus_conn) {
        LOG_INF("NUS client disconnected: %s (reason=%u)", addr, reason);
        bt_conn_unref(nus_conn);
        nus_conn = NULL;
        nus_bytes_read = 0;
        nus_escaped = false;
        nus_overflowed = false;
#if NUS_LATENCY_INSTRUMENTATION
        nus_latency_pending = false;
#endif
        nus_start_advertising();
        return;
    }
    if (bt_conn_get_info(conn, &info) == 0 && info.role == BT_CONN_ROLE_PERIPHERAL) {
        LOG_INF("Peripheral client disconnected: %s (reason=%u)", addr, reason);
        return;
    }

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

static void nus_optimize_connection(struct bt_conn* conn) {
    struct bt_le_conn_param param = {
        .interval_min = 6,
        .interval_max = 9,
        .latency = 0,
        .timeout = 400,
    };
    int err = bt_conn_le_param_update(conn, &param);
    if (err) {
        LOG_WRN("NUS conn param update failed: %d", err);
    } else {
        LOG_INF("NUS conn param update requested (7.5-11.25 ms)");
    }

#if defined(CONFIG_BT_CTLR_PHY_2M)
    struct bt_conn_le_phy_param phy = BT_CONN_LE_PHY_PARAM_INIT(BT_GAP_LE_PHY_2M,
                                                                  BT_GAP_LE_PHY_2M);
    err = bt_conn_le_phy_update(conn, &phy);
    if (err) {
        LOG_WRN("NUS 2M PHY update failed: %d", err);
    }
#endif
}

static void security_changed(struct bt_conn* conn, bt_security_t level, enum bt_security_err err) {
    char addr[BT_ADDR_LE_STR_LEN];
    struct bt_conn_info info;

    if (conn == nus_conn) {
        if (!err && level >= BT_SECURITY_L2) {
            nus_optimize_connection(conn);
        } else if (err) {
            LOG_ERR("NUS security failed: level=%u, err=%d", level, err);
        }
        return;
    }

    if (bt_conn_get_info(conn, &info) == 0 && info.role == BT_CONN_ROLE_PERIPHERAL) {
        return;
    }

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
    buf.external_report_id = 0;
    buf.len = bt_hogp_rep_size(rep) + 1;
    buf.data[0] = bt_hogp_rep_id(rep);

    memcpy(buf.data + 1, data, buf.len - 1);
    if (k_msgq_put(&report_q, &buf, K_NO_WAIT)) {
        //        printk("error in k_msg_put(report_q\n");
    }

    return BT_GATT_ITER_CONTINUE;
}

static struct descriptor_type their_descriptors[CONFIG_BT_MAX_CONN];

static void hogp_map_read_cb(struct bt_hogp* hogp, uint8_t err, const uint8_t* data, size_t size, size_t offset) {
    int8_t conn_idx = hogp_index(hogp);
    if (conn_idx < 0) {
        return;
    }

    if (err) {
        LOG_ERR("HOGP descriptor read failed for conn_idx=%d err=%d", conn_idx, err);
        return;
    }

    struct descriptor_type* their_descriptor = &their_descriptors[conn_idx];

    if (data == NULL) {
        their_descriptor->size = offset;
        their_descriptor->conn_idx = conn_idx;
        CHK(k_msgq_put(&descriptor_q, their_descriptor, K_NO_WAIT));
        return;
    }

    if (offset >= sizeof(their_descriptor->data)) {
        LOG_WRN("HOGP descriptor too large for conn_idx=%d; dropping remainder", conn_idx);
        return;
    }
    if ((offset + size) > sizeof(their_descriptor->data)) {
        LOG_WRN("HOGP descriptor too large for conn_idx=%d; truncating", conn_idx);
        size = sizeof(their_descriptor->data) - offset;
    }
    memcpy(their_descriptor->data + offset, data, size);

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
        buf.external_report_id = 0;
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
    bool sent = false;
    if (interface == 0) {
        sent = CHK(hid_int_ep_write(hid_dev0, report_with_id, len, NULL));
    } else if (interface == 1) {
        sent = CHK(hid_int_ep_write(hid_dev1, report_with_id, len, NULL));
    }
#if NUS_LATENCY_INSTRUMENTATION
    if (sent && interface == 0 && nus_latency_pending) {
        uint32_t delta = k_cycle_get_32() - nus_rx_cycles;
        nus_latency_pending = false;
        if ((nus_latency_samples++ & 0x3f) == 0) {
            LOG_INF("NUS RX->USB latency: %u us", k_cyc_to_us_near32(delta));
        }
    }
#endif
    return sent;
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
    nus_init_virtual_device();
    update_their_descriptor_derivates();
    their_descriptor_updated = false;
    nus_start_advertising();

    // Initialize 6-axis IMU AFTER mapping system is ready
#if DT_NODE_EXISTS(DT_NODELABEL(lsm6ds3tr_c))
    if (imu_enabled) {
        if (!imu_init()) {
            LOG_ERR("Failed to initialize 6-axis IMU");
        }
    } else {
        LOG_INF("IMU disabled in configuration - skipping IMU initialization");
    }
#else
    LOG_INF("IMU not available on this board - skipping IMU initialization");
#endif

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
            handle_received_report(incoming_report.data, incoming_report.len,
                                   (uint16_t) incoming_report.interface,
                                   incoming_report.external_report_id);
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
