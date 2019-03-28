#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int rand_num(int a, int b) {
    return (rand() % (b-a+1)) + a;
}

int main(int argc, char *argv[]) {
    srand(time(0)); 

    if(argc != 5) {
        gen_error("wrong number of arguments, [file pmin pmax bytes]\n");
    }


    char *file_path = argv[1];
    int pmin = atoi(argv[2]);
    int pmax = atoi(argv[3]);
    int bytes = atoi(argv[4]);

    if(pmin > pmax) {
        gen_error("pmin larger than pmax\n");
    }


    FILE* f;
    char* buff = malloc(bytes);
    time_t modif_time;
    struct tm* timeinfo;
    char date[80];

    while (1) {
        int sleep_time = rand_num(pmin, pmax);

        modif_time = time(0);
        timeinfo = localtime(&modif_time);
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", timeinfo);

        for(int i = 0; i < bytes; ++i) {
            buff[i] = (int)'a' + rand_num(0, 27);
        }

        f = fopen(file_path, "a");
        fprintf(f, "pid: %d\trand sleep: %d\tcurrent date: %s\tbytes: %s\n",
            getpid(), sleep_time, date, buff);
        fclose(f);

        sleep(sleep_time);
    }

    free(buff);

    return 0;
}