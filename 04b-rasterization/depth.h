#ifndef DEPTH_H
#define DEPTH_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "macros.h"
/*************************************************************************/
/* variable externs                                                     */
/*************************************************************************/
extern float depth_buffer[WIN_H][WIN_W];

void clear_depth_buffer (float value);
void draw_depth_buffer (void);
#endif
