//this file should always be synced with the common.h in the server after any changes
#pragma once

#include <stdlib.h>
#include <stdint.h>

#define PORT 5500

typedef enum {
    CONNECT,
    DISCONNECT,
    TURN_ON,
    TURN_OFF,
    CHANGE_BRIGHTNESS,
    CHANGE_COLOUR
}msg_type_e;

typedef struct {
    msg_type_e msg_type;
    uint64_t data;
}msg_t;

typedef enum {
    VERSION_1 = 1,
}protocol_version_e;

typedef struct {
    protocol_version_e version;
    uint32_t msg_length;
}protocol_hdr;


typedef unsigned char byte;