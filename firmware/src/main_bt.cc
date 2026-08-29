// Entry point for the "remapper_bt" target: HID Remapper running on a
// Raspberry Pi Pico W, taking input from Bluetooth Classic (BR/EDR) HID
// devices and presenting itself to the host computer as a USB HID device.
//
// This is the Pico W / pico-sdk counterpart of firmware-bluetooth/src/main.cc
// (which does the same thing for Bluetooth LE on the nRF52840 with Zephyr).
// It provides its own int main() and its own implementations of the
// platform.h / remapper.h seams instead of sharing firmware/src/main.cc.
//
// The Bluetooth side uses the BTstack stack bundled with pico-sdk, in "poll"
// mode: everything runs in the single main loop, so BTstack packet handlers
// and the remapper's process_mapping() never run concurrently and no locking
// is needed between them.

#include <cstdio>
#include <cstring>

extern "C" {
#include "btstack.h"
#include "btstack_tlv.h"
#ifdef ENABLE_LOG_INFO
#include "hci_dump_embedded_stdout.h"
#endif
}

#include "pico/bootrom.h"
#include "pico/cyw43_arch.h"
#include "pico/mutex.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "hardware/flash.h"
#include "hardware/sync.h"

#include "config.h"
#include "descriptor_parser.h"
#include "globals.h"
#include "our_descriptor.h"
#include "platform.h"
#include "remapper.h"
#include "tick.h"

// USB device glue lives in main_bt_usb.cc, which includes <tusb.h>. BTstack and
// TinyUSB both define hid_report_type_t / HID_REPORT_TYPE_* and can't be
// included in the same translation unit, so we keep them apart.
void bt_usb_init();
void bt_usb_task();
bool bt_usb_hid_ready(uint8_t itf);
bool bt_usb_suspended();
bool bt_usb_do_send_report(uint8_t interface, const uint8_t* report_with_id, uint8_t len);

// RP2350 UF2s wipe the last sector of flash every time (RP2350-E10 errata
// mitigation), so the config goes one sector down there. Same as main.cc.
#if PICO_RP2350
#define CONFIG_OFFSET_IN_FLASH (PICO_FLASH_SIZE_BYTES - PERSISTED_CONFIG_SIZE - 4096)
#else
#define CONFIG_OFFSET_IN_FLASH (PICO_FLASH_SIZE_BYTES - PERSISTED_CONFIG_SIZE)
#endif
#define FLASH_CONFIG_IN_MEMORY (((uint8_t*) XIP_BASE) + CONFIG_OFFSET_IN_FLASH)

// Maximum number of Bluetooth HID devices connected at the same time.
// Keep in sync with MAX_NR_HID_HOST_CONNECTIONS in btstack_config.h.
#define MAX_DEVICES 4

#define INQUIRY_DURATION 5        // units of 1.28s
#define PAIRING_TIMEOUT_MS 60000  // give up looking for a new device after this

// Diagnostic output, on the stdio UART (GP0, 921600 baud on the Pico W).
#define BT_LOG(fmt, ...) printf("[bt] " fmt "\n", ##__VA_ARGS__)

// ---------------------------------------------------------------------------
// platform.h: mutexes, time, unique id, flash, reset
// ---------------------------------------------------------------------------

static mutex_t mutexes[(uint8_t) MutexId::N];

void my_mutexes_init() {
    for (int i = 0; i < (int8_t) MutexId::N; i++) {
        mutex_init(&mutexes[i]);
    }
}

void my_mutex_enter(MutexId id) {
    mutex_enter_blocking(&mutexes[(uint8_t) id]);
}

void my_mutex_exit(MutexId id) {
    mutex_exit(&mutexes[(uint8_t) id]);
}

uint64_t get_time() {
    return time_us_64();
}

uint64_t get_unique_id() {
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);
    uint64_t ret = 0;
    for (int i = 0; i < 8; i++) {
        ret |= (uint64_t) unique_id.id[7 - i] << (8 * i);
    }
    return ret;
}

void do_persist_config(uint8_t* buffer) {
    // We run from flash (not copy_to_ram), so interrupts have to be off while
    // we touch flash. This briefly stalls Bluetooth too, but config writes are
    // rare. Same approach as the other flash-resident variants (serial, dual_a).
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(CONFIG_OFFSET_IN_FLASH, PERSISTED_CONFIG_SIZE);
    flash_range_program(CONFIG_OFFSET_IN_FLASH, buffer, PERSISTED_CONFIG_SIZE);
    restore_interrupts(ints);
}

