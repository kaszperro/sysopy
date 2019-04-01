#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

pid_t child_pid = -1;

void handler_SIGINT(int signum) {
    printf("received siginit - exiting\n");
    if(child_pid != -1) {
        kill(child_pid, SIGKILL);
        child_pid = -1;
    }
    exit(0);
}

void fork_dates() {
    child_pid = fork();
    if(child_pid < 0) {
        printf("unable to fork\n");
        exit(1);
    } else if(child_pid == 0) {
        execlp("bash", "bash", "shell_date.sh", NULL);
    } else {
        printf("fork PID: %d\n", child_pid);
    }
}

void handler_SIGTSTP(int signum) {
    if(child_pid != -1) {
        printf("Wating for CTRL+Z - continue or CTR+C - exit prigram\n");
        kill(child_pid, SIGKILL);
        child_pid = -1;
    } else {
        printf("continuing\n");
        fork_dates();
    }
}

int main() {
    signal(SIGINT, handler_SIGINT);
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handler_SIGTSTP;
    sigaction(SIGTSTP, &sa, NULL);

    fork_dates();

    while(1) {  }

    return 0;
}