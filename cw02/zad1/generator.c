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

void sort_lib(const char *file_name, const unsigned int num_records, const unsigned int byte_num) { 
    FILE *f = fopen(file_name, "r+");

    if(f == NULL) {
        fprintf(stderr, "cant open file sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    char *tmp1 = malloc(byte_num);
    char *tmp2 = malloc(byte_num);
    
    for(int i = 0; i < num_records; ++i) {
        if (fseek(f, i * byte_num, SEEK_SET) < 0) {
            fprintf(stderr, "cant fseek sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (fread(tmp1, 1, byte_num, f) != byte_num) {
            fprintf(stderr, "cant read sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        unsigned char min_char = tmp1[0];
        int min_index = i;

        for(int j = i+1; j < num_records; ++j) {

            if (fread(tmp2, 1, byte_num, f) != byte_num) {
                fprintf(stderr, "cant read sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if(tmp2[0] < min_char) {
                min_char = tmp2[0];
                min_index = j;
            } 
        }

        if(min_index != i) {
            if (fseek(f, min_index * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if (fread(tmp2, 1, byte_num, f) != byte_num) {
                fprintf(stderr, "cant read sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if (fseek(f, i * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "cant fseek sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if (fwrite(tmp2, 1, byte_num, f) != byte_num) {
                fprintf(stderr, "error while writing to file sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if (fseek(f, min_index * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "error while fseek sort lib: %s\n", strerror(errno));
                exit(-1);
            }

            if (fwrite(tmp1, 1, byte_num, f) != byte_num) {
                fprintf(stderr, "error while fwrite sort lib: %s\n", strerror(errno));
                exit(-1);
            }
        }
    }
    free(tmp1);
    free(tmp2);
    fclose(f);
}

void sort_sys(const char *file_name, const unsigned int num_records, const unsigned int byte_num) {
    int f = open(file_name, O_RDWR);
    if (f < 0) {
        fprintf(stderr, "cant open file sort sys: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned char* block1 = malloc(byte_num);
    unsigned char* block2 = malloc(byte_num);

    for (int i = 0; i < num_records; ++i) {
        lseek(f, i * byte_num, SEEK_SET);

        if (read(f, block1, byte_num) < 0) {
            fprintf(stderr, "cant read file sort sys: %s\n", strerror(errno));
            exit(-1);
        }

        unsigned int min_index = i;
        unsigned char min_char = block1[0];

        for (int j = i + 1; j < num_records; ++j) {
            if (read(f, block2, byte_num) < 0) {
               fprintf(stderr, "cant read file sort sys: %s\n", strerror(errno));
                exit(-1);
            }

            if (block2[0] < min_char) {
                min_index = j;
                min_char = block2[0];
            }
        }

        if (min_index != i) {
            if (lseek(f, min_index * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "cant lseek file sort sys: %s\n", strerror(errno));
                exit(-1);
            }   

            if (read(f, block2, byte_num) < 0) {
                fprintf(stderr, "cant read file sort sys: %s\n", strerror(errno));
                exit(-1);
            }

            if (lseek(f, i * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "cant lread file sort sys: %s\n", strerror(errno));
                exit(-1);
            }

            if (write(f, block2, byte_num) < 0) {
                fprintf(stderr, "cant write file sort sys: %s\n", strerror(errno));
                exit(-1);
            }

            if (lseek(f, min_index * byte_num, SEEK_SET) < 0) {
                fprintf(stderr, "cant lseek file sort sys: %s\n", strerror(errno));
                exit(-1);
            }

            if (write(f, block1, byte_num) < 0) {
                fprintf(stderr, "cant write file sort sys: %s\n", strerror(errno));
                exit(-1);
            }
        }
    }

    free(block1);
    free(block2);
    close(f);
}

void copy_sys( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num){
    int from = open(file_from, O_RDONLY);
    if (from < 0) {
        fprintf(stderr, "cant open source file copy sys: %s\n", strerror(errno));
        exit(-1);
    }

    int to = open(file_to, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (to < 0) {
        fprintf(stderr, "cant open dest file copy sys: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned char* holder = malloc(byte_num);
    
    for(int i = 0; i < num_records; ++i) {
        if (read(from, holder, byte_num) < 0) {
            fprintf(stderr, "cant read from source file copy sys: %s\n", strerror(errno));
            exit(-1);
        }

        if (write(to, holder, byte_num) < 0) {
            fprintf(stderr, "cant write to dst file copy sys: %s\n", strerror(errno));
            exit(-1);
        }
    }

    free(holder);
    close(from);
    close(to);
}

void copy_lib( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num) {
    FILE* src_file = fopen(file_from, "r");
    if (src_file == NULL) {
        fprintf(stderr, "cant open source file copy lib: %s\n", strerror(errno));
        exit(-1);
    }

    FILE* dst_file = fopen(file_to, "w");
    if (dst_file == NULL) {
        fprintf(stderr, "cant open dst file copy lib: %s\n", strerror(errno));
        exit(-1);
    }

     unsigned char* holder = malloc(byte_num);

    for(int i = 0; i < num_records; ++i) {

        if (fread(holder, 1, byte_num, src_file) != byte_num) {
            fprintf(stderr, "cant read from source file copy lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (fwrite(holder, 1, byte_num, dst_file) != byte_num) {
            fprintf(stderr, "cant write to dst file copy lib: %s\n", strerror(errno));
            exit(-1);
        }
    }

    free(holder);
    fclose(src_file);
    fclose(dst_file);
}