#define _XOPEN_SOURCE 500
 
#include "error.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
 
 
void follow_dir() {
 
    DIR* root_dir = opendir(".");
    if (root_dir == NULL) {
        gen_error("cant open directory, errno: %s\n", strerror(errno));
    }
 
    struct dirent* file;
 
 
    while ((file = readdir(root_dir)) != NULL) {
        struct stat sb;
 
        if (lstat(file->d_name, &sb) < 0) {
            gen_error("can't lstat file %s, errno: %s\n", file->d_name, strerror(errno));
        }
 
         if (S_ISDIR(sb.st_mode)) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }
 
            if (chdir(file->d_name) != 0) {
                gen_error("unable to change path to %s", file->d_name);
            }
 
 
            follow_dir();
 
            pid_t pid_fork = fork();
 
            if(pid_fork < 0) {
                gen_error("cant fork, errno: %s", strerror(errno));
            } else if(pid_fork == 0) {
                 
                char cwd[4096];
                if (getcwd(cwd, 4096) == NULL) {
                    gen_error("unable to get current working directory, errno: %s", strerror(errno));
                }
 
                printf("dir: %s, PID: %d\n", cwd, getpid());
                int exec_status = execlp("ls", "ls", "-l", NULL);
                if(exec_status != 0) {
                    gen_error("exec failed, errno: %s\n", strerror(errno));
                }
                exit(exec_status);
            } else {
                wait(0);
            }
 
 
            if (chdir("..") != 0) {
                gen_error("unable to change path to %s/../, errno: %s", file->d_name, strerror(errno));
            }
 
         }
 
    }
 
    closedir(root_dir);
}
 
int main(int argc, char *argv[]) {
    if (argc != 2) {
        gen_error("wrong arguments\n");
    }
 
    if(chdir(argv[1]) != 0) {
        gen_error("cant chmod to %s\n", argv[1]);
    }
     
    follow_dir();
    return 0;
}