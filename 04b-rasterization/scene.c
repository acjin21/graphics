#include "scene.h"


#include <stdio.h>

#include "macros.h"
#include "util.h"
#include "model.h"
#include "math_lib/mat4.h"
/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern int draw_mode;  /* draw model as wireframe or filled (FRAME/FILL) */
extern int proj_mode;  /* projection type (ORTHO/PERSPECT) */
extern int buffer;     /* which buffer to draw from (COLOR/DEPTH) */

extern int depth_test;  /* whether depth testing turned on (OFF/ON) */
extern int texturing;   /* whether texturing is turned on (OFF/ON) */
extern int modulate;    /* whether modulating is turned on (OFF/ON) */
extern int alpha_blend; /* whether alpha blending is turned on (OFF/ON) */
extern int shading_mode;
extern int perspective_correct;
extern int modulate_type;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
OBJECT objects[MAX_N_OBJECTS];
int num_objects = 0;

/*************************************************************************/
/* custom init scene functions                                          */
/*************************************************************************/
//void set_object (OBJECT *o_ptr,
//                 int type, float cx, float cy, float cz,
//                 float scale, float r0, float r1)
//{
//    o_ptr->type = type;
//    o_ptr->center[X] = cx;
//    o_ptr->center[Y] = cy;
//    o_ptr->center[Z] = cz;
//    o_ptr->scale = scale;
//    o_ptr->radii[0] = r0;
//    o_ptr->radii[1] = r1;
//}
//
///* randomly drawn 3d objects */
//void init_objects_random (void)
//{
//    for(int i = 0; i < MAX_N_OBJECTS; i++)
//    {
//        float rand_x, rand_y, rand_z, rand_scale;
//        rand_x = random_float(-2, 0);
//        rand_y = random_float(-3, 3);
//        rand_z = random_float(0, 3);
//        rand_scale = random_float(1, 5);
//
//        set_object(&objects[i], CUBE, rand_x, rand_y, rand_z, rand_scale, NA, NA);
//    }
//    num_objects = MAX_N_OBJECTS;
//}
//
///* non-overlapping objects */
//void init_objects(void)
//{                                     //cx, cy, cz, scale, r0, r1
//    set_object(&objects[0], CUBE,       -3, 3,  0, 1,   NA,     NA);
//    set_object(&objects[1], CYLINDER,   0,  3,  0, 1,   0.5,    NA);
//    set_object(&objects[2], CONE,       3,  3,  0, 1,   0.5,    NA);
//    set_object(&objects[3], SPHERE,     -3, -3, 0, NA,  1,      NA);
//    set_object(&objects[4], TORUS,      0,  -3, 0, NA,  0.5,    1);
//    set_object(&objects[5], MESH,       3,  -3, 0, 0.7, NA,     NA);
//    set_object(&objects[6], TEAPOT,     0,  0,  0, 0.5,  NA,     NA);
//    num_objects = 7;
//}


/*************************************************************************/
/* functions                                                             */
/*************************************************************************/

OBJECT *get_curr_object (int objectID)
{
    return &objects[objectID];
}

/* write object metadata out to a scene file */
void write_scene (char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "w");
    
    if(fp == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        printf("%s has been opened and its contents overwritten.\n", file_name);
        fprintf(fp, "SCENE FILE: %s\n", file_name);
        fprintf(fp, "%i\n", num_objects);
        fprintf(fp, "%i %i %i\n", draw_mode, proj_mode, buffer);
        fprintf(fp, "%i %i %i %i %i %i %i\n", depth_test, texturing, modulate,
               alpha_blend, shading_mode, perspective_correct, modulate_type);
        
        int type;
        float cx, cy, cz, scale, r0, r1, dx_angle, dy_angle, dz_angle;
        for(int i = 0 ; i < num_objects; i++)
        {
            OBJECT *o = &objects[i];
            type = o->type;
            cx = o->center[X];
            cy = o->center[Y];
            cz = o->center[Z];
            scale = o->scale;
            r0 = o->radii[0];
            r1 = o->radii[1];
            
            //iterate through objects list
            fprintf(fp, "type: %i\tcx: %.2f\tcy: %.2f\tcz: %.2f\tscale: %.2f\tr0: %.2f\tr1: %.2f\tinit_orientation: %.2f, %.2f, %.2f\trotation: %.2f, %.2f, %.2f\n",
                    type, cx, cy, cz, scale, r0, r1,
                    o->init_orientation[X], o->init_orientation[Y], o->init_orientation[Z],
                    o->rotation[X], o->rotation[Y], o->rotation[Z]);
            //write each object out into file
        }
        printf("Done writing scene file to %s\n", file_name);
        fclose(fp);
    }
}

void read_scene (char *file_name)
{
    num_objects = 0;
    FILE *fp;
    fp = fopen(file_name, "r");
    char name[100];
    
    if(fp == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        printf("%s has been opened and its contents read.\n", file_name);
        fscanf(fp, "SCENE FILE: %s\n", name);
        printf("SCENE FILE: %s\n", name);
        fscanf(fp, "%i\n", &num_objects);
        fscanf(fp, "%i %i %i\n", &draw_mode, &proj_mode, &buffer);
        fscanf(fp, "%i %i %i %i %i %i %i\n", &depth_test, &texturing, &modulate,
               &alpha_blend, &shading_mode, &perspective_correct, &modulate_type);

        int type;
        float cx, cy, cz, scale, r0, r1, init_orient_x, init_orient_y, init_orient_z, rot_x, rot_y, rot_z;
        for(int i = 0 ; i < num_objects; i++)
        {
            //iterate through lines in file
            int ret = fscanf(fp, "type: %i\tcx: %f\tcy: %f\tcz: %f\tscale: %f\tr0: %f\tr1: %f\tinit_orientation: %f, %f, %f\trotation: %f, %f, %f\n",
                             &type, &cx, &cy, &cz, &scale, &r0, &r1,
                             &init_orient_x, &init_orient_y, &init_orient_z,
                             &rot_x, &rot_y, &rot_z);
            if(ret != 13)
            {
                printf("Error while reading %s\n", file_name);
                return;
            }
            OBJECT *o = &objects[i];
            o->type = type;
            o->ID = i;
            
            o->center[X] = cx;
            o->center[Y] = cy;
            o->center[Z] = cz;
            o->scale = scale;
            o->radii[0] = r0;
            o->radii[1] = r1;
            o->init_orientation[X] = init_orient_x;
            o->init_orientation[Y] = init_orient_y;
            o->init_orientation[Z] = init_orient_z;
            o->rotation[X] = rot_x;
            o->rotation[Y] = rot_y;
            o->rotation[Z] = rot_z;
            
            /* construct model matrix */
            MAT4 scale_mat, rotX, rotY, rotZ, translate;
            set_scale_nonuniform(&scale_mat, scale, scale, scale);
            set_rot_X (&rotX, init_orient_x);
            set_rot_Y (&rotY, init_orient_y);
            set_rot_Z (&rotZ, init_orient_z);
            set_transl (&translate, cx, cy, cz);
//            o->model_mat =

            
        }
        printf("Done reading scene file from %s\n", file_name);
        fclose(fp);
    }
}
