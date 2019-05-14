#ifndef QUEUE_H
#define QUEUE_H

#include "pack.h"
#include "shared_mem.h"

#define MAX_CAPACITY 256

#define QUEUE_SIZE sizeof(queue_t)

typedef struct queue_t
{
     int size;
    int capacity;

    int weight;
    int max_weight;

    int head;
    int tail;

    int is_truck;

    pack_t packs[MAX_CAPACITY];
} queue_t;

queue_t new_queue(int capacity, int max_weight);

int queue_push(queue_t *q, pack_t pack, sem_id_t sem);
pack_t* queue_pop(queue_t *q, sem_id_t sem, pack_t* pack);

#endif