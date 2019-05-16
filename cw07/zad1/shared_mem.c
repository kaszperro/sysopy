#define _XOPEN_SOURCE 500

#include "shared_mem.h"
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>

int create_shared_mem(int key, size_t len) {
    int id = shmget(key, len, IPC_CREAT | IPC_EXCL | 0644);
    if(id == -1) {
        perror("cant crate shared memory");
        exit(1);
    }

    return id;
}


int get_shared_mem(int key) {
    int id = shmget(key, 0, 0);
    if(id == -1) {
        perror("cant get shared memory");
        exit(1);
    }

    return id;
}

void *attach_shared_mem(int id, size_t len) {
    void *ptr = shmat(id, NULL, 0);
    if(ptr == (void*)-1){
        perror("cant attach shared memory");
        exit(1);
    }
    return ptr;
}

void detach_shared_mem(void* addr, size_t len) {
    if(shmdt(addr) == -1) {
        perror("cant detach shared memory");
        exit(1);
    }
}

void free_shared_mem(int key, int id) {
    if(shmctl(id, IPC_RMID, NULL) == -1) {
        perror("cant free shared memory");
        exit(1);
    }
}


sem_id_t create_semaphore(int key) {
    sem_id_t id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (id == -1) {
        perror("cant create semaphore");
        exit(1);
    }

    if (semctl(id, 0, SETVAL, 1) == -1) {
        perror("unable to initialize semaphore");
        exit(1);
    }

    return id;
}

sem_id_t open_semaphore(int key){
    sem_id_t id = semget(key, 1, 0);
    if (id == -1) {
        perror("cant open semaphore");
        exit(1);
    }

    return id;
}

void lock_semaphore(sem_id_t id){
    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = -1;
    sbuf.sem_flg = 0;

    if (semop(id, &sbuf, 1) == -1) {
        perror("cant lock semaphore");
        exit(1);
    }
}

void unlock_semaphore(sem_id_t id){
    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = 1;
    sbuf.sem_flg = 0;

    if (semop(id, &sbuf, 1) == -1) {
        perror("cant unlock semaphore");
        exit(1);
    }
}

void close_semaphore(sem_id_t id){}

void remove_semaphore(int key, sem_id_t id){
    if (semctl(id, 0, IPC_RMID) == -1) {
        perror("unable to remove semaphore");
        exit(1);
    }
}