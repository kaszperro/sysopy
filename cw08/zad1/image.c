#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "image.h"

int row_col_to_index(img_t*image, int row, int col) {
    int index = row*image->width + col;
    if(image->color_mode == COLOR_RGB)
        index *= 3;

    return index;
}

int image_get_arr_size(img_t* image) {
    switch (image->color_mode) {
        case COLOR_BW:
        case COLOR_GRAY:
            return image->width * image->height; 
            break;
        case COLOR_RGB:
            return image->width * image->height * 3; 
        default:
            return -1;
    }
}

void load_image(const char *path, img_t* image) {
    FILE *fd;
    if ((fd = fopen(path, "r")) == NULL) {
        perror("cant load image");
        exit(1);
    }


    char buff[2048];

    int line_no = 0;

    while (line_no < 3 && fgets(buff, 2048, fd) != NULL) {
        if(buff[0] == '#') continue;

        if(line_no == 0) {
            if (strncmp("P1", buff, 2) == 0)
                image->color_mode = COLOR_BW;
            else if (strncmp("P2", buff, 2) == 0)
                image->color_mode = COLOR_GRAY;
            else if (strncmp("P3", buff, 2) == 0)
                image->color_mode = COLOR_RGB;
            else {
                fprintf(stderr, "wrong image color mode\n");
                fclose(fd);
                exit(1);
            }
        } else if(line_no == 1) {
            if(sscanf(buff, "%d %d\n", &image->width, &image->height) != 2) {
                fprintf(stderr, "cant get image width and image height\n");
                fclose(fd);
                exit(1);
            }
        } else if(line_no == 2) {
            if(sscanf(buff, "%d\n", &image->max_value) != 1) {
                fprintf(stderr,"cant get image max value\n");
                fclose(fd);
                exit(1);
            }

            image->array = malloc( image_get_arr_size(image) * sizeof(int) );
        }

        line_no ++;
    }

    if(line_no != 3) {
        fprintf(stderr, "image parse error\n");
        fclose(fd);
        exit(1);
    }
    

    for(int i = 0; i < image_get_arr_size(image); ++i) {
        if(fscanf(fd, "%d", &image->array[i]) != 1) {
            fprintf(stderr, "cant fill image array\n");
            fclose(fd);
            exit(1);
        }
    }


    fclose(fd);
}

void save_image(const char *path, img_t* image) {
    FILE *fd;
    if ((fd = fopen(path, "w")) != NULL) {
        switch (image->color_mode) {
            case COLOR_BW:
                fprintf(fd, "%s", "P1\n");
                break;
            case COLOR_GRAY:
                fprintf(fd, "%s", "P2\n");
                break;
            case COLOR_RGB:
                fprintf(fd, "%s", "P3\n");
                break;
            default:
                fprintf(stderr, "unknown color format\n");
                exit(1);
                break;
        }
        fprintf(fd, "%d %d\n%d", image->width, image->height, image->max_value);
        for (int i = 0; i < image_get_arr_size(image); i++)
        {
            if (i % image->width == 0) fprintf(fd, "\n");
            fprintf(fd, "%d ", image->array[i]);
        }
    } else {
        perror("cant save image");
        exit(1);
    }
    fclose(fd);
}

void copy_image(img_t *input, img_t *output) {
    output->color_mode = input->color_mode;
    output->height = input->height;
    output->width = input->width;
    output->max_value = input->max_value;

    output->array = malloc(image_get_arr_size(output) * sizeof(int));
}