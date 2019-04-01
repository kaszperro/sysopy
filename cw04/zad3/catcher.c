#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum Mode {
    KILL, QUEUE, SIGRT
} Mode;

int COUNT_SIGNAL;
int END_SIGNAL;

Mode mode;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "wrong arguments, usage: [mode]\n");
        exit(1);
    }

    char *mode_str = argv[1];
    
    if(strcmp("kill", mode_str) == 0) {
        mode = KILL;
        COUNT_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    } else if(strcmp("queue", mode_str) == 0) {
        mode = QUEUE;
        COUNT_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    } else if(strcmp("sigrt", mode_str) == 0) {
        mode = SIGRT;
        COUNT_SIGNAL = SIGRTMIN;
        END_SIGNAL = SIGRTMAX;
    } else {
        fprintf(stderr, "wring mode, allowed: [kill, queue, sigrt]\n");
        exit(1);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, COUNT_SIGNAL);
    sigdelset(&mask, END_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("cant block signals\n");
        exit(1);
    }

    printf("created catcher with PID: %d\n", getpid());

    return 0;
}