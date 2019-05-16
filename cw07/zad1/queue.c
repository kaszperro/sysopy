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
    lock_semaphore(sem);

    int err = 0;
    if(q->weight + pack.weight > q->max_weight)
        err = -2;

    else if (q->size == q->capacity) {
        err = -1;
    } else {
        q->tail = (q->tail + 1) % q->capacity;
        q->packs[q->tail] = pack;
        q->size += 1;
        q->weight += pack.weight;
        err = 0;
    }

    unlock_semaphore(sem);
    return err;
}

pack_t* queue_pop_non_sem(queue_t *q, pack_t* pack) {
    if ( q->size == 0) {
        pack = NULL;
    } else {
        *pack = q->packs[q->head];
        q->head = (q->head + 1) % q->capacity;
        q->size -= 1;
        q->weight -= pack->weight;
    }
    return pack;
}

pack_t* queue_pop(queue_t *q, sem_id_t sem, pack_t* pack) {
    lock_semaphore(sem);

    pack = queue_pop_non_sem(q, pack);
    
    unlock_semaphore(sem);

    return pack;
}
