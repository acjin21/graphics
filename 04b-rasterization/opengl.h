#ifndef OPENGL_H
#define OPENGL_H

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

#include "point.h"

enum Renderer {ALL_SW = 0, SW_HW = 1, ALL_HW = 2};
extern int renderer;

void draw_line_gl (POINT *v0, POINT *v1);
void draw_triangle_gl (POINT *v0, POINT *v1, POINT *v2);

void passthrough_gl_state(void);
void change_gl_state(void);
void init_gl_state( void );
#endif
