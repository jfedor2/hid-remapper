// btstack_config.h for the remapper_bt target (Pico W, Bluetooth Classic HID host).
//
// Based on the configuration shipped with pico-examples' Bluetooth examples.
// ENABLE_CLASSIC / ENABLE_BLE are defined automatically by linking
// pico_btstack_classic / pico_btstack_ble, so they are not set here. This
// variant links only pico_btstack_classic.

#ifndef _PICO_BTSTACK_BTSTACK_CONFIG_H
#define _PICO_BTSTACK_BTSTACK_CONFIG_H

// BTstack features that can be enabled
#ifdef ENABLE_BLE
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_CENTRAL
#define ENABLE_L2CAP_LE_CREDIT_BASED_FLOW_CONTROL_MODE
#endif

#define ENABLE_LOG_ERROR
#define ENABLE_PRINTF_HEXDUMP
// Uncomment for a very verbose HCI/L2CAP/SDP trace on the stdio UART, useful
// when a device won't connect and the [bt] logs don't say why.
// #define ENABLE_LOG_INFO

// BTstack configuration. buffers, sizes, ...
#define HCI_OUTGOING_PRE_BUFFER_SIZE 4
#define HCI_ACL_PAYLOAD_SIZE (1021 + 4)
#define HCI_ACL_CHUNK_SIZE_ALIGNMENT 4
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES 2
#define MAX_NR_GATT_CLIENTS 0
#define MAX_NR_HCI_CONNECTIONS 4
#define MAX_NR_HID_HOST_CONNECTIONS 4
#define MAX_NR_L2CAP_CHANNELS 12
#define MAX_NR_L2CAP_SERVICES 4
#define MAX_NR_SERVICE_RECORD_ITEMS 4
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 16
#define MAX_NR_LE_DEVICE_DB_ENTRIES 0

// Limit number of ACL/SCO Buffers to use by stack to avoid cyw43 shared bus overrun
#define MAX_NR_CONTROLLER_ACL_BUFFERS 3
#define MAX_NR_CONTROLLER_SCO_PACKETS 3

// Enable and configure HCI Controller to Host Flow Control to avoid cyw43 shared bus overrun
#define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL
#define HCI_HOST_ACL_PACKET_LEN 128
#define HCI_HOST_ACL_PACKET_NUM 8
#define HCI_HOST_SCO_PACKET_LEN 0
#define HCI_HOST_SCO_PACKET_NUM 0

// Link Key DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 16
#define NVM_NUM_LINK_KEYS 32

// We don't give btstack a malloc, so use a fixed-size ATT DB.
#define MAX_ATT_DB_SIZE 512

// BTstack HAL configuration
#define HAVE_EMBEDDED_TIME_MS

// map btstack_assert onto Pico SDK assert()
#define HAVE_ASSERT

// Some Bluetooth Classic HID devices are slow to respond
#define HCI_RESET_RESEND_TIMEOUT_MS 1000

#define ENABLE_SOFTWARE_AES128
#define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS

#ifdef ENABLE_CLASSIC
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE
#endif

#define HAVE_MALLOC

#endif  // _PICO_BTSTACK_BTSTACK_CONFIG_H
