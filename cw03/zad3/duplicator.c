#define _XOPEN_SOURCE 500

#include "duplicator.h"
#include "error.h"

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
#include <libgen.h>
#include <sys/time.h>
#include <sys/resource.h>

char *file_buff = NULL;
long file_size = 0;

char *get_duplicate_path(char *file_path, const char *archive_path, time_t modif_time) {
    struct tm* timeinfo;
    char* ret_path = malloc(4096);

    timeinfo = localtime(&modif_time);

    char date[80];
    strftime(date, sizeof(date), "%Y-%m-%d_%H-%M-%S", timeinfo);

    sprintf(ret_path, "%s/%s_%s", archive_path, basename(file_path), date);
    return ret_path;
}


void get_file_buff(char * file_path) {
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "cant open file: %s: %s\n", file_path, strerror(errno));
        return;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    file_buff = realloc(file_buff, file_size);
    rewind(file);
    fread(file_buff, file_size, 1, file);
    fclose(file);
}


void write_buff_to_file(char *file_path) {
    FILE* file = fopen(file_path, "wb");
    if (file == NULL) {
        gen_error("cant open file: %s: %s\n", file_path, strerror(errno));
    }

    fwrite(file_buff, file_size, 1, file);
    fclose(file);
}

void duplicate_mem(char * file_path, const char * archive_path, time_t modif_time) {
    char* duplicate_path = get_duplicate_path(file_path, archive_path, modif_time);

    if(file_buff != NULL) {
        write_buff_to_file(duplicate_path);
    }

    get_file_buff(file_path);

    free(duplicate_path);
}

void duplicate_copy(char * file_path, const char * archive_path, time_t modif_time) {
    char* duplicate_path = get_duplicate_path(file_path, archive_path, modif_time);

    pid_t child_pid = fork();
    int status;

    if (child_pid == -1) {
        gen_error("unable to fork: %s\n", strerror(errno));
    } else if (child_pid > 0) {
        waitpid(child_pid, &status, 0);
        if(status != 0) {
            fprintf(stderr, "exec copy failed\n");
        }
    } else {
        exit(execlp("cp", "cp", file_path, duplicate_path, NULL));
    }
    if(status == 0) {
        printf("CP\tPID: %d\t%s -> %s\n", getpid(), file_path, duplicate_path);
    }
    
    free(duplicate_path);
}

double get_seconds(struct timeval t) {
    return t.tv_sec + t.tv_usec / 1000000.0;
}

void monitor_file(char *file_path, const char *archive_path, int interval, int timeout, Mode mode, int cpu_limit, int mem_limit) {
    struct rlimit cpu_rlimit, mem_rlimit;

    cpu_rlimit.rlim_cur = cpu_limit;
    cpu_rlimit.rlim_max = cpu_limit;
    mem_rlimit.rlim_cur = mem_limit * 1048576;
    mem_rlimit.rlim_max = mem_limit * 1048576;

    if (setrlimit(RLIMIT_CPU, &cpu_rlimit) < 0) {
        perror("cant set cpu limit");
        exit(0);
    }
    if (setrlimit(RLIMIT_AS, &mem_rlimit) < 0) {
        perror("cant set memory limit");
        exit(0);
    }

    pid_t pid = getpid();

    time_t last_modif;
    struct stat sb;

    if (lstat(file_path, &sb) < 0) {
        fprintf(stderr, "PID: %d unable to lstat file %s\n", pid, file_path);
        exit(0);
    }

    last_modif = sb.st_mtime;

    if(mode == COPY) {
        duplicate_copy(file_path, archive_path, last_modif);
    } else {
        duplicate_mem(file_path, archive_path, last_modif);
    }
    int num_copies = 0;
    time_t start = time(NULL);
    while(difftime(time(NULL), start) < timeout){
        if (lstat(file_path, &sb) < 0) {
            fprintf(stderr, "PID: %d unable to lstat file %s\n", pid, file_path);
            exit(0);
        }

        if(sb.st_mtime != last_modif) {
            last_modif = sb.st_mtime;
            if(mode == COPY) {
                duplicate_copy(file_path, archive_path, last_modif);
            } else {
                duplicate_mem(file_path, archive_path, last_modif);
            }
            num_copies++;
        }

        sleep(interval);
    }

    if(mode == MEM) {
        free(file_buff);
    } 

    struct rusage rusage;

    getrusage(RUSAGE_SELF, &rusage);
    printf("PID: %d SELF\tuser time: %f, sys time: %f\n", getpid(), get_seconds(rusage.ru_utime), get_seconds(rusage.ru_stime));
    getrusage(RUSAGE_CHILDREN, &rusage);
    printf("PID: %d CHILDREN\tuser time: %f, sys time: %f\n", getpid(), get_seconds(rusage.ru_utime), get_seconds(rusage.ru_stime));

    exit(num_copies);
}