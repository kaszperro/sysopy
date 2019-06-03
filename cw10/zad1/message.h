#pragma once

#define MESSAGE_ANSWER 1
#define MESSAGE_DISCONNECT 2
#define MESSAGE_PING 3
#define MESSAGE_NAME_TAKEN 4
#define MESSAGE_REGISTERED 5

#define MAX_TEXT_SIZE 1000


typedef struct message_t {
    int type;
    char text[MAX_TEXT_SIZE];
} message_t;
