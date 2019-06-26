#include "model.h"
#include <math.h>
#include <stdio.h>

extern float depth_buffer[WIN_H][WIN_W];

typedef struct face
{
    int vertices[3];
    int colors[3];
    int tex[3];
    float f_normal[4]; //face normal in world coordinates
} FACE;

#define NUM_VERTS 1000000

/****************************************************************/
/* global variables */
/****************************************************************/
POINT vertex_list[NUM_VERTS];
float tex_list[NUM_VERTS][4];
float color_list[NUM_VERTS][4];

FACE face_list[10000];
int num_triangles = 0;
int num_vertices = 0;
int num_normals = 0; //for finding centroids + endpoints in vertex_list
int num_textures = 0;

extern int texturing; // mode: whether texturing or not
extern int perspective_correct; // mode: for perspective correct interpolation
extern int normals; // mode: whether drawing normals or not
extern int phong_shading;

extern float light[4]; // light vector from light.c
/****************************************************************/
/* helper functions */
/****************************************************************/
void add_face (int v0, int v1, int v2,
               int c0, int c1, int c2,
               int t0, int t1, int t2)
{
    face_list[num_triangles].vertices[0] = v0;
    face_list[num_triangles].vertices[1] = v1;
    face_list[num_triangles].vertices[2] = v2;
    
    face_list[num_triangles].colors[0] = c0;
    face_list[num_triangles].colors[1] = c1;
    face_list[num_triangles].colors[2] = c2;

    face_list[num_triangles].tex[0] = t0;
    face_list[num_triangles].tex[1] = t1;
    face_list[num_triangles].tex[2] = t2;
    
    POINT *p = &vertex_list[v0];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    
    p = &vertex_list[v1];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    
    p = &vertex_list[v2];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    
    num_triangles++;
}

void reset_num_tris (int num_verts)
{
    for(int i = 0; i < num_verts; i++)
    {
        vertex_list[i].num_tris = 0;
    }
}

/****************************************************************/
/* model init functions */
/****************************************************************/

void init_quad (void)
{
    num_vertices = 4;
    
    set_vec4(vertex_list[0].world, -0.5, 0.5, 0, 1.0);
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0, 1.0);
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0, 1.0);
    set_vec4(vertex_list[3].world, -0.5, -0.5, 0, 1.0);
    
    set_vec4(color_list[0], 1, 0, 0, 1);
    set_vec4(color_list[1], 0, 1, 0, 1);
    set_vec4(color_list[2], 0, 0, 1, 1);
    set_vec4(color_list[3], 0.5, 0.5, 0.5, 1);
    set_vec4(color_list[4], 0, 0, 0, 1);
    
    reset_num_tris(num_vertices);
    num_triangles = 0;
    set_vec4(tex_list[0], 0, 0, 0, 0);
    set_vec4(tex_list[1], 1, 0, 0, 0);
    set_vec4(tex_list[2], 1, 1, 0, 0);
    set_vec4(tex_list[3], 0, 1, 0, 0);
    
    add_face(0, 1, 2,    0, 1, 2,    0, 2, 1);
    add_face(0, 2, 3,    0, 1, 2,    0, 3, 2);
}

/* set vertices of a unit cube with world space coordinates and
    random color + random tex coords */
