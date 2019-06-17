#ifndef POINT_H
#define POINT_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "macros.h"
#include "texture.h"

typedef struct point {
    float position[4];
    float color[4];
    float tex[4];
} POINT;

void draw_point (POINT *p);
#endif
