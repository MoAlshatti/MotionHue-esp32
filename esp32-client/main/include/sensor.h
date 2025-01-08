#pragma once

#define TIMER_WAIT_TIME 15000000    //15 seconds in μs
#define INFINITE_WAIT_TIME UINT64_MAX   // 5000 centuries in μs lmao

typedef struct {
    int* fd;
    int* light_state;
}timer_callback_args;

void create_sensor_task(int fd);
void sensor_init(void);
void register_sensor_ISR(void);