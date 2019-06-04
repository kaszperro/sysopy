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

#include <signal.h>

#include <pthread.h>

#include "message.h"

#define MAX_CLIENTS 32


int max(int a, int b){
    return (a>b)?a:b;
}


typedef struct client_t {
    char name[CLIENT_NAME_SIZE];
    int socket;
    int ping;
    int busy;
    struct sockaddr* addr;
    socklen_t addr_len;
} client_t;


client_t clients[MAX_CLIENTS];

int connected_clients=0;

int epoll = -1;
int sock_net=-1;
int sock_unix=-1;

int sent_task_counter = 0;

char *unix_path = NULL;

void send_to_client(client_t *client, void *message, int size) {
    sendto(client->socket, message, size, 0, client->addr, client->addr_len);
}

void clean() {
    for(int i = 0; i < connected_clients; ++i) {
        int type = MESSAGE_DISCONNECT;
        send_to_client(&clients[i], &type, sizeof(int));
    }

    if(unix_path != NULL) {
        unlink(unix_path);
    }
    if(sock_net != -1) {
        close(sock_net);
    }
    if(sock_unix != -1) {
        close(sock_unix);
    }
}



void sigint_hanlder() {
    exit(0);
}

void send_msg_client(char *text, client_t *client) {
    int task_number = sent_task_counter++;
    client->busy++;
    int msg_type = MESSAGE_ANSWER;

    send_to_client(client, &msg_type, sizeof(int));
    send_to_client(client, &task_number, sizeof(int));
    send_to_client(client, text,MAX_TEXT_SIZE);
}

