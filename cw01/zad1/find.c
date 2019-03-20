#include "find.h"
#include <stdio.h>
#include <stdlib.h>

char **blocks_array = NULL;
unsigned int num_blocks = 0;
unsigned int initialized = 0;


char *search_dir = NULL;
char *file_search = NULL;
char *tmp_name = NULL;

int create_table(unsigned int size) {
    if(initialized == 1) {
        fprintf(stderr, "library already initialized\n");
        return -1;
    }
    num_blocks = size;
    blocks_array = calloc(num_blocks, sizeof(char*));

    initialized = 1;
    return 0;
}

int set_search_directory(char *dir, char *file, char *tmp_file_name) {
    search_dir = dir;
    file_search = file;
    tmp_name = tmp_file_name;
    return 0;
}

int search_directory() {
    char buf[256];
    snprintf(buf, sizeof buf, "find %s -name %s > %s", search_dir, file_search, tmp_name);
    int find_status = system(buf);
    if(find_status != 0)
        return find_status;
        
    return 0;
}

long get_file_size(FILE *f) {
    fseek(f, 0L, SEEK_END);
    long file_size = ftell(f);
    rewind(f);
    return file_size;
}

int load_file(char ** buffer, char *file_path) {
    FILE * f = fopen (file_path, "rb");
    if(!f) {
        fprintf(stderr, "problem with reading file\n");
        return -1;
    }
    
    long file_size = get_file_size(f);
    *buffer = calloc(file_size, sizeof(char));
    fread(*buffer, sizeof(char), file_size, f);

    fclose(f);

    return 0;
}

int last_search_to_array() {
    return file_to_array(tmp_name);
}

int file_to_array(char *file_name) {
    for(int i = 0; i < num_blocks; ++i) {
        if(blocks_array[i] == NULL) {
            
            int load_status = load_file(&blocks_array[i], file_name);
            if(load_status < 0) {
                fprintf(stderr, "cant load into array, error\n");
                return load_status;
            }
            return i;
        }
    }
    fprintf(stderr, "cant load into array, array is full\n");
    return -1;
}


char *get_block(int idx) {
    return blocks_array[idx];
}

int delete_block(int idx) {
    free(blocks_array[idx]);
    blocks_array[idx] = NULL;
    return 0;
}

void delete_array() {
    for(int i = 0; i < num_blocks; ++i) {
        delete_block(i);
    }
    free(blocks_array);
}