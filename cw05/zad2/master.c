#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong arguments, usage: [fifo name]\n");
        exit(1);
    }

    char *fifo_name = argv[1];

    if(access(fifo_name, F_OK) != -1 && remove(fifo_name)) {
        fprintf(stderr, "can remove existing fifo file, %s\n", strerror(errno));
        exit(1);
    }

    if (mkfifo(fifo_name, 0644) < 0) {
        fprintf(stderr, "can make fifo file, %s\n", strerror(errno));
        exit(1);
    }

    int fifo = open(fifo_name, O_RDONLY);
    char buff[1024];
    while(1) {
        int num_read = read(fifo, buff, sizeof(buff));
        if(num_read < 0) {
            fprintf(stderr, "read from fifo, %s\n", strerror(errno));
            exit(1);    
        } else if(num_read == 0) {
            break;
        }
        buff[num_read] = EOF;

        write(STDOUT_FILENO, buff, num_read);
    }

    close(fifo);

    if (remove(fifo_name) < 0) {
        fprintf(stderr, "can remove fifo file, %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}