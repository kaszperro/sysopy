#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h> 
#include <sys/socket.h>
 #include <sys/epoll.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/un.h>

#include <pthread.h>

#include "message.h"

#define MAX_CLIENTS 32
#define CLIENT_NAME_SIZE 32


int min(int a, int b){
    return (a>b)?b:a;
}


typedef struct client_t {
  char name[CLIENT_NAME_SIZE];
  int socket;
  int ping;
  int busy;
} client_t;


client_t clients[MAX_CLIENTS];

int connected_clients=0;

int epoll = -1;
int sock_net;
int sock_unix;

void send_message(message_t *message, client_t *client) {
    client->busy++;
    
    write(client->socket, &message->type, sizeof(int));
    write(client->socket, message, sizeof(message_t));
}

int find_socket_index(int socket) {
    for(int i=0; i<connected_clients; i++){
        if(clients[i].socket == socket) return i;
    }
    return -1;

}

int is_name_taken(char *name) {
    for(int i = 0; i < connected_clients; ++i) {
        if(strcmp(clients[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

void remove_client(int socket) {
    int index = find_socket_index(socket);
    connected_clients--;
    for(int i = index; i < connected_clients; ++i) {
        clients[i].busy = clients[i+1].busy;
        strcpy(clients[i].name, clients[i+1].name);
        clients[i].socket = clients[i+1].socket;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLET;
    ev.data = (epoll_data_t)socket;
    if(epoll_ctl(epoll, EPOLL_CTL_DEL, socket, &ev) != 0){
        perror("cannot delete from epoll");
        exit(1);
    }

    close(socket);

}

void send_message_free(message_t *message) {
    for(int i = 0; i < connected_clients; ++i) {
        if(clients[i].busy == 0) {
            send_message(message, &clients[i]);
            return;
        }
    }

    int rand_index = rand()%connected_clients;
    send_message(message, &clients[rand_index]);
}


void *ping_task(void *arg) {
    while(1) {
        int before_connected = connected_clients;
        for(int i = 0; i < connected_clients; ++i) {
            clients[i].ping = 0;
            int type = MESSAGE_PING;
            write(clients[i].socket, &type, sizeof(int));
        }

        sleep(5);

        if(before_connected != connected_clients) continue;

       
        for(int i = 0; i < connected_clients; ++i) {
            if(clients[i].ping == 0) {
                printf("client: %s was disconeted, no response\n", clients[i].name);
                remove_client(clients[i].socket);
            }
        }
    }
    return NULL;
}


void *receive_task(void *arg) {
    struct epoll_event event;
    message_t message;
    while (1) {
        int how_many = epoll_wait(epoll, &event, 1, 1000);
        if(how_many < 1){
            continue;
        }

        int which = event.data.fd;
        int msg_type;

        if(read(which, &msg_type, sizeof(msg_type)) > 0) {
             if(msg_type == MESSAGE_DISCONNECT) {
                printf("client wants to disconect\n");
                
            }
            else if(msg_type == MESSAGE_ANSWER){
                int index = find_socket_index(which);
              
                printf("got answer from: %s\n",  clients[index].name);
                read(which, &message, sizeof(message));
                printf("%s", message.text);


            }
            else if(msg_type == MESSAGE_PING){
                clients[find_socket_index(which)].ping = 1;
            }

        }

    }
}

void create_client(char *name, int socket) {
    clients[connected_clients].ping = 1;
    clients[connected_clients].busy = 0;
    strcpy(clients[connected_clients].name ,name);
    clients[connected_clients].socket = socket;

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket;

    if(epoll_ctl(epoll, EPOLL_CTL_ADD, socket, &ev) != 0){
        perror("cannot register epoll event");
        exit(1);
    }

    connected_clients++;
}

void accept_from_socket(int sock, char *name_buff) {
    int new_client = accept(sock, NULL, 0);
    if(new_client == -1) {
        perror("inet accept error");
    }

    if(new_client != -1) {
        printf("client wants to connect through inet\n");

        recv(new_client, name_buff, CLIENT_NAME_SIZE, MSG_WAITALL);

        printf("client wants name: %s\n", name_buff);
        if(is_name_taken(name_buff)) {
            printf("name is already taken :(\n");
            int type = MESSAGE_NAME_TAKEN;
            write(new_client, &type, sizeof(type));
            return;
        }

        create_client(name_buff, new_client);
        int type = MESSAGE_REGISTERED;
        write(new_client, &type, sizeof(type));

        printf("client: %s successfully registered\n", name_buff);
    } 
}

void *inet_accept_task(void *arg) {
    char name_buff[CLIENT_NAME_SIZE];
    while(1) {
       accept_from_socket(sock_net, name_buff);
    }
}

void *commands_task(void *arg) {
    char path[128];
    message_t message;
    while(1) {
        scanf("%s", path);
         FILE *f = fopen(path, "r");
         if(f != NULL) {
           fread(&message.text, MAX_TEXT_SIZE, 1, f);
      
            message.type = MESSAGE_ANSWER;
            fclose(f);

            send_message_free(&message);
         } else {
            perror("cant open file");
         }
    }
}


int create_inet_socket(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("unable to create network socket");
        exit(1);
    }
    int op = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int));

    struct sockaddr_in address; 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( port ); 

    if(bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("unable to bind tcp");
        exit(1);
    }

    return sock;
}

int create_unix_socket(char *path) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("unable to create network socket");
        exit(1);
    }

    int op = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int));

    struct sockaddr_un name;
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, path);

    unlink(path);

    if(bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
        perror("unable to bind unix");
        exit(1);
    }

    return sock;
}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: [port path]\n");
        exit(1);
    }

    int port;
    char* unix_paht = argv[2];

    if (sscanf(argv[1], "%d", &port) != 1) {
        perror("unable to parse port");
        exit(1);
    }

    printf("starting server...\n");

    sock_net = create_inet_socket(port);
    sock_unix = create_unix_socket(unix_paht);

    epoll = epoll_create1(0);


    if(listen(sock_net, 5) == -1 || listen(sock_unix, 5) == -1){
        perror("cant listen");
        exit(1);
    }

    pthread_t ping_thread; 
    pthread_create(&ping_thread, NULL, ping_task, NULL);


    pthread_t receive_thread; 
    pthread_create(&receive_thread, NULL, receive_task, NULL);


    pthread_t accept_thread; 
    pthread_create(&accept_thread, NULL, inet_accept_task, NULL);

    pthread_t commands_thread;
    pthread_create(&commands_thread, NULL, commands_task, NULL);

    char name_buff[CLIENT_NAME_SIZE];

    while(1) {
        accept_from_socket(sock_unix, name_buff);
    }

    close(sock_net);
    close(sock_unix);
    return 0;
}
