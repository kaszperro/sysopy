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

int server_queue, private_queue;
int id;

int sender_pid =- 1;

void clean() {
    if(sender_pid != 0) {
        msgctl(private_queue, IPC_RMID, NULL);
        if(sender_pid != - 1) {
            kill(sender_pid, SIGKILL);
        }
    }
   
}

void send_to_server(message_t* message) {
  if ((msgsnd(server_queue, message, MAX_MESSAGE_SIZE, 0) == -1)) {
    perror("unable to send message to server");
  }
}


void handle_sigint(int sig) {
    message_t message;
    message.type = TYPE_STOP;
    sprintf(message.text, "%d", id);
    send_to_server(&message);
    exit(0);
}

void handle_list() {
    message_t message;
    message.type = TYPE_LIST;
    sprintf(message.text, "%d", id);
    send_to_server(&message);
}

void sender_handle_line(char *line) {
    char command[256];
    sscanf(line, "%s", command);

    if(strcmp("LIST", command) == 0) {
        handle_list();
    } else {

    }
    
}

void sender() {
    char *res;
    char line[256];
    while(1){
        res = fgets(line, 1024, stdin);
        
        sender_handle_line(line);

    }
    
}

void catcher() {
    message_t message;
    while(1){
        if (msgrcv(private_queue, &message, MAX_MESSAGE_SIZE, -TYPE_LAST, 0) == -1) {
            perror("cant receive message");
        }
        if(message.type == TYPE_STOP) {
            printf("server shutdown, exiting\n");
            exit(0);
        }

        printf("%s\n", message.text);
    }
}


int main(int argc, char* argv[]) {
    atexit(clean);

  
  
    if ((server_queue = msgget(get_public_key(), 0)) == -1) {
        perror("cant open server queue");
        exit(1);
    }


    key_t private_key = get_private_key();
    if ((private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("cant create private queue");
        exit(1);
    }



    message_t message;
    message.type = TYPE_INIT;
    sprintf(message.text, "%d", private_key);
    if ((msgsnd(server_queue, &message, MAX_MESSAGE_SIZE, 0) == -1)) {
        perror("unable to register");
        exit(1);
    }

    if ((msgrcv(private_queue, &message, MAX_MESSAGE_SIZE, -TYPE_LAST, 0) == -1)) {
        perror("unable to register");
        exit(1);
    }

    sscanf(message.text, "%d", &id);
    printf("successfully registered with id %d, key: %u\n", id, private_key);


    sender_pid = fork();
    if(sender_pid == -1) {
        perror("cant fork");
        exit(1);
    } else if(sender_pid == 0)  {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT,&sa,NULL);

        sender();
    } else {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handle_sigint;
        sigaction(SIGINT,&sa,NULL);

        catcher();
    }

    return 0;
}