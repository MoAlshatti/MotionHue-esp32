#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "sensor.h"
#include "client.h"
#define SENSOR_PIN 5

static TaskHandle_t sensor_task;

void IRAM_ATTR sensor_interrupt_handler(void *args){

    BaseType_t xHigherPriorityHasAwoken = pdFALSE;
    vTaskNotifyGiveFromISR(sensor_task,&xHigherPriorityHasAwoken);
    portYIELD_FROM_ISR(xHigherPriorityHasAwoken);
}

void register_sensor_ISR(void){
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(gpio_isr_handler_add(SENSOR_PIN,sensor_interrupt_handler,NULL));
    return;
}

static void sensor_task_function(void *args){
    int fd = *(int *) args;
    
    
    int state = 0;
    while(1){
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        if (state == 0){
            turn_on_light(fd);
            state = 1;
        }
        

        //task logic 




    }
}

void create_sensor_task(int fd){
    xTaskCreate(sensor_task_function,"sensor_task",2048,(void *)&fd,3,&sensor_task);
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

                    