int find_client_index(struct sockaddr* addr,socklen_t addr_len) {
    for(int i=0; i<connected_clients; i++){
        if(clients[i].addr_len == addr_len &&
        memcmp(addr, clients[i].addr, addr_len) == 0)
         return i;
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

void remove_client(struct sockaddr* addr, socklen_t addr_len) {
    int index = find_client_index(addr, addr_len);
    connected_clients--;
    free(clients[index].addr);
    for(int i = index; i < connected_clients; ++i) {
        clients[i].busy = clients[i+1].busy;
        strcpy(clients[i].name, clients[i+1].name);
        clients[i].socket = clients[i+1].socket;
        clients[i].addr = clients[i+1].addr;
        clients[i].addr_len =  clients[i+1].addr_len;
    }
}

void send_message_free(char *text) {
    if(connected_clients == 0) {
        printf("there are no clients\n");
    }
    for(int i = 0; i < connected_clients; ++i) {
        if(clients[i].busy == 0) {
            send_msg_client(text, &clients[i]);
            return;
        }
    }

    int rand_index = rand()%connected_clients;
    send_msg_client(text, &clients[rand_index]);
}


void *ping_task(void *arg) {
    while(1) {
        int before_connected = connected_clients;
        for(int i = 0; i < connected_clients; ++i) {
            clients[i].ping = 0;
            int type = MESSAGE_PING;
            
            send_to_client(&clients[i], &type, sizeof(int));
        }

        sleep(5);

        if(before_connected != connected_clients) continue;

       
        for(int i = 0; i < connected_clients; ++i) {
            if(clients[i].ping == 0) {
                printf("client: %s was disconeted, no response\n", clients[i].name);
                remove_client(clients[i].addr, clients[i].addr_len);
            }
        }
    }
    return NULL;
}

void create_client(char *name, int socket,  struct sockaddr* addr, socklen_t addr_len) {
    clients[connected_clients].ping = 1;
    clients[connected_clients].busy = 0;
    strcpy(clients[connected_clients].name ,name);
    clients[connected_clients].socket = socket;
    clients[connected_clients].addr_len = addr_len;
    clients[connected_clients].addr = addr;

    connected_clients++;
}


void *receive_task(void *arg) {
    struct epoll_event event;
    char text_buff[MAX_TEXT_SIZE];
    text_buff[0] = 0;

    struct sockaddr* from_addr;
    socklen_t from_addr_len;
    while (1) {
        int how_many = epoll_wait(epoll, &event, 1, 1000);
        if(how_many < 1){
            continue;
        }

        int which = event.data.fd;
        int msg_type=0;

        if(which ==  sock_net) {
            from_addr = malloc(sizeof(struct sockaddr_in));
            from_addr_len = sizeof(struct sockaddr_in);
        } else {
            from_addr = malloc(sizeof(struct sockaddr_un));
            from_addr_len = sizeof(struct sockaddr_un);
        }
        
        if(recvfrom(which, &msg_type, sizeof(msg_type), 0, from_addr, &from_addr_len) == sizeof(msg_type)) {
            if(msg_type == MESSAGE_HELLO) {
                printf("client wants to connect\n");
                recv(which, text_buff, CLIENT_NAME_SIZE, 0);
                printf("client wants name: %s\n", text_buff);
                
                if(is_name_taken(text_buff)) {
                    printf("name is already taken :(\n");
                    int type = MESSAGE_NAME_TAKEN;
                    sendto(which, &type, sizeof(int), 0, from_addr, from_addr_len);
                   
                    continue;
                }

                int type = MESSAGE_REGISTERED;
                if(sendto(which, &type, sizeof(int), 0, from_addr, from_addr_len) != sizeof(int)) {
                    perror("cant send response to client");
                }

                create_client(text_buff, which, from_addr, from_addr_len);

                printf("client: %s successfully registered\n", text_buff);

            } else if(msg_type == MESSAGE_PING) {
                int index = find_client_index(from_addr, from_addr_len);
                clients[index].ping = 1;
            } else if(msg_type == MESSAGE_DISCONNECT) {
                int index = find_client_index(from_addr, from_addr_len);
                printf("client %s wants to disconect\n", clients[index].name);
                remove_client(from_addr, from_addr_len);
                printf("disconected\n");
            } else if(msg_type == MESSAGE_ANSWER) {
                int index = find_client_index(from_addr, from_addr_len);
                int task_number;
                read(which, &task_number, sizeof(int));
                read(which, &text_buff, MAX_TEXT_SIZE);
                
                printf("got answer from: %s, task number: %d\n",  clients[index].name, task_number);
                
                printf("%s",text_buff);

                clients[index].busy --;
            }
        }

    }
}

void *commands_task(void *arg) {
    char path[128];
    char text_buffer[MAX_TEXT_SIZE];

    while(1) {
        scanf("%s", path);
        printf("reading from: %s\n", path);
         FILE *f = fopen(path, "r");
         if(f != NULL) {
            int num_read = fread(&text_buffer, sizeof(char), MAX_TEXT_SIZE, f);
            fclose(f);
            text_buffer[num_read] ='\0';
            send_message_free(text_buffer);
         } else {
            perror("cant open file");
         }
    }
}


int create_inet_socket(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
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
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
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
    atexit(clean);
    signal(SIGINT, sigint_hanlder);
    if (argc != 3) {
        fprintf(stderr, "usage: [port path]\n");
        exit(1);
    }

    int port;
    unix_path = argv[2];

    if (sscanf(argv[1], "%d", &port) != 1) {
        perror("unable to parse port");
        exit(1);
    }

    printf("starting server...\n");

    sock_net = create_inet_socket(port);
    sock_unix = create_unix_socket(unix_path);

    epoll = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock_net;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, sock_net, &ev) != 0){perror("Cannot register epoll event");exit(1);}
    
    ev.data.fd = sock_unix;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, sock_unix, &ev) != 0){perror("Cannot register epoll event");exit(1);}

    pthread_t ping_thread; 
    pthread_create(&ping_thread, NULL, ping_task, NULL);

    pthread_t receive_thread; 
    pthread_create(&receive_thread, NULL, receive_task, NULL);

    pthread_t commands_thread;
    pthread_create(&commands_thread, NULL, commands_task, NULL);

    while(1) {
        sleep(1);
    }

    return 0;
}
