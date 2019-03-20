#include "generator.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>



void generate(const char *file_name, const unsigned int num_records, const unsigned int byte_num){
    char buff[64];
    snprintf(buff, sizeof buff, "head -c %d /dev/urandom > %s", num_records * byte_num, file_name);
    int find_status = system(buff);
    if(find_status != 0) {
        fprintf(stderr, "error while generating: %s\n", strerror(errno));
        exit(-1);
    }
}


void sort(unsigned char * (*read_fun)(unsigned int, unsigned int, const char * ), 
            void (*write_fun)(unsigned int , unsigned char *, const char *, size_t ), const char *file_name, 
                const unsigned int num_records, const unsigned int byte_num){
                    

    for(int i = 0; i < num_records; ++i) {
        int min_index = i;
        unsigned char* min_char = read_fun(i*byte_num, i*byte_num, file_name);
       
        for(int j = i+1; j < num_records; ++j) {
            unsigned char *my_char = read_fun(j*byte_num, j*byte_num, file_name);
            
            if(my_char[0] < min_char[0]) {
                free(min_char);
                min_char = my_char;
                min_index = j;
            } else {
                free(my_char);
            }
        }
        free(min_char);
        //now swap
        unsigned char * my_record = read_fun(i*byte_num, (i+1)*byte_num-1, file_name);
        unsigned char * min_record = read_fun(min_index*byte_num, (min_index+1)*byte_num-1, file_name);

        write_fun(min_index*byte_num, my_record, file_name, byte_num);
        write_fun(i*byte_num, min_record, file_name, byte_num);

        free(my_record);
        free(min_record);
    }
}

unsigned char * read_sys_fun(unsigned int byte_start, unsigned int byte_end, const char * file_name) {
    int file = open(file_name, O_RDONLY); 

    if(file < 0) {
        fprintf(stderr, "error while reading sys: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned int bytes_to_read = byte_end-byte_start+1;
    unsigned char *buffer = calloc(bytes_to_read, sizeof(unsigned char));

    lseek(file, byte_start,0);
    read(file, buffer, bytes_to_read);

    close(file);

    return buffer;
}

void write_sys_fun (unsigned int position, unsigned char *text, const char *file_name, size_t bytes) {
    int file = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if(file < 0) {
        fprintf(stderr, "error while writing sys: %s\n", strerror(errno));
        exit(-1);
    }

    lseek(file, position, 0);

    write(file, text, bytes);

    close(file);
}

unsigned char * read_lib_fun(unsigned int byte_start, unsigned int byte_end, const char * file_name) {
    FILE *file = fopen(file_name, "r");

    if(file == NULL) {
        fprintf(stderr, "error while reading lib: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned int bytes_to_read = byte_end-byte_start+1;
    unsigned char *buffer = calloc(bytes_to_read, sizeof(unsigned char));

    fseek(file, byte_start, SEEK_SET);

    fread(buffer, sizeof(unsigned char), bytes_to_read, file);
    
    fclose(file);

    return buffer;
}

void write_lib_fun (unsigned int position, unsigned char *text, const char *file_name, size_t bytes) {
    FILE *file =  fopen(file_name, "r+");
    if(file == NULL) {
        file =  fopen(file_name, "a");
    }

    if(file == NULL) {
        fprintf(stderr, "error while writing lib: %s\n", strerror(errno));
        exit(-1);
    }

    fseek(file, position, SEEK_SET);

    fwrite(text, sizeof (unsigned char), bytes, file);

    fclose(file);
}


void sort_sys(const char *file_name, const unsigned int num_records, const unsigned int byte_num) { 
    sort(
        read_sys_fun,
        write_sys_fun,
        file_name,
        num_records,
        byte_num);
}

void sort_lib(const char *file_name, const unsigned int num_records, const unsigned int byte_num) {
    sort(
        read_lib_fun,
        write_lib_fun,
        file_name,
        num_records,
        byte_num);
}

void copy_sys( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num){
    for(int i = 0; i < num_records; ++i) {
        unsigned char *my_rec = read_sys_fun(i*byte_num, (i+1)*byte_num-1, file_from);
        if(my_rec == NULL) {
            fprintf(stderr, "error while copying sys: %s\n", strerror(errno));
            exit(-1);
        }
        write_sys_fun(i*num_records, my_rec, file_to, byte_num);
        free(my_rec);
    }
}

void copy_lib( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num){
    for(int i = 0; i < num_records; ++i) {
        unsigned char *my_rec = read_lib_fun(i*byte_num, (i+1)*byte_num-1, file_from);
        if(my_rec == NULL) {
            fprintf(stderr, "error while copying lib: %s\n", strerror(errno));
            exit(-1);
        }
        write_lib_fun(i*num_records, my_rec, file_to, byte_num);
        free(my_rec);
    }
}


void print_file(const char *file_name, const unsigned int num_records, const unsigned int byte_num) {
    for(int i = 0; i < num_records; ++i) {
        unsigned char* my_char = read_sys_fun(i*byte_num, (i+1)*byte_num-1, file_name);
        if(my_char == NULL) {
            fprintf(stderr, "error while printing file: %s\n", strerror(errno));
            exit(-1);
        }
        printf("record: %d, is: %s, first char num: %d\n", i, my_char, (int)my_char[0]);
        free(my_char);
    }
}