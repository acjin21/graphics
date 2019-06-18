#ifndef TEXTURE_H
#define TEXTURE_H

#include "macros.h"
#include "util.h"

typedef struct image
{
    int width;
    int height;
    unsigned char data[2000][2000][4];
} IMAGE;

//extern IMAGE texture;
//extern IMAGE texture_in;

void random_texture (IMAGE *img);
void checkerboard_texture (IMAGE *img);
void read_ppm (char *file_name, IMAGE *img);
void write_ppm (char *file_name, IMAGE *img);

void clear_texture (IMAGE *texture, float r, float g, float b, float a);
#endif
