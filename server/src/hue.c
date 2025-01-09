#include<curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <stdbool.h>
#include "common.h"

#define HUE_KEY getenv("HUE_KEY")   //the key is stored manually as an env variable
const char *LIGHT_ID = "4d76c1f4-39da-4672-99fc-2ee7ee53b652";
const char *HUE_IP = "192.168.178.120";

static struct curl_slist* hdr_init(struct curl_slist *hdr);
static cJSON* create_json_request(msg_t req);
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int send_request(msg_t req){

    char URL [2000];
    sprintf(URL,"https://%s/clip/v2/resource/light/%s",HUE_IP,LIGHT_ID);

    CURL *curl_handle = NULL;
    CURLcode res_code;

    if((curl_handle = curl_easy_init()) == NULL){
        fprintf(stderr,"curl handle failed to initialize\n");
        return -1;
    } 
    curl_easy_setopt(curl_handle,CURLOPT_SSL_VERIFYPEER,false); //ideally verify the certificate instead
    curl_easy_setopt(curl_handle,CURLOPT_SSL_VERIFYHOST,false);

    cJSON *json = create_json_request(req);
    if(!json){
        fprintf(stderr,"failed to initialize req body\n");
        return -1;
    }
    curl_easy_setopt(curl_handle,CURLOPT_CUSTOMREQUEST, "PUT");
    char *body = cJSON_Print(json);
    curl_easy_setopt(curl_handle,CURLOPT_POSTFIELDS,body);

    struct curl_slist *hdr = NULL;
    hdr = hdr_init(hdr);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, hdr);
    
    curl_easy_setopt(curl_handle,CURLOPT_URL,URL);
    curl_easy_setopt(curl_handle,CURLOPT_WRITEFUNCTION,write_callback); //to disable prinitng repsonse to the terminal
    
    res_code = curl_easy_perform(curl_handle);
    if (res_code != CURLE_OK){
    printf("%s\n",curl_easy_strerror(res_code));
    }

    cJSON_free(body);
    cJSON_free(json);
    curl_slist_free_all(hdr);
    curl_easy_cleanup(curl_handle);
    return 0;
}

static cJSON* create_json_request(msg_t req){

    if(req.msg_type == TURN_ON || req.msg_type == TURN_OFF){
        cJSON *outerON = cJSON_CreateObject();
        cJSON *innerON = cJSON_CreateObject();

        switch(req.msg_type){
            case TURN_ON:
                cJSON_AddBoolToObject(innerON,"on",true);
                cJSON_AddItemToObject(outerON,"on",innerON);
                return outerON;
            case TURN_OFF:
                cJSON_AddBoolToObject(innerON,"on",false);
                cJSON_AddItemToObject(outerON,"on",innerON);
                return outerON;
            default:
                return NULL;
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
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    (void)ptr;
    (void)userdata;
    return size * nmemb;
}

static struct curl_slist* hdr_init(struct curl_slist *hdr){
    char host_hdr [500], key_hdr [500];

    sprintf(host_hdr,"Host: %s",HUE_IP);
    sprintf(key_hdr,"hue-application-key: %s",HUE_KEY);

    hdr = curl_slist_append(hdr,host_hdr);
    hdr = curl_slist_append(NULL,"Accept:");
    hdr = curl_slist_append(NULL,"Accept-Encoding: gzip, deflate, br");
    hdr = curl_slist_append(NULL,"Connection: keep-alive");
    hdr = curl_slist_append(NULL,"Transfer-Encoding: chunked");
    hdr = curl_slist_append(hdr,"Content-Type: application/json");
    hdr = curl_slist_append(hdr,"Cache-Control: no-cache");
    hdr = curl_slist_append(hdr,key_hdr);
    return hdr;
}