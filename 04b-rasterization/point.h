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
#include "vector.h"

#define ON 1
#define OFF 0

typedef struct point {
    float world[4];
    float position[4];
    float color[4];
    float tex[4];
    
    int tri_list[100];
    int num_tris;
    float v_normal[4];
} POINT;
void set_diffuse_term (float normal[4], float diffuse_term[4]);
void set_specular_term (float normal[4], float spec_term[4]);
void shade_point (float diffuse[4], float spec[4], POINT *p);

void draw_point (POINT *p);
void set_position (POINT *p, float x, float y, float z, float w);
void set_color (POINT *p, float r, float g, float b, float a);
void set_tex (POINT *p, float s, float t);
#endif
