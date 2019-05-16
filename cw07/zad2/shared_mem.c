#define _XOPEN_SOURCE 500

#include "shared_mem.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int create_shared_mem(int key, size_t len) {
    char key_str[32];
    sprintf(key_str, "%d", key);
    int id = shm_open(key_str, O_RDWR | O_CREAT | O_EXCL, 0644);
 
    if(id == -1) {
        perror("cant crate shared memory");
        exit(1);
    }

    int err = ftruncate(id, len);
    if(err == -1) {
        perror("cant ftruncate shared memory");
        exit(1);
    }

    return id;
}


int get_shared_mem(int key) {
    char key_str[32];
    sprintf(key_str, "%d", key);
    int id = shm_open(key_str,  O_RDWR, 0);
    if(id == -1) {
        perror("cant get shared memory");
        exit(1);
    }

    return id;
}

void *attach_shared_mem(int id, size_t len) {
    void *ptr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, id, 0);
    if(ptr == (void*)-1){
        perror("cant attach shared memory");
        exit(1);
    }
    return ptr;
}

void detach_shared_mem(void* addr, size_t len) {
    if(munmap(addr, len) == -1) {
        perror("cant detach shared memory");
        exit(1);
    }
}

void free_shared_mem(int key, int id) {
    char key_str[32];
    sprintf(key_str, "%d", key);
    if(shm_unlink(key_str) == -1) {
        perror("cant free shared memory");
        exit(1);
    }
}


sem_id_t create_semaphore(int key) {
    char key_str[32];
    sprintf(key_str, "%d", key);

    sem_id_t id = sem_open(key_str, O_RDWR | O_CREAT | O_EXCL, 0644, 1);

    if (id == SEM_FAILED) {
        perror("cant create semaphore");
        exit(1);
    }

    return id;
}

sem_id_t open_semaphore(int key){
    char key_str[32];
    sprintf(key_str, "%d", key);

    sem_id_t id =  sem_open(key_str, O_RDWR);
    if (id == SEM_FAILED) {
        perror("cant open semaphore");
        exit(1);
    }

    return id;
}

void lock_semaphore(sem_id_t id){
    if (sem_wait(id) == -1) {
        perror("cant lock semaphore");
        exit(1);
    }
}

void unlock_semaphore(sem_id_t id){
    if (sem_post(id) == -1) {
        perror("cant unlock semaphore");
        exit(1);
    }
}

void close_semaphore(sem_id_t id){
    if( sem_close(id)==-1){
        perror("cant close semaphore");
        exit(1);
    }
}

void remove_semaphore(int key, sem_id_t id){
    char key_str[32];
    sprintf(key_str, "%d", key);

    if (sem_unlink(key_str) == -1) {
        perror("cant remove semaphore");
        exit(1);
    }
}