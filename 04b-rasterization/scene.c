#include "scene.h"


#include <stdio.h>

#include "macros.h"
#include "util.h"
#include "model.h"
#include "math_lib/mat4.h"
#include "app.h"
#include "point.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
OBJECT objects[MAX_N_OBJECTS];
int num_objects = 0;

/*************************************************************************/
/* custom init scene functions                                          */
/*************************************************************************/
void set_object (OBJECT *o_ptr,
                 int type, float cx, float cy, float cz,
                 float scale, float r0, float r1,
                 float init_rx, float init_ry, float init_rz)
{
    o_ptr->type = type;
    o_ptr->center[X] = cx;
    o_ptr->center[Y] = cy;
    o_ptr->center[Z] = cz;
    o_ptr->scale = scale;
    o_ptr->radii[0] = r0;
    o_ptr->radii[1] = r1;
    o_ptr->init_orientation[X] = init_rx;
    o_ptr->init_orientation[Y] = init_ry;
    o_ptr->init_orientation[Z] = init_rz;
}


/* non-overlapping objects */
void init_benchmark_scene (void)
{                                     //cx, cy, cz, scale,  r0, r1
    set_object(&objects[0], QUAD,       -3, 3,  0,  1,      NA, NA, 0, 0, 0);
    num_objects = 1;
}


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
        fprintf(fp, "%i %i %i\n", draw_mode, proj_mode, framebuffer_src);
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
            fprintf(fp, "%i\t[%.2f, %.2f, %.2f]\t[%.2f, %.2f, %.2f]\t[%.2f, %.2f, %.2f]\t[%.2f, %.2f, %.2f]\t[%i, %i, %i, %i]\n",
                    type, cx, cy, cz, scale, r0, r1,
                    o->init_orientation[X], o->init_orientation[Y], o->init_orientation[Z],
                    o->rotation[X], o->rotation[Y], o->rotation[Z],
                    o->texturing, o->texture_idx, o->tex_gen_mode, o->persp_corr);
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
        fscanf(fp, "%i %i %i\n", &draw_mode, &proj_mode, &framebuffer_src);
        fscanf(fp, "%i %i %i %i %i %i %i\n", &depth_test, &texturing, &modulate,
               &alpha_blend, &shading_mode, &perspective_correct, &modulate_type);

        int type, texturing, tex_idx, tex_gen_mode, persp_corr;
        float cx, cy, cz, scale, r0, r1, init_orient_x, init_orient_y, init_orient_z, rot_x, rot_y, rot_z;
        
        for(int i = 0 ; i < num_objects; i++)
        {
            //iterate through lines in file
            int ret = fscanf(fp, "%i\t[%f, %f, %f]\t[%f, %f, %f]\t[%f, %f, %f]\t[%f, %f, %f]\t[%i, %i, %i, %i]\n",
                             &type, &cx, &cy, &cz, &scale, &r0, &r1,
                             &init_orient_x, &init_orient_y, &init_orient_z,
                             &rot_x, &rot_y, &rot_z, &texturing, &tex_idx, &tex_gen_mode, &persp_corr);
            if(ret != 17)
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
            
            /* init rotation when move into world space */
            o->init_orientation[X] = init_orient_x;
            o->init_orientation[Y] = init_orient_y;
            o->init_orientation[Z] = init_orient_z;
            /* individual rotations */
            o->rotation[X] = rot_x;
            o->rotation[Y] = rot_y;
            o->rotation[Z] = rot_z;
            
            o->texturing = texturing;
            o->texture_idx = tex_idx;
            o->tex_gen_mode = tex_gen_mode;
            o->persp_corr = persp_corr;

        }
        printf("Done reading scene file from %s\n", file_name);
        fclose(fp);
    }
}
