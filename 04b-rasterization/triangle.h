
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

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define PI 3.1415926

#define DRAW 0
#define WALK 1

#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))

#define SWAP(a,b)\
{                   \
POINT *tmp = a;  \
a = b;          \
b = tmp;\
}

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
typedef struct point {
    float position[4];
    float color[4];
    float tex[4];
} POINT;

//extern struct IMAGE;

/*************************************************************************/
/* enums                                                                 */
/*************************************************************************/
enum ColorChannels {R = 0, G = 1, B = 2, A};
enum Axes {X = 0, Y = 1, Z = 2, W = 3};
enum Textures {S = 0, T = 1};

/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern int window_size;

extern float random_float(int low, int high);
extern void draw_point (POINT *p);
extern void set_position (POINT *p, float x, float y, float z, float w);
extern void set_color (POINT *p, float r, float g, float b, float a);
extern void set_tex (POINT *p, float s, float t);
extern void draw_triangle (POINT *v0, POINT *v1, POINT *v2);

extern void draw_color_buffer(void);


