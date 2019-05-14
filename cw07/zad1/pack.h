#ifndef PACK_H
#define PACK_H

#include <sys/time.h>

typedef struct pack_t
{
    int weight;
    int pid;
    struct timeval time;
} pack_t;

pack_t new_pack(int weight);


#endif