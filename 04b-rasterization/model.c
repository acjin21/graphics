#include "model.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "raster.h"
#include "vector.h"
#include "macros.h"
#include "point.h"

/****************************************************************/
/* defines */
/****************************************************************/
#define MAX_N_VERTS 100000000000
#define MAX_N_FACES 100000000000

#define V_NORM_SCALE 10

/****************************************************************/
/* externs */
/****************************************************************/
/* modes */
extern int texturing;           // whether texturing or not
extern int perspective_correct; // for perspective correct interpolation
extern int normal_type;         // whether drawing normals or not
extern int shading_mode;
extern int drawing_normals;
extern int drawing_2D_select_box;
extern int drawing_axes;
extern int modulate;
extern int specular_highlight;
extern int obj_has_vnorms;
extern int draw_coord_axes;
extern int material;

/* data */
extern float depth_buffer[WIN_H][WIN_W];
extern float material_ambient[4];
extern float light_ambient[4];

/****************************************************************/
/* global variables */
/****************************************************************/
/* modes */
int modulate_type = MOD_COLOR;  // MOD_COLOR or MOD_LIGHT (for texture modulation)
int drawing_backside = OFF;     // if drawing backside of a triangle
int tex_gen_mode = SPHERICAL;

/* data */
POINT vertex_list[MAX_N_VERTS];
FACE face_list[MAX_N_FACES];
POINT bottom_left, top_right;

float tex_list[MAX_N_VERTS][4];
float color_list[MAX_N_VERTS][4];
float normal_list[MAX_N_VERTS][4];

/* counts */
int num_triangles = 0;          // total num of triangles (curr. faces)
int num_vertices = 0;           // total number of model vertices (not including norms or axes)
int num_face_normals = 0;       // for finding centroids + endpoints in vertex_list
int num_vertex_normals = 0;     // set by read_obj() if obj provides, else by insert_coord_axes()
int num_tex_coords = 0;         // set by read_obj(), and (TODO FIX) set in some init functions
int axes_start_idx = 0;         // vertex_list index of the first axes POINT

/****************************************************************/
/* helper functions */
/****************************************************************/
void add_face (int v0, int v1, int v2,
               int c0, int c1, int c2,
               int t0, int t1, int t2,
               int n0, int n1, int n2)
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
    
    face_list[num_triangles].v_normals[0] = n0;
    face_list[num_triangles].v_normals[1] = n1;
    face_list[num_triangles].v_normals[2] = n2;
    
    // set SOME point data (tri_list, num_tris, and v_normal)
    //      the init_model functions that call this have set WCs in vertex_list
    //      right now, we've only set the indices v, vn, vt (for vtx : face)
    POINT *p = &vertex_list[v0];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    if(obj_has_vnorms) cpy_vec4(p->v_normal, normal_list[n0]);

    
    p = &vertex_list[v1];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    if(obj_has_vnorms) cpy_vec4(p->v_normal, normal_list[n1]);

    p = &vertex_list[v2];
    p->tri_list[p->num_tris] = num_triangles;
    p->num_tris++;
    if(obj_has_vnorms) cpy_vec4(p->v_normal, normal_list[n2]);
    
    num_triangles++; // should be only location in which we increment num_triangles
}

/* reset num_tris for POINTs vertex_list[0, num_verts) */
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
/* init a 2D quad of two right triangles */
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
    
    add_face(0, 1, 2,    0, 1, 2,    0, 1, 2,   0, 0, 0);
    add_face(0, 2, 3,    0, 1, 2,    0, 2, 3,   0, 0, 0);
    
    num_face_normals = num_triangles;

}

/* object space -> world space transform using model matrix */
void model_xform (MAT4 *model)
{
    for(int i = 0; i < num_vertices; i++)
    {
        float local_coord[4];
        cpy_vec4(local_coord, vertex_list[i].world);
        mat_vec_mul(model, local_coord, vertex_list[i].world);
    }
}

/* set vertices of a unit cube with world space coordinates and
    random color + random tex coords */
