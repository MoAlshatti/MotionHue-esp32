#pragma once

void nvs_wifi_cred_set(char* ssid, char* pass);
esp_err_t wifi_connect(void);