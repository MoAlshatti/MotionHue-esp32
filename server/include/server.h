#pragma once

#include "common.h"

#define BUFF_SIZE 256


typedef struct{
    int fd;
    byte buffer[BUFF_SIZE];
    
}client_t;