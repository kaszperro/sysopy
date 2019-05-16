#define _XOPEN_SOURCE 500

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr,"wrong arguments, usage: [num_workers, max_weight]\n");
        exit(1);
    }

    int workers;
    int max_weight;


    if (sscanf(argv[1], "%d", &workers) != 1 ||
      sscanf(argv[2], "%d", &max_weight) != 1) {
        fprintf(stderr,"cant parse arguments\n");
        exit(1);
    }

    srand(time(0));

    char weight_str[32];

    for (int i = 0; i < workers; ++i) {
        sprintf(weight_str, "%d", rand() % max_weight + 1);

        int pid = fork();
        if(pid == -1) {
            perror("cant fork\n");
            exit(1);
        } else if(pid == 0) {
            execl("./loader", "loader", weight_str, NULL);
        }
    }

    int status;
    for (int i = 0; i < workers; ++i) {
        wait(&status);
        printf("worker %d finished with exit code %d\n", i, WEXITSTATUS(status));
    }
    
}