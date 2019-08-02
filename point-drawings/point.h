/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

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
#define SIGN(_a)        (((_a) < 0.0) ? -1 : (((_a) > 0.0) ? 1 : 0))
#define ABS(_a)         (((_a) < 0.0) ? -(_a) : (_a))
#define SQR(_a)         ((_a) * (_a))
#define CLAMP(_a,_l,_h) (((_a) <= (_l)) ? (_l) : (((_a) >= (_h)) ? (_h) : (_a)))


#define PI 3.1415926

extern float random_float( int low, int high );     // def in point.c
extern void draw_point(float x, float y);           // def in point.c

extern int window_size; //in point.c

extern void draw_mc_circle(void);
extern void draw_tv_screen(void);
extern void draw_all_pixels(void);
extern void draw_gradient(void);
extern void draw_hop_fract(void);
extern void draw_mdb(void);
extern void draw_sierp(void);
extern void draw_bifurcate(void);
extern void draw_lorenz(void);
extern void draw_sine(void);
extern void draw_cosine(void);
extern void draw_quadratic(void);
extern void draw_pow2(void);

