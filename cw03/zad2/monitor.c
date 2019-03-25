#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "error.h"

#define MAX_FILE_NAME 4096
#define MAX_FILES 4096


char *monitored_files[MAX_FILES];
int monitored_intervals[MAX_FILES];
int num_files = 0;

void get_monitor_files(char *file_name) {
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        gen_error("cant open list file %s, errno: %s\n", file_name, strerror(errno));
    }

    while(1) {
        char *file_path = malloc(MAX_FILE_NAME);
        int interval;

        int res = fscanf(f, "%s %d\n", file_path, &interval);

        if(res == EOF)  {
            free(file_path);
            break;
        }
            
        if(res != 2) {
            gen_error("fscanf error: %s\n", strerror(errno));
        }

        monitored_files[num_files] = file_path;
        monitored_intervals[num_files] = interval;

        num_files++;
    }

    fclose(f);
}

void make_dir(char *path) {
    struct stat sb;
    if (lstat(path, &sb) < 0) {
        if (mkdir(path, 777) == -1) {
            gen_error("cant mkdir %s, errno: %s\n", path, strerror(errno));
        }
    } else if(!S_ISDIR(sb.st_mode)) {
        gen_error("path exists and isn't directory :%s\n", path);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        gen_error("wrong arguments, usage: [list] [timeout] [mode]");
    }

    char *file_name = argv[1];
    int sec_monitor = atoi(argv[2]);
    char *mode = argv[3];

    make_dir("archiwum");
    get_monitor_files(file_name);   

    for (int i = 0; i < monitor_size; ++i) {
        
    }

    return 0;
}