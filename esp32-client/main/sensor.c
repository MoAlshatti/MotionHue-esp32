#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define SENSOR_PIN 5

static TaskHandle_t sensor_task;

void IRAM_ATTR sensor_interrupt_handler(void *args){

    BaseType_t xHigherPriorityHasAwoken = pdFALSE;
    vTaskNotifyGiveFromISR(sensor_task,&xHigherPriorityHasAwoken);
    portYIELD_FROM_ISR(xHigherPriorityHasAwoken);
}

static void sensor_task_function(void *args){

    while(1){
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

        //task logic 




    }
}

void create_sensor_task(void){
    xTaskCreate(sensor_task_function,"sensor_task",2048,NULL,3,sensor_task);
    return;
}

void sensor_init(void){
    gpio_config_t conf = {.intr_type = GPIO_INTR_POSEDGE, 
                          .mode = GPIO_MODE_INPUT, 
                          .pin_bit_mask = 1ULL << SENSOR_PIN, 
                          .pull_down_en = GPIO_PULLDOWN_DISABLE, 
                          .pull_up_en = GPIO_PULLUP_DISABLE};
    ESP_ERROR_CHECK(gpio_config(&conf));
    return;
}

                    
