#ifndef FILES_LIB_H
#define FILES_LIB_H

#include <time.h>
#include <stdlib.h>
#include <ftw.h>

void print_files(char *root_path, char mode, time_t date, void (*print_fun)( const char * , const struct stat * ));

#endif
