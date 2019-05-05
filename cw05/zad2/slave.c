#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong arguments, usage: [fifo name] [N]\n");
        exit(1);
    }

    char *fifo_name = argv[1];

    if(access(fifo_name, F_OK) == -1) {
        fprintf(stderr, "cant access fifo\n");
        exit(1);
    }

    struct stat sb;
    if (stat(fifo_name, &sb) == -1) {
        fprintf(stderr, "unable to stat %s\n",fifo_name);
        exit(1);
    }

    if (!S_ISFIFO(sb.st_mode)) {
        fprintf(stderr, "%sis not a fifo\n",fifo_name);
        exit(1);
    }

    int n = atoi(argv[2]);
    pid_t pid = getpid();
    printf("slave pid: %d\n", pid);

    int fifo = open(fifo_name, O_WRONLY);
    if(fifo < 0) {
        fprintf(stderr,"error while opeing fifo: %s\n", strerror(errno));
        exit(1);
    }
    
    char buff[1024];
    char date[128];
    for(int i = 0; i < n; ++i) {
        FILE* fdate = popen("date", "r");
        if(fgets(date, sizeof(date),fdate) == NULL) {
            close(fifo);
            fprintf(stderr,"cant read date %s\n", strerror(errno));
            exit(1);
        }
        pclose(fdate);

        int size = sprintf(buff, "%d: %s", pid, date);

        write(fifo, buff, size);

        sleep(2 + rand() % 4);
    }

    close(fifo);

    return 0;
}