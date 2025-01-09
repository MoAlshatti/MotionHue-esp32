#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"
#include <string.h>
#include "common.h"
#include <unistd.h>
#include "hue.h"

#define MAX_CLIENTS 24

client_t clients [MAX_CLIENTS];

void init_clients(void){
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].fd = -1;
        memset(&clients[i].buffer,0,sizeof(byte)*BUFF_SIZE);
    }
    return;
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
    client_t *client = &clients[client_index];

    ssize_t bytes_read = read(client->fd,client->buffer,sizeof(protocol_hdr) + sizeof(msg_t));
    if(bytes_read <= 0){
        close(client->fd);
        client->fd = -1;
        memset(client->buffer,0,sizeof(client->buffer));
        printf("client disconnected\n");
        return;
    }
    protocol_hdr *hdr =(protocol_hdr *)client->buffer;
    hdr->version = ntohs(hdr->version);
    hdr->msg_length = ntohl(hdr->msg_length);

    if(hdr->version != VERSION_1){
        fprintf(stderr,"protocol version doesnt match!\n");
        return;
    }
    if(hdr->msg_length != sizeof(msg_t)){
        fprintf(stderr,"unexpected message length!\n");
        return;
    }

    msg_t *msg = (msg_t *)&hdr[1];
    msg->msg_type = ntohs(msg->msg_type);
    msg->data =ntohl(msg->data);

    switch(msg->msg_type){
        case TURN_ON:
            printf("recieved turn on command\n");
            send_request(*msg);
            break;
        case TURN_OFF:
            printf("recieved turn off command\n");
            send_request(*msg);
            break;
        case CHANGE_BRIGHTNESS:
            printf("received change brightness command to %lu\n",msg->data);
            break;
        case CHANGE_COLOUR:
            printf("recieved change colour to %lX\n",msg->data);
            break;
        default:
            printf("unexpected message!\n");
            return;
    }


}

int main(void){

    int temp_fd, slot;
    int nfds = 0;

    struct sockaddr_in serverInfo = {.sin_addr.s_addr = INADDR_ANY, .sin_family = AF_INET, .sin_port = htons(PORT)};

    struct sockaddr_in clientInfo = {0};
    socklen_t client_size = sizeof(clientInfo);

    

    init_clients();

    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1) {
        perror("socket");
        return -1;
    }

    if(setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)) == -1){
        perror("setsockopt");
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

    memset(clients_poll,0,sizeof(clients_poll));
    clients_poll[0].fd = lfd;
    clients_poll[0].events = POLLIN;
    nfds = 1;

    while(1){
        int index = 1;
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].fd != -1){
                clients_poll[index].fd = clients[i].fd;
                clients_poll[index].events = POLLIN;
                index++;
            }
        }

        int num_events = poll(clients_poll,nfds,-1);
        if(num_events == -1){
            perror("poll");
            return -1;
        }

        if(clients_poll[0].revents & POLLIN){
            if((temp_fd = accept(lfd, (struct sockaddr *)&clientInfo, &client_size)) == -1){
                perror("accept");
                continue;   // we continue because fd of -1 is equivalent to not adding a client
            }
            if ((slot = find_empty_slot()) == -1){
                fprintf(stderr,"slots are full\n");
                close(temp_fd);
            }else{
                clients[slot].fd = temp_fd;
                printf("client connected...\n");
                nfds++;
                
            }
            num_events--;
        }
    
        for(int i = 1; i <= nfds && num_events > 0; i++){
            
            if(clients_poll[i].revents & POLLIN){
                int client_index = find_client_from_fd(clients_poll[i].fd);
                
                handle_request(client_index);

                num_events--;
            }
        }

    }
    




    return 0;
}