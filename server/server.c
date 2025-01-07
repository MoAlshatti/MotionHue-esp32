#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"
#include <string.h>
#include "common.h"

#define MAX_CLIENTS 24

client_t clients [MAX_CLIENTS];

void init_clients(void){
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].fd = -1;
        memset(&clients[i].buffer,0,sizeof(byte)*BUFF_SIZE);
        return;
    }
}

int find_empty_slot(void){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].fd == -1){
            return i;
        }
    }
    return -1;
}

int find_client_from_fd(int fd){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].fd == fd) return i;
    }
    return -1;
}

void handle_request(int client_index){
    client_t client = clients[client_index];



}

int main(void){

    int temp_fd, slot;

    struct sockaddr_in serverInfo = {.sin_addr.s_addr = INADDR_ANY, .sin_family = AF_INET, .sin_port = htons(PORT)};

    struct sockaddr_in clientInfo = {0};
    socklen_t client_size = sizeof(clientInfo);

    

    init_clients();

    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1) {
        perror("socket");
        return -1;
    }

    if(bind(lfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo)) == -1){
        perror("bind");
        return -1;
    }

    if(listen(lfd,0) == -1){
        perror("listen");
        return -1;
    }

    struct pollfd clients_poll [MAX_CLIENTS + 1];

    clients_poll[0].fd = lfd;
    clients_poll[0].events = POLLIN;

    while(1){
        nfds_t nfds = 1;
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].fd != -1){
                clients_poll[nfds].fd = clients[i].fd;
                clients_poll[nfds].events = POLLIN;
                nfds++;
            }
        }

        int num_events = poll(clients_poll,nfds,-1);

        if(clients_poll[0].revents & POLLIN){
            if(temp_fd = accept(lfd,(struct sockaddr *)&clientInfo,&client_size) == -1){
                perror("accept");
                continue;   // we continue because fd of -1 is equivalent to not adding a client
            }
            if (slot = find_empty_slot() == -1){
                fprintf(stderr,"slots are full\n");
            }else{
                clients[slot].fd = slot;
            }
            num_events--;
        }

        for(int i = 1; i <= nfds && num_events > 0; i++){
            if(clients_poll[i].revents & POLLIN){
                int client_index = find_client_from_fd(clients_poll[i].fd);


                num_events--;
            }
        }

    }
    




    return 0;
}