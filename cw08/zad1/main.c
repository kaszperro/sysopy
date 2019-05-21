#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#include "image.h"
#include "filter.h"

#define THREAD_TYPE_BLOCK 0
#define THREAD_TYPE_INTERLEAVED 1

img_t input_image;
img_t out_image; 
filter_t filter;
int thread_type;
int num_threads;

void * thread_function(void *arg) {
    int index = *((int*)arg);

    struct timeval timestamp_start;
    gettimeofday(&timestamp_start, NULL);


    if (thread_type == THREAD_TYPE_BLOCK) {
        int start_col = index * ceil(input_image.width/(float)num_threads);
        int end_col = (index+1) * ceil(input_image.width/(float)num_threads);

        if(end_col> input_image.width) end_col = input_image.width;

        for(int c = start_col; c < end_col; ++c) {
            for(int r = 0; r < input_image.height; ++r) {
                apply_filter(&filter, &input_image, &out_image, r, c);   
            }
        }
    } else {
        for(int c = index; c < input_image.width; c += num_threads) {
            for(int r = 0; r < input_image.height; ++r) {
                apply_filter(&filter, &input_image, &out_image, r, c);   
            }
        }
    }
    struct timeval timestamp_end;
    gettimeofday(&timestamp_end, NULL);

    double start = timestamp_start.tv_sec + timestamp_start.tv_usec / 1000000.0 ;
    double end = timestamp_end.tv_sec  + timestamp_end.tv_usec / 1000000.0;

    double *ret = malloc(sizeof(double));
    *ret = end-start;

    return ret;
}

int main(int argc, char*argv[]) {
    if(argc != 6) {
        fprintf(stderr, "wrong arguments, expected: [threads] [block/interleaved] [input image path] [filter path] [output path]\n");
        exit(1);
    }
    
    char *input_path;
    char *filter_path;
    char *output_path;

    num_threads = atoi(argv[1]);
    if(strcmp("block", argv[2]) == 0)
        thread_type = THREAD_TYPE_BLOCK;
    else if(strcmp("interleaved", argv[2]) == 0)
        thread_type = THREAD_TYPE_INTERLEAVED;
    else {
        fprintf(stderr, "unknown block type\n");
        exit(1);
    }

    input_path = argv[3];
    filter_path = argv[4];
    output_path = argv[5];

 
    load_image(input_path, &input_image);
    copy_image(&input_image, &out_image);
    load_filter(filter_path, &filter);

   
    struct timeval timestamp_start;
    gettimeofday(&timestamp_start, NULL);

    pthread_t *threads = malloc( num_threads * sizeof(pthread_t) );

    int *indexes = malloc(num_threads * sizeof(int));
    for(int i = 0 ; i < num_threads; ++i) {
        indexes[i] = i;
        pthread_create(&threads[i], NULL, &thread_function, &indexes[i]);
    }

    for(int i = 0; i < num_threads; ++i) {
        double *time;
        pthread_join(threads[i], (void**) &time);

        printf("thread %d time: %lf\n", i, *time);
        free(time);
    }

    struct timeval timestamp_end;
    gettimeofday(&timestamp_end, NULL);

    save_image(output_path, &out_image);


    double start = timestamp_start.tv_sec + timestamp_start.tv_usec / 1000000.0 ;
    double end = timestamp_end.tv_sec  + timestamp_end.tv_usec / 1000000.0;

    printf("program finished, time took: %lf\n", end-start);

    return 0;
}