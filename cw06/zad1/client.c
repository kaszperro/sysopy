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

int server_queue, private_queue;
int id;

int sender_pid = 0;


void send_to_server(message_t* message) {
  if (send(server_queue, message)) {
    perror("unable to send message to server");
  }
}


void clean() {
    if(sender_pid != 0) {
        message_t message;
        message.type = TYPE_STOP;
        message.id = id;

        send_to_server(&message);

        kill(sender_pid, SIGKILL);
        close_queue(server_queue);
        delete_queue(private_queue, get_private_key());  
    }
   
}


void handle_sigint(int sig) {
    exit(0);
}

void handle_list() {
    message_t message;
    message.type = TYPE_LIST;
    message.id = id;
    send_to_server(&message);
}

void handle_echo(char *text) {
    message_t message;
    message.type = TYPE_ECHO;
    message.id = id;
   
    strcpy(message.text, text);

    send_to_server(&message);
}

void handle_2all(char *rest) {
    if(rest[0] == '\0') {
        fprintf(stderr, "provide message\n");
        return;
    }

    message_t message;
    message.type = TYPE_2ALL;
    message.id = id;

    strcpy(message.text, rest);
    send_to_server(&message);
}

void handle_2friends(char *rest) {
    if(rest[0] == '\0') {
        fprintf(stderr, "provide message\n");
        return;
    }

    message_t message;
    message.type = TYPE_2FRIENDS;
    message.id = id;

    strcpy(message.text, rest);
    send_to_server(&message);
}

void handle_2one(char *rest) {
    if(rest[0] == '\0') {
        fprintf(stderr, "provide id\n");
        return;
    }

    char to_id_str[32];
    char text[256];
    separate_command(rest, to_id_str, text);
    if(text[0] == '\0') {
        fprintf(stderr, "provide message\n");
        return;
    }

    message_t message;
    message.type = TYPE_2ONE;
    message.id = id;

    strcpy(message.text, rest);
    send_to_server(&message);
}

void hanlde_stop() {
    kill(getppid(), SIGINT);
}

void handle_friends(char *fr_list) {
    message_t message;
    message.type = TYPE_FRIENDS;
    message.id = id;
    strcpy(message.text, fr_list);
    send_to_server(&message);
}

void handle_add(char *fr_list) {
    message_t message;
    message.type = TYPE_ADD;
    message.id = id;
    strcpy(message.text, fr_list);
    send_to_server(&message);
}

void handle_del(char *fr_list) {
    message_t message;
    message.type = TYPE_DEL;
    message.id = id;
    strcpy(message.text, fr_list);
    send_to_server(&message);
}

void sender_handle_line(char *command, char*rest) {

    if(strcmp("LIST", command) == 0) {
        handle_list();
    } else if(strcmp("ECHO", command ) == 0) {
        handle_echo(rest);
    } else if(strcmp("2FRIENDS", command) == 0) {
         handle_2friends(rest);
    } else if(strcmp("2ALL", command) == 0 ) {
        handle_2all(rest);
    } else if(strcmp("2ONE", command) == 0 ) {
        handle_2one(rest);
    } else if(strcmp("STOP", command) == 0 ) {
        hanlde_stop();
    } else if(strcmp("FRIENDS", command) == 0) {
        handle_friends(rest);
    } else if(strcmp("ADD", command) == 0) {
        handle_add(rest);
    } else if(strcmp("DEL", command) == 0) {
        handle_del(rest);
    }
    
}

void sender() {
    char line[256];
    char command[256];
    char rest[256];
    while(1){
        fgets(line, 1024, stdin);
        separate_command(line, command, rest);
        if(strcmp("READ", command) == 0) {
            int lines_count = 0;
            char *lines[256];

            get_lines_from_file(rest, lines, &lines_count);

            printf("jest: %d lini\n", lines_count);

            for(int i = 0; i < lines_count; ++i) {
                separate_command(lines[i], command, rest);
                sender_handle_line(command, rest);
            }

        } else {
            sender_handle_line(command, rest);
        }
    }
    
}

void catcher() {
    message_t message;
    while(1){
        if ( receive(private_queue, &message) == -1) {
             if(errno != EINTR) {
                perror("cant receive message");
                exit(1);    
            }
        }
        printf("------\n");
        switch (message.type) {
            case TYPE_STOP:
                printf("exiting\n");
                exit(0);
                break;

            case TYPE_LIST:
                printf("%s\n", message.text);
                break;

            case TYPE_ECHO: {
                char* tim_info= ctime (&message.timestamp);
                printf("time: %s%s\n", tim_info, message.text);
                break;
            }
            case TYPE_2ALL:
            case TYPE_2FRIENDS:
            case TYPE_2ONE: {
                char* tim_info= ctime (&message.timestamp);
                printf("time: %sfrom: %d, message: %s\n", tim_info, message.id, message.text);
            }
            default:
                break;
        }

        printf("------\n");

        
    }
}

void initialize() {
    if ((server_queue = get_queue(get_public_key())) == -1) {
        perror("cant open server queue");
        exit(1);
    }


    key_t private_key = get_private_key();
    if ((private_queue = create_queue(private_key)) == -1) {
        perror("cant create private queue");
        exit(1);
    }



    message_t message;
    message.type = TYPE_INIT;
    sprintf(message.text, "%d", private_key);


    if ( send(server_queue, &message) == -1) {
        perror("unable to register");
        exit(1);
    }

    if (  receive(private_queue, &message)  == -1) {
        perror("unable to register");
        exit(1);
    }

    id = message.id;
    printf("successfully registered with id %d\n", id);
}


int main(int argc, char* argv[]) {
    atexit(clean);
   
    initialize();

    
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