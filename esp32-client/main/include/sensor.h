#pragma once

typedef struct {
    int* fd;
    int* light_state;
}timer_callback_args;

void create_sensor_task(int fd);
void sensor_init(void);
void register_sensor_ISR(void);