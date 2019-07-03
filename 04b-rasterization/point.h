#ifndef POINT_H
#define POINT_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define ON 1
#define OFF 0

typedef struct point {
    float world[4];
    float position[4];
    float color[4];
    float tex[4];
    
    int tri_list[100]; // indices of neighboring triangles
    int num_tris; // number of neighboring triangles
    float v_normal[4];
} POINT;

void set_diffuse_term (float normal[4], float diffuse_term[4]);
void set_specular_term (float normal[4], float spec_term[4]);
void shade_point (float diffuse[4], float spec[4], POINT *p);

void draw_point (POINT *p);
#endif
