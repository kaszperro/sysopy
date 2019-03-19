#define _XOPEN_SOURCE 500

#include "files.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>


int print_files(char *root_path, char mode, time_t date, int (*print_fun)( const char * , const struct stat* )){
    if (root_path == NULL)
        return 0;

    DIR *dir = opendir(root_path);

    if (dir == NULL) {
        fprintf(stderr, "dir error\n");
        return -1;
    }
    struct dirent* file;

    char new_path[256];

    while ((file = readdir(dir)) != NULL) {
        strcpy(new_path, root_path);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat sb;      
        
        if (lstat(new_path, &sb) < 0) {
            fprintf(stderr, "unable to lstat file %s", new_path);
            return -1;
            //perr("unable to lstat file %s", file->d_name);
        }


        if (S_ISDIR(sb.st_mode)) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }

            print_files(new_path, mode, date, print_fun);
        }



        time_t modif_time = sb.st_mtime;

        int diff = difftime(modif_time, date);
        
        if (!( (diff == 0 && mode == '=')
                || (diff > 0 && mode == '>')
                || (diff < 0 && mode == '<') )) 
           continue;


        print_fun(new_path, &sb);

    }

    return 0;

}