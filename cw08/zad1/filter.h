#ifndef FILTER_H
#define FILTER_H

#include "image.h"

typedef struct filter_t
{
    double *array;
    int size;
    
} filter_t;

void apply_filter(filter_t* filter, img_t *input, img_t *output, int row, int col);

void load_filter(const char *path, filter_t *filter);
void save_filter(const char *path, filter_t *filter);
void normalize_filter(filter_t *filter);

#endif