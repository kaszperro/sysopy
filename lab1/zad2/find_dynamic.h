#ifndef FIND_DYN_H
#define FIND_DYN_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void* handle = NULL;

int (*_create_table)(unsigned int);
int (*_set_search_directory)(const char*, const char*, const char*);
int (*_search_directory)();
int (*_last_search_to_array)();
int (*_file_to_array)(const char*);
char* (*_get_block)(const int);
int (*_delete_block)(const int);
void (*_delete_array)();

void init_dynamic_library() {
    handle = dlopen("libfind.so", RTLD_NOW);
    if (handle == NULL) {
        fprintf(stderr, "Error dymianc library open");
        return;
    }


    _create_table = dlsym(handle, "create_table");
    _set_search_directory = dlsym(handle, "set_search_directory");
    _search_directory = dlsym(handle, "search_directory");
    _last_search_to_array = dlsym(handle, "last_search_to_array");
    _file_to_array = dlsym(handle, "file_to_array");
    _get_block = dlsym(handle, "get_block");
    _delete_block = dlsym(handle, "delete_block");
    _delete_array = dlsym(handle, "delete_array");

    char* error;
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}

int create_table(unsigned int size){
    return (*_create_table)(size);
}
int set_search_directory(const char *dir, const char *file, const char *tmp_file_name){
    return (*_set_search_directory)(dir, file, tmp_file_name);
}
int search_directory(){
    return (*_search_directory)();
}
int last_search_to_array(){
    return (*_last_search_to_array)();
}
int file_to_array(const char *file_name){
    return (*_file_to_array)(file_name);
}
char *get_block(const int idx){
    return (**_get_block)(idx);
}
int delete_block(const int idx){
    return (*_delete_block)(idx);
}
void delete_array(){
    (*_delete_array)();
}

#endif