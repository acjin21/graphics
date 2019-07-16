#ifndef SCENE_H
#define SCENE_H

#include "math_lib/mat4.h"
#include "point.h"

#define MAX_N_OBJECTS 10

#define ORTHO 0
#define PERSPECT 1

#define DEPTH 0
#define COLOR 1

/* type of scene object */
typedef struct object {
    int type;
    int ID;
    
    /* current status during edit mode */
    float center[3];
    float scale;
    float radii[2];
    
    float init_orientation[3]; //dx_angle, dy_angle, dz_angle
    float rotation[3]; // for rotating objects individually
    float translate[3];
    float scale_vec[3];
    
    POINT bb_bl;
    POINT bb_tr;
    MAT4 model_mat;
    MAT4 ws_transform; // holds transformations within world space (i.e. rotation, etc)
    
    int texturing;
    int texture_idx;
    int cube_map;
    
} OBJECT;

OBJECT *get_curr_object (int object_ID);

void write_scene (char *file_name);
void read_scene (char *file_name);
#endif
