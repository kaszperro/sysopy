#include "queue.h"
#include "shared_mem.h"
#include <stdio.h>

#include <stdlib.h>

queue_t new_queue(int capacity, int max_weight) {
    queue_t q;
    q.size = 0;
    q.capacity = capacity;
    q.weight = 0;
    q.max_weight = max_weight;
    q.head = 0;
    q.tail = capacity - 1;
    q.is_truck = 1;
    return q;
}

int queue_push(queue_t *q, pack_t pack, sem_id_t sem) {
    if(q->weight + pack.weight > q->max_weight)
        return -2;

    lock_semaphore(sem);

    if(q->is_truck == 0) {
        printf("queue not truck\n");
        unlock_semaphore(sem);
        exit(0);
    }
        
    
    int err_code = 0;

    if (q->size == q->capacity) {
        err_code = -1;
    } else {
        q->tail = (q->tail + 1) % q->capacity;
        q->packs[q->tail] = pack;
        q->size += 1;
        q->weight += pack.weight;
        err_code = 0;
    }

    unlock_semaphore(sem);
    return err_code;
}

pack_t* queue_pop(queue_t *q, sem_id_t sem, pack_t* pack) {
    lock_semaphore(sem);
   
    if ( q->size == 0) {
        pack = NULL;
    } else {
        *pack = q->packs[q->head];
        q->head = (q->head + 1) % q->capacity;
        q->size -= 1;
        q->weight -= pack->weight;
    }

    unlock_semaphore(sem);
    return pack;
}
