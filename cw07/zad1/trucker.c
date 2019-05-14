#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "shared_mem.h"
#include "queue.h"
#include "keygen.h"

queue_t* q;

int mem_id;

sem_id_t q_sem;

void clean() {
    q->is_truck=0;


    printf("clening queue\n");
    pack_t pack;
    while( q->size > 0){
         queue_pop(q, q_sem, &pack);
         printf("usunalem\n");
    }
    printf("queue cleand, leaving\n");


    detach_shared_mem(q, QUEUE_SIZE);
    free_shared_mem(get_queue_mem_key(), mem_id);
    remove_semaphore(get_queue_mem_key(), q_sem);
}

void handle_sigint(int sig) {
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "wrong arugments, usage: [truck_capacity, queue_size, queue_max_weight]\n");
        exit(1);
    }

    atexit(clean);

    signal(SIGINT, handle_sigint);

    int truck_capacity;
    int queue_size;
    int queue_max_weight;

    if (sscanf(argv[1], "%d", &truck_capacity) != 1 ||
      sscanf(argv[2], "%d", &queue_size) != 1 ||
      sscanf(argv[3], "%d", &queue_max_weight) != 1) {
        perror("cant parse arguments");
    }


    mem_id = create_shared_mem(get_queue_mem_key(), QUEUE_SIZE);
    q = attach_shared_mem(mem_id, QUEUE_SIZE);

    q_sem = create_semaphore(get_queue_sem_key());
    *q = new_queue(queue_size, queue_max_weight);

    int weight = 0;
    pack_t pack;
    struct timeval time;

    printf("new truck arrived\n");
    while(1) {
        usleep(500);
        if(queue_pop(q, q_sem, &pack)==NULL) {
            printf("queue is empty\n");
            continue;
        }

        if(weight + pack.weight > truck_capacity) {
            lock_semaphore(q_sem);

            printf("truck is full, leaving...\n");

            sleep(1);

            printf("new truck arrived\n");
            weight = 0;

            unlock_semaphore(q_sem);
        }

        if(pack.weight > truck_capacity) {
            fprintf(stderr, "pack has is heavier than truck capacity\n");
            exit(1);
        }
        
        weight += pack.weight;
        
        gettimeofday(&time, NULL);
        double seconds = (time.tv_sec - pack.time.tv_sec)  +
                        (time.tv_usec - pack.time.tv_usec)/1000000.0;


        printf("pack loaded  PID %d\tTIME %f\tPACK WEIGHT %d\tTRUCK WEIGHT %d\tTRUCK SPACE LEFT %d\n",
        pack.pid, seconds, pack.weight, weight, truck_capacity - weight);
      
    }

    return 0;
}