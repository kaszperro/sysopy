#define _XOPEN_SOURCE 500

#include "keygen.h"

#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>

key_t get_public_key() {
    key_t public_key;
    if ((public_key = ftok(getenv("HOME"), 1)) == -1) {
         perror("cant generate public key");
    }

    return public_key;
}

key_t get_private_key() {
    key_t private_key;
    if ((private_key = ftok(getenv("HOME"), getpid())) == -1) {
       perror("cant generate private key");
    }

    return private_key;
}