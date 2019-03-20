#define _XOPEN_SOURCE 500

#include "files.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


const char format[] = "%Y-%m-%d %H:%M:%S";

void print_from_stat(const char * filename, const struct stat* statptr) {
    char file_type[64] = "undefined";

    if(S_ISREG(statptr->st_mode))    
        strcpy(file_type, "file");
    else if(S_ISDIR(statptr->st_mode))
        strcpy(file_type, "dir");
    else if(S_ISLNK(statptr->st_mode))
        strcpy(file_type, "slink");
    else if(S_ISCHR(statptr->st_mode))
        strcpy(file_type, "char dev");
    else if(S_ISBLK(statptr->st_mode))
        strcpy(file_type, "block dev");
    else if(S_ISFIFO(statptr->st_mode))
        strcpy(file_type, "fifo");
    else if(S_ISSOCK(statptr->st_mode))
        strcpy(file_type, "socket");

    struct tm tm_modif_time;
    localtime_r(&statptr->st_mtime, &tm_modif_time);
    char modif_time_str[255];
    strftime(modif_time_str, 255, format, &tm_modif_time);

    
    struct tm tm_access_time;
    localtime_r(&statptr->st_atime, &tm_access_time);
    char access_time_str[255];
    strftime(access_time_str, 255, format, &tm_access_time);


     printf("%s || type: %s, size: %ld, modification time: %s, access time: %s\n", 
            filename, file_type, statptr->st_size, modif_time_str, access_time_str);
}

int main(int argc, char *argv[]) {
    const char format[] = "%Y-%m-%d %H:%M:%S";
    struct tm *timestamp = malloc(sizeof(struct tm));
    char *date = argv[3];

    strptime(date, format, timestamp);

    print_files(argv[1], argv[2][0], mktime(timestamp), print_from_stat);

    return 0;
}
