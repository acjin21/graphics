#include "camera.h"

#include "vector.h"
#include "macros.h"
#include "math_lib/mat4.h"
#include "app.h"
#include <stdio.h>

float eye[4] = { 0, 0, 0, 1 };

CAMERA camera;
CAMERA light_camera;
void set_camera (CAMERA *c, float pos[4], float lookat[4], float up[4])
{
    float u[4], v[4];
    float col3[4] = {0, 0, 0, 1};
    float local_up[4];
    cpy_vec4(local_up, up);
    if(pos[X] == lookat[X] && pos[Y] == lookat[Y] && pos[Z] == lookat[Z])
    {
        printf("camera error: pos == lookat\n");
        return;
    }
    else
    {
        if(current_rs.render_pass_type == SHADOW_PASS)
        {
            cpy_vec4(v, lookat);
            normalize(v);
        }
        else
        {
            vector_subtract(lookat, pos, v);
            normalize(v);
        }
        vector_cross (local_up, v, u);
        normalize (u);
        
        vector_cross (v, u, local_up);
        normalize(up);
        
        u[W] = 0.0;
        v[W] = 0.0;
        up[W] = 0.0;
        
        cpy_vec4(c->orig_pos, pos);
        cpy_vec4(c->orig_u, u);
        cpy_vec4(c->orig_v, v);
        cpy_vec4(c->orig_up, local_up);
        
        cpy_vec4(c->pos, pos);
        cpy_vec4(c->u, u);
        cpy_vec4(c->v, v);
        cpy_vec4(c->up, local_up);
    }
}

void rotate_camera (CAMERA *c, float rx, float ry, float rz)
{
    MAT4 rot;
    set_3d_rot(&rot, rx, ry, rz);
    mat_vec_mul(&rot, c->u, c->u);
    mat_vec_mul(&rot, c->v, c->v);
    mat_vec_mul(&rot, c->up, c->up);
    
//    print_vec4(c->u);
//    print_vec4(c->v);
//    print_vec4(c->up);

    c->rot[X] += rx;
    c->rot[Y] += ry;
    c->rot[Z] += rz;
}

void translate_camera (CAMERA *c, float tx, float ty, float tz)
{
    // not using translation matrix for performance (dont need a matrix mul)
    // translating camera means moving its center point/eye (dont need to recalc unit basis vecs),
    //  so don't need to worry about homogenous coord (it will be 1, but we just don't add to it
    float transl[4], tmp[4];
    set_vec4(transl, tx, ty, 0, 0);
    vector_add(c->pos, transl, c->pos);
    scalar_multiply(tz, c->v, tmp);
    vector_add(c->pos, tmp, c->pos);
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
