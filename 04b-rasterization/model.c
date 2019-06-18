#include "model.h"

/* modeling */
POINT vertex_list[2000];
int num_vertices = 0;

int triangle_list[4000][3];
int num_triangles = 0;

void set_vec4 (float vec[4], float w_x, float w_y, float w_z, float w_w)
{
    vec[0] = w_x;
    vec[1] = w_y;
    vec[2] = w_z;
    vec[3] = w_w;
}

void set_triangle (int t_idx, int v0_idx, int v1_idx, int v2_idx)
{
    triangle_list[t_idx][0] = v0_idx;
    triangle_list[t_idx][1] = v1_idx;
    triangle_list[t_idx][2] = v2_idx;
}


void init_cube(void)
{
    set_vec4(vertex_list[0].world, 0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[0].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[0].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[1].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[1].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[2].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[2].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[3].world, 0.5, -0.5, -0.5, 1.0);
    set_vec4(vertex_list[3].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[3].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[4].world, -0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[4].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[4].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[5].world, -0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[5].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[5].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[6].world, -0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[6].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[6].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
    set_vec4(vertex_list[7].world, -0.5, -0.5, -0.5, 1.0);
    set_vec4(vertex_list[7].color, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    set_vec4(vertex_list[7].tex, random_float(0, 1),
             random_float(0, 1), random_float(0, 1), 0);
    
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

void xform_model(void)
{
    int scale = 200;
    for(int i = 0; i < num_vertices; i++)
    {
        scalar_multiply(scale, vertex_list[i].world, vertex_list[i].position);
    }
}

void draw_model(void)
{
    for(int i = 0; i < num_triangles; i++)
    {
        draw_line(&vertex_list[triangle_list[i][0]],
                  &vertex_list[triangle_list[i][1]], DRAW);
        draw_line(&vertex_list[triangle_list[i][1]],
                  &vertex_list[triangle_list[i][2]], DRAW);
        draw_line(&vertex_list[triangle_list[i][0]],
                  &vertex_list[triangle_list[i][2]], DRAW);
    }
}