void reset_to_bootloader() {
    reset_usb_boot(0, 0);
}

void flash_b_side() {
}

void interval_override_updated() {
}

// GPIO-as-input support is not implemented for this variant yet.
uint32_t get_gpio_valid_pins_mask() {
    return 0;
}

void set_gpio_inout_masks(uint32_t in_mask, uint32_t out_mask) {
}

// ---------------------------------------------------------------------------
// Device bookkeeping: map BTstack hid_cid <-> small dense device index
// ---------------------------------------------------------------------------

static uint16_t device_cid[MAX_DEVICES];     // 0 == slot free
static bd_addr_t device_addr[MAX_DEVICES];
// hid_host delivers reports before the HID descriptor on incoming (reconnect)
// connections. Feeding those into handle_received_report() poisons the shared
// interface_index map (operator[] inserts interface->0), so different devices
// end up sharing interface_idx 0 and their input states collide. Only process
// a device's reports once parse_descriptor() has run for it.
static bool device_ready[MAX_DEVICES];

static int index_for_cid(uint16_t cid, bool allocate) {
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (device_cid[i] == cid) {
            return i;
        }
    }
    if (!allocate) {
        return -1;
    }
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (device_cid[i] == 0) {
            device_cid[i] = cid;
            return i;
        }
    }
    return -1;
}

static void free_index(int idx) {
    if (idx >= 0 && idx < MAX_DEVICES) {
        device_cid[idx] = 0;
        device_ready[idx] = false;
        memset(device_addr[idx], 0, sizeof(bd_addr_t));
    }
}

static int num_connected() {
    int n = 0;
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (device_cid[i] != 0) {
            n++;
        }
    }
    return n;
}

