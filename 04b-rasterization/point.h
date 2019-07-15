#ifndef POINT_H
#define POINT_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "math_lib/mat4.h"

#define NUM_TEX_MODES 6
#define NAIVE 1
#define CYLINDRICAL 2
#define SPHERICAL 3
#define REFLECTION 4
#define CUBE_MAP 5

typedef struct point {
    float world[4];
    float position[4];
    float color[4];
    float tex[4];
    
    int tri_list[100]; // indices of neighboring triangles
    int num_tris; // number of neighboring triangles
    
    float v_normal[4];
    float light[4]; // direction to light
    
//    MAT4 tbn;
} POINT;

void set_diffuse_term (float normal[4], float light[4], float diffuse_term[4]);
void set_specular_term (float normal[4], float light[4], float spec_term[4]);
void shade_point (float diffuse[4], float spec[4], POINT *p);

void draw_point (POINT *p);
#endif