void init_cube (MAT4 *model)
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
    
    /* transform from local to world coordinates */
    model_xform(model);
    
    /* set tex coordinates to four corners of texture */
    //0.1, 0.9 for the rock bump map
    set_vec4(tex_list[0], 0.1, 0.1, 0, 0);
    set_vec4(tex_list[1], 0.9, 0.1, 0, 0);
    set_vec4(tex_list[2], 0.9, 0.9, 0, 0);
    set_vec4(tex_list[3], 0.1, 0.9, 0, 0);
    num_tex_coords = 4;

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
    //       vertices    colors      texture coords vertex normals
    add_face(0, 1, 2,    0, 1, 2,    0, 1, 2,       0, 0, 0);
    add_face(0, 2, 3,    1, 1, 1,    0, 2, 3,       0, 0, 0);
    add_face(1, 5, 6,    0, 1, 2,    0, 1, 2,       0, 0, 0);
    add_face(1, 6, 2,    2, 2, 2,    0, 2, 3,       0, 0, 0);
    add_face(5, 4, 7,    0, 1, 2,    0, 1, 2,       0, 0, 0);
    add_face(5, 7, 6,    0, 1, 2,    0, 2, 3,       0, 0, 0);
    add_face(4, 0, 3,    0, 0, 0,    0, 1, 2,       0, 0, 0);
    add_face(4, 3, 7,    0, 1, 2,    0, 2, 3,       0, 0, 0);
    add_face(0, 4, 5,    0, 1, 2,    0, 1, 2,       0, 0, 0);
    add_face(0, 5, 1,    0, 1, 2,    0, 2, 3,       0, 0, 0);
    add_face(7, 3, 2,    0, 1, 2,    0, 1, 2,       0, 0, 0);
    add_face(7, 2, 6,    0, 1, 2,    0, 2, 3,       0, 0, 0);
    /* should now have 12 triangles */
    
    num_face_normals = num_triangles;
}

/* reset num_tris for all vertices, and add mesh faces to face_list */
/*      called in init functions built ontop of init_mesh           */
void add_mesh_faces (int w, int h)
{
    reset_num_tris(num_vertices);
    
    // for grayscale; comment out if want interpolated colors
    set_vec4(color_list[0], 0.5, 0.5, 0.5, 1);

    /* add triangles/faces */
    num_triangles = 0;
    int v0, v1, v2;

    for(int r = 0; r < h - 1; r++) // height - 2 for mesh
    {
        for(int c = 0; c < w - 1; c++) // width - 2 for mesh
        {
            v0 = r * w + c;
            v1 = (r + 1) * w + (c + 1);
            v2 = (r + 1) * w + c;
            add_face(v0, v1, v2, /* v */
                     v0, v1, v2, //0, 0, 0,    /* c; v0, v1, v2 if want interp color */
                     v0, v1, v2, /* vt */
                     0, 0, 0);   /* vn -- only used if reading in obj w/ vn */

            v1 = r * w + (c + 1);
            v2 = (r + 1) * w + (c + 1);
            add_face(v0, v1, v2,
                     v0, v1, v2, //0, 0, 0,
                     v0, v1, v2,
                     0, 0, 0);
        }
    }
    num_face_normals = num_triangles;
}

/* init a n x n wavy mesh that starts at angle mesh_da */
void init_mesh (float scale, float cx, float cy, float cz, float da)
{
    int n = 32;
    int width = n;
    int height = n;
    /* add vertices */
    num_vertices = width * height;
    for(int r = 0; r < height; r++)
    {
        for(int c = 0; c < width; c++)
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
            set_vec4(tex_list[(r * width) + c], u, v, 0, 0);
            set_vec4(color_list[(r * width) + c], u, v, 0, 1);
        }
    }
    add_mesh_faces(width, height);
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
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    add_mesh_faces(width, height);
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
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    add_mesh_faces(width, height);
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
            int i = r * n + c;
            float u = (float) c / (width - 1);
            float v = (float) r / (height - 1);
            
            POINT *p = &vertex_list[(r * n) + c];
            
            /* world coordinates */
            p->world[X] = radius * cos(u * 1 * PI) + cx;
            p->world[Y] = radius * cos(v * 2 * PI) * sin(u * PI) + cy;
            p->world[Z] = radius * sin(v * 2 * PI) * sin(u * PI) + cz;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    add_mesh_faces(width, height);
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
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
        count += 0.05;
    }
    add_mesh_faces(width, height);
}

