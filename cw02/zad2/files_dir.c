#include "files.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>

int print_files(char *root_path, char mode, time_t date, int (*print_fun)( const char * , const struct stat )){
    if (root_path == NULL)
        return 0;

    DIR *dir = opendir(root_path);

    if (dir == NULL) {
        fprintf(stderr, "dir error\n");
        return -1;
    }
}