
/*
 *
 * line.c - simple GLUT app that draws one frame with a single point at origin
 *
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

#define PI 3.1415926

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
typedef struct point {
    float position[4];
    float color[4];
} POINT;

/*************************************************************************/
/* enums                                                                 */
/*************************************************************************/
enum ColorChannels {R = 0, G = 1, B = 2, A};
enum Axes {X = 0, Y = 1, Z = 2, W = 3};

/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern int window_size;

extern float random_float(int low, int high);
extern void draw_point (float x, float y);
extern void set_position (POINT *p, float x, float y, float z, float w);
extern void set_color (POINT *p, float r, float g, float b, float a);

extern void draw_fan (void);
extern void draw_random_line (void);
extern void draw_coord_grid (void);

extern void draw_line (float start_x, float start_y, float end_x, float end_y,
                       float start_r, float start_g, float start_b,
                       float end_r, float end_g, float end_b);
extern void draw_line_2 (POINT *p1, POINT *p2);


