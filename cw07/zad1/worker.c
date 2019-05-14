#define _XOPEN_SOURCE 500

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "shared_mem.h"
#include "keygen.h"
#include "queue.h"

int pid;
int weight;

queue_t* q = NULL;
sem_id_t sem;


void clean() {
    close_semaphore(sem);
    detach_shared_mem(q, QUEUE_SIZE);
}

void place_pack() {
    if (q->is_truck == 0) {
        exit(0);
    }
    pack_t p = new_pack(weight);
    int err = queue_push(q, p, sem);
    if(err == 0) {
        printf("%d placed new pack at %ld.%ld weight: %d\n", pid, p.time.tv_sec, p.time.tv_usec, weight);
    } else if(err == -1) {
        printf("%d cant place pack, queue full\n", pid);
    } else if(err == -2) {
        printf("%d cant place pack, package to heavy\n", pid);
    }
}

void handle_sigint(int sig) {
    exit(0);
}

int main(int argc, char*argv[]) {
    signal(SIGINT, handle_sigint);

    int cycles = 0;

    if(argc == 2) {
        if (sscanf(argv[1], "%d", &weight) != 1) {
            fprintf(stderr, "wrong weight\n");
            exit(1);
        }
    } else if(argc == 3) {
        if (sscanf(argv[1], "%d", &weight) != 1) {
            fprintf(stderr, "wrong weight\n");
            exit(1);
        }
        if (sscanf(argv[2], "%d", &cycles) != 1) {
            fprintf(stderr, "wrong cycles\n");
            exit(1);
        }
    } else {
        fprintf(stderr, "wrong arguments, usage: [Weight, Cycles[optional]]\n");
        exit(1);
    }

    pid = getpid();

    int sh_id = get_shared_mem(get_queue_mem_key());
    q = attach_shared_mem(sh_id, QUEUE_SIZE);
    sem = open_semaphore(get_queue_sem_key());

    atexit(clean);

    if(cycles == 0) {
        while(1) {
            place_pack();
            usleep(1000);
        }
    } else {
        for(int i = 0; i < cycles; ++i) {
            place_pack();
            usleep(1000);
        }
        
    }


    return 0;
}