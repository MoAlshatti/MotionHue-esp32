#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "sensor.h"
#include "client.h"

#define SENSOR_PIN 5
#define TIMER_WAIT_TIME 15000000    //15 seconds in μs
#define INFINITE_WAIT_TIME UINT64_MAX   // 5000 centuries in μs lmao

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

    //initialize esp timer
    timer_callback_args cb_args = {.fd = &fd, .light_state = &state};
    esp_timer_create_args_t timer_config = {.callback = &lights_off_timer_callback, .arg = (void *)&cb_args, .name = "off timer"};
    esp_timer_handle_t timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&timer_config,&timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle,INFINITE_WAIT_TIME)); // to avoid starting inside the loop
    
    while(1){
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        if (state == 0){
            turn_on_light(fd);
            ESP_ERROR_CHECK(esp_timer_restart(timer_handle,TIMER_WAIT_TIME));
            state = 1;
        }
        else {
            ESP_ERROR_CHECK(esp_timer_restart(timer_handle,TIMER_WAIT_TIME));
        }
        
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

                    