void init_cube (float scale, float cx, float cy, float cz)
{
    /* add vertices */
    num_vertices = 8;
    
    set_vec4(vertex_list[0].world, 0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[3].world, 0.5, -0.5, -0.5, 1.0);
    set_vec4(vertex_list[4].world, -0.5, 0.5, -0.5, 1.0);
    set_vec4(vertex_list[5].world, -0.5, 0.5, 0.5, 1.0);
    set_vec4(vertex_list[6].world, -0.5, -0.5, 0.5, 1.0);
    set_vec4(vertex_list[7].world, -0.5, -0.5, -0.5, 1.0);
    
    float center[4] = {cx, cy, cz, 0};
    for(int i = 0; i < num_vertices; i++)
    {
        scalar_multiply(scale, vertex_list[i].world, vertex_list[i].world);
        vector_add(vertex_list[i].world, center, vertex_list[i].world);
        vertex_list[i].world[W] = 1;
    }
    
    /* set tex coordinates to four corners of texture */
    set_vec4(tex_list[0], 0, 0, 0, 0);
    set_vec4(tex_list[1], 1, 0, 0, 0);
    set_vec4(tex_list[2], 0, 1, 0, 0);
    set_vec4(tex_list[3], 1, 1, 0, 0);
    num_textures = 4;

    /* r, g, b color options */
    set_vec4(color_list[0], 1, 0, 0, 1);
    set_vec4(color_list[1], 0, 1, 0, 1);
    set_vec4(color_list[2], 0, 0, 1, 1);
    set_vec4(color_list[3], 0.5, 0.5, 0.5, 1);
    set_vec4(color_list[4], 0, 0, 0, 1);


    /* reset num_tris for each vertex */
    reset_num_tris(num_vertices);
    
    /* add faces/triangles */
    num_triangles = 0; // reset num of triangles
    //       vertices    colors      texture coords
    add_face(0, 1, 2,    0, 1, 2,    0, 3, 1);
    add_face(0, 2, 3,    1, 1, 1,    0, 2, 3);
    add_face(1, 5, 6,    0, 1, 2,    0, 3, 1);
    add_face(1, 6, 2,    2, 2, 2,    0, 2, 3);
    add_face(5, 4, 7,    0, 1, 2,    0, 3, 1);
    add_face(5, 7, 6,    0, 1, 2,    0, 2, 3);
    add_face(4, 0, 3,    0, 0, 0,    0, 3, 1);
    add_face(4, 3, 7,    0, 1, 2,    0, 2, 3);
    add_face(0, 4, 5,    0, 1, 2,    0, 3, 1);
    add_face(0, 5, 1,    0, 1, 2,    0, 2, 3);
    add_face(7, 3, 2,    0, 1, 2,    0, 3, 1);
    add_face(7, 2, 6,    0, 1, 2,    0, 2, 3);
    /* should now have 12 triangles */
}

void read_obj_file (char *file_name, float scale, float cx, float cy, float cz)
{
    /* set tex coordinates to four corners of texture */
    set_vec4(tex_list[0], 0, 0, 0, 0);
    set_vec4(tex_list[1], 1, 0, 0, 0);
    set_vec4(tex_list[2], 0, 1, 0, 0);
    set_vec4(tex_list[3], 1, 1, 0, 0);
    num_textures = 4;
    
    /* r, g, b color options */
    set_vec4(color_list[0], 1, 0, 0, 1);
    set_vec4(color_list[1], 0, 1, 0, 1);
    set_vec4(color_list[2], 0, 0, 1, 1);
    set_vec4(color_list[3], 0.5, 0.5, 0.5, 1);
    
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        num_vertices = 0;
        float x, y, z;
        int i, j, k;
        while(fscanf(fp, "v %f %f %f\n", &x, &y, &z) == 3)
        {
            set_vec4(vertex_list[num_vertices].world,
                     cx + scale * x,
                     cy + scale * y,
                     cz + scale * z, 1.0);
            num_vertices++;
        }
        printf("%i vertices read\n", num_vertices);
        /* reset num_tris for each vertex */
        reset_num_tris(num_vertices);
        
        /* add faces/triangles */
        num_triangles = 0; // reset num of triangles
        fscanf(fp, "\n");
        while(fscanf(fp, "f %d %d %d\n", &i, &j, &k) == 3)
        {
            //       vertices       colors      texture coords
            add_face(i-1, j-1, k-1,    3, 3, 3,    0, 3, 1);
        }
        printf("%i faces read\n", num_triangles);
        
        fclose(fp);
    }
}

void write_obj_file (char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "w");
    
    if (fp == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        printf("%s has been opened and its contents overwritten.\n", file_name);
        float x, y, z;
        int i, j, k;
        printf("num_vertices = %i\n", num_vertices);
        for(int v = 0; v < num_vertices; v++)
        {
            x = vertex_list[v].world[X];
            y = vertex_list[v].world[Y];
            z = vertex_list[v].world[Z];
            fprintf(fp, "v %f %f %f\n", x, y, z);
        }
        for(int f = 0; f < num_triangles; f++)
        {
            i = face_list[f].vertices[0];
            j = face_list[f].vertices[1];
            k = face_list[f].vertices[2];

            fprintf(fp, "f %d %d %d\n", i+1, j+1, k+1);
        }

        printf("Done writing obj file to %s\n", file_name);
        fclose(fp);
    }
}

