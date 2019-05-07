#define _XOPEN_SOURCE 500

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


#include "keygen.h"
#include "message.h"
#include "types.h"


int send(int queue, message_t* message) {
  return msgsnd(queue, message, MAX_MESSAGE_SIZE, 0);
}

int receive(int queue, message_t* message) {
  return msgrcv(queue, message, MAX_MESSAGE_SIZE, -TYPE_LAST, 0);
}

int receive_nowait(int queue, message_t* message) {
  return msgrcv(queue, message, MAX_MESSAGE_SIZE, -TYPE_LAST, IPC_NOWAIT);
}

int create_queue(int key) {
  return msgget(key, IPC_CREAT | IPC_EXCL | 0600);
}


int delete_queue(int queue) {
    return msgctl(queue, IPC_RMID, NULL);
}

int get_queue(int key) {
    return msgget(key, 0);
}
