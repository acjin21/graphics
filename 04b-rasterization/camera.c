#include "camera.h"

#include "vector.h"
#include "macros.h"
#include "math_lib/mat4.h"

#include <stdio.h>

float eye[4] = { 0, 0, 0, 1 };

CAMERA camera;

void set_camera (CAMERA *c, float pos[4], float lookat[4], float up[4])
{
    float u[4], v[4];
    float col3[4] = {0, 0, 0, 1};

    if(pos[X] == lookat[X] && pos[Y] == lookat[Y] && pos[Z] == lookat[Z])
    {
        printf("camera error: pos == lookat\n");
        return;
    }
    else
    {
        vector_subtract(lookat, pos, v);
        normalize(v);
        
        vector_cross (up, v, u);
        normalize(u);
        
        vector_cross (v, u, up);
        normalize(up);
        
        u[W] = 0.0;
        v[W] = 0.0;
        up[W] = 0.0;
        
        cpy_vec4(c->orig_pos, pos);
        cpy_vec4(c->orig_u, u);
        cpy_vec4(c->orig_v, v);
        cpy_vec4(c->orig_up, up);
        
        cpy_vec4(c->pos, pos);
        cpy_vec4(c->u, u);
        cpy_vec4(c->v, v);
        cpy_vec4(c->up, up);
    }
}

void rotate_camera (CAMERA *c, float rx, float ry, float rz)
{
    MAT4 rot;
    set_3d_rot(&rot, rx, ry, rz);
    mat_vec_mul(&rot, c->u, c->u);
    mat_vec_mul(&rot, c->v, c->v);
    mat_vec_mul(&rot, c->up, c->up);
}

void reset_camera (CAMERA *c)
{
    cpy_vec4(c->pos, c->orig_pos);
    cpy_vec4(c->u, c->orig_u);
    cpy_vec4(c->v, c->orig_v);
    cpy_vec4(c->up, c->orig_up);
    
    c->rot[X] = 0;
    c->rot[Y] = 0;
    c->rot[Z] = 0;
}