/*************************************************************************/
/* insert additional coordinates into vertex_list (normals, axes coords) */
/*************************************************************************/
/* inserts FACE normals centers and endpoints after vertices in vertex_list */
/*  NOTE: - call after calculate_face_normals()                             */
/*        - this function adds 2 * num_normals extra points to vertex_list  */
/*        - call before 3D transformations so the normals are transformed   */
/*              along with model vertices                                   */
void insert_normal_coords(void)
{
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
        cpy_vec4(vertex_list[num_vertices + 2 * i].world, center.world);
        cpy_vec4(vertex_list[num_vertices + 2 * i].color, color);
        
        //calculate endpoint
        scalar_divide(6, face_list[i].f_normal, tmp);
        vector_add(center.world, tmp, end.world);
        //store endpoint
        cpy_vec4(vertex_list[num_vertices + 2 * i + 1].world, end.world);
        cpy_vec4(vertex_list[num_vertices + 2 * i + 1].color, color);
    }
}

/* insert endpoints of coordinate axis into vertex_list */
void insert_coord_axes (float cx, float cy, float cz, float scale)
{
    if(normal_type == F_NORMALS)
    {
        axes_start_idx = num_vertices + 2 * num_face_normals;
    }
    else
    {
        axes_start_idx = num_vertices;
    }
    set_vec4(vertex_list[axes_start_idx].world, cx, cy, cz, 1 );
    set_vec4(vertex_list[axes_start_idx + 1].world, cx + 2, cy, cz, 1);
    set_vec4(vertex_list[axes_start_idx + 2].world, cx, cy + 2, cz, 1);
    set_vec4(vertex_list[axes_start_idx + 3].world, cx, cy, cz + 2, 1);
}

/* return the greatest index that points to relevant data in vertex_list */
/*    dep on if drawing F_NORMALS and if drawing local axes              */
int get_max_idx (int normal_mode)
{
    int max_idx = 0;
    switch(normal_mode)
    {
        case F_NORMALS:
            max_idx = num_vertices + 2 * num_face_normals;
            break;
        case NO_NORMALS:
        case V_NORMALS:
            max_idx = num_vertices;
            break;
    }
    if(axes_start_idx != 0) max_idx += 4;
    return max_idx;
}

/****************************************************************/
/* model transformations */
/****************************************************************/
/* transform model from world space to screen space */
void xform_model(float scale)
{
    int max_idx = get_max_idx (normal_type);
    for(int i = 0; i < max_idx; i++)
    {
        vertex_list[i].position[X] = vertex_list[i].world[X] * scale;
        vertex_list[i].position[Y] = vertex_list[i].world[Y] * scale;
        vertex_list[i].position[Z] = vertex_list[i].world[Z];
        vertex_list[i].position[W] = 1.0;
    }
}

/* 3d rotation x_angle about the x axis, y_angle about the y axis, and
    z_angle about the z axis */
