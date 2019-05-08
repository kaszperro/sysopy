#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "keygen.h"
#include "message.h"
#include "types.h"
#include "que.h"
#include "utils.h"

#define MAX_CLIENTS 128
#define MAX_GROUP 32

int queue;

int clients[MAX_CLIENTS];
int clients_friends[MAX_CLIENTS][MAX_CLIENTS];

int num_clients = 0;
int clients_tab_size = 0;

int stops = 0;

void clean() {
    delete_queue(queue, get_public_key());
}

void send_private(unsigned int client_id, message_t* message) {
    if(client_id >= clients_tab_size || clients[client_id] == -1) {
        fprintf(stderr, "wrong client!\n");
        return;
    }
    if ((send(clients[client_id], message) == -1)) {
        perror("unable to send private message");
    }
}

void handle_sigint(int sig) {
    stops = 1;
    if(num_clients == 0) {
        exit(0);
    }
       

    message_t message;
    message.type = TYPE_STOP;
    for(int i = 0; i < clients_tab_size; ++i) {
        if(clients[i] != -1) {
            send_private(i, &message);
        }
    }
}



int find_cliend_id() {

    if(clients_tab_size < MAX_CLIENTS) {
        num_clients ++;
        return clients_tab_size++;
    }
    for(int i =0 ; i < MAX_CLIENTS; ++i) {
        if(clients[i] == -1) {
            num_clients++;
            return i;
        }
    }

   
    return -1;
}

void handle_init(message_t *msg) {
    key_t key;
    sscanf(msg->text, "%d", &key);

    int cl_id = find_cliend_id();
    if(cl_id != -1) {
        if ((clients[cl_id] = get_queue(key)) == -1) {
            perror("cant open client private queue");
        }

        message_t message;
        message.type = TYPE_INIT;
        message.id = cl_id;
       
        send_private(cl_id, &message);

        printf("sent register confirmation for client: %d\n", cl_id);
    } else {
         fprintf(stderr, "clients full error\n");
    }
   
}

void handle_stop(message_t *msg) {
    int client_id = msg->id;
    if(close_queue(clients[client_id]) == -1) {
        perror("cant close client queue\n");
    }
    clients[client_id] = -1;
    num_clients--;

    for(int i = 0; i < clients_tab_size; ++i) {
        clients_friends[i][client_id] = clients_friends[client_id][i] = 0;
    }
   
    printf("client: %d stops, clients left: %d\n", client_id, num_clients);
    if(num_clients == 0 && stops ==1){
       exit(0);
    }

}

void handle_list(message_t *msg) {
    int client_id = msg->id;
  
    message_t message;
    message.text[0] = '\0';
    message.type=TYPE_LIST;
    message.timestamp = 0;
    char buff[64];
    for(int i = 0; i < clients_tab_size; ++i) {
        if(clients[i] != -1) {
            sprintf(buff, "%d: %d\n", i, clients[i]);
            strcat(message.text,buff);
        }
    }


    send_private(client_id, &message);
}

void handle_echo(message_t *msg) {
    message_t message;
    message.type=TYPE_ECHO;
    time(&message.timestamp);
    strcpy(message.text, msg->text);
    send_private(msg->id, &message);
}

void handle_2friends(message_t *msg) {
    int cl = msg->id;
    message_t message;
    message.id =cl;
    message.type = TYPE_2FRIENDS;
    strcpy(message.text, msg->text);
    time(&message.timestamp);
    
    for(int i = 0; i < clients_tab_size; ++i) {
        if(clients_friends[cl][i])
            send_private(i, &message);
    }
}

void handle_2all(message_t *msg) {
    int cl = msg->id;
    message_t message;
    message.id =cl;
    message.type = TYPE_2ALL;
    strcpy(message.text, msg->text);
    time(&message.timestamp);
    
    for(int i = 0; i < clients_tab_size; ++i) {
        if(clients[i] != -1) {
            send_private( i, &message);
        }
      
    }
}

void handle_2one(message_t *msg) {
    int cl = msg->id;
    message_t message;
    message.id = cl;
    message.type = TYPE_2ONE;
    char to_id_str[32];
    char text[256];
   
    separate_command(msg->text, to_id_str, text);
    int to_id = atoi(to_id_str);

    strcpy(message.text, text);
    time(&message.timestamp);
    send_private( to_id, &message);
}

void set_friends(message_t *msg, int add) {
    int cl = msg->id;
    int friends_count = 0;
    char *friends[32];
    split_line(msg->text, friends, &friends_count);


    for(int i = 0; i < friends_count; ++i)  {
        int fr = atoi(friends[i]);
        clients_friends[cl][fr] = add;
    }
    for(int i = 0; i < friends_count; ++i) {
        free(friends[i]);
    }
}

void handle_friends(message_t *msg) {
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        clients_friends[msg->id][i] = 0;
    }
    set_friends(msg, 1);
}

void handle_add(message_t *msg) {
    set_friends(msg, 1);
}

void handle_del(message_t *msg) {
    set_friends(msg, 0);
}

void init() {
    for(int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = -1;
        for(int j = 0; j < MAX_CLIENTS; ++j) {
            clients_friends[i][j] = 0;
        }
    }
}

int main(int argc, char* argv[]) {

    init();

    atexit(clean);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT,&sa,NULL);
  

    key_t key = get_public_key();
    if ((queue = create_queue(key)) == -1) {
        perror("cant create queue");
        exit(1);
    }

    message_t message;

    while (1) {
        if (  receive(queue, &message)   == -1) {
            printf("cant receive\n");
            if(errno != EINTR) {
                perror("cant receive message");
                exit(1);    
            } 
            
            continue;
        }
         switch (message.type) {
              case TYPE_INIT: {
                    handle_init(&message);
                    break;
              } case TYPE_STOP: {
                   handle_stop(&message);
                   break;
              } case TYPE_LIST: {
                  handle_list(&message);
                  break;
              } case TYPE_ECHO: {
                  handle_echo(&message);
                  break;
              } case TYPE_2ALL: {
                  handle_2all(&message);
                  break;
              } case TYPE_2FRIENDS: {
                  handle_2friends(&message);
                  break;
              } case TYPE_2ONE: {
                   handle_2one(&message);
                  break;
              } case TYPE_FRIENDS: {
                  handle_friends(&message);
                  break;
              } case TYPE_ADD: {
                  handle_add(&message);
                  break;
              } case TYPE_DEL: {
                  handle_del(&message);
                  break;
              } default: {
                  fprintf(stderr, "wrong type\n");
                  break;
              }
            
         }
    }


    return 0;
}