#include "scene.h"

#include "macros.h"
#include "util.h"
#include <stdio.h>

OBJECT objects[MAX_N_OBJECTS];
int num_objects = 0;

void set_object (OBJECT *o_ptr, int type, float cx, float cy, float cz,
                 float scale, float r0, float r1)
{
    o_ptr->type = type;
    o_ptr->center[X] = cx;
    o_ptr->center[Y] = cy;
    o_ptr->center[Z] = cz;
    o_ptr->scale = scale;
    o_ptr->radii[0] = r0;
    o_ptr->radii[1] = r1;
}

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
            dx_angle = o->rotation[X];
            dy_angle = o->rotation[Y];
            dz_angle = o->rotation[Z];
            
            //iterate through objects list
            fprintf(fp,
                    "type: %i\tcx: %.2f\tcy: %.2f\tcz: %.2f\tscale: %.2f\tr0: %.2f\tr1: %.2f\trotation: %.2f, %.2f, %.2f\n",
                    type, cx, cy, cz, scale, r0, r1, dx_angle, dy_angle, dz_angle);
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
        
        int type;
        float cx, cy, cz, scale, r0, r1, dx_angle, dy_angle, dz_angle;
        for(int i = 0 ; i < num_objects; i++)
        {
            //iterate through lines in file
            int ret = fscanf(fp, "type: %i\tcx: %f\tcy: %f\tcz: %f\tscale: %f\tr0: %f\tr1: %f\trotation: %f, %f, %f\n",
                             &type, &cx, &cy, &cz, &scale, &r0, &r1,
                             &dx_angle, &dy_angle, &dz_angle);
            if(ret != 10)
            {
                printf("Error while reading %s\n", file_name);
                return;
            }
            OBJECT *o = &objects[i];
            o->type = type;
            o->center[X] = cx;
            o->center[Y] = cy;
            o->center[Z] = cz;
            o->scale = scale;
            o->radii[0] = r0;
            o->radii[1] = r1;
            o->rotation[X] = dx_angle;
            o->rotation[Y] = dy_angle;
            o->rotation[Z] = dz_angle;

        }
        printf("Done reading scene file from %s\n", file_name);
        fclose(fp);
    }
}
