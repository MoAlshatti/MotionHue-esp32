#include "lwip/sockets.h"
#include "common.h"
#include "client.h"
#include "esp_timer.h"
#include "sensor.h"


#define SERVER_ADDR "0.0.0.0"
#define PORT 5500

int server_connect(void){
    
    struct sockaddr_in serverInfo = {.sin_addr.s_addr = inet_addr(SERVER_ADDR), .sin_family = AF_INET, .sin_port = htons(PORT)};

    int fd = socket(AF_INET,SOCK_STREAM, 0);

    if (fd == -1){
        //deal with error

        esp_restart();
    }

    if(connect(fd,(struct sockaddr *)&serverInfo,sizeof(serverInfo)) == -1){
        //deal with error

        esp_restart();
    }

    return fd;
}

esp_err_t turn_on_light(int fd){

    void *buffer [256];
    protocol_hdr *hdr = (protocol_hdr *)buffer;
    hdr->version = htons(VERSION_1);
    hdr->msg_length = htonl(sizeof(msg_t));

    msg_t *msg= (msg_t *)&hdr[1];
    msg->msg_type = htons(TURN_ON);
    msg->data = htonl(0);

    int bytes_written = 0;
    while(bytes_written != sizeof(protocol_hdr) + sizeof(msg_t)){
        if (bytes_written == -1){
            return ESP_FAIL;
        }

        bytes_written = write(fd,buffer,(sizeof(protocol_hdr) + sizeof(msg_t)) - bytes_written);
    }
    return ESP_OK;
}

esp_err_t turn_off_light(int fd){

    void *buffer [256];
    protocol_hdr *hdr = (protocol_hdr *)buffer;
    hdr->version = htons(VERSION_1);
    hdr->msg_length = htonl(sizeof(msg_t));

    msg_t *msg= (msg_t *)&hdr[1];
    msg->msg_type = htons(TURN_OFF);
    msg->data = htonl(0);

    int bytes_written = 0;
    while(bytes_written != sizeof(protocol_hdr) + sizeof(msg_t)){
        if (bytes_written == -1){
            return ESP_FAIL;
        }

        bytes_written = write(fd,buffer,(sizeof(protocol_hdr) + sizeof(msg_t)) - bytes_written);
    }
    return ESP_OK;
}

esp_err_t change_light_brightness(int fd, unsigned long brightness){

    if (brightness > 100){
        return ESP_FAIL;
    }
    void *buffer [256];
    protocol_hdr *hdr = (protocol_hdr *)buffer;
    hdr->version = htons(VERSION_1);
    hdr->msg_length = htonl(sizeof(msg_t));

    msg_t *msg= (msg_t *)&hdr[1];
    msg->msg_type = htons(CHANGE_BRIGHTNESS);
    msg->data = htonl(brightness);

    int bytes_written = 0;
    while(bytes_written != sizeof(protocol_hdr) + sizeof(msg_t)){
        if (bytes_written == -1){
            return ESP_FAIL;
        }

        bytes_written = write(fd,buffer,(sizeof(protocol_hdr) + sizeof(msg_t)) - bytes_written);
    }
    return ESP_OK;
}

esp_err_t change_light_colour(int fd, unsigned long colour){

    void *buffer [256];
    protocol_hdr *hdr = (protocol_hdr *)buffer;
    hdr->version = htons(VERSION_1);
    hdr->msg_length = htonl(sizeof(msg_t));

    msg_t *msg= (msg_t *)&hdr[1];
    msg->msg_type = htons(CHANGE_COLOUR);
    msg->data = htonl(colour);

    int bytes_written = 0;
    while(bytes_written != sizeof(protocol_hdr) + sizeof(msg_t)){
        if (bytes_written == -1){
            return ESP_FAIL;
        }

        bytes_written = write(fd,buffer,(sizeof(protocol_hdr) + sizeof(msg_t)) - bytes_written);
    }
    return ESP_OK;
}

void lights_off_timer_callback(void *arg){

    timer_callback_args *args = (timer_callback_args *)arg;

    int fd = *(args->fd);

    int err = turn_off_light(fd);
    if (err == ESP_FAIL){
        //deal with error
    }
    *(args->light_state) = 0;
    return;
}