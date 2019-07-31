/*
 * implements a depth buffer and
 *    provides functions to clear and draw the entire buffer
 */
#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#ifndef WINDOWS
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "macros.h"

#ifdef WINDOWS
#include <GL/glut.h>
#endif 
/*************************************************************************/
/* variable externs                                                     */
/*************************************************************************/
extern float depth_buffer[MAX_WIN_H][MAX_WIN_W];

void clear_depth_buffer (float value);
void draw_depth_buffer (void);
#endif
