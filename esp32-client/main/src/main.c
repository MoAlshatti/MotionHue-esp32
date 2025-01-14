#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_mac.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "sensor.h"
#include "client.h"

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NO_MEM){
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_flash_init();
    }  
    //nvs_wifi_cred_set("SSID","PASS");   // Only run once to copy credentials to nvs memory. change SSID & PASS to actual values

    err = wifi_connect();

    if (err == ESP_FAIL){
        esp_restart();
    }
    
    int fd = server_connect();

    sensor_init();
    create_sensor_task(fd);
    register_sensor_ISR();

    while(1){
        vTaskDelay(150);
    }

}