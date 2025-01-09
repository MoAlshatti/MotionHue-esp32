#include<curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <stdbool.h>
#include "common.h"

#define HUE_KEY getenv("HUE_KEY")   //the key is stored manually as an env variable
const char *LIGHT_ID = "fCWxGYAEjsDzDIxHlffJNE23mm4c2ROd40GSv7g8";
const char *HUE_IP = "192.168.178.120";

void hdr_init(struct curl_slist *hdr);
cJSON* create_json_request(msg_t req);

int send_request(msg_t req){

    char URL [4068];
    sprintf(URL,"https://%s/clip/resource/light/%s",HUE_IP,LIGHT_ID);

    CURL *curl_handle = NULL;
    CURLcode res_code;

    if((curl_handle = curl_easy_init) == NULL){
        fprintf(stderr,"curl handle failed to initialize\n");
        return -1;
    } 
    struct curl_slist *hdr = NULL;
    hdr_init(hdr);

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, hdr);
    curl_easy_setopt(curl_handle,CURLOPT_URL,URL);



    cJSON *json = create_json_request(req);





    
    cJSON_free(json);
    curl_slist_free_all(hdr);
    curl_easy_cleanup(curl_handle);
    return 0;
}

cJSON* create_json_request(msg_t req){

    if(req.msg_type == TURN_ON || req.msg_type == TURN_OFF){
        cJSON *outerON = cJSON_CreateObject();
        cJSON *innerON = cJSON_CreateObject();

        switch(req.msg_type){
            case TURN_ON:
                cJSON_AddBoolToObject(innerON,"on",true);
                cJSON_AddItemToObject(outerON,"on",innerON);
                break;
            case TURN_OFF:
                cJSON_AddBoolToObject(innerON,"on",false);
                cJSON_AddItemToObject(outerON,"on",innerON);
                break;
            return outerON;
        }

    } 
    else if(req.msg_type == CHANGE_BRIGHTNESS){
        cJSON *dimming = cJSON_CreateObject();
        cJSON *brightness = cJSON_CreateObject();

        cJSON_AddNumberToObject(brightness,"brightness",(double)req.data);
        cJSON_AddItemToObject(dimming,"dimming",brightness);
        return dimming;
    }
    else if (req.msg_type == CHANGE_COLOUR){

    }
    
    return NULL;
}

void hdr_init(struct curl_slist *hdr){
    char host_hdr [468], key_hdr [468];

    hdr = curl_slist_append(hdr,"Accept:");
    hdr = curl_slist_append(hdr,"Content-Type: application/json");

    sprintf(host_hdr,"Host: %s",HUE_IP);
    hdr = curl_slist_append(hdr,host_hdr);

    sprintf(key_hdr,"hue-application-key: %s",HUE_KEY);
    hdr = curl_slist_append(hdr,key_hdr);
    return;
}