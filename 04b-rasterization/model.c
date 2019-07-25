#include "model.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "raster.h"
#include "vector.h"
#include "macros.h"

#include "point.h"
#include "app.h"
#include "depth.h"
#include "light.h"
#include "material.h"
#include "frustum.h"
//#include "camera.h"
/****************************************************************/
/* defines */
/****************************************************************/
#define MAX_N_VERTS 100000000000
#define MAX_N_FACES 100000000000

#define V_NORM_SCALE 10

/****************************************************************/
/* global variables */
/****************************************************************/
/* modes */
int modulate_type = MOD_COLOR;  // MOD_COLOR or MOD_LIGHT (for texture modulation)
int drawing_backside = OFF;     // if drawing backside of a triangle
int tex_gen_mode = SPHERICAL;

int max_n_addl_tris = 1000;
/* data */
POINT vertex_list[MAX_N_VERTS];
FACE face_list[MAX_N_FACES];
POINT bottom_left, top_right;

float tex_list[MAX_N_VERTS][4];
float color_list[MAX_N_VERTS][4];
float normal_list[MAX_N_VERTS][4];

int num_peripherals = 12;
POINT peripherals[MAX_N_VERTS];

/* counts */
int num_triangles = 0;          // total num of triangles (curr. faces)
int num_vertices = 0;           // total number of model vertices (not including norms or axes)
int num_face_normals = 0;       // for finding centroids + endpoints in vertex_list
int num_vertex_normals = 0;     // set by read_obj() if obj provides, else by insert_coord_axes()
int num_tex_coords = 0;         // set by read_obj(), and (TODO FIX) set in some init functions
int axes_start_idx = 0;         // vertex_list index of the first axes POINT

int bb_start_idx = 0;           // starting index of bounding box vertices in vertex_list

MAT4 cam, ortho, perspective, viewport;
//float w;

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

/* init a 2D quad of two right triangles */
void init_quad(MAT4 *model)
{
    num_vertices = 4;
    
    set_vec4(vertex_list[0].world, -0.5, 0.5, 0, 1.0);
    set_vec4(vertex_list[1].world, 0.5, 0.5, 0, 1.0);
    set_vec4(vertex_list[2].world, 0.5, -0.5, 0, 1.0);
    set_vec4(vertex_list[3].world, -0.5, -0.5, 0, 1.0);

    /* transform from local to world coordinates */
    model_xform(model);
    
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

/* set vertices of a unit cube with world space coordinates and
    random color + random tex coords */
void init_cube (MAT4 *model)
{
    /* add vertices */
    num_vertices = 8;
    
    set_vec4(vertex_list[0].world, 1, 1, -1, 1.0);
    set_vec4(vertex_list[1].world, 1, 1, 1, 1.0);
    set_vec4(vertex_list[2].world, 1, -1, 1, 1.0);
    set_vec4(vertex_list[3].world, 1, -1, -1, 1.0);
    set_vec4(vertex_list[4].world, -1, 1, -1, 1.0);
    set_vec4(vertex_list[5].world, -1, 1, 1, 1.0);
    set_vec4(vertex_list[6].world, -1, -1, 1, 1.0);
    set_vec4(vertex_list[7].world, -1, -1, -1, 1.0);
    
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
void init_mesh (MAT4 *model)
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
            p->world[X] = u;
            p->world[Y] = v;
            p->world[Z] = cos(v * 2 * PI) * sin(u * 2 * PI);
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * width) + c], u, v, 0, 0);
            set_vec4(color_list[(r * width) + c], u, v, 0, 1);
        }
    }
    /* transform from local to world coordinates */
    model_xform(model);
    
    add_mesh_faces(width, height);
}

/* init a cylinder */
void init_cylinder (float radius, float height_scale, MAT4 *model)
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
            p->world[X] = radius * cos(u * 2 * PI);
            p->world[Y] = radius * sin(u * 2 * PI);
            p->world[Z] = height_scale * v;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    /* transform from local to world coordinates */
    model_xform(model);

    add_mesh_faces(width, height);
}