void rotate_model(float cx, float cy, float cz, float x_angle, float y_angle, float z_angle)
{
    float nx, ny, nz;
    int max_idx = get_max_idx (normal_type);
    
    for(int i = 0; i < max_idx; i++)
    {
        POINT *p = &vertex_list[i];
        float in_vec[4];
        
        cpy_vec4 (in_vec, p->world);
        /* matrices */
        MAT4 tmp, rotate_about_origin;
        
        set_identity(&rotate_about_origin);
        set_transl (&tmp, -cx, -cy, -cz);
        mat_mul (&rotate_about_origin, &tmp, &rotate_about_origin);
        set_3d_rot(&tmp, x_angle, y_angle, z_angle);
        mat_mul (&rotate_about_origin, &tmp, &rotate_about_origin);
        set_transl (&tmp, cx, cy, cz);
        mat_mul (&rotate_about_origin, &tmp, &rotate_about_origin);

        /* matrix transform world coords */
        mat_vec_mul (&rotate_about_origin, in_vec, vertex_list[i].world);
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
        int p0_idx = face_list[i].vertices[0];
        int p1_idx = face_list[i].vertices[1];
        int p2_idx = face_list[i].vertices[2];
        
        POINT *p0 = &vertex_list[p0_idx];
        POINT *p1 = &vertex_list[p1_idx];
        POINT *p2 = &vertex_list[p2_idx];
        
        float v1[4], v2[4], f_normal[4];
        vector_subtract(p1->world, p0->world, v1);
        vector_subtract(p2->world, p0->world, v2);
        
        vector_cross(v1, v2, f_normal);
        normalize(f_normal);
        cpy_vec4(face_list[i].f_normal, f_normal);
        
    }
}

/* calculate each vertex's normal as the average of surrounding triangles' face normals */
void calculate_vertex_normals (void)
{
    num_vertex_normals = num_vertices; //set num_vertex_normals
    
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
    int max_idx = get_max_idx (normal_type);
    
    for(int i = 0; i < max_idx; i++)
    {
        vertex_list[i].world[Z] += distance;
    }
}

/* perspective transform from world to screen coordinates */
void perspective_xform(float near, float far)
{
    int max_idx = get_max_idx (normal_type);

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
            vertex_list[i].position[Z] = (float)(far - near) / z;
        }
        else
        {
            vertex_list[i].position[Z] = (float) z / (far - near);
        }
        vertex_list[i].position[W] = 1.0;
    }
}

