#include "pack.h"

#include <sys/time.h>
#include <unistd.h>

pack_t new_pack(int weight) {
    pack_t pa;
    pa.pid = getpid();
    pa.weight = weight;
    gettimeofday(&pa.time, NULL);
    return pa;
}