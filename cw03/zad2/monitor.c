#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "error.h"
#include "duplicator.h"

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

void make_dir(const char *path) {
    struct stat sb;
    if (lstat(path, &sb) < 0) {
        if (mkdir(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
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
    const char archive_path[] = "archiwum";
    char *file_name = argv[1];
    int monitor_timeout = atoi(argv[2]);
    char *mode_txt = argv[3];

    Mode mode = MEM;
    if(strcmp(mode_txt, "mem") == 0) {
        mode = MEM;
    } else if(strcmp(mode_txt, "cp") == 0) {
        mode = COPY;
    } else {
        gen_error("wrong mode\n");
    }

    make_dir(archive_path);
    get_monitor_files(file_name);   

    pid_t *ch_pids = malloc(sizeof(pid_t) * num_files);

    for (int i = 0; i < num_files; ++i) {
        pid_t child_pid = fork();

        if(child_pid == -1) {
            gen_error("cant fork: %s", strerror(errno));
        }
        if(child_pid > 0) {
            ch_pids[i] = child_pid;
        } else {
            monitor_file(monitored_files[i], archive_path, monitored_intervals[i], monitor_timeout, mode);
        }
    }

    sleep(monitor_timeout);

    for (int i = 0; i < num_files; ++i) {
        int status;
        waitpid(ch_pids[i], &status, 0);
        int copies = WEXITSTATUS(status);
        printf("PID: %d made %d copies of file %s\n", ch_pids[i], copies, monitored_files[i]);
        free(monitored_files[i]);
    }


    free(ch_pids);

    return 0;
}