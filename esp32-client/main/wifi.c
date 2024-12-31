#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_mac.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "wifi.h"

#define SSID "ssid"
#define PASS "password"
#define SUCCESS_BIT BIT0
#define FAILURE_BIT BIT1
#define MAX_RETRY 5

static const char* TAG = "WIFI";
static EventGroupHandle_t event_group;
static int curr_retry = 0;

static void wifi_event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data){
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    }
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED){
        if(curr_retry < MAX_RETRY){ 
            ESP_LOGI(TAG, "connection failed, retrying...\n");
            esp_wifi_connect();
            curr_retry++;
        }
        else {
            xEventGroupSetBits(event_group,FAILURE_BIT);
        }
    }
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP){
        curr_retry = 0;
        xEventGroupSetBits(event_group, SUCCESS_BIT);
    }
    
}

esp_err_t wifi_connect(void){
    esp_err_t ret_val = ESP_FAIL;

    event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler,NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_conf = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_conf));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_conf = {
        .sta = {
            .ssid = SSID, .password = PASS
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&wifi_conf));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t event_bits = xEventGroupWaitBits(event_group, SUCCESS_BIT | FAILURE_BIT, pdTRUE, pdFALSE,portMAX_DELAY);

    if (event_bits & SUCCESS_BIT) {
        ESP_LOGI(TAG, "successfully connected!\n");
        ret_val = ESP_OK;
    }
    else if (event_bits & FAILURE_BIT){
        ESP_LOGI(TAG,"failed to connect, quitting...\n");
        ret_val = ESP_FAIL;
    }
    else {
        ESP_LOGI(TAG, "Unexpected event, quitting...\n");
        ret_val = ESP_FAIL;
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT,ESP_EVENT_ANY_ID,wifi_event_handler));
    vEventGroupDelete(event_group);
    return ret_val;
}