#ifndef MESSAGE_H
#define MESSAGE_H

//#define MESSAGE_BUFFER_SIZE 1024

typedef struct message_t {
  long type;
  char text[1024];
} message_t;

#define MAX_MESSAGE_SIZE sizeof(message_t) - sizeof(long)

#endif