/* scale normalized view coordinates to screen coordinates */
/*  (for perspective proj)                                 */
void viewport_xform(float scale)
{
    int max_idx = get_max_idx (normal_type);

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
/* draw wire-frame or filled in model (mode = FRAME or FILL) */
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

        if(perspective_correct && texturing)
        {
            scalar_multiply(p0.position[Z], p0.tex, p0.tex);
            scalar_multiply(p1.position[Z], p1.tex, p1.tex);
            scalar_multiply(p2.position[Z], p2.tex, p2.tex);
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
    
            if(shading_mode == FLAT)
            {
                float diffuse, tmp_diff[4], tmp_spec[4];

                set_diffuse_term (f.f_normal, tmp_diff);
                set_specular_term (f.f_normal, tmp_spec);
                
                //modulate interpolated color * texture
                if(!modulate || (modulate && modulate_type == MOD_COLOR))
                {
                    shade_point(tmp_diff, tmp_spec, &p0);
                    shade_point(tmp_diff, tmp_spec, &p1);
                    shade_point(tmp_diff, tmp_spec, &p2);
                }
                //modulate texture and intensity i.e. lighting
                else if(modulate && modulate_type == MOD_LIGHT)
                {
                    cpy_vec4(p0.color, tmp_diff);
                    cpy_vec4(p1.color, tmp_diff);
                    cpy_vec4(p2.color, tmp_diff);
                    if(specular_highlight)
                    {
                        vector_add(tmp_spec, p0.color, p0.color);
                        vector_add(tmp_spec, p1.color, p1.color);
                        vector_add(tmp_spec, p2.color, p2.color);
                    }
                    
                    float ambient[4] = {0.5 , 0.5 , 0.5 , 0};
                    if(material)
                    {
                        vector_multiply(light_ambient, material_ambient, ambient);
                    }
                    
                    vector_add(p0.color, ambient, p0.color);
                    vector_add(p1.color, ambient, p1.color);
                    vector_add(p2.color, ambient, p2.color);
                }
            }
            if(f.f_normal[Z] >= 0) //pointing away from us
            {
                drawing_backside = ON;
                set_vec4(p0.color, 0.5, 0.5, 0.5, 1);
                set_vec4(p1.color, 0.5, 0.5, 0.5, 1);
                set_vec4(p2.color, 0.5, 0.5, 0.5, 1);
                draw_triangle_barycentric (&p0, &p2, &p1);
                
            }
            else {
                drawing_backside = OFF;
                draw_triangle_barycentric (&p0, &p1, &p2);
                
                if (normal_type == V_NORMALS)
                {
                    drawing_normals = ON;
                    POINT v_norm_endpt, vtx;
                    float tmp[4];
                    set_vec4(v_norm_endpt.color, 0, 1, 0, 1);
                    
                    set_vec4(p0.color, 0, 1, 0, 1);
                    scalar_multiply(V_NORM_SCALE, p0.v_normal, tmp);
                    vector_add(p0.position, tmp, v_norm_endpt.position);
                    v_norm_endpt.position[Z] = vtx.position[Z];
                    draw_line(&p0, &v_norm_endpt, DRAW);
                    
                    set_vec4(p1.color, 0, 1, 0, 1);
                    set_vec4(v_norm_endpt.color, 0, 1, 0, 1);
                    
                    scalar_multiply(V_NORM_SCALE, p1.v_normal, tmp);
                    vector_add(p1.position, tmp, v_norm_endpt.position);
                    v_norm_endpt.position[Z] = p1.position[Z];
                    draw_line(&p1, &v_norm_endpt, DRAW);
                    
                    set_vec4(p2.color, 0, 1, 0, 1);
                    set_vec4(v_norm_endpt.color, 0, 1, 0, 1);
                    
                    scalar_multiply(V_NORM_SCALE, p2.v_normal, tmp);
                    vector_add(p2.position, tmp, v_norm_endpt.position);
                    v_norm_endpt.position[Z] = p2.position[Z];
                    draw_line(&p2, &v_norm_endpt, DRAW);
                    drawing_normals = OFF;

                }

            }

            if(normal_type == F_NORMALS)
            {
                //draw normals
                drawing_normals = ON;
                set_vec4(vertex_list[num_vertices + 2 * i].color, 1, 0, 0, 1);
                set_vec4(vertex_list[num_vertices + 2 * i + 1].color, 1, 0, 0, 1);
                draw_line(&vertex_list[num_vertices + 2 * i],
                          &vertex_list[num_vertices + 2 * i + 1], DRAW);
                drawing_normals = OFF;

            }
        }
    }


}

void draw_local_axes (void)
{
    if(axes_start_idx != 0 && draw_coord_axes)
    {
        drawing_axes = ON;
        //draw coord axes of model
        set_vec4(vertex_list[axes_start_idx].color, 1, 0, 0, 1);
        set_vec4(vertex_list[axes_start_idx + 1].color, 1, 0, 0, 1);
        draw_line(&vertex_list[axes_start_idx],
                  &vertex_list[axes_start_idx + 1], DRAW);
        
        set_vec4(vertex_list[axes_start_idx].color, 0, 1, 0, 1);
        set_vec4(vertex_list[axes_start_idx + 2].color, 0, 1, 0, 1);
        draw_line(&vertex_list[axes_start_idx],
                  &vertex_list[axes_start_idx + 2], DRAW);
        
        set_vec4(vertex_list[axes_start_idx].color, 0, 0, 1, 1);
        set_vec4(vertex_list[axes_start_idx + 3].color, 0, 0, 1, 1);
        draw_line(&vertex_list[axes_start_idx],
                  &vertex_list[axes_start_idx + 3], DRAW);
        drawing_axes = OFF;
    }
}

