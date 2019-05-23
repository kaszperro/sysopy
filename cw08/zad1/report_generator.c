#define _XOPEN_SOURCE 500

#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>

int main() {
    const char img_path[] = "images/balloons.ascii.pgm";
    const char out_path[] = "outputs/generated.ascii.pgm";

    const char report_path[] = "Times.txt";

    FILE *fp = fopen(report_path, "w");


    fprintf(fp, "-=-=-=-interleaved-=-=-=-\n");

    for(int fs = 3; fs <= 65; fs += 5) {
        fprintf(fp, "\n==================\nfilter size: %d\n------------\n", fs);
        for(int t = 1; t <= 8; t*=2) {
            fprintf(fp, "\tthreads: %d\n", t);

            char sys_call[255];
            sprintf(sys_call, "./main %d interleaved %s generated_filters/%d %s", t, img_path, fs, out_path);

            FILE *res = popen(sys_call, "r");

            char output[1024];
            int num_read = fread(output, sizeof(char),1024, res);
            output[num_read] = '\0';
            fwrite(output, sizeof(char), num_read, fp);

            pclose(res);
        }
       fprintf(fp, "==================\n");
       
    }

    fprintf(fp, "-=-=-=-block-=-=-=-\n");

    for(int fs = 3; fs <= 65; fs += 5) {
        fprintf(fp, "\n==================\nfilter size: %d\n------------\n", fs);
        for(int t = 1; t <= 8; t*=2) {
            fprintf(fp, "\tthreads: %d\n", t);

            char sys_call[255];
            sprintf(sys_call, "./main %d block %s generated_filters/%d %s", t, img_path, fs, out_path);

            FILE *res = popen(sys_call, "r");

            char output[1024];
            int num_read = fread(output, sizeof(char),1024, res);
            output[num_read] = '\0';
            fwrite(output, sizeof(char), num_read, fp);

            pclose(res);
        }
       fprintf(fp, "==================\n");
       
    }


    fclose(fp);

    return 0;
}