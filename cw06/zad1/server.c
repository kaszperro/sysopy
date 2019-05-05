#define _XOPEN_SOURCE 500

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "keygen.h"

int queue;

void clean() {
    msgctl(queue, IPC_RMID, NULL);
}

void handle_sigint(int sig) {
    exit(0);
}

int main(int argc, char* argv[]) {
    atexit(clean);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT,&sa,NULL);
  

    key_t key = get_public_key();
    if ((queue = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("cant create queue");
    }

    
    struct msgbuf message;

    while (1) {
        if (msgrcv(queue, &message, 1024, -10, 0) == -1) {
            perr("unable to receive message");
        }

        printf("%ld: %s\n", message.type, message.buffer);
    }

    return 0;
}