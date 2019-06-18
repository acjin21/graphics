#include "model.h"
#include <math.h>
/* modeling */
POINT vertex_list[2000];
int num_vertices = 0;

int triangle_list[4000][3];
int num_triangles = 0;

/* set vec to  */
void set_vec4 (float vec[4], float x, float y, float z, float w)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = w;
}

/* set triangle vertices to indices v0_idx, v1_idx, and v2_idx. */
void set_triangle (int t_idx, int v0_idx, int v1_idx, int v2_idx)
{
    triangle_list[t_idx][0] = v0_idx;
    triangle_list[t_idx][1] = v1_idx;
    triangle_list[t_idx][2] = v2_idx;
}

/* set vertices of a unit cube with world space coordinates and
    random color + random tex coords */
void init_cube(void)
{
    set_vec4(vertex_list[0].world, 0.5, 0.5, -0.5, 1.0);
    //set_vec4(vertex_list[0].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[0].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0.5, 1.0);
    //set_vec4(vertex_list[1].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[1].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0.5, 1.0);
    //set_vec4(vertex_list[2].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[2].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[3].world, 0.5, -0.5, -0.5, 1.0);
    //set_vec4(vertex_list[3].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[3].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[4].world, -0.5, 0.5, -0.5, 1.0);
    //set_vec4(vertex_list[4].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[4].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[5].world, -0.5, 0.5, 0.5, 1.0);
    //set_vec4(vertex_list[5].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[5].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[6].world, -0.5, -0.5, 0.5, 1.0);
    //set_vec4(vertex_list[6].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[6].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    
    set_vec4(vertex_list[7].world, -0.5, -0.5, -0.5, 1.0);
    //set_vec4(vertex_list[7].color, random_float(0, 1),
//             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[7].tex, random_float(0, 1),
             random_float(0, 1), 0, 0);
    num_vertices = 8;
    
    set_triangle(0, 0, 2, 1);
    set_triangle(1, 0, 3, 2);
    set_triangle(2, 1, 6, 5);
    set_triangle(3, 1, 2, 6);
    set_triangle(4, 5, 7, 4);
    set_triangle(5, 5, 6, 7);
    set_triangle(6, 4, 3, 0);
    set_triangle(7, 4, 7, 3);
    set_triangle(8, 4, 0, 1);
    set_triangle(9, 4, 1, 5);
    set_triangle(10, 7, 2, 3);
    set_triangle(11, 7, 6, 2);
    
    num_triangles = 12;
}

/* transform model from world space to screen space */
void xform_model(void)
{
    int scale = 200;
    for(int i = 0; i < num_vertices; i++)
    {
        scalar_multiply(scale, vertex_list[i].world, vertex_list[i].position);
    }
}

/* draw wire-frame or filled in model */
void draw_model(int mode)
{
    for(int i = 0; i < num_triangles; i++)
    {
        int t0 = triangle_list[i][0];
        int t1 = triangle_list[i][1];
        int t2 = triangle_list[i][2];

        // FRAME = 0, FILL = 1
        if(mode == FRAME)
        {
            draw_line(&vertex_list[t0], &vertex_list[t1], DRAW);
            draw_line(&vertex_list[t1], &vertex_list[t2], DRAW);
            draw_line(&vertex_list[t0], &vertex_list[t2], DRAW);
        }
        else if(mode == FILL)
        {
            draw_triangle(&vertex_list[t0], &vertex_list[t1], &vertex_list[t2]);
        }
    }
}

void rotate_model(float x_angle, float y_angle, float z_angle)
{
    z_angle *= (PI / 180.0);
    y_angle *= (PI / 180.0);
    x_angle *= (PI / 180.0);

    float nx, ny, nz;
    for(int i = 0; i < num_vertices; i++)
    {
        POINT *p = &vertex_list[i];

        /* about z axis */
        nx = p->world[X] * cos(z_angle) - p->world[Y] * sin(z_angle);
        ny = p->world[X] * sin(z_angle) + p->world[Y] * cos(z_angle);
        vertex_list[i].world[X] = nx;
        vertex_list[i].world[Y] = ny;

        /* about y axis */
        nx = p->world[X] * cos(y_angle) - p->world[Z] * sin(y_angle);
        nz = p->world[X] * sin(y_angle) + p->world[Z] * cos(y_angle);
        vertex_list[i].world[X] = nx;
        vertex_list[i].world[Z] = nz;

        /* about x axis */
        ny = p->world[Y] * cos(x_angle) - p->world[Z] * sin(x_angle);
        nz = p->world[Y] * sin(x_angle) + p->world[Z] * cos(x_angle);
        vertex_list[i].world[Y] = ny;
        vertex_list[i].world[Z] = nz;
    }
}
