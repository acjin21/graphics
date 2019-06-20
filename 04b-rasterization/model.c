#include "model.h"
#include <math.h>
#include <stdio.h>

extern float depth_buffer[800][800];

typedef struct face
{
    int vertices[3];
    int colors[3];
    int tex[3];
} FACE;

/* modeling */
POINT vertex_list[2000];
float tex_list[100][4];
float color_list[100][4];

int num_vertices = 0;

int triangle_list[4000][3];
FACE face_list[4000];
int num_triangles = 0;

/* set triangle vertices to indices v0_idx, v1_idx, and v2_idx. */
void set_triangle (int t_idx, int v0_idx, int v1_idx, int v2_idx)
{
    triangle_list[t_idx][0] = v0_idx;
    triangle_list[t_idx][1] = v1_idx;
    triangle_list[t_idx][2] = v2_idx;
}

void set_face (int i, int v0, int v1, int v2,
                      int c0, int c1, int c2,
                      int t0, int t1, int t2)
{
    face_list[i].vertices[0] = v0;
    face_list[i].vertices[1] = v1;
    face_list[i].vertices[2] = v2;
    
    face_list[i].colors[0] = c0;
    face_list[i].colors[1] = c1;
    face_list[i].colors[2] = c2;

    face_list[i].tex[0] = t0;
    face_list[i].tex[1] = t1;
    face_list[i].tex[2] = t2;
}

/* set vertices of a unit cube with world space coordinates and
    random color + random tex coords */
void init_cube (void)
{
    set_vec4(vertex_list[0].world, 0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[3].world, 0.5, -0.5, -0.5, 1.0);
    set_vec4(vertex_list[4].world, -0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[5].world, -0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[6].world, -0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[7].world, -0.5, -0.5, -0.5, 1.0);

    set_vec4(vertex_list[0].color, 1, 0, 0, 0);
    set_vec4(vertex_list[1].color, 0, 1, 0, 0);
    set_vec4(vertex_list[2].color, 0, 0, 1, 0);
    set_vec4(vertex_list[3].color, 1, 0, 0, 0);
    set_vec4(vertex_list[4].color, 0, 1, 0, 0);
    set_vec4(vertex_list[5].color, 0, 0, 1, 0);
    set_vec4(vertex_list[6].color, 1, 0, 0, 0);
    set_vec4(vertex_list[7].color, 0, 1, 0, 0);

    set_vec4(tex_list[0], 0, 0, 0, 0);
    set_vec4(tex_list[1], 1, 0, 0, 0);
    set_vec4(tex_list[2], 0, 1, 0, 0);
    set_vec4(tex_list[3], 1, 1, 0, 0);

    set_vec4(color_list[0], 1, 0, 0, 1);
    set_vec4(color_list[1], 0, 1, 0, 1);
    set_vec4(color_list[2], 0, 0, 1, 1);

//    set_vec4(vertex_list[0].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[1].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[2].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[3].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[4].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[5].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[6].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//    set_vec4(vertex_list[7].tex, random_float(0, 1), random_float(0, 1), 0, 0);
//
    
    num_vertices = 8;
    //              vertices    colors      texture coords
    set_face(0,     0, 2, 1,    0, 1, 2,    0, 3, 1);
    set_face(1,     0, 3, 2,    0, 1, 2,    0, 2, 3);
    set_face(2,     1, 6, 5,    0, 1, 2,    0, 3, 1);
    set_face(3,     1, 2, 6,    0, 1, 2,    0, 2, 3);
    set_face(4,     5, 7, 4,    0, 1, 2,    0, 3, 1);
    set_face(5,     5, 6, 7,    0, 1, 2,    0, 2, 3);
    set_face(6,     4, 3, 0,    0, 1, 2,    0, 3, 1);
    set_face(7,     4, 7, 3,    0, 1, 2,    0, 2, 3);
    set_face(8,     0, 5, 4,    0, 1, 2,    0, 3, 1);
    set_face(9,     0, 1, 5,    0, 1, 2,    0, 2, 3);
    set_face(10,    7, 2, 3,    0, 1, 2,    0, 3, 1);
    set_face(11,    7, 6, 2,    0, 1, 2,    0, 2, 3);
    
    num_triangles = 12;
}

