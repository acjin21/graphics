#ifndef POINT_H
#define POINT_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "math_lib/mat4.h"
#include "texture.h"



typedef struct point {
    float world[4]; //used to store world and eye space coords
    float ndc[4];
    float position[4];
    float color[4];
    float tex[4];
    
    float world_pos[4];
    
    float view[4];
    
    int tri_list[100]; // indices of neighboring triangles
    int num_tris; // number of neighboring triangles
    
    float v_normal[4];
    float light[4]; // direction to light
    
    int rendered_flag; 
//    MAT4 tbn;
} POINT;

/*************************************************************************/
/* Variable externs                                                      */
/*************************************************************************/
/* for model.c */              // to avoid texturing the non-model vertices
extern int drawing_normals;
extern int drawing_axes;
extern int drawing_bounding_box; //status

/* for main.c */
extern IMAGE cube_map[6];
extern IMAGE bump_map;
/*************************************************************************/
/* Functions                                                      */
/*************************************************************************/
void set_diffuse_term
(float diffuse_term[4], float normal[4], float light[4], float world_pos[4]);

void set_specular_term
(float spec_term[4], float normal[4], float light[4], float view[4],
 float world_pos[4]);

void shade_point
(float diffuse[4], float spec[4], POINT *p, int mod_type);

void draw_point (POINT *p);
#endif
