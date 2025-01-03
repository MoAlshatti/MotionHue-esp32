#pragma once

#include "esp_timer.h"

//  returns: the file descriptor of the socket
int server_connect(void);

esp_err_t turn_on_light(int fd);
esp_err_t turn_off_light(int fd);
esp_err_t change_light_brightness(int fd, unsigned long brightness);
esp_err_t change_light_colour(int fd, unsigned long colour);
void lights_off_timer_callback(void *arg);