#ifndef COLOR_H
#define COLOR_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#ifndef WINDOWS
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#ifdef WINDOWS
#include <GL/glut.h>
#endif 

#include "macros.h"
/****************************************************************/
/* variable externs */
/****************************************************************/
extern float color_buffer[WIN_H][WIN_W][4];


/****************************************************************/
/* functions */
/****************************************************************/
void clear_color_buffer(float r, float g, float b, float a);
void draw_color_buffer (void);

#endif
