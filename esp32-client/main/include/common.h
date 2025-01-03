#pragma once

#include <stdlib.h>

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
    unsigned long data;
}msg_t;

typedef enum {
    VERSION_1,
}protocol_version_e;

typedef struct {
    protocol_version_e version;
    size_t msg_length;
}protocol_hdr;


typedef unsigned char byte;