/* init a cone */
void init_cone (float radius, float height_scale, MAT4 *model)
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
            p->world[X] = new_rad * cos(u * 2 * PI);
            p->world[Y] = new_rad * sin(u * 2 * PI);
            p->world[Z] = height_scale * v;
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    /* transform from local to world coordinates */
    model_xform(model);
    
    add_mesh_faces(width, height);
}

/* init a sphere */
void init_sphere (float radius, MAT4 *model)
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
            p->world[X] = radius * cos(u * 1 * PI);
            p->world[Y] = radius * cos(v * 2 * PI) * sin(u * PI);
            p->world[Z] = radius * sin(v * 2 * PI) * sin(u * PI);
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
    }
    /* transform from local to world coordinates */
    model_xform(model);
    
    add_mesh_faces(width, height);
}

/* init a torus */
void init_torus (float tube_radius, float hole_radius, MAT4 *model)
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
                            cos(v * 2 * PI);
            p->world[Y] = tube_radius * sin(u * 2 * PI);
            p->world[Z] = (tube_radius * cos(u * 2 * PI) + 0.5 * hole_radius) *
                            sin(v * 2 * PI);
            p->world[W] = 1.0;
            
            /* set colors and textures for each vertex */
            set_vec4(tex_list[(r * n) + c], u, v, 0, 0);
            set_vec4(color_list[(r * n) + c], u, v, 0, 1);
        }
        count += 0.05;
    }
    /* transform from local to world coordinates */
    model_xform(model);
    
    add_mesh_faces(width, height);
}

/* build mesh in model-space & xform to world-space coords */
void build_model (OBJECT *o)
{
    set_model_mat (&o->model_mat, o->scale,
                   o->init_orientation[X],
                   o->init_orientation[Y],
                   o->init_orientation[Z],
                   o->center[X], o->center[Y], o->center[Z]);

    MAT4 *model = &o->model_mat;
    float height_scale = o->scale;
    float r0 = o->radii[0];
    float r1 = o->radii[1];
    
    switch (o->type)
    {
        case QUAD:      init_quad(model);                           break;
        case CUBE:      init_cube(model);                           break;
        case MESH:      init_mesh(model);                           break;
        case CYLINDER:  init_cylinder(r0, height_scale, model);     break;
        case CONE:      init_cone(r0, height_scale, model);         break;
        case SPHERE:    init_sphere(r0, model);                     break;
        case TORUS:     init_torus(r0, r1, model);                  break;
        case TEAPOT:    read_obj_file("obj/teapot.obj", model);     break;
        case CAT:       read_obj_file("obj/cat.obj", model);        break;
        case DEER:      read_obj_file("obj/deer.obj", model);       break;
        case BUNNY:     read_obj_file("obj/bunnyNV.obj", model);    break;
        case BUDDHA:    read_obj_file("obj/buddha.obj", model);     break;
        case WOLF:      read_obj_file("obj/wolf.obj", model);       break;
        case TREE:      read_obj_file("obj/tree.obj", model);       break;
    }
}
/*************************************************************************/
/* insert add'l coordinates into vertex_list (normals, axes, bb) */
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
    set_vec4(peripherals[0].world, cx, cy, cz, 1 );
    set_vec4(peripherals[1].world, cx + 2, cy, cz, 1);
    set_vec4(peripherals[2].world, cx, cy + 2, cz, 1);
    set_vec4(peripherals[3].world, cx, cy, cz + 2, 1);
}

