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


void follow_dir(const char * root) {

    DIR* root_dir = opendir(root);
    if (root_dir == NULL) {
        gen_error("cant open directory %s, errno: %s\n", root, strerror(errno));
    }

    struct dirent* file;
    char new_path[256];

    while ((file = readdir(root_dir)) != NULL) {
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat sb;

        if (lstat(file->d_name, &sb) < 0) {
            gen_error("can't lstat file %s, errno: %s\n", file->d_name, strerror(errno));
        }

         if (S_ISDIR(sb.st_mode)) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }

            follow_dir(new_path);

            pid_t pid_fork = fork();

            if(pid_fork < 0) {
                gen_error("cant fork, errno: %s", strerror(errno));
            }

         }

    }

    closedir(root_dir);
}

int main(int argc, char *argv[]) {

    follow_dir("./awf");
    return 0;
}