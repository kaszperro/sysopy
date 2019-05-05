#define _XOPEN_SOURCE 500

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "keygen.h"
#include "message.h"
#include "types.h"

#define MAX_CLIENTS 128
#define MAX_GROUP 32

int queue;

int clients[MAX_CLIENTS];
int clients_friends[MAX_CLIENTS][MAX_GROUP];
int clients_friends_count[MAX_CLIENTS];
int num_clients = 0;

void clean() {
    msgctl(queue, IPC_RMID, NULL);
}

void send_private(unsigned int client_id, message_t* message) {
  if ((msgsnd(clients[client_id], message, MAX_MESSAGE_SIZE, 0) == -1)) {
    perror("unable to send private message");
  }
}

void handle_sigint(int sig) {
    message_t message;
    message.type=TYPE_STOP;
    for(int i = 0; i < num_clients; ++i) {
        if(clients[i] != -1) {
            send_private(i, &message);
        }
    }

    exit(0);
}



int find_cliend_id() {
    if(num_clients < MAX_CLIENTS) {
        return num_clients++;
    }
    for(int i =0 ; i < MAX_CLIENTS; ++i) {
        if(clients[i] == -1)
            return i;
    }

   
    return -1;
}

void handle_init(char * text) {
    key_t key;
    sscanf(text, "%d", &key);

    int cl_id = find_cliend_id();
    if(cl_id != -1) {
        if ((clients[cl_id] = msgget(key, 0)) == -1) {
            perror("cant open client private queue");
        }
        printf("client registered with key: %d\n", key);

        message_t message;
        message.type = TYPE_INIT;
        sprintf(message.text, "%d", cl_id);
        send_private(cl_id, &message);

        printf("sent register confirmation for client: %d\n", cl_id);
    } else {
         fprintf(stderr, "clients full error\n");
    }
   
}

void handle_stop(char * text) {
    int client_id;
    sscanf(text, "%d", &client_id);
    clients[client_id] = -1;

    for(int i = 0; i < num_clients; ++i) {
        for(int j = 0; j < clients_friends_count[i]; ++j) {
            if(clients_friends[i][j] == client_id) {
                for(int k = j; k < clients_friends_count[i]-1; ++k) {
                    clients_friends[i][k] = clients_friends[i][k+1];
                }

                clients_friends_count[j]--;
                break;
            }
        }
    }

}

void handle_list(char *text) {
    int client_id;
    sscanf(text, "%d", &client_id); 
    message_t message;
    message.text[0] = '\0';
    message.type=TYPE_LIST;
    char buff[64];
    for(int i = 0; i < num_clients; ++i) {
        if(clients[i] != -1) {
            sprintf(buff, "%d: %d\n", i, clients[i]);
            strcat(message.text,buff);
        }
    }


    send_private(client_id, &message);
}


int main(int argc, char* argv[]) {

//init
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = -1;
        clients_friends_count[i] = 0;
    }
//end init

    atexit(clean);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT,&sa,NULL);
  

    key_t key = get_public_key();
    if ((queue = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("cant create queue");
        exit(1);
    }

    message_t message;

    while (1) {
        if (msgrcv(queue, &message, MAX_MESSAGE_SIZE, -TYPE_LAST, 0) == -1) {
            perror("cant receive message");
            exit(1);
        }
         switch (message.type) {
              case TYPE_INIT: {
                    handle_init(message.text);
                    break;
              }
              case TYPE_STOP: {
                   handle_stop(message.text);
                   break;
              }
              case TYPE_LIST: {
                  handle_list(message.text);
                  break;
              }
            
         }

     //   printf("%ld: %s\n", message.type, message.text);
    }

    return 0;
}