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

#define NUM_VERTS 5000

/****************************************************************/
/* global variables */
/****************************************************************/
POINT vertex_list[NUM_VERTS];
float tex_list[NUM_VERTS][4];
float color_list[NUM_VERTS][4];

int num_vertices = 0;

FACE face_list[10000];
int num_triangles = 0;

/****************************************************************/
/* functions */
/****************************************************************/
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

    /* set tex coordinates to four corners of texture */
    set_vec4(tex_list[0], 0, 0, 0, 0);
    set_vec4(tex_list[1], 1, 0, 0, 0);
    set_vec4(tex_list[2], 0, 1, 0, 0);
    set_vec4(tex_list[3], 1, 1, 0, 0);

    /* r, g, b color options */
    set_vec4(color_list[0], 1, 0, 0, 1);
    set_vec4(color_list[1], 0, 1, 0, 1);
    set_vec4(color_list[2], 0, 0, 1, 1);

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

/* init a n x n wavy mesh that starts at angle mesh_da */
void init_mesh (int n, float mesh_da)
{
    for(int r = 0; r < n; r++)
    {
        for(int c = 0; c < n; c++)
        {
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            p->world[X] = (float) c / (float) n ;
            p->world[Z] = (float) r / (float) n;
            p->world[Y] = cos(mesh_da + (float) r / (float) n * 2 * PI) *
                          sin(mesh_da + (float) c / (float) n * 2 * PI);
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / (float) n,
                                            (float) r / (float) n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / (float) n,
                                              (float) r / (float) n, 0, 1);
        }
    }
    num_vertices = n * n;
    
    int t = 0;
    for(int r = 0; r < n - 2; r++)
    {
        for(int c = 0; c < n - 2; c++)
        {
            set_face(t, r * n + c, (r + 1) * n + c, (r + 1) * n + (c + 1),
                        r * n + c, (r + 1) * n + c, (r + 1) * n + (c + 1),
                        r * n + c, (r + 1) * n + c, (r + 1) * n + (c + 1));
            t++;
            set_face(t, r * n + c, (r + 1) * n + (c + 1), r * n + (c + 1),
                        r * n + c, (r + 1) * n + (c + 1), r * n + (c + 1),
                        r * n + c, (r + 1) * n + (c + 1), r * n + (c + 1));
            t++;
        }
    }
    num_triangles = t;

}

/* transform model from world space to screen space */
void xform_model(float scale)
{
    for(int i = 0; i < num_vertices; i++)
    {
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
        FACE f = face_list[i];
        /* get indices from face object */
        int v0 = f.vertices[0];
        int v1 = f.vertices[1];
        int v2 = f.vertices[2];
        
        int c0 = f.colors[0];
        int c1 = f.colors[1];
        int c2 = f.colors[2];
        
        int t0 = f.tex[0];
        int t1 = f.tex[1];
        int t2 = f.tex[2];
        
        /* fix the colors and textures of each point in vertex_list to the
            color and tex coords specified by FACE object */
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
            draw_triangle_barycentric (&vertex_list[v0], &vertex_list[v1],
                                       &vertex_list[v2]);
        }
    }
}

/* 3d rotation x_angle about the x axis, y_angle about the y axis, and
    z_angle about the z axis */
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

/* scale normalized view coordinates to screen coordinates
 *  (for perspective proj) */
void viewport_xform(float scale)
{
    for(int i = 0; i < num_vertices; i++)
    {
        vertex_list[i].position[X] *= scale;
        vertex_list[i].position[Y] *= scale;
        vertex_list[i].position[Z] *= 1;

    }
}