void set_2D_bb (OBJECT *o)
{
    POINT top_left, bottom_right;
    float max_x, max_y, min_x, min_y;
    max_x = INT_MIN;
    max_y = INT_MIN;
    min_x = INT_MAX;
    min_y = INT_MAX;
    
    for(int i = 0; i < num_vertices; i++)
    {
        if(vertex_list[i].position[X] < min_x)
        {
            min_x = vertex_list[i].position[X];
        }
        else if(vertex_list[i].position[X] > max_x)
        {
            max_x = vertex_list[i].position[X];
        }
        if(vertex_list[i].position[Y] < min_y)
        {
            min_y = vertex_list[i].position[Y];
        }
        else if(vertex_list[i].position[Y] > max_y)
        {
            max_y = vertex_list[i].position[Y];
        }
    }
    
    o->bb_bl.position[X] = min_x - 5;
    o->bb_bl.position[Y] = min_y - 5;
    o->bb_tr.position[X] = max_x + 5;
    o->bb_tr.position[Y] = max_y + 5;
//    printf("bottom left: (%.2f, %.2f), top right: (%.2f, %.2f)\n",
//           o->bb_bl.position[X], o->bb_bl.position[Y],
//           o->bb_tr.position[X], o->bb_tr.position[Y]);
}

/* for SCENE mode to draw a red 2D box around the object being modified */
void draw_2D_bb (OBJECT *o)
{
    POINT top_left, bottom_right;
    
    top_left.position[X] = o->bb_bl.position[X];
    top_left.position[Y] = o->bb_tr.position[Y];
    top_left.position[Z] = 0;
    
    bottom_right.position[X] = o->bb_tr.position[X];
    bottom_right.position[Y] = o->bb_bl.position[Y];
    bottom_right.position[Z] = 0;
    
    set_vec4(bottom_right.color, 1, 0, 0, 1);
    set_vec4(top_left.color, 1, 0, 0, 1);
    set_vec4(o->bb_bl.color, 1, 0, 0, 1);
    set_vec4(o->bb_tr.color, 1, 0, 0, 1);

    drawing_2D_select_box = ON;
    draw_line(&top_left, &o->bb_tr, DRAW);
    draw_line(&o->bb_tr, &bottom_right, DRAW);
    draw_line(&bottom_right, &o->bb_bl, DRAW);
    draw_line(&o->bb_bl, &top_left, DRAW);
    drawing_2D_select_box = OFF;
}

/* go through all vertices and generate texture coordinates from normals */
void get_tex_coords (void)
{
    if(tex_gen_mode == OFF)
    {
        return;
    }
    if(tex_gen_mode == NAIVE)
    {
        for(int i = 0; i < num_vertices; i++)
        {
            naive_map(vertex_list[i].v_normal, tex_list[i]);
        }
    }
    if(tex_gen_mode == CYLINDRICAL)
    {
        for(int i = 0; i < num_vertices; i++)
        {
            cylindrical_map(vertex_list[i].v_normal, tex_list[i]);
        }
    }
    if(tex_gen_mode == SPHERICAL)
    {
        for(int i = 0; i < num_vertices; i++)
        {
            spherical_map(vertex_list[i].v_normal, tex_list[i]);
        }
    }
    if(tex_gen_mode == REFLECTION)
    {
        for(int i = 0; i < num_vertices; i++)
        {
            reflection_map(vertex_list[i].v_normal, tex_list[i]);
        }
    }
}

/****************************************************************/
/* OBJ I/O */
/****************************************************************/

