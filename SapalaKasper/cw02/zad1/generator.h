#ifndef GENERATOR_LIB_H
#define GENERATOR_LIB_H

int generate(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

int sort_sys(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

int sort_lib(const char *file_name, const unsigned int num_records, const unsigned int byte_num);

int copy_sys( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num);

int copy_lib( const char *file_from, const char * file_to, const unsigned int num_records, const unsigned int byte_num);

#endif