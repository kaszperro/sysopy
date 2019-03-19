#define _XOPEN_SOURCE 500

#include "files.h"
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

char follow_mode;
time_t follow_date;
int (*my_print_fun)( const char * , const struct stat *);

int file_info(const char *filename, const struct stat *statptr,
    int fileflags, struct FTW *pfwt) {

    time_t modif_time = statptr->st_mtime;

    int diff = difftime(modif_time, follow_date);
    
    if (!( (diff == 0 && follow_mode == '=')
            || (diff > 0 && follow_mode == '>')
            || (diff < 0 && follow_mode == '<') )) 
        return 0;

    return my_print_fun(filename, statptr);
}

int print_files(char *root_path, char mode, time_t date, int (*print_fun)( const char *, const struct stat * )){
    int flags = FTW_PHYS;
    int fd_limit = 5;
    follow_mode = mode;
    follow_date = date;

    my_print_fun = print_fun; 

    return nftw(root_path,
        file_info,
        fd_limit, flags);
}