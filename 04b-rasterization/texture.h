#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct image
{
    int width;
    int height;
    unsigned char data[2000][2000][4];
} IMAGE;

void random_texture (IMAGE *img);
void checkerboard_texture (IMAGE *img);
void read_ppm (char *file_name, IMAGE *img);
#endif
