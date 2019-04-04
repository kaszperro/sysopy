#define _XOPEN_SOURCE 500

#include "duplicator.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>

char *file_buff = NULL;
long file_size = 0;
int running = 0;
int num_copies = 0;

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
        fprintf(stderr,"cant open file: %s: %s\n", file_path, strerror(errno));
        exit(0);
    }
    if(fseek(file, 0, SEEK_END)) {
        fprintf(stderr,"cant fseek file: %s: %s\n", file_path, strerror(errno));
        exit(0);
    }
    file_size = ftell(file);

    file_buff = realloc(file_buff, file_size);
    if(file_buff == NULL) {
        fprintf(stderr,"cant realloc file buffer for size %ld\n",file_size);
        exit(0);
    }
 
    rewind(file);

    if(fread(file_buff, 1, file_size, file)!= file_size) {
        fprintf(stderr,"cant fread file: %s\n", file_path);
        exit(0);
    }
    fclose(file);
}
void write_buff_to_file(char *file_path) {
    FILE* file = fopen(file_path, "wb");
    if (file == NULL) {
        fprintf(stderr,"cant open file: %s: %s\n", file_path, strerror(errno));
        exit(0);
    }

    if(fwrite(file_buff, 1, file_size, file) != file_size) {
        fprintf(stderr,"cant fwrite: %s\n", file_path);
        exit(0);
    }
 
    fclose(file);
}

void duplicate_mem(char * file_path, const char * archive_path, time_t modif_time) {
    char* duplicate_path = get_duplicate_path(file_path, archive_path, modif_time);

    if(file_buff != NULL) {
        write_buff_to_file(duplicate_path);
        //printf("MEM\tPID: %d\t%s -> %s\n", getpid(), file_path, duplicate_path);
    }

    get_file_buff(file_path);

    free(duplicate_path);
}

void end(int s) {
    printf("PID %d ENDS\n", getpid());
    free(file_buff);
    exit(num_copies);
}

void stop(int s) {
    if(running == 0) {
        fprintf(stderr,"PID %d already stopped\n", getpid());
    } else {
        running = 0;
        printf("PID %d STOPS\n", getpid());
    }

}

void start(int s) {
    if(running == 1) {
        fprintf(stderr, "PID %d is already running\n", getpid());
    } else {
        running = 1;
        printf("PID %d STARTS\n", getpid());
    }

}

void monitor_file(char *file_path, const char *archive_path, int interval) {
    struct sigaction sa_start, sa_stop, sa_end, sa_ignore;

    sigemptyset(&sa_start.sa_mask);
    sigemptyset(&sa_stop.sa_mask);
    sigemptyset(&sa_end.sa_mask);
    sigemptyset(&sa_ignore.sa_mask);

    sa_start.sa_flags = 0;
    sa_stop.sa_flags = 0;
    sa_end.sa_flags = 0;
    sa_ignore.sa_flags = 0;

    sa_start.sa_handler = start;
    sa_stop.sa_handler = stop;
    sa_end.sa_handler = end;
    sa_ignore.sa_handler = SIG_IGN;

    sigaction(SIGUSR1, &sa_start, NULL);
    sigaction(SIGUSR2, &sa_stop, NULL);
    sigaction(SIGTERM, &sa_end, NULL);
    sigaction(SIGINT, &sa_ignore, NULL);

    running = 1;

    pid_t pid = getpid();
    time_t last_modif;
    struct stat sb;

    if (lstat(file_path, &sb) < 0) {
        fprintf(stderr,"PID: %d unable to lstat file %s\n", pid, file_path);
        exit(0);
    }

    last_modif = sb.st_mtime;

    duplicate_mem(file_path, archive_path, last_modif);
  
    while(1){
        if(running == 1) {
            if (lstat(file_path, &sb) < 0) {
                fprintf(stderr, "PID: %d unable to lstat file %s\n", pid, file_path);
                exit(0);
            }

            if(sb.st_mtime != last_modif) {
                duplicate_mem(file_path, archive_path, last_modif);
                last_modif = sb.st_mtime;
                num_copies++;
            }

            sleep(interval);
        }  
    }
}