void init_mesh (void)
{
    for(int r = 0; r < 32; r++)
    {
        for(int c = 0; c < 32; c++)
        {
            POINT *p = &vertex_list[(r * 32) + c];
            
            p->world[X] = (float) c / 32.0;
            p->world[Y] = (float) r / 32.0;
            p->world[Z] = cos((float) r / 32.0 * 2.0 * PI) *
                sin((float) c / 32.0 * 2.0 * PI);
            p->world[W] = 1.0;
        }
    }
    num_vertices = 32 * 32;
    int n = 0;
    for(int r = 0; r < 30; r++)
    {
        for(int c = 0; c < 30; c++)
        {
            set_face(n, r * 32 + c, (r + 1) * 32 + c, (r + 1) * 32 + (c + 1),
                     0, 1, 2,   0, 3, 1);
            n++;
            set_face(n, r * 32 + c, (r + 1) * 32 + (c + 1), r * 32 + (c + 1),
                     0, 1, 2,   0, 3, 1);
            n++;
        }
    }
    num_triangles = n;

}

/* transform model from world space to screen space */
void xform_model(float scale)
{
    for(int i = 0; i < num_vertices; i++)
    {
//        scalar_multiply(scale, vertex_list[i].world, vertex_list[i].position);
        vertex_list[i].position[X] = vertex_list[i].world[X] * scale;
        vertex_list[i].position[Y] = vertex_list[i].world[Y] * scale;
        vertex_list[i].position[Z] = vertex_list[i].world[Z];
        vertex_list[i].position[W] = 1.0;


    }
}

/* draw wire-frame or filled in model */
void draw_model(int mode)
{
    for(int i = num_triangles - 1; i >= 0; i--)
    {
        FACE f0 = face_list[i];
        int v0 = f0.vertices[0];
        int v1 = f0.vertices[1];
        int v2 = f0.vertices[2];
        
        int c0 = f0.colors[0];
        int c1 = f0.colors[1];
        int c2 = f0.colors[2];
        
        int t0 = f0.tex[0];
        int t1 = f0.tex[1];
        int t2 = f0.tex[2];

        cpy_vec4(vertex_list[v0].color, color_list[c0]);
        cpy_vec4(vertex_list[v0].tex, tex_list[t0]);
        
        cpy_vec4(vertex_list[v1].color, color_list[c1]);
        cpy_vec4(vertex_list[v1].tex, tex_list[t1]);
        
        cpy_vec4(vertex_list[v2].color, color_list[c2]);
        cpy_vec4(vertex_list[v2].tex, tex_list[t2]);

        // FRAME = 0, FILL = 1
        if(mode == FRAME)
        {
            draw_line(&vertex_list[v0], &vertex_list[v1], DRAW);
            draw_line(&vertex_list[v1], &vertex_list[v2], DRAW);
            draw_line(&vertex_list[v0], &vertex_list[v2], DRAW);
        }
        else if(mode == FILL)
        {
            draw_triangle_barycentric (&vertex_list[v0],
                                      &vertex_list[v1],
                                      &vertex_list[v2]);
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

/* translate model in world space by distance units along the Z axis */
void translate_model (float distance)
{
    for(int i = 0; i < num_vertices; i++)
    {
        vertex_list[i].world[Z] += distance;
    }
}

/* perspective transform from world to screen coordinates */
void perspective_xform(float near, float far)
{
    for(int i = 0; i < num_vertices; i++)
    {
        float x, y, z;
        x = vertex_list[i].world[X];
        y = vertex_list[i].world[Y];
        z = vertex_list[i].world[Z];
        
        vertex_list[i].position[X] = near * x / z;
        vertex_list[i].position[Y] = near * y / z;
        vertex_list[i].position[Z] = (float) z / (far - near); //normalize Z        
        vertex_list[i].position[W] = 1.0;
    }
}

void viewport_xform(float scale)
{
    for(int i = 0; i < num_vertices; i++)
    {
//        scalar_multiply(scale, vertex_list[i].position, vertex_list[i].position);
        vertex_list[i].position[X] *= scale;
        vertex_list[i].position[Y] *= scale;
        vertex_list[i].position[Z] *= 1;

    }
}