/* init a n x n wavy mesh that starts at angle mesh_da */
void init_mesh (float scale, float cx, float cy, float cz, float da)
{
    int n = 32;
    int width = n;
    int height = n;
    /* add vertices */
    num_vertices = n * n;
    for(int r = 0; r < n; r++)
    {
        for(int c = 0; c < n; c++)
        {
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            p->world[X] = scale * u + cx;
            p->world[Y] = scale * v + cy;
            p->world[Z] = cos(da + v * 2 * PI) * sin(da + u * 2 * PI) + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / n, (float) r / n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / n, (float) r / n, 0, 1);
        }
    }
    reset_num_tris(num_vertices);
    
    /* add triangles/faces */
    num_triangles = 0;

    for(int r = 0; r < n - 2; r++)
    {
        for(int c = 0; c < n - 2; c++)
        {
            add_face(r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c );
            add_face(r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1));
        }
    }

}

/* init a cylinder */
void init_cylinder (float radius, float scale, float cx, float cy, float cz)
{
    int n = 32;
    int width = n;
    int height = n;
    /* add vertices */
    num_vertices = n * n;
    for(int r = 0; r < n; r++)
    {
        for(int c = 0; c < n; c++)
        {
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            p->world[X] = radius * cos(u * 2 * PI) + cx;
            p->world[Y] = radius * sin(u * 2 * PI) + cy;
            p->world[Z] = scale * v + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / n, (float) r / n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / n, (float) r / n, 0, 1);
//            set_vec4(color_list[0], 0.5, 0.5, 0.5, 1);
        }
    }
    reset_num_tris(num_vertices);
    
    /* add triangles/faces */
    num_triangles = 0;
    
    for(int r = 0; r < n - 1; r++)
    {
        for(int c = 0; c < n - 1; c++)
        {
            add_face(r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     //                     0, 0, 0,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c);
            add_face(r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     //                     0, 0, 0,
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1));
        }
    }
    
}

/* init a cone */
void init_cone (float radius, float scale, float cx, float cy, float cz)
{
    int n = 32;
    int width = n;
    int height = n;
    /* add vertices */
    num_vertices = n * n;
    for(int r = 0; r < n; r++)
    {
        for(int c = 0; c < n; c++)
        {
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            float new_rad = radius * ((float)height - 1 - r) / (height - 1);
            p->world[X] = new_rad * cos(u * 2 * PI) + cx;
            p->world[Y] = new_rad * sin(u * 2 * PI) + cy;
            p->world[Z] = scale * v + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / n, (float) r / n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / n, (float) r / n, 0, 1);
//            set_vec4(color_list[0], 0.5, 0.5, 0.5, 1);
        }
    }
    reset_num_tris(num_vertices);
    
    /* add triangles/faces */
    num_triangles = 0;
    
    for(int r = 0; r < n - 1; r++)
    {
        for(int c = 0; c < n - 1; c++)
        {
            add_face(r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     //                     0, 0, 0,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c);
            add_face(r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     //                     0, 0, 0,
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1));
        }
    }
    
}

/* init a sphere */
void init_sphere (float radius, float cx, float cy, float cz)
{
    int n = 32;
    int width = n;
    int height = n;
    /* add vertices */
    num_vertices = n * n;
    for(int r = 0; r < n; r++)
    {
        for(int c = 0; c < n; c++)
        {
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            p->world[X] = radius * cos(u * 1 * PI) + cx;
            p->world[Y] = radius * cos(v * 2 * PI) * sin(u * PI) + cy;
            p->world[Z] = radius * sin(v * 2 * PI) * sin(u * PI) + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / n, (float) r / n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / n, (float) r / n, 0, 1);
//            set_vec4(color_list[0], 0.5, 0.5, 0.5, 1);
        }
    }
    reset_num_tris(num_vertices);
    
    /* add triangles/faces */
    num_triangles = 0;
    
    for(int r = 0; r < n - 1; r++)
    {
        for(int c = 0; c < n - 1; c++)
        {
            add_face(r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     //                     0, 0, 0,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c);
            add_face(r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     //                     0, 0, 0,
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1));
        }
    }
    
}

