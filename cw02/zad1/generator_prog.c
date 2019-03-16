#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_generate(int argc, char *argv[], int i) {
    char * file_name = argv[i+1];
    int rec_num = atoi(argv[i+2]);
    int byte_num = atoi(argv[i+3]);
    
    generate(file_name, rec_num, byte_num);

    return i+4;
}

int parse_sort(int argc, char *argv[], int i) {
    char * file_name = argv[i+1];
    int rec_num = atoi(argv[i+2]);
    int byte_num = atoi(argv[i+3]);
    char * lib_sys = argv[i+4];

    if(!strcmp(lib_sys, "lib")) {
        sort_lib(file_name, rec_num, byte_num);
    } else if(!strcmp(lib_sys, "sys")) {
        sort_sys(file_name, rec_num, byte_num);
    }
    return i+5;
}

int parse_copy(int argc, char *argv[], int i) {
    char * file_from = argv[i+1];
    char * file_to = argv[i+2];
    int rec_num = atoi(argv[i+3]);
    int byte_num = atoi(argv[i+4]);
    char * lib_sys = argv[i+5];

    if(!strcmp(lib_sys, "lib")) {
        copy_lib(file_from, file_to, rec_num, byte_num);
    } else if(!strcmp(lib_sys, "sys")) {
        copy_sys(file_from, file_to, rec_num, byte_num);
    }
    return i+6;
}

int main(int argc, char *argv[]) {
    int i = 1;
    while(i < argc) {
        if(!strcmp(argv[i], "generate")) {
            i = parse_generate(argc, argv, i);
        } else if(!strcmp(argv[i], "sort")) {
            i = parse_sort(argc, argv, i);
        } else if(!strcmp(argv[i], "copy")) {
            i = parse_copy(argc, argv, i);
        } else {
            fprintf(stderr,"Wrong command\n");
            return -1;
        }
    }
    return 0;
}