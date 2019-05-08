#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "message.h"
#include "types.h"

int send(int queue, message_t* message) {
  return mq_send(queue, (char*)message, ALL_MESSAGE_SIZE, message->type);
}

int receive(int queue, message_t* message) {
 return mq_receive(queue, (char*)message, ALL_MESSAGE_SIZE, NULL);
}

int get_queue(int key) {
  char que_path[32];
  sprintf(que_path, "/%d", key);

  return mq_open(que_path, O_WRONLY);
}

int create_queue(int key) {
  char path[32];
  sprintf(path, "/%d", key);

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = ALL_MESSAGE_SIZE;
  attr.mq_curmsgs = 0;

  return mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
}


int delete_queue(int queue, int key) {
  if(mq_close(queue) == -1)
    return -1;

  char que_path[32];
  sprintf(que_path, "/%d", key);

  return mq_unlink(que_path);
}




int close_queue(int queue) {
  return mq_close(queue);
}