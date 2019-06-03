#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "message.h"
#include "list.h"

#define NAME_SIZE 128

#define QUEUE_SIZE 64

int server_sock;

message_t queue[QUEUE_SIZE];
int queue_size=0;

void push_queue(message_t message) {
    queue[queue_size++] = message;
}

message_t pop_queue() {
    message_t empty;
    empty.type = -1;
    if(queue_size == 0) return empty;
    message_t to_ret = queue[0];
    for(int i = 0; i < queue_size-1; ++i) {
        queue[i].type = queue[i+1].type;
        strcpy( queue[i].text, queue[i+1].text);
    }
    queue_size -= 1;
    return to_ret;
}

void *receive_task(void * arg) {
    int type;
    message_t buff;
    while(1){
        read(server_sock, &type, sizeof(type));
        if(type == MESSAGE_PING) {
            printf("ping response\n");
            write(server_sock, &type, sizeof(type));
        } else if(type == MESSAGE_ANSWER) {
            if( read(server_sock, &buff, sizeof(buff)) != sizeof(buff) ){
                perror("cant recive task");
            }

            push_queue(buff);
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 4){
        fprintf(stderr, "wrong args: name, [inet/unix], [addr/path]\n");
        exit(1);
    }

    if(strcmp(argv[2], "unix") == 0) {
        server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if(server_sock == -1){perror("cannot make socket");exit(1);}

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, argv[3]);

        if(connect(server_sock, (struct sockaddr*)&addr, sizeof(addr)) != 0){perror("cannot connect");exit(1);}

    } else if(strcmp(argv[2], "inet") == 0){
        server_sock =  socket(AF_INET, SOCK_DGRAM, 0);
        if(server_sock == -1){perror("cannot make socket");exit(1);}

        struct sockaddr_in iaddr;
        iaddr.sin_family = AF_INET;
        for(size_t i=0; i < strlen(argv[3]); i++){
            if(argv[3][i] == ':'){
                argv[3][i] = 0;
                iaddr.sin_port =  htons( atoi(argv[3] + i + 1) );
                break;
            }
        }

        inet_aton(argv[3], &iaddr.sin_addr);
        
        printf("port %d\n", ntohs(iaddr.sin_port));
        printf("adres %s\n",  inet_ntoa(iaddr.sin_addr));

        if(connect(server_sock, (struct sockaddr*)&iaddr, sizeof(iaddr)) != 0){
            perror("cannot connect");
            exit(1);
        }

    }


    char name[NAME_SIZE];
    strcpy(name, argv[1]);
    if(write(server_sock, name, NAME_SIZE) != NAME_SIZE) {
        perror("cant send name to server\n");
        exit(1);
    }

    int response;
    recv(server_sock, &response, sizeof(int), MSG_WAITALL);
    if(response == MESSAGE_NAME_TAKEN){
        printf("cannot connect, name is taken");
        exit(1);
    }
    printf("connected\n");

    pthread_t receive_thread;

    pthread_create(&receive_thread, NULL, receive_task, NULL);


    message_t message;
    
    while(1) {

        while((message = pop_queue()).type == -1) {
            usleep(40);
        }
        
        char rec_text[MAX_TEXT_SIZE];
        strcpy(rec_text, message.text);

        printf("received task: %s\n", rec_text);

        list_t *list = new_list();

        char *tmp = strtok(rec_text, " ,.-");
        while(tmp != NULL){
            insert_list(list, tmp);
            tmp = strtok(NULL, " ,.-");
        }
        
        printf("inserted\n");

        message.text[0] = 0;
        int s = 0;
        int num_words = 0;
        node_t *node = list->root;
        while(node != NULL) {
            if(s >= MAX_TEXT_SIZE - 10) break;
            s += sprintf(message.text+s, "%s: %d\n", node->word, node->count);
            node = node->next;
            num_words++;
        }


        sprintf(message.text+s, "all words: %d\n", num_words);

        message.type = MESSAGE_ANSWER;
        write(server_sock, &message.type, sizeof(int));
        write(server_sock, &message, sizeof(message));
        printf("sent answer\n");
        free_list(list);
    }

    return 0;
}
