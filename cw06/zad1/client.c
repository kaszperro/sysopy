#define _XOPEN_SOURCE 500

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "keygen.h"
#include "message.h"

int server_queue, private_queue;

void clean() {
    msgctl(private_queue, IPC_RMID, NULL);
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
  
    if ((server_queue = msgget(get_public_key(), 0)) == -1) {
        perror("cant open server queue");
    }


    key_t private_key = get_private_key();
    if ((private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perror("cant create private queue");
    }

    

    return 0;
}