static bool addr_is_connected(const bd_addr_t addr) {
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (device_cid[i] != 0 && bd_addr_cmp(device_addr[i], addr) == 0) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Incoming HID report queue (filled by the BTstack packet handler, drained by
// read_report() in the main loop).
// ---------------------------------------------------------------------------

struct queued_report_t {
    uint16_t interface;
    uint16_t len;
    uint8_t data[64];
};

#define REPORT_RING_SIZE 16
static queued_report_t report_ring[REPORT_RING_SIZE];
static uint16_t report_ring_head = 0;
static uint16_t report_ring_tail = 0;

static void queue_report(uint16_t interface, const uint8_t* data, uint16_t len) {
    if ((uint16_t) (report_ring_tail - report_ring_head) >= REPORT_RING_SIZE) {
        return;  // overflow, drop
    }
    if (len > sizeof(report_ring[0].data)) {
        len = sizeof(report_ring[0].data);
    }
    queued_report_t* slot = &report_ring[report_ring_tail % REPORT_RING_SIZE];
    slot->interface = interface;
    slot->len = len;
    memcpy(slot->data, data, len);
    report_ring_tail++;
}

// ---------------------------------------------------------------------------
// Bluetooth bonding / pairing state
// ---------------------------------------------------------------------------

static bool hci_ready = false;
static bool pairing_mode = false;
static uint32_t pairing_started_ms = 0;

// Outgoing connection setup for a newly-discovered device. hid_host_connect()
// runs an SDP query then opens the HID L2CAP channels. Inquiry must be stopped
// for the whole sequence - it hogs the radio and stalls the connection.
#define CONNECT_TIMEOUT_MS 12000
static bool connecting = false;
static uint32_t connecting_started_ms = 0;
static bd_addr_t connecting_addr;
static uint16_t connecting_handle = HCI_CON_HANDLE_INVALID;
static uint16_t connecting_cid = 0;

// Per-device throttle so a device that fails to pair (or isn't really in
// pairing mode) gets retried periodically rather than tried once and then
// ignored for the rest of the pairing window.
#define RETRY_COOLDOWN_MS 5000
struct connect_attempt_t {
    bd_addr_t addr;
    uint32_t last_ms;
};
static connect_attempt_t connect_attempts[8];
static int connect_attempts_count = 0;

// SDP server record: DS4/DS5 query the host's Device ID record when reconnecting;
// without an SDP server answering, they give up.
static uint8_t device_id_sdp_service_buffer[100];

static int count_bonds() {
    btstack_link_key_iterator_t it;
    if (!gap_link_key_iterator_init(&it)) {
        return 0;
    }
    bd_addr_t addr;
    link_key_t link_key;
    link_key_type_t type;
    int n = 0;
    while (gap_link_key_iterator_get_next(&it, addr, link_key, &type)) {
        n++;
    }
    gap_link_key_iterator_done(&it);
    return n;
}

static bool is_bonded(const bd_addr_t addr) {
    btstack_link_key_iterator_t it;
    if (!gap_link_key_iterator_init(&it)) {
        return false;
    }
    bd_addr_t a;
    link_key_t link_key;
    link_key_type_t type;
    bool found = false;
    while (gap_link_key_iterator_get_next(&it, a, link_key, &type)) {
        if (bd_addr_cmp(a, addr) == 0) {
            found = true;
            break;
        }
    }
    gap_link_key_iterator_done(&it);
    return found;
}

// Stable bd_addr -> slot (1..MAX_BOND_SLOTS) map, used as a "virtual port" for
// per-device mappings. BTstack's own link-key iteration order isn't stable (a
// newly paired device comes out first, shifting everyone), so we keep our own
// assignment: each device gets the lowest free slot on first connect and keeps
// it. Persisted alongside the link keys in BTstack's TLV.
#define MAX_BOND_SLOTS 15
#define BOND_SLOT_TLV_TAG 0x484d5053u  // 'H','M','P','S'
static bd_addr_t bond_slots[MAX_BOND_SLOTS];  // index i -> slot i+1; all-zero == free
static bool bond_slots_loaded = false;

static void bond_slots_load() {
    if (bond_slots_loaded) {
        return;
    }
    bond_slots_loaded = true;
    memset(bond_slots, 0, sizeof(bond_slots));
    const btstack_tlv_t* tlv = NULL;
    void* ctx = NULL;
    btstack_tlv_get_instance(&tlv, &ctx);
    if (tlv != NULL) {
        tlv->get_tag(ctx, BOND_SLOT_TLV_TAG, (uint8_t*) bond_slots, sizeof(bond_slots));
    }
}

static void bond_slots_save() {
    const btstack_tlv_t* tlv = NULL;
    void* ctx = NULL;
    btstack_tlv_get_instance(&tlv, &ctx);
    if (tlv != NULL) {
        tlv->store_tag(ctx, BOND_SLOT_TLV_TAG, (const uint8_t*) bond_slots, sizeof(bond_slots));
    }
}

static void bond_slots_reset() {
    memset(bond_slots, 0, sizeof(bond_slots));
    bond_slots_loaded = true;
    const btstack_tlv_t* tlv = NULL;
    void* ctx = NULL;
    btstack_tlv_get_instance(&tlv, &ctx);
    if (tlv != NULL) {
        tlv->delete_tag(ctx, BOND_SLOT_TLV_TAG);
    }
}

static uint8_t bond_slot_for_addr(const bd_addr_t addr) {
    bond_slots_load();
    bd_addr_t zero;
    memset(zero, 0, sizeof(zero));
    for (int i = 0; i < MAX_BOND_SLOTS; i++) {
        if (bd_addr_cmp(bond_slots[i], addr) == 0) {
            return i + 1;
        }
    }
    for (int i = 0; i < MAX_BOND_SLOTS; i++) {
        if (bd_addr_cmp(bond_slots[i], zero) == 0) {
            bd_addr_copy(bond_slots[i], addr);
            bond_slots_save();
            return i + 1;
        }
    }
    return 0;  // table full
}

static void start_pairing() {
    pairing_mode = true;
    pairing_started_ms = to_ms_since_boot(get_absolute_time());
    connect_attempts_count = 0;
    if (hci_ready && !connecting) {
        BT_LOG("pairing mode ON, starting inquiry");
        gap_inquiry_start(INQUIRY_DURATION);
    } else {
        BT_LOG("pairing mode ON%s", connecting ? " (connect in progress)" : ", waiting for HCI");
    }
}

static void stop_pairing() {
    if (!pairing_mode) {
        return;
    }
    BT_LOG("pairing mode OFF");
    pairing_mode = false;
    connect_attempts_count = 0;
    if (hci_ready && !connecting) {
        gap_inquiry_stop();
    }
}

// Resume looking for a device to pair, if we still should.
static void resume_inquiry_if_pairing() {
    if (pairing_mode && !connecting && hci_ready) {
        BT_LOG("resuming inquiry");
        gap_inquiry_start(INQUIRY_DURATION);
    }
}

static void connect_finished() {
    connecting = false;
    connecting_handle = HCI_CON_HANDLE_INVALID;
    connecting_cid = 0;
    resume_inquiry_if_pairing();
}

static void abort_connect() {
    if (connecting_cid != 0) {
        hid_host_disconnect(connecting_cid);
    } else if (connecting_handle != HCI_CON_HANDLE_INVALID) {
        gap_disconnect(connecting_handle);
    }
    connect_finished();
}

void pair_new_device() {
    BT_LOG("pair_new_device requested");
    start_pairing();
}

void clear_bonds() {
    BT_LOG("clear_bonds requested");
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (device_cid[i] != 0) {
            hid_host_disconnect(device_cid[i]);
        }
    }
    gap_delete_all_link_keys();
    bond_slots_reset();
    start_pairing();
}

// ---------------------------------------------------------------------------
// Status LED (the Pico W's LED hangs off the CYW43 chip)
// ---------------------------------------------------------------------------

// Talking to the CYW43 LED is a bus transaction, so only write it when the
// desired state actually changes.
static void led_set(bool on) {
    static int last = -1;
    if ((int) on != last) {
        last = on;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
    }
}

static void update_status_led() {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (pairing_mode) {
        led_set(true);  // solid while looking for a device to pair
        return;
    }

    // Otherwise: blink once per connected device, then a pause.
    // No devices -> slow heartbeat.
    int n = num_connected();
    static uint32_t phase_started = 0;
    static int blink = 0;  // counts half-blinks within a cycle, then a pause
    static bool led = false;

    if (n == 0) {
        led_set((now / 1000) & 1);
        return;
    }

    uint32_t elapsed = now - phase_started;
    if (blink < 2 * n) {
        if (elapsed >= 150) {
            led = !led;
            led_set(led);
            phase_started = now;
            blink++;
        }
    } else {
        led_set(false);
        if (elapsed >= 900) {
            blink = 0;
            led = false;
            phase_started = now;
        }
    }
}

// ---------------------------------------------------------------------------
// BTstack packet handler (poll mode -> runs in the main loop context)
// ---------------------------------------------------------------------------

static uint8_t hid_descriptor_storage[2048];
static btstack_packet_callback_registration_t hci_event_callback_registration;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }

    uint8_t event = hci_event_packet_get_type(packet);
    bd_addr_t addr;

    switch (event) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                hci_ready = true;
                bd_addr_t local_addr;
                gap_local_bd_addr(local_addr);
                int bonds = count_bonds();
                BT_LOG("HCI up, local address %s, %d bonded device(s)", bd_addr_to_str(local_addr), bonds);
                // Bonded Bluetooth Classic HID devices reconnect by paging us;
                // we stay connectable and accept them. Only actively look for
                // something new when nothing is bonded.
                if (bonds == 0) {
                    start_pairing();
                }
            }
            break;

        case HCI_EVENT_PIN_CODE_REQUEST:
            // Legacy pairing (old keyboards): try the usual default PIN.
            hci_event_pin_code_request_get_bd_addr(packet, addr);
            BT_LOG("PIN code request from %s, responding '0000'", bd_addr_to_str(addr));
            gap_pin_code_response(addr, "0000");
            break;

        case HCI_EVENT_CONNECTION_REQUEST:
            hci_event_connection_request_get_bd_addr(packet, addr);
            BT_LOG("incoming ACL connection request from %s (CoD 0x%06lx)",
                bd_addr_to_str(addr), (unsigned long) hci_event_connection_request_get_class_of_device(packet));
            break;

        case HCI_EVENT_CONNECTION_COMPLETE: {
            uint8_t st = hci_event_connection_complete_get_status(packet);
            hci_con_handle_t handle = hci_event_connection_complete_get_connection_handle(packet);
            hci_event_connection_complete_get_bd_addr(packet, addr);
            BT_LOG("ACL connection complete: %s status 0x%02x handle 0x%04x link_type %u",
                bd_addr_to_str(addr), st, handle, hci_event_connection_complete_get_link_type(packet));
            if (st != ERROR_CODE_SUCCESS) {
                break;
            }
            if (hci_event_connection_complete_get_link_type(packet) != 0x01) {  // ACL only
                break;
            }
            if (connecting && bd_addr_cmp(addr, connecting_addr) == 0) {
                // Our outgoing connect. Start pairing/encryption now rather than
                // waiting for hid_host to open the HID channels - some devices
                // won't even answer the SDP query on an unauthenticated link.
                connecting_handle = handle;
                BT_LOG("outgoing ACL up, requesting encryption");
                gap_request_security_level(handle, LEVEL_2);
            }
            // For an incoming reconnect we do nothing here: the device opens the
            // HID L2CAP channels itself and the LEVEL_2 service requirement
            // brings up encryption as part of that. Firing a bare
            // gap_request_security_level() right after the role switch upsets
            // some cheap devices (they drop the link, HCI reason 0x13).
            break;
        }

        case HCI_EVENT_DISCONNECTION_COMPLETE: {
            hci_con_handle_t h = hci_event_disconnection_complete_get_connection_handle(packet);
            BT_LOG("ACL disconnected: handle 0x%04x status 0x%02x reason 0x%02x",
                h, hci_event_disconnection_complete_get_status(packet),
                hci_event_disconnection_complete_get_reason(packet));
            if (connecting && h == connecting_handle) {
                BT_LOG("connect attempt lost its ACL");
                connect_finished();
            }
            break;
        }

        case HCI_EVENT_LINK_KEY_REQUEST:
            hci_event_link_key_request_get_bd_addr(packet, addr);
            BT_LOG("link key request for %s", bd_addr_to_str(addr));
            break;

        case HCI_EVENT_LINK_KEY_NOTIFICATION:
            reverse_bd_addr(&packet[2], addr);
            BT_LOG("link key notification for %s", bd_addr_to_str(addr));
            break;

        case HCI_EVENT_AUTHENTICATION_COMPLETE:
            BT_LOG("authentication complete: handle 0x%04x status 0x%02x",
                hci_event_authentication_complete_get_connection_handle(packet),
                hci_event_authentication_complete_get_status(packet));
            break;

        case HCI_EVENT_ENCRYPTION_CHANGE:
            BT_LOG("encryption change: handle 0x%04x status 0x%02x enabled %u",
                hci_event_encryption_change_get_connection_handle(packet),
                hci_event_encryption_change_get_status(packet),
                hci_event_encryption_change_get_encryption_enabled(packet));
            break;

        case HCI_EVENT_ROLE_CHANGE:
            hci_event_role_change_get_bd_addr(packet, addr);
            BT_LOG("role change: %s status 0x%02x role %u (0=master)",
                bd_addr_to_str(addr), hci_event_role_change_get_status(packet),
                hci_event_role_change_get_role(packet));
            break;

        case HCI_EVENT_USER_CONFIRMATION_REQUEST:
            hci_event_user_confirmation_request_get_bd_addr(packet, addr);
            BT_LOG("SSP confirmation from %s, value %lu (auto-accept)",
                bd_addr_to_str(addr), (unsigned long) hci_event_user_confirmation_request_get_numeric_value(packet));
            break;

        case HCI_EVENT_SIMPLE_PAIRING_COMPLETE:
            hci_event_simple_pairing_complete_get_bd_addr(packet, addr);
            BT_LOG("pairing with %s complete, status 0x%02x",
                bd_addr_to_str(addr), hci_event_simple_pairing_complete_get_status(packet));
            break;

        case GAP_EVENT_INQUIRY_RESULT: {
            uint32_t cod = gap_event_inquiry_result_get_class_of_device(packet);
            gap_event_inquiry_result_get_bd_addr(packet, addr);
            if (gap_event_inquiry_result_get_name_available(packet)) {
                BT_LOG("inquiry: %s CoD 0x%06lx name \"%.*s\"", bd_addr_to_str(addr), (unsigned long) cod,
                    (int) gap_event_inquiry_result_get_name_len(packet),
                    (const char*) gap_event_inquiry_result_get_name(packet));
            } else {
                BT_LOG("inquiry: %s CoD 0x%06lx", bd_addr_to_str(addr), (unsigned long) cod);
            }
            if (!pairing_mode || connecting) {
                break;
            }
            // Major device class 0x05 == Peripheral (keyboards, mice, gamepads).
            if (((cod >> 8) & 0x1f) != 0x05) {
                BT_LOG("  not a peripheral, ignoring");
                break;
            }
            if (addr_is_connected(addr)) {
                break;
            }
            uint32_t now = to_ms_since_boot(get_absolute_time());
            int slot = -1;
            for (int i = 0; i < connect_attempts_count; i++) {
                if (bd_addr_cmp(connect_attempts[i].addr, addr) == 0) {
                    slot = i;
                    break;
                }
            }
            if (slot >= 0 && (now - connect_attempts[slot].last_ms) < RETRY_COOLDOWN_MS) {
                break;  // tried this one recently
            }
            if (slot < 0 && connect_attempts_count < (int) (sizeof(connect_attempts) / sizeof(connect_attempts[0]))) {
                slot = connect_attempts_count++;
                bd_addr_copy(connect_attempts[slot].addr, addr);
            }
            if (slot >= 0) {
                connect_attempts[slot].last_ms = now;
            }
            // The user explicitly asked to pair and this device showed up in
            // inquiry (so it's in pairing mode). If we already have a key for
            // it, drop it so we pair fresh - a device put back in pairing mode
            // has a new key, and presenting the old one makes authentication
            // "succeed" but then encryption time out (0x22).
            if (is_bonded(addr)) {
                BT_LOG("  dropping existing link key for fresh pairing");
                gap_drop_link_key_for_bd_addr(addr);
            }
            // Stop inquiry for the duration of the connection setup.
            gap_inquiry_stop();
            connecting = true;
            connecting_started_ms = to_ms_since_boot(get_absolute_time());
            bd_addr_copy(connecting_addr, addr);
            connecting_handle = HCI_CON_HANDLE_INVALID;
            uint16_t cid = 0;
            uint8_t status = hid_host_connect(addr, HID_PROTOCOL_MODE_REPORT, &cid);
            connecting_cid = cid;
            BT_LOG("  connecting to %s (cid 0x%04x, status 0x%02x)", bd_addr_to_str(addr), cid, status);
            if (status != ERROR_CODE_SUCCESS) {
                connect_finished();
            }
            break;
        }

        case GAP_EVENT_INQUIRY_COMPLETE:
            if (pairing_mode && !connecting) {
                BT_LOG("inquiry complete, scanning again");
                gap_inquiry_start(INQUIRY_DURATION);
            } else {
                BT_LOG("inquiry complete");
            }
            break;

        case HCI_EVENT_HID_META: {
            uint8_t subevent = hci_event_hid_meta_get_subevent_code(packet);
            switch (subevent) {
                case HID_SUBEVENT_INCOMING_CONNECTION: {
                    uint16_t cid = hid_subevent_incoming_connection_get_hid_cid(packet);
                    hid_subevent_incoming_connection_get_address(packet, addr);
                    bool accept = pairing_mode || is_bonded(addr);
                    BT_LOG("incoming connection from %s: %s", bd_addr_to_str(addr), accept ? "accepting" : "declining");
                    if (accept) {
                        hid_host_accept_connection(cid, HID_PROTOCOL_MODE_REPORT);
                    } else {
                        hid_host_decline_connection(cid);
                    }
                    break;
                }

                case HID_SUBEVENT_CONNECTION_OPENED: {
                    uint16_t cid = hid_subevent_connection_opened_get_hid_cid(packet);
                    uint8_t status = hid_subevent_connection_opened_get_status(packet);
                    hid_subevent_connection_opened_get_bd_addr(packet, addr);
                    if (connecting_cid != 0 && cid == connecting_cid) {
                        connecting = false;
                        connecting_handle = HCI_CON_HANDLE_INVALID;
                        connecting_cid = 0;
                    }
                    if (status != ERROR_CODE_SUCCESS) {
                        BT_LOG("connection to %s failed, status 0x%02x", bd_addr_to_str(addr), status);
                        resume_inquiry_if_pairing();
                        break;
                    }
                    int idx = index_for_cid(cid, true);
                    if (idx < 0) {
                        BT_LOG("connection to %s opened but no free device slot, dropping", bd_addr_to_str(addr));
                        hid_host_disconnect(cid);
                        break;
                    }
                    bd_addr_copy(device_addr[idx], addr);
                    device_ready[idx] = false;  // until the descriptor is parsed
                    if (pairing_mode) {
                        stop_pairing();
                    }
                    uint8_t slot = bond_slot_for_addr(device_addr[idx]);
                    BT_LOG("connected to %s as device %d (bond slot %d, %s)", bd_addr_to_str(addr), idx, slot,
                        is_bonded(addr) ? "bonded" : "NOT bonded - device did not pair, won't reconnect");
                    // VID/PID reported as 1/1 like the nRF variant; real values
                    // would need a separate Device ID SDP query (only matters
                    // for device-specific quirks, which this variant has none of).
                    device_connected_callback(idx << 8, 1, 1, slot);
                    break;
                }

                case HID_SUBEVENT_DESCRIPTOR_AVAILABLE: {
                    uint16_t cid = hid_subevent_descriptor_available_get_hid_cid(packet);
                    uint8_t status = hid_subevent_descriptor_available_get_status(packet);
                    int idx = index_for_cid(cid, false);
                    if (status != ERROR_CODE_SUCCESS) {
                        BT_LOG("device %d: HID descriptor not available, status 0x%02x", idx, status);
                        break;
                    }
                    if (idx < 0) {
                        break;
                    }
                    const uint8_t* desc = hid_descriptor_storage_get_descriptor_data(cid);
                    uint16_t desc_len = hid_descriptor_storage_get_descriptor_len(cid);
                    BT_LOG("device %d: HID descriptor available, %u bytes", idx, desc_len);
                    parse_descriptor(1, 1, desc, desc_len, idx << 8, 0);
                    device_ready[idx] = true;
                    break;
                }

                case HID_SUBEVENT_REPORT: {
                    uint16_t cid = hid_subevent_report_get_hid_cid(packet);
                    int idx = index_for_cid(cid, false);
                    if (idx < 0 || !device_ready[idx]) {
                        break;  // ignore reports until the descriptor is parsed
                    }
                    const uint8_t* report = hid_subevent_report_get_report(packet);
                    uint16_t report_len = hid_subevent_report_get_report_len(packet);
                    // Strip the leading HID transaction header byte; only pass
                    // on DATA/INPUT reports (0xa1).
                    if (report_len < 1 || report[0] != 0xa1) {
                        break;
                    }
                    queue_report(idx << 8, report + 1, report_len - 1);
                    break;
                }

                case HID_SUBEVENT_CONNECTION_CLOSED: {
                    uint16_t cid = hid_subevent_connection_closed_get_hid_cid(packet);
                    if (connecting_cid != 0 && cid == connecting_cid) {
                        BT_LOG("connection setup (cid 0x%04x) closed", cid);
                        connect_finished();
                    }
                    int idx = index_for_cid(cid, false);
                    if (idx >= 0) {
                        BT_LOG("device %d (%s) disconnected", idx, bd_addr_to_str(device_addr[idx]));
                        device_disconnected_callback(idx);
                        free_index(idx);
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// remapper.h: report I/O seams
// ---------------------------------------------------------------------------

void extra_init() {
}

// Process at most one report between process_mapping() runs. If a key-down and
// key-up from the same device were both applied to the input state before
// process_mapping() got to see the key-down, the keystroke would be lost.
static bool report_processing_pending = false;

void read_report(bool* new_report, bool* tick) {
    *tick = get_and_clear_tick_pending();
    *new_report = false;

    if (report_processing_pending) {
        return;
    }
    if (report_ring_head != report_ring_tail) {
        queued_report_t* slot = &report_ring[report_ring_head % REPORT_RING_SIZE];
        handle_received_report(slot->data, slot->len, slot->interface);
        report_ring_head++;
        *new_report = true;
        report_processing_pending = true;
    }
}

void queue_out_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    int idx = interface >> 8;
    if (idx < 0 || idx >= MAX_DEVICES || device_cid[idx] == 0) {
        return;
    }
    // Best effort: drop the update if a previous SET_REPORT is still in flight.
    hid_host_send_set_report(device_cid[idx], HID_REPORT_TYPE_OUTPUT, report_id, buffer, len);
}

void queue_set_feature_report(uint16_t interface, uint8_t report_id, const uint8_t* buffer, uint8_t len) {
    // TODO: feature-report passthrough (used by PS controller auth) not wired yet.
}

void queue_get_feature_report(uint16_t interface, uint8_t report_id, uint8_t len) {
    // TODO: feature-report passthrough (used by PS controller auth) not wired yet.
}

void send_out_report() {
}

void __no_inline_not_in_flash_func(sof_callback)() {
    set_tick_pending();
}

// ---------------------------------------------------------------------------

int main() {
    my_mutexes_init();
    tick_init();

    load_config(FLASH_CONFIG_IN_MEMORY);
    our_descriptor = &our_descriptors[our_descriptor_number];
    parse_our_descriptor();
    set_mapping_from_config();

    bt_usb_init();
    stdio_init_all();

    BT_LOG("HID Remapper Bluetooth starting");

#ifdef ENABLE_LOG_INFO
    // Route BTstack's log_info / packet dumps to the stdio UART. Do this before
    // cyw43_arch_init() so the link-key flash-bank init logs are captured too.
    hci_dump_init(hci_dump_embedded_stdout_get_instance());
#endif

    if (cyw43_arch_init()) {
        BT_LOG("cyw43_arch_init failed");
        while (true) {
            bt_usb_task();
        }
    }
    BT_LOG("cyw43_arch_init ok");

    // BTstack Classic HID host setup.
    l2cap_init();
    // Require encryption (LEVEL_2) for the HID L2CAP channels and for outgoing
    // connections - input devices must be encrypted. A device that refuses to
    // pair simply won't connect.
    gap_set_security_level(LEVEL_2);
    gap_ssp_set_enable(1);
    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    gap_ssp_set_auto_accept(1);
    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    gap_set_default_link_policy_settings(LM_LINK_POLICY_ENABLE_SNIFF_MODE | LM_LINK_POLICY_ENABLE_ROLE_SWITCH);
    hci_set_master_slave_policy(HCI_ROLE_MASTER);
    gap_set_page_scan_type(PAGE_SCAN_MODE_INTERLACED);
    gap_set_local_name("HID Remapper Bluetooth 00:00:00:00:00:00");
    // Connectable (page scan) so bonded devices can reconnect. Never
    // discoverable: we're always the initiator - we inquiry-scan for a device
    // in pairing mode and page it - so nothing needs to find us by inquiry, and
    // this keeps us out of nearby phones' Bluetooth menus.
    gap_connectable_control(1);
    gap_discoverable_control(0);

    hid_host_init(hid_descriptor_storage, sizeof(hid_descriptor_storage));
    hid_host_register_packet_handler(packet_handler);

    // An SDP server with a Device ID record: DS4/DS5 query it while pairing /
    // reconnecting, before the link is encrypted, so register it at LEVEL_0.
    gap_set_security_level(LEVEL_0);
    sdp_init();
    device_id_create_sdp_record(device_id_sdp_service_buffer, 0x10003,
        DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
    sdp_register_service(device_id_sdp_service_buffer);
    gap_set_security_level(LEVEL_2);

    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    BT_LOG("powering on Bluetooth");
    hci_power_control(HCI_POWER_ON);

    while (true) {
        cyw43_arch_poll();

        bool tick;
        bool new_report;
        read_report(&new_report, &tick);

        if (their_descriptor_updated) {
            update_their_descriptor_derivates();
            their_descriptor_updated = false;
        }

        if (tick) {
            process_mapping(true);
            report_processing_pending = false;
        }

        bt_usb_task();

        if (boot_protocol_updated) {
            parse_our_descriptor();
            boot_protocol_updated = false;
            config_updated = true;
        }
        if (resume_pending) {
            resume_pending = false;
            suspended = false;
        }
        if (config_updated) {
            set_mapping_from_config();
            config_updated = false;
        }
        if (bt_usb_hid_ready(0) || bt_usb_suspended()) {
            send_report(bt_usb_do_send_report);
        }
        if (monitor_enabled && bt_usb_hid_ready(1)) {
            send_monitor_report(bt_usb_do_send_report);
        }
        if (our_descriptor->main_loop_task != nullptr) {
            our_descriptor->main_loop_task();
        }
        if (need_to_persist_config) {
            persist_config_return_code = persist_config();
            need_to_persist_config = false;
        }

        if (connecting &&
            (to_ms_since_boot(get_absolute_time()) - connecting_started_ms > CONNECT_TIMEOUT_MS)) {
            BT_LOG("connect attempt to %s timed out", bd_addr_to_str(connecting_addr));
            abort_connect();
        }

        if (pairing_mode && !connecting &&
            (to_ms_since_boot(get_absolute_time()) - pairing_started_ms > PAIRING_TIMEOUT_MS)) {
            BT_LOG("pairing timed out");
            stop_pairing();
        }

        update_status_led();
    }

    return 0;
}
