#pragma once

#include "common.h"

#define BUFF_SIZE 256


typedef struct{
    int fd;
    byte buffer[256];
    
}client_t;