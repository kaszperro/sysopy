#define _XOPEN_SOURCE 500

#include "keygen.h"

#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>

key_t get_queue_sem_key() {
    int key;
    if ((key = ftok(getenv("HOME"), 1234)) == -1) {
         perror("cant generate public key");
         exit(1);
    }

    return key;
}

key_t get_queue_mem_key() {
    key_t key;
    if ((key = ftok(getenv("HOME"), 5678)) == -1) {
       perror("cant generate private key");
       exit(1);
    }

    return key;
}