/* init a torus */
void init_torus (float tube_radius, float hole_radius,  float cx, float cy, float cz)
{
    int n = 32;
    int width = n;
    int height = n;
    tube_radius *= 0.5;
    float count = 0;
    /* add vertices */
    num_vertices = n * n;
    for(int r = 0; r < height; r++)
    {
        for(int c = 0; c < width; c++)
        {
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            /* world coordinates */
            p->world[X] = (tube_radius * cos(u * 2 * PI) + 0.5 * hole_radius) *
                            cos(v * 2 * PI) + cx;
            p->world[Y] = tube_radius * sin(u * 2 * PI) + cy;
            p->world[Z] = (tube_radius * cos(u * 2 * PI) + 0.5 * hole_radius) *
                            sin(v * 2 * PI) + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], (float) c / n, (float) r / n, 0, 0);
            set_vec4(color_list[(r * n) + c], (float) c / n, (float) r / n, 0, 1);
//            set_vec4(color_list[0], 0.5, 0.5, 0.5, 1);
        }
        count += 0.05;
    }
    reset_num_tris(num_vertices);
    
    /* add triangles/faces */
    num_triangles = 0;
    
    for(int r = 0; r < n - 1; r++)
    {
        for(int c = 0; c < n - 1; c++)
        {
            add_face(r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     //                     0, 0, 0,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c,
                     r * n + c, (r + 1) * n + (c + 1), (r + 1) * n + c);
            add_face(r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     //                     0, 0, 0,
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1),
                     r * n + c, r * n + (c + 1), (r + 1) * n + (c + 1));
        }
    }
    
}


//call after calculate_face_normals()
//insert normals into vertex_list for a specific 3d model
    //(2 * num_normals extra points added by this function)
//call before 3D transformations; actually draw the normals in draw_model
void insert_normal_coords(void)
{
    num_normals = 0;
    float tmp[4], color[4];
    POINT center, end;
    for(int i = 0; i < num_triangles; i++)
    {
        //get vertices of triangle
        int t0 = face_list[i].vertices[0];
        int t1 = face_list[i].vertices[1];
        int t2 = face_list[i].vertices[2];
        
        //calculate centroid
        avg_of_3_vecs(vertex_list[t0].world, vertex_list[t1].world,
                      vertex_list[t2].world, center.world);
        
        //draw red normals
        set_vec4(color, 1, 0, 0, 1);

        //store centroid
        cpy_vec4(vertex_list[num_vertices + 2 * num_normals].world, center.world);
        cpy_vec4(vertex_list[num_vertices + 2 * num_normals].color, color);
        
        //calculate endpoint
        scalar_divide(6, face_list[i].f_normal, tmp);
        vector_add(center.world, tmp, end.world);
        //store endpoint
        cpy_vec4(vertex_list[num_vertices + 2 * num_normals + 1].world, end.world);
        cpy_vec4(vertex_list[num_vertices + 2 * num_normals + 1].color, color);


        //increment num_normals
        num_normals++;
    }
}

/****************************************************************/
/* model transformations */
/****************************************************************/
/* transform model from world space to screen space */
void xform_model(float scale)
{
    for(int i = 0; i < num_vertices + 2 * num_normals; i++)
    {
        
        vertex_list[i].position[X] = vertex_list[i].world[X] * scale;
        vertex_list[i].position[Y] = vertex_list[i].world[Y] * scale;
        vertex_list[i].position[Z] = vertex_list[i].world[Z] * 1;
        vertex_list[i].position[W] = 1.0;
    }
}

/* 3d rotation x_angle about the x axis, y_angle about the y axis, and
    z_angle about the z axis */
