#define _XOPEN_SOURCE 500

#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int running = 1;

void handler_SIGINT(int signum) {
    printf("received siginit - exiting\n");
    exit(0);
}

void handler_SIGTSTP(int signum) {
    if(running == 1) {
        running = 0;
        printf("Wating for CTRL+Z - continue or CTR+C - exit prigram\n");
    } else {
        running = 1;
    }
}

int main() {
    signal(SIGINT, handler_SIGINT);
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handler_SIGTSTP;
    sigaction(SIGTSTP, &sa, NULL);

    time_t t;
    struct tm tm;

    while(1) {
        if(running == 1) {
            t = time(0);
            tm = *localtime(&t);

            printf("%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900,
                    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
      
    }

    return 0;
}