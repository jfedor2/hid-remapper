#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "globals.h"
#include "ps_auth.h"
#include "remapper.h"

static uint16_t auth_dev;
static uint8_t nonce_id;
static uint8_t nonce[280];
static uint8_t nonce_part = 0;
static uint8_t signature[1064];
static uint8_t signature_part = 0;
static uint8_t signature_ready = 0;
static uint8_t nonce_ready = 0;

static uint8_t set_buffer[64];
static bool busy = false;

enum {
    IDLE = 0,
    SENDING_RESET = 1,
    SENDING_NONCE = 2,
    WAITING_FOR_SIG = 3,
    RECEIVING_SIG = 4,
};

static uint8_t state = IDLE;

// Razer Raion
static const uint8_t output_0x03[] = {
    0x21, 0x27, 0x04, 0xc0, 0x07, 0x2c, 0x56,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0d, 0x0d, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t output_0xf3[] = { 0x0, 0x38, 0x38, 0, 0, 0, 0 };

void ps4_device_connected(uint16_t interface, uint16_t vid, uint16_t pid) {
    if (their_feature_usages[interface].count(0x03) &&
        their_feature_usages[interface].count(0xF0) &&
        their_feature_usages[interface].count(0xF1) &&
        their_feature_usages[interface].count(0xF2) &&
        their_feature_usages[interface].count(0xF3)) {
        printf("ps auth candidate detected\n");
        if (auth_dev == 0) {
            auth_dev = interface;
        }
    }
}

void ps4_device_disconnected(uint8_t dev_addr) {
    if (dev_addr == (auth_dev >> 8)) {
        auth_dev = 0;
    }
}

void ps4_main_loop_task() {
    if (!busy && auth_dev) {
        switch (state) {
            case IDLE:
                break;
            case SENDING_RESET:
                queue_get_feature_report(auth_dev, 0xF3, 7 + 1);
                busy = true;
                break;
            case SENDING_NONCE:
                set_buffer[0] = 0xF0;
                set_buffer[1] = nonce_id;
                set_buffer[2] = nonce_part;
                set_buffer[3] = 0;
                memcpy(set_buffer + 4, nonce + (nonce_part * 56), 56);
                queue_set_feature_report(auth_dev, 0xF0, set_buffer, 64);
                busy = true;
                nonce_part++;
                break;
            case WAITING_FOR_SIG:
                queue_get_feature_report(auth_dev, 0xF2, 15 + 1);
                busy = true;
                break;
            case RECEIVING_SIG:
                queue_get_feature_report(auth_dev, 0xF1, 63 + 1);
                busy = true;
                break;
        }
    }
}

void ps4_handle_received_report(const uint8_t* report, int len, uint16_t interface, uint8_t external_report_id) {
    if (!ignore_auth_dev_inputs || (interface != auth_dev)) {
        do_handle_received_report(report, len, interface, external_report_id);
    }
}

uint16_t ps4_handle_get_report(uint8_t report_id, uint8_t* buffer, uint16_t reqlen) {
    switch (report_id) {
        case 0x03:
            memcpy(buffer, output_0x03, reqlen);
            return reqlen;
        case 0xF3:
            memcpy(buffer, output_0xf3, reqlen);
            signature_ready = false;
            return reqlen;
        case 0xF1: {  // GET_SIGNATURE_NONCE
            buffer[0] = nonce_id;
            buffer[1] = signature_part;
            buffer[2] = 0;
            memcpy(&buffer[3], &signature[signature_part * 56], 56);
            signature_part++;
            if (signature_part == 19) {
                signature_part = 0;
            }
            return reqlen;
        }
        case 0xF2: {  // GET_SIGNING_STATE
            buffer[0] = nonce_id;
            buffer[1] = signature_ready ? 0 : 16;
            memset(&buffer[2], 0, 9);
            return reqlen;
        }
    }
    return reqlen;
}

void ps4_handle_set_report(uint8_t report_id, const uint8_t* buffer, uint16_t reqlen) {
    if (report_id == 0xF0) {  // SET_AUTH_PAYLOAD
        nonce_id = buffer[0];
        uint8_t part = buffer[1];
        if (part > 4) {
            return;
        }
        memcpy(&nonce[part * 56], &buffer[3], 56);
        if (part == 4) {
            nonce_ready = 1;
            state = SENDING_RESET;
            nonce_part = 0;
        }
    }
}

void ps4_handle_get_report_response(uint16_t interface, uint8_t report_id, uint8_t* report, uint16_t len) {
    if (interface == auth_dev) {
        busy = false;
        switch (report_id) {
            case 0xF3:
                state = SENDING_NONCE;
                break;
            case 0xF2:
                if (report[2] == 0) {
                    signature_part = 0;
                    state = RECEIVING_SIG;
                }
                break;
            case 0xF1:
                memcpy(signature + (signature_part * 56), report + 4, 56);
                signature_part++;
                if (signature_part == 19) {
                    state = IDLE;
                    signature_ready = true;
                    signature_part = 0;
                }
                break;
        }
    }
}

void ps4_handle_set_report_complete(uint16_t interface, uint8_t report_id) {
    if ((interface == auth_dev) && (report_id == 0xF0)) {
        busy = false;
        if (nonce_part == 5) {
            state = WAITING_FOR_SIG;
        }
    }
}
