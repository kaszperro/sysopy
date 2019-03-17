#ifndef FILES_LIB_H
#define FILES_LIB_H

#include <time.h>
#include <stdlib.h>
#include <ftw.h>

int print_files(char *root_path, char mode, time_t date, int (*print_fun)( const char * , const struct stat ));

#endif
