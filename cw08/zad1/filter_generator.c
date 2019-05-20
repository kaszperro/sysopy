#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
    const char filters_path[]  = "./generated_filters";

    srand(time(0));

    for(int i = 3; i <= 65; ++i) {
        filter_t filter;
        filter.size = i;
        filter.array = malloc(i*i*sizeof(double));

        for(int j = 0; j < i*i; ++j) {
            filter.array[j] = rand() % 50 - 25;
        }
        normalize_filter(&filter);
        char my_name[255];
        sprintf(my_name, "%s/%d", filters_path, i);
        save_filter(my_name, &filter);

        free(filter.array);
    }

    

    return 0;
}