void rotate_model(float cx, float cy, float cz, float x_angle, float y_angle, float z_angle)
{
    z_angle *= (PI / 180.0);
    y_angle *= (PI / 180.0);
    x_angle *= (PI / 180.0);

    float nx, ny, nz;
    for(int i = 0; i < num_vertices + 2 * num_normals; i++)
    {
        POINT *p = &vertex_list[i];

        /* about z axis */
        nx = (p->world[X] - cx) * cos(z_angle) - (p->world[Y] - cy) * sin(z_angle);
        ny = (p->world[X] - cx) * sin(z_angle) + (p->world[Y] - cy)* cos(z_angle);
        vertex_list[i].world[X] = nx + cx;
        vertex_list[i].world[Y] = ny + cy;

        /* about y axis */
        nx = (p->world[X] - cx) * cos(y_angle) - (p->world[Z] - cz) * sin(y_angle);
        nz = (p->world[X] - cx) * sin(y_angle) + (p->world[Z] - cz) * cos(y_angle);
        vertex_list[i].world[X] = nx + cx;
        vertex_list[i].world[Z] = nz + cz;

        /* about x axis */
        ny = (p->world[Y] - cy) * cos(x_angle) - (p->world[Z] - cz) * sin(x_angle);
        nz = (p->world[Y] - cy) * sin(x_angle) + (p->world[Z] - cz) * cos(x_angle);
        vertex_list[i].world[Y] = ny + cy;
        vertex_list[i].world[Z] = nz + cz;
    }
}

/****************************************************************/
/* generate face normals */
/****************************************************************/
/* calculate normal of each triangular face */
void calculate_face_normals (void)
{
    for(int i = 0; i < num_triangles ; i++)
    {
        //get p0, p1, p2
        int p0_idx = face_list[i].vertices[0];
        int p1_idx = face_list[i].vertices[1];
        int p2_idx = face_list[i].vertices[2];
        
        POINT *p0 = &vertex_list[p0_idx];
        POINT *p1 = &vertex_list[p1_idx];
        POINT *p2 = &vertex_list[p2_idx];
        
        //calculate v1, v2
        float v1[4], v2[4], f_normal[4];
        vector_subtract(p1->world, p0->world, v1);
        vector_subtract(p2->world, p0->world, v2);
        
        //cross v1, v2 = n
        vector_cross(v1, v2, f_normal);
        //normalize(n)
        normalize(f_normal);
        //store normal in face_list's normal property
        cpy_vec4(face_list[i].f_normal, f_normal);
        
    }
}

/* calculate each vertex's normal as the average of surrounding triangles' face normals */
void calculate_vertex_normals (void)
{
    float v_normal[4];
    for(int i = 0; i < num_vertices; i++)
    {
        //get the vertex
        POINT *p = &vertex_list[i];
        //if that vertex is a part of at least 1 triangle
        if(p->num_tris > 0)
        {
            // reset v_normal
            set_vec4(v_normal, 0, 0, 0, 0);
            // for each triangle that this vertex participates in,
            //    calculate the avg of all the face normals
            for(int t = 0; t < p->num_tris; t++)
            {
                int tri_idx = p->tri_list[t];
                vector_add(face_list[tri_idx].f_normal, v_normal, v_normal);
            }
            scalar_divide(p->num_tris, v_normal, v_normal);
            normalize(v_normal);
            //store normal in POINT struct's v_normal field
            cpy_vec4(p->v_normal, v_normal);
        }
    }
}

/****************************************************************/
/* for perspective projections */
/****************************************************************/
/* translate model in world space by distance units along the Z axis */
void translate_model (float distance)
{
    int max_idx;
    max_idx = normals ? num_vertices + 2 * num_normals : num_vertices;
    for(int i = 0; i < max_idx; i++)
    {
        vertex_list[i].world[Z] += distance;
    }
}

/* perspective transform from world to screen coordinates */
void perspective_xform(float near, float far)
{
    int max_idx;
    max_idx = normals ? num_vertices + 2 * num_normals : num_vertices;
    for(int i = 0; i < max_idx; i++)
    {
        float x, y, z;
        x = vertex_list[i].world[X];
        y = vertex_list[i].world[Y];
        z = vertex_list[i].world[Z];
        
        vertex_list[i].position[X] = near * x / z;
        vertex_list[i].position[Y] = near * y / z;

        if(perspective_correct && texturing)
        {
            vertex_list[i].position[Z] = 1.0 / (z / (far - near));
        }
        else
        {
            vertex_list[i].position[Z] = (float) z / (far - near);
        }
        vertex_list[i].position[W] = 1.0;
    }
}