// do this in world space
void insert_bb_coords (void)
{
    //set 2D select/bounding box
    float max_x, max_y, max_z, min_x, min_y, min_z;
    max_x = -FLT_MAX;
    max_y = -FLT_MAX;
    max_z = -FLT_MAX;
    
    min_x = FLT_MAX;
    min_y = FLT_MAX;
    min_z = FLT_MAX;
    
    for(int i = 0; i < num_vertices; i++)
    {
        min_x = MIN(vertex_list[i].world[X], min_x);
        max_x = MAX(vertex_list[i].world[X], max_x);

        min_y = MIN(vertex_list[i].world[Y], min_y);
        max_y = MAX(vertex_list[i].world[Y], max_y);
        
        min_z = MIN(vertex_list[i].world[Z], min_z);
        max_z = MAX(vertex_list[i].world[Z], max_z);

    }
    // get offset in peripherals list
    bb_start_idx = draw_coord_axes ? 4 : 0;
    set_vec4(peripherals[bb_start_idx].world, min_x, max_y, min_z, 1);
    set_vec4(peripherals[bb_start_idx + 1].world, max_x, max_y, min_z, 1);
    set_vec4(peripherals[bb_start_idx + 2].world, max_x, min_y, min_z, 1);
    set_vec4(peripherals[bb_start_idx + 3].world, min_x, min_y, min_z, 1);
    set_vec4(peripherals[bb_start_idx + 4].world, min_x, max_y, max_z, 1);
    set_vec4(peripherals[bb_start_idx + 5].world, max_x, max_y, max_z, 1);
    set_vec4(peripherals[bb_start_idx + 6].world, max_x, min_y, max_z, 1);
    set_vec4(peripherals[bb_start_idx + 7].world, min_x, min_y, max_z, 1);
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
    if(bb_start_idx != 0) max_idx += 8;
    return max_idx;
}

