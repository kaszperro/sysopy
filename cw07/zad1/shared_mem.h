#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <semaphore.h>
#include <stddef.h>

#ifdef POSIX
typedef sem_t* sem_id_t;
#else
typedef int sem_id_t;
#endif

int create_shared_mem(int key, size_t len);
int get_shared_mem(int key);
void *attach_shared_mem(int id, size_t len);
void detach_shared_mem(void* addr, size_t len) ;
void free_shared_mem(int key, int id);

sem_id_t create_semaphore(int key);
sem_id_t open_semaphore(int key);
void lock_semaphore(sem_id_t id);
void unlock_semaphore(sem_id_t id);
void close_semaphore(sem_id_t id);
void remove_semaphore(int key, sem_id_t id);

#endif
