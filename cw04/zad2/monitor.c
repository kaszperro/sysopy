#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "duplicator.h"

#define MAX_FILE_NAME 4096
#define MAX_FILES 4096

char *monitored_files[MAX_FILES];
int monitored_intervals[MAX_FILES];
pid_t* child_pids;
int num_files = 0;

void get_monitor_files(char *file_name) {
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr, "cant open list file %s, errno: %s\n", file_name, strerror(errno));
        exit(1);
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
            fprintf(stderr, "fscanf error for file %s\n", file_path);
            exit(1);
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
        if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            fprintf(stderr, "cant mkdir %s, errno: %s\n",path, strerror(errno));
            exit(1);
        }
    } else if(!S_ISDIR(sb.st_mode)) {
        fprintf(stderr, "path exists and isn't directory :%s\n", path);
        exit(1);
    }
}

int is_integer(char* text){
    int i = 0;
    char c;
    while((c = text[i++]) != '\0'){
        if(c < 48 || c > 57) {
            return 0;
        }
    }
    return 1;
}

void list_pids() {
    for(int i = 0; i < num_files; ++i) {
        printf("PID: %d monitores file: %s with interval: %d s\n", child_pids[i], monitored_files[i], monitored_intervals[i]);
    }
}

int get_index_from_pid(pid_t pid) {
    for(int i = 0; i < num_files; ++i) {
        if(child_pids[i] == pid)
            return i;
    }
    return -1;
}

void stop_process_pid(pid_t pid) {
    if(kill(pid, SIGUSR2) != 0) {
        fprintf(stderr,"cant kill PID %d with signal SIGUSR2, errno: %s\n", pid, strerror(errno));
    }   
}

void stop_all_processes() {
    for(int i = 0; i < num_files; ++i) {
        stop_process_pid(child_pids[i]);
    }
}

void start_process_pid(pid_t pid) {
    if(kill(pid, SIGUSR1) != 0) {
        fprintf(stderr,"cant kill PID %d with signal SIGUSR1, errno: %s\n", pid, strerror(errno));
    }  
}

void start_all_processes() {
    for(int i = 0; i < num_files; ++i) {
        start_process_pid(child_pids[i]);
    }
}

void end_process_pid(pid_t pid) {
    if(kill(pid, SIGTERM) != 0) {
        fprintf(stderr,"cant kill PID %d with signal SIGTERM, errno: %s\n", pid, strerror(errno));
    }
}

void end_all_processes(int s) {
    printf(" ending program\n");
    for (int i = 0; i < num_files; ++i) {
        end_process_pid(child_pids[i]);
        int status;
        waitpid(child_pids[i], &status, 0);
        if(WIFEXITED(status)) {
            int copies = WEXITSTATUS(status);
            printf("PID: %d made %d copies of file %s\n", child_pids[i], copies, monitored_files[i]);
        } else {
            printf("PID: %d was terminated automatically\n", child_pids[i]);
        }
       
        free(monitored_files[i]);
    }
    free(child_pids);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "wrong arguments, usage: [list]\n");
        exit(1);
    }
    const char archive_path[] = "archiwum";
    char *file_name = argv[1];


    make_dir(archive_path);
    get_monitor_files(file_name);   

    child_pids = malloc(sizeof(pid_t) * num_files);

    for (int i = 0; i < num_files; ++i) {
        pid_t child_pid = fork();

        if(child_pid == -1) {
            fprintf(stderr, "cant fork\n");
            exit(1);
        }
        if(child_pid > 0) {
            child_pids[i] = child_pid;
        } else {
            monitor_file(monitored_files[i], archive_path, monitored_intervals[i]);
        }
    }

    list_pids();


    struct sigaction sa_end;
    memset(&sa_end, 0, sizeof(struct sigaction));
    sa_end.sa_handler = end_all_processes;
    sigaction(SIGINT, &sa_end, NULL);

    
    char input[256];
    while(1) {
        scanf("%s", input);
        
        if(strcmp("LIST", input) == 0) {
            list_pids();
        } else if(strcmp(input, "STOP") == 0) {
            scanf("%s", input);
            if(strcmp(input, "ALL") == 0) {
                stop_all_processes();
            } else if(is_integer(input)){
                pid_t pid = atoi(input);
                int index = get_index_from_pid(pid);
                if(index == -1) {
                    fprintf(stderr, "wrong pid %d\n", pid);
                    continue;
                } 
                stop_process_pid(pid);
            } else {
                fprintf(stderr, "STOP should be ALL or pid\n");
            }
        } else if(strcmp(input, "START") == 0) {
            scanf("%s", input);
            if(strcmp(input, "ALL") == 0) {
                start_all_processes();
            } else if(is_integer(input)){
                pid_t pid = atoi(input);
                int index = get_index_from_pid(pid);
                if(index == -1) {
                    fprintf(stderr, "wrong pid %d\n", pid);
                    continue;
                } 
                start_process_pid(pid);
            } else {
                fprintf(stderr, "START should be ALL or pid\n");
            }
        } else if(strcmp(input, "END") == 0) {
            end_all_processes(SIGINT);
        } else {
            fprintf(stderr, "wrong command\n");
        }

    }

    free(child_pids);

    return 0;
}