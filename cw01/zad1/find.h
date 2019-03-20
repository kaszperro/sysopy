#ifndef LAB1_FIND_H
#define LAB1_FIND_H
#pragma once

int create_table(unsigned int size);
int set_search_directory(char *dir, char *file, char *tmp_file_name);
int search_directory();
int last_search_to_array();
int file_to_array(char *file_name);
char *get_block(int idx);
int delete_block(int idx);
void delete_array();


#endif //LAB1_FIND_H