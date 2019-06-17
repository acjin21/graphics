#ifndef TRIANGLE_H
#define TRIANGLE_H

/*
 * To build:  gcc -framework OpenGL -framework GLUT line.c -o line
 *
 */
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#include "macros.h"
#include "vector.h"
#include "texture.h" //IMAGE typedef, texture and ppm functions
#include "image.h" //uses IMAGE typedef, image processing funcs
#include "util.h" //random_float
#include "color.h"
#include "depth.h"
#include "raster.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define ON 1
#define OFF 0

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
//typedef struct point {
//    float position[4];
//    float color[4];
//    float tex[4];
//} POINT;

//extern struct IMAGE;

/*************************************************************************/
/* enums                                                                 */
/*************************************************************************/
//enum ColorChannels {R = 0, G = 1, B = 2, A};
//enum Axes {X = 0, Y = 1, Z = 2, W = 3};
//enum Textures {S = 0, T = 1};
#define S 0
#define T 1
/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern int window_size;
void draw_color_buffer (void);
void draw_point (POINT *p);



#endif
