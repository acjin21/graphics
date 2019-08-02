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
#ifndef PI
#define PI 3.1415926
#endif


/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern float random_float(float, float);
extern void home(void);
extern void right(float);
extern void left(float);
extern void forward(float);
extern void backward(float);
extern void set_xy(float, float);
extern void set_heading(float);


extern void draw_square(float);
extern void draw_pinwheel();
extern void draw_square_rec(float);
extern void draw_circle(float);
extern void draw_spiral(float r, int n_turns);
extern void draw_star(float);
extern void draw_V(float, float);
extern void draw_V_rec(float);
extern void draw_witch_hat(float);
extern void draw_witch_hat_rec(float);
extern void draw_wh_rec(float);

extern void draw_equil_tri(float);
extern void draw_pent(float);
extern void draw_n_gon(int n, float len);
extern void raw_random_shape(void);
extern void do_random_walk(void);
extern void draw_random_walk(int, int);