/****************************************************************/
/* model world space transformations */
/****************************************************************/
void world_xforms(OBJECT *o)
{
    float in_vec[4], cx, cy, cz, rx, ry, rz, tx, ty, tz, sx, sy, sz;
    POINT *p;
    /* matrices */
    MAT4 tmp, rotate, translate, scale, manip;
    
    cx = o->center[X];
    cy = o->center[Y];
    cz = o->center[Z];
    rx = o->rotation[X];
    ry = o->rotation[Y];
    rz = o->rotation[Z];
    tx = o->translate[X];
    ty = o->translate[Y];
    tz = o->translate[Z];
    sx = o->scale_vec[X];
    sy = o->scale_vec[Y];
    sz = o->scale_vec[Z];

    /* rotation matrix */
    set_identity(&rotate);
    set_transl (&tmp, -cx, -cy, -cz);
    mat_mul (&rotate, &tmp, &rotate);
    set_3d_rot(&tmp, rx, ry, rz);
    mat_mul (&rotate, &tmp, &rotate);
    set_transl (&tmp, cx, cy, cz);
    mat_mul (&rotate, &tmp, &rotate);
    /* translation matrix */
    set_transl (&translate, tx, ty, tz);
    /* scale matrix */
    set_scale_nonuniform (&scale, sx, sy, sz);
    
    /* combine all matrices */
    mat_mul (&manip, &scale, &rotate);
    mat_mul (&manip, &translate, &manip);

    for(int i = 0; i < num_vertices; i++)
    {
        p = &vertex_list[i];
        cpy_vec4 (in_vec, p->world);
        mat_vec_mul (&manip, in_vec, vertex_list[i].world);
    }
    for(int i = 0; i < num_peripherals; i++)
    {
        p = &peripherals[i];
        cpy_vec4 (in_vec, p->world);
        mat_vec_mul (&manip, in_vec, peripherals[i].world);
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
        float v1[4], v2[4], f_normal[4];

        POINT *p0 = &vertex_list[face_list[i].vertices[0]];
        POINT *p1 = &vertex_list[face_list[i].vertices[1]];
        POINT *p2 = &vertex_list[face_list[i].vertices[2]];

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
/* for camera projections */
/****************************************************************/
void camera_xform (CAMERA *c)
{
    set_camera_mat (&cam, c);
    for(int i = 0; i < num_vertices; i++)
    {
        mat_vec_mul (&cam, vertex_list[i].world, vertex_list[i].world);
        vertex_list[i].world[W] = 1.0;
    }
    for(int i = 0; i < num_peripherals; i++)
    {
        mat_vec_mul (&cam, peripherals[i].world, peripherals[i].world);
        peripherals[i].world[W] = 1.0;
    }
}


/****************************************************************/
/* for orthographic projections */
/****************************************************************/
/* transform model from world space to screen space */
void xform_model(float x_min, float x_max,
                 float y_min, float y_max,
                 float z_min, float z_max)
{
    set_ortho_mat (&ortho, x_min, x_max, y_min, y_max, z_min, z_max);
    
    for(int i = 0; i < num_vertices; i++)
    {
        mat_vec_mul (&ortho, vertex_list[i].world, vertex_list[i].position);
        vertex_list[i].position[W] = 1.0;
    }
    for(int i = 0; i < num_peripherals; i++)
    {
        mat_vec_mul (&ortho, peripherals[i].world, peripherals[i].position);
        peripherals[i].position[W] = 1.0;
    }
}

void viewport_mat_xform (int vp_w, int vp_h)
{
    set_viewport_mat (&viewport, vp_w, vp_h);
    float translation_vec[] = {WIN_W / 2.0, WIN_H / 2.0, 0, 0};

    for(int i = 0; i < num_vertices; i++)
    {
        mat_vec_mul (&viewport, vertex_list[i].position, vertex_list[i].position);
        
        // do translation separately so that position[W] = 1/w for persp corr.
        //      does not interfere with viewport_x, viewport_y calculation
        // TODO: do we always want to add this translation vector regardless of
        //      whether we're working with points or directions?
        vector_add(vertex_list[i].position, translation_vec, vertex_list[i].position);
    }
    for(int i = 0; i < num_peripherals; i++)
    {
        mat_vec_mul (&viewport, peripherals[i].position, peripherals[i].position);
        vector_add(peripherals[i].position, translation_vec, peripherals[i].position);
    }
}

/****************************************************************/
/* for perspective projections */
/****************************************************************/

/* determine if object BB is entirely outside view frustrum */
/*  in eye space; assuming eye/camera is at origin */
int cull_model (float near, float far)
{
    int b = bb_start_idx;
    /* near and far clipping planes */
    if(peripherals[b+6].world[Z] < near || peripherals[b].world[Z] > far)
    {
        return 1;
    }
    /* left and right clipping planes */
    if(-peripherals[b].world[X] > peripherals[b].world[Z] &&
       -peripherals[b + 4].world[X] > peripherals[b + 4].world[Z] &&
       peripherals[b + 1].world[X] > peripherals[b+1].world[Z] &&
       peripherals[b + 5].world[X] > peripherals[b+5].world[Z])
    {
        return 1;
    }
    /* top and bottom clipping planes */
    if(-peripherals[b + 2].world[Y] > peripherals[b + 2].world[Z] &&
       -peripherals[b + 6].world[Y] > peripherals[b + 6].world[Z] &&
       peripherals[b + 1].world[Y] > peripherals[b + 1].world[Z] &&
       peripherals[b + 5].world[Y] > peripherals[b + 5].world[Z])
    {
        return 1;
    }
    return 0;
}

/* perspective transform from world to screen coordinates */
float perspective_xform(float near, float far, float x_min, float x_max, float y_min, float y_max)
{
    float w;
    set_perspective_mat (&perspective, near, far, x_min, x_max, y_min, y_max);
    for(int i = 0; i < num_vertices; i++)
    {
        mat_vec_mul (&perspective, vertex_list[i].world, vertex_list[i].position);
        w = vertex_list[i].position[W];

        scalar_divide (w, vertex_list[i].position, vertex_list[i].position);
        // {x/w, y/w, z/w, 1}
        if(perspective_correct && texturing)
        {
            vertex_list[i].position[W] = 1.0 / w; //carry 1/w in W slot to interpolate
            // {x/w, y/w, z/w, 1/w}
        }
    }
    for(int i = 0; i < num_peripherals; i++)
    {
        float local_w;
        set_perspective_mat (&perspective, near, far, x_min, x_max, y_min, y_max);
        mat_vec_mul (&perspective, peripherals[i].world, peripherals[i].position);
        local_w = peripherals[i].position[W];
        scalar_divide (local_w, peripherals[i].position, peripherals[i].position);
        if(perspective_correct && texturing)
        {
            peripherals[i].position[W] = 1.0 / local_w;
        }
    }
//    printf("w: %.2f\n", w);
    return w;
}

//viewport (screen space) -> camera space (stored in world)
void unproject_screen_to_camera (float out[4], float in[4], float w)
{
    float tmp[4];
    cpy_vec4(tmp, in);
    MAT4 forward, backward, inv_viewport;
    float translation_vec[4] = {-WIN_W/2, -WIN_H/2, 0, 0};
    
    vector_add(tmp, translation_vec, tmp);
    
    if(proj_mode == ORTHO)
    {
        mat_mul(&forward, &ortho, &viewport);
        invert_mat4(&backward, &forward);
        mat_vec_mul(&backward, tmp, out);
    }
    else
    {
        mat_mul(&forward, &perspective, &viewport);
        invert_mat4(&backward, &viewport);
        mat_vec_mul(&backward, tmp, out);
        scalar_multiply(w, out, out);
        invert_mat4(&backward, &perspective);
        mat_vec_mul(&backward, out, out);
    }
}

/* project screen space coordinates "in" to world space coords "out" */
void unproject_screen_to_world (float out[4], float in[4], float w)
{
    MAT4 backward;
    unproject_screen_to_camera (out, in, w);
    invert_mat4(&backward, &cam);
    mat_vec_mul(&backward, out, out);
}

/* called when everything is in world space */
void set_backface_flags (CAMERA *c)
{
    for(int i = 0; i < num_triangles; i++)
    {
        FACE *f = &face_list[i];
        POINT *p0 = &vertex_list[f->vertices[0]];
        POINT *p1 = &vertex_list[f->vertices[1]];
        POINT *p2 = &vertex_list[f->vertices[2]];
        
        float eye_ray[4], dot;
        
        vector_subtract(c->pos, p0->world, eye_ray);
        normalize(eye_ray);
        dot = vector_dot(eye_ray, f->f_normal);
        f->backface_factor = dot;
    }
}

// set each point's eye ray in world space
void set_view_rays (CAMERA *c)
{
    float eye_ray[4];
    for(int i = 0; i < num_vertices; i++)
    {
        POINT *p = &vertex_list[i];
        
        vector_subtract(c->pos, p->world, eye_ray);
        normalize(eye_ray);
        cpy_vec4(p->view, eye_ray);
    }
}

/* set the clip_flags of all triangles in model */
void set_triangle_clip_flags (void)
{
    POINT verts[6];
    POINT *p;
    int num_addl = 0;
    for(int i = 0; i < num_triangles; i++)
    {
        if(num_addl > max_n_addl_tris) return;
        FACE f = face_list[i];
        
        POINT p0 = vertex_list[f.vertices[0]];
        POINT p1 = vertex_list[f.vertices[1]];
        POINT p2 = vertex_list[f.vertices[2]];
        
        cpy_vec4(p0.color, color_list[f.colors[0]]);
        cpy_vec4(p1.color, color_list[f.colors[1]]);
        cpy_vec4(p2.color, color_list[f.colors[2]]);
        
        cpy_vec4(p0.tex, tex_list[f.tex[0]]);
        cpy_vec4(p1.tex, tex_list[f.tex[1]]);
        cpy_vec4(p2.tex, tex_list[f.tex[2]]);
        
        verts[0] = p0;
        verts[1] = p1;
        verts[2] = p2;
        if(entire_tri_outside_frustum(verts))
        {
            face_list[i].clip_flag = 1;
        }
        else if(entire_tri_inside_frustum(verts))
        {
            face_list[i].clip_flag = 0;
        }
        else //if partially, then clipped = 1
        {
            face_list[i].clip_flag = 1;
            if(1) //debugging_mode)
            {
                int num_clipped = clip_triangle (verts);
                if(num_clipped == 0)
                {
                    continue;
                }
                //subdivide polygon into triangles
                for(int j = 1; j < num_clipped - 1; j++)
                {
                    int new_v0 = num_vertices;
                    //v0
                    p = &vertex_list[num_vertices++];
                    *p = verts[0];
                    p->num_tris = 0;
                    cpy_vec4(color_list[new_v0], p->color);
                    cpy_vec4(tex_list[new_v0], p->tex);
                    
                    //v1
                    p = &vertex_list[num_vertices++];
                    *p = verts[j];
                    p->num_tris = 0;
                    cpy_vec4(color_list[new_v0 + 1], p->color);
                    cpy_vec4(tex_list[new_v0 + 1], p->tex);
                    
                    //v2
                    p = &vertex_list[num_vertices++];
                    *p = verts[j + 1];
                    p->num_tris = 0;
                    cpy_vec4(color_list[new_v0 + 2], p->color);
                    cpy_vec4(tex_list[new_v0 + 2], p->tex);
                    
                    int new_f = num_triangles;
                    
                    add_face (new_v0, new_v0 + 1, new_v0 + 2,
                              new_v0, new_v0 + 1, new_v0 + 2,
                              new_v0, new_v0 + 1, new_v0 + 2,
                              0, 0, 0);
                    num_addl++;
                    
                    /* copy data from original face */
                    cpy_vec4 (face_list[new_f].f_normal, face_list[i].f_normal);
                    face_list[new_f].clip_flag = 0;
                    face_list[new_f].backface_factor = face_list[i].backface_factor;
                }
            }
            num_face_normals = num_triangles;
        }
    }
}
/****************************************************************/
/* draw functions */
/****************************************************************/
/* draw wire-frame or filled in model (mode = FRAME or FILL) */
void draw_model(int mode)
{
//    printf("num_triangles: %i\n", num_triangles);
    for(int i = 0; i < num_triangles; i++)
    {
        FACE f = face_list[i];
        if(f.clip_flag)
        {
            continue;
        }
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

        // set tex coords to s/w and t/w to interpolate
        if(perspective_correct && texturing)
        {
            scalar_multiply(p0.position[W], p0.tex, p0.tex);
            scalar_multiply(p1.position[W], p1.tex, p1.tex);
            scalar_multiply(p2.position[W], p2.tex, p2.tex);
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

                if(light_type == LOCAL_L)
                {
                    /* using one of vertices' light vecs instead of global light dir */
                    set_diffuse_term (f.f_normal, p0.light, tmp_diff);
                    set_specular_term (f.f_normal, p0.light, tmp_spec, p0.view);
                }
                else if(light_type == GLOBAL_L)
                {
                    set_diffuse_term (f.f_normal, light, tmp_diff);
                    set_specular_term (f.f_normal, light, tmp_spec, p0.view);
                }
                
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
        
        
            if(f.backface_factor < 0) //pointing away from us
            {
                drawing_backside = ON;
                scalar_add(f.backface_factor / 2.0, p0.color, p0.color);
                scalar_add(f.backface_factor / 2.0, p1.color, p1.color);
                scalar_add(f.backface_factor / 2.0, p2.color, p2.color);
                
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

/* draw object space coord axes */
void draw_local_axes (void)
{
    if(draw_coord_axes)
    {
        drawing_axes = ON;
        //draw coord axes of model
        set_vec4(peripherals[0].color, 1, 0, 0, 1);
        set_vec4(peripherals[1].color, 1, 0, 0, 1);
        draw_line(&peripherals[0], &peripherals[1], DRAW);
        
        set_vec4(peripherals[0].color, 0, 1, 0, 1);
        set_vec4(peripherals[2].color, 0, 1, 0, 1);
        draw_line(&peripherals[0], &peripherals[2], DRAW);
        
        set_vec4(peripherals[0].color, 0, 0, 1, 1);
        set_vec4(peripherals[3].color, 0, 0, 1, 1);
        draw_line(&peripherals[0], &peripherals[3], DRAW);
        drawing_axes = OFF;
    }
}

/* set 2D click frame for sensing mouse clicks in main.c */
void set_click_frame (OBJECT *o)
{
    cpy_vec4(o->bb_tr.position, peripherals[bb_start_idx + 1].position);
    cpy_vec4(o->bb_bl.position, peripherals[bb_start_idx + 3].position);
    cpy_vec4(o->bb_tr.world, peripherals[bb_start_idx + 1].world);
    cpy_vec4(o->bb_bl.world, peripherals[bb_start_idx + 3].world);
}

/* for SCENE mode to draw a black 3D box around the object being modified */
void draw_3D_bb (float bb_color[4])
{
    if(draw_bounding_box)
    {
        for(int i = 0; i < 8; i++)
        {
            cpy_vec4(peripherals[bb_start_idx + i].color, bb_color);
        }
        drawing_bounding_box = ON;
        draw_line(&peripherals[bb_start_idx + 0],
                  &peripherals[bb_start_idx + 1], DRAW);
        draw_line(&peripherals[bb_start_idx + 1],
                  &peripherals[bb_start_idx + 2], DRAW);
        draw_line(&peripherals[bb_start_idx + 2],
                  &peripherals[bb_start_idx + 3], DRAW);
        draw_line(&peripherals[bb_start_idx + 3],
                  &peripherals[bb_start_idx + 0], DRAW);
        
        draw_line(&peripherals[bb_start_idx + 0],
                  &peripherals[bb_start_idx + 4], DRAW);
        draw_line(&peripherals[bb_start_idx + 1],
                  &peripherals[bb_start_idx + 5], DRAW);
        draw_line(&peripherals[bb_start_idx + 2],
                  &peripherals[bb_start_idx + 6], DRAW);
        draw_line(&peripherals[bb_start_idx + 3],
                  &peripherals[bb_start_idx + 7], DRAW);
        
        draw_line(&peripherals[bb_start_idx + 4],
                  &peripherals[bb_start_idx + 5], DRAW);
        draw_line(&peripherals[bb_start_idx + 5],
                  &peripherals[bb_start_idx + 6], DRAW);
        draw_line(&peripherals[bb_start_idx + 6],
                  &peripherals[bb_start_idx + 7], DRAW);
        draw_line(&peripherals[bb_start_idx + 7],
                  &peripherals[bb_start_idx + 4], DRAW);
        drawing_bounding_box = OFF;
    }
}

/****************************************************************/
/* Texture Generation */
/****************************************************************/
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

char *tex_gen_name (int mode)
{
    switch (mode)
    {
        case OFF:
            return "NONE";
        case NAIVE:
            return "NAIVE";
        case CYLINDRICAL:
            return "CYLINDRICAL";
        case SPHERICAL:
            return "SPHERICAL";
        case REFLECTION:
            return "REFLECTION";
        case CUBE_MAP:
            return "CUBE MAP";
        default:
            return "ERROR";
    }
}
/****************************************************************/
/* lighting */
/****************************************************************/
void calculate_light_vectors (void)
{
    POINT *p;
    for(int i = 0; i < num_vertices; i++)
    {
        p = &vertex_list[i];
        vector_subtract(light_pos, p->world, p->light);
        normalize(p->light);
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

