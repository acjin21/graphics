#ifndef IMAGE_H
#define IMAGE_H

#include "texture.h"

void fill (IMAGE *img, float r, float g, float b);
void copy (IMAGE *input, IMAGE *output);
void negative (IMAGE *input, IMAGE *output);
void flip_vertical (IMAGE *input, IMAGE *output);
void flip_horizontal (IMAGE *input, IMAGE *output);
void luminosity (IMAGE *input, IMAGE *output);
void sepia (IMAGE *input, IMAGE *output);
void avg (IMAGE *input, IMAGE *output);
void min (IMAGE *input, IMAGE *output);
void max (IMAGE *input, IMAGE *output);
void cartesian_to_polar (float x, float y, float *radius, float *angle,
                         int width, int height);
void polar_to_cartesian (float radius, float angle, float *x, float *y,
                         int width, int height);
void rotate_ccw (IMAGE *input, IMAGE *output, float angle);

void lincoln (IMAGE *input, IMAGE *output, int scale);
void fisheye (IMAGE *input, IMAGE *output);
void einstein (IMAGE *input, IMAGE *output);
#endif