/* scale normalized view coordinates to screen coordinates
 *  (for perspective proj) */
void viewport_xform(float scale)
{
    int max_idx;
    max_idx = normals ? num_vertices + 2 * num_normals : num_vertices;
    for(int i = 0; i < max_idx; i++)
    {
        vertex_list[i].position[X] *= scale;
        vertex_list[i].position[Y] *= scale;
        vertex_list[i].position[Z] *= 1;
        vertex_list[i].position[W] = 1.0;
    }
}

/****************************************************************/
/* draw model into color buffer */
/****************************************************************/
/* draw wire-frame or filled in model */
void draw_model(int mode)
{
//    printf("num_triangles: %i\n", num_triangles);
    for(int i = 0; i < num_triangles; i++)
    {
        FACE f = face_list[i];
        
        POINT p0 = vertex_list[f.vertices[0]];
        POINT p1 = vertex_list[f.vertices[1]];
        POINT p2 = vertex_list[f.vertices[2]];
        
        /* fix the colors and textures of each point in vertex_list to the
         color and tex coords specified by FACE object */
        cpy_vec4(p0.color, color_list[f.colors[0]]);
        cpy_vec4(p0.tex, tex_list[f.tex[0]]);
        
        cpy_vec4(p1.color, color_list[f.colors[1]]);
        cpy_vec4(p1.tex, tex_list[f.tex[1]]);
        
        cpy_vec4(p2.color, color_list[f.colors[2]]);
        cpy_vec4(p2.tex, tex_list[f.tex[2]]);
        
        if(perspective_correct)
        {
            p0.tex[S] *= p0.position[Z];
            p0.tex[T] *= p0.position[Z];
            
            p1.tex[S] *= p1.position[Z];
            p1.tex[T] *= p1.position[Z];
            
            p2.tex[S] *= p2.position[Z];
            p2.tex[T] *= p2.position[Z];
        }
        
        // FRAME = 0, FILL = 1
        if(mode == FRAME)
        {
            draw_line(&p0, &p1, DRAW);
            draw_line(&p1, &p2, DRAW);
            draw_line(&p0, &p2, DRAW);
        }
        else if(mode == FILL)
        {
            if(!phong_shading)
            {
                normalize(light);
                float brightness = vector_dot(face_list[i].f_normal, light);
                scalar_multiply(brightness, p0.color, p0.color);
                scalar_multiply(brightness, p1.color, p1.color);
                scalar_multiply(brightness, p2.color, p2.color);

            }
            
            if(f.f_normal[Z] >= 0 ) //pointing away from us
            {
                float scale = 1 - p0.position[Z];
                if(p0.position[Z] < 0) scale = fabsf(p0.position[Z]);

//                p0.color[R] = 1;    p0.color[G] = 0;    p0.color[B] = 0;
//                p1.color[R] = 1;    p1.color[G] = 0;    p1.color[B] = 0;
//                p2.color[R] = 1;    p2.color[G] = 0;    p2.color[B] = 0;
                scalar_multiply(0.1, p0.color, p0.color);
                scalar_multiply(0.1, p1.color, p1.color);
                scalar_multiply(0.1, p2.color, p2.color);
                draw_triangle_barycentric (&p0, &p2, &p1);
            }
            else {
                draw_triangle_barycentric (&p0, &p1, &p2);
            }
//            draw_triangle_barycentric (&p0, &p1, &p2);

        }


        if(normals == ON)
        {
            //draw normals
            set_vec4(vertex_list[num_vertices + 2 * i].color, 1, 0, 0, 1);
            set_vec4(vertex_list[num_vertices + 2 * i + 1].color, 1, 0, 0, 1);
            draw_line(&vertex_list[num_vertices + 2 * i],
                      &vertex_list[num_vertices + 2 * i + 1], DRAW);
        }

    }
}


