#ifndef FOURIER_H
#define FOURIER_H
#include "texture.h"

typedef struct image_f {
    int             width;
    int             height;
    float           data[3000][3000][4];
} IMAGE_F;

extern IMAGE_F     x;
extern IMAGE_F     y;
extern IMAGE_F     p;
extern IMAGE_F     r;
extern IMAGE_F     i;

void convert_to_float_image( IMAGE *input, IMAGE_F *output );
void convert_to_image( IMAGE_F *input, IMAGE *output );
void dft_2D(IMAGE_F *in, IMAGE_F *real, IMAGE_F *imaginary);
void invdft_2D(IMAGE_F *x, IMAGE_F *y, IMAGE_F *real, IMAGE_F *imaginary);
void power_spectrum (IMAGE_F *real, IMAGE_F *imaginary, IMAGE_F *output);

#endif
