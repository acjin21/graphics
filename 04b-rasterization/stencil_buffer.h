#ifndef STENCIL_BUFFER_H
#define STENCIL_BUFFER_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#ifndef WINDOWS
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "window.h"
#include "macros.h"

extern int stencil_buffer[MAX_WIN_H][MAX_WIN_W];

void clear_stencil_buffer(void);
void draw_stencil_buffer(void);
#endif
