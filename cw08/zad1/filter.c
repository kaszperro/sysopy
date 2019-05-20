#include "filter.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPS 0.0001

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void save_filter(const char *path, filter_t *filter) {
    FILE *fd;
    if ((fd = fopen(path, "w")) == NULL) {
        perror("cant save filter");
        exit(1);
    }

    fprintf(fd, "%d", filter->size);
    for(int i = 0; i < filter->size*filter->size; ++i) {
        if(i%filter->size == 0) fprintf(fd, "\n");
        fprintf(fd, "%lf ", filter->array[i]);
    }

    fclose(fd);
}

void load_filter(const char *path, filter_t *filter) {
    FILE *fd;
    if ((fd = fopen(path, "r")) == NULL) {
        perror("cant load filter");
        exit(1);
    }


    if(fscanf(fd, "%d", &filter->size) != 1) {
        fprintf(stderr, "cant load filter size\n");
        fclose(fd);
        exit(1);
    }

    filter->array = malloc(filter->size * filter->size * sizeof(double));

    for(int i = 0; i <  filter->size; ++i) {
        for(int j = 0; j < filter->size; ++j) {
            if(fscanf(fd, "%lf", &filter->array[i*filter->size+j]) != 1) {
                fprintf(stderr, "cant load filter size\n");
                fclose(fd);
                exit(1);
            }
        }
    }

    normalize_filter(filter);

    fclose(fd);
}

void normalize_filter(filter_t *filter) {
    double sum = 0;
    for(int i = 0; i < filter->size*filter->size; ++i) {
        sum += filter->array[i];
    }
    if(abs(sum) <= EPS)
        return;
    for(int i = 0; i < filter->size*filter->size; ++i) {
        filter->array[i] /= sum;
    }
}

void apply_filter(filter_t* filter, img_t *input, img_t *output, int row, int col) {
    int filter_offset = ceil(filter->size/2);

    double buffer[3];
    buffer[0]=buffer[1]=buffer[2] = 0;

    for(int i = 0; i < filter->size; ++i) {
        for(int j = 0; j < filter->size; ++j) {
            int new_row = MAX(0, row - filter_offset + i);
            int new_col = MAX(0, col - filter_offset + j);

            new_row = MIN(input->height - 1, new_row);
            new_col = MIN(input->width - 1, new_col);

            int inp_index = row_col_to_index(input, new_row, new_col);
            int f_index = filter->size * i + j;

            switch (input->color_mode) {
                case COLOR_BW:
                case COLOR_GRAY:
                     buffer[0] += input->array[inp_index] * filter->array[f_index];
                    break;
                case COLOR_RGB:
                    for (int i = 0; i < 3; i++)
                        buffer[i] += input->array[inp_index+i] * filter->array[f_index];
                    break;
                default:
                    fprintf(stderr, "image wrong color mode\n");
                    exit(1);
                    break;
            }
        }
    }

    for(int i = 0; i < 3; ++i) {
        buffer[i] = MAX(0, buffer[i]);
        buffer[i] = MIN(output->max_value, buffer[i]);
    }

    int out_index = row_col_to_index(output, row, col);

    switch (input->color_mode) {
        case COLOR_BW:
        case COLOR_GRAY:
            output->array[out_index] = round(buffer[0]);
            break;
        case COLOR_RGB:
            for (int i = 0; i < 3; i++)
                output->array[out_index+i] = round(buffer[i]);
            break;
        default:
            fprintf(stderr, "image wrong color mode\n");
            exit(1);
            break;
    }
}