/* read a .obj file and transform vertices to world space */
void read_obj_file (char *file_name, MAT4 *model)
{
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
        float x, y, z, s, t, r, nx, ny, nz;
        int i, j, k, vt1, vt2, vt3, vn1, vn2, vn3;
        vt1 = 0;
        vt2 = 1;
        vt3 = 2;
        /* handle any possible comments */
        int next_ch;
        char comment[500];
        char next_line[500];
        
        next_ch = getc(fp);
        if(next_ch == '#') fgets(comment, 500, fp);
        else ungetc(next_ch, fp);
        
        while(fgets(next_line, 500, fp)[0] != 'v')
        {
            ;
        }
        fputs(next_line, fp);
        
        /******************/
        /* vertices */
        /******************/
        do
        {
            sscanf(next_line, "v %f %f %f\n", &x, &y, &z);
            set_vec4(vertex_list[num_vertices].world, x, y, z, 1.0);
            num_vertices++;
            fgets(next_line, 500, fp);
        } while(!strncmp(next_line, "v ", 2));
        
        /* local to world space */
        model_xform(model);
        
        /* reset num_tris for each vertex */
        reset_num_tris(num_vertices);
        
#ifdef PRINT_DEBUG_OBJ
        printf("%i vertices read\n", num_vertices);
        printf("NEXT: %s\n", next_line);
#endif
        
        while(next_line[0] == '\n')
        {
            
            fgets(next_line, 500, fp);
        }
        fputs(next_line, fp);
        /******************/
        /* textures */
        /******************/
        num_tex_coords = 0;
        while (!strncmp(next_line, "vt", 2))
        {
            sscanf(next_line, "vt %f %f %f\n", &s, &t, &r);
            set_vec4(tex_list[num_tex_coords], s, t, r, 0.0);
            num_tex_coords++;
            fgets(next_line, 500, fp);
        }
        fputs(next_line, fp);
        
#ifdef PRINT_DEBUG_OBJ
        printf("%i textures read\n", num_tex_coords);
        printf("NEXT: %s\n", next_line);
#endif
        
        while(next_line[0] == '\n')
        {
            fgets(next_line, 500, fp);
        }
        fputs(next_line, fp);
        
        /******************/
        /* normals */
        /******************/
        num_vertex_normals = 0;
        while (!strncmp(next_line, "vn", 2))
        {
            sscanf(next_line, "vn %f %f %f\n", &nx, &ny, &nz);
            set_vec4(normal_list[num_vertex_normals], nx, ny, nz, 0.0);
            num_vertex_normals++;
            fgets(next_line, 500, fp);
            
        }
        fputs(next_line, fp);
        
#ifdef PRINT_DEBUG_OBJ
        printf("%i normals read\n", num_vertex_normals);
        printf("NEXT: %s\n", next_line);
#endif
        
        while(next_line[0] != 'f')
        {
            fgets(next_line, 500, fp);
        }
        fputs(next_line, fp);
        
        
        /******************/
        /* add faces/triangles */
        /******************/
        num_triangles = 0; /* reset num of triangles */
        while(!strncmp(next_line, "f", 1))
        {
            if(num_tex_coords > 0 && num_vertex_normals > 0)
            /* obj file has vt, vn */
            {
                obj_has_vnorms = TRUE;
                sscanf(next_line, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                       &i, &vt1, &vn1,
                       &j, &vt2, &vn2,
                       &k, &vt3, &vn3);
            }
            else if (num_tex_coords > 0)
            /* obj file has vt, no vn */
            {
                obj_has_vnorms = FALSE;
                sscanf(next_line, "f %d/%d %d/%d %d/%d\n",
                       &i, &vt1, &j, &vt2, &k, &vt3);
            }
            else if (num_vertex_normals > 0)
            /* obj has vn, no vt */
            {
                obj_has_vnorms = TRUE;
                sscanf(next_line, "f %d//%d %d//%d %d//%d\n",
                       &i, &vn1, &j, &vn2, &k, &vn3);
            }
            else {
                /* obj has neither vt nor vn */
                obj_has_vnorms = FALSE;
                sscanf(next_line, "f %d %d %d\n", &i, &j, &k);
            }
            
            vt1 = i;
            vt2 = j;
            vt3 = k;
            add_face(i - 1,     j - 1,      k - 1,      //vtx indices
                     3,         3,          3,          //colors
                     vt1 - 1,   vt2 - 1,    vt3 - 1,    //tex coord indices
                     vn1,       vn2,        vn3);       //normal indices
            
            if(fgets(next_line, 500, fp) == NULL)
            {
                break; // reach end of file
            }
            while(next_line[0] != 'f')
            {
                if(fgets(next_line, 500, fp) == NULL)
                {
                    break;
                }
            }
        }
#ifdef PRINT_DEBUG_OBJ
        printf("%i faces read\n", num_triangles);
        printf("obj has vnorms: %s\n", obj_has_vnorms ? "YES" : "NO");
#endif
        fclose(fp);
    }
    /* in order to establish proper spacing in vertex_list */
    num_face_normals = num_triangles;
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

