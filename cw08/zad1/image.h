#ifndef IMAGE_H
#define IMAGE_H

typedef int color_mode_t;

#define COLOR_BW (color_mode_t)1
#define COLOR_GRAY (color_mode_t)2
#define COLOR_RGB (color_mode_t)3

typedef struct img_t
{
    int *array;
    int width;
    int height;
    int max_value;
    color_mode_t color_mode;
} img_t;

int image_get_arr_size(img_t* image);
int row_col_to_index(img_t*image, int row, int col);

void load_image(const char *path, img_t* image);
void save_image(const char *path, img_t* image);
void copy_image(img_t *input, img_t *output);

#endif