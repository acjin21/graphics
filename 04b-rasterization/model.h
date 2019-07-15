#ifndef MODEL_H
#define MODEL_H

#include "math_lib/mat4.h"
#include "scene.h"

/****************************************************************/
/* defines */
/****************************************************************/
#define FRAME 0
#define FILL 1

/****************************************************************/
/* types */
/****************************************************************/
typedef struct face
{
    /* indices into respective lists */
    int vertices[3];
    int colors[3];
    int tex[3];
    int v_normals[3];
    
    float f_normal[4]; //face normal in world coordinates
} FACE;

/****************************************************************/
/* functions */
/****************************************************************/
/* model init functions (set initial world space coords) */
void init_quad(MAT4 *model);
void init_cube (MAT4 *model);
void init_mesh (float scale, float cx, float cy, float cz, float mesh_da);
void init_cylinder (float radius, float scale, float cx, float cy, float cz);
void init_cone (float radius, float scale, float cx, float cy, float cz);
void init_sphere (float radius, float cx, float cy, float cz);
void init_torus (float tube_r, float hole_r, float cx, float cy, float cz);

/* insert peripheral coordinates */
void insert_normal_coords(void);
void insert_coord_axes (float cx, float cy, float cz, float scale);
void insert_bb_coords (void);

/* world space transformations */
void rotate_model(float cx, float cy, float cz,
                  float x_angle, float y_angle, float z_angle);

/* calculate/set normals */
void calculate_face_normals (void);
void calculate_vertex_normals (void);

/* perspective transformations */
void xform_model(float scale);
void translate_model (float distance);
int cull_model (float near, float far);
void perspective_xform(float near, float far);
void viewport_xform(float scale);

/* draw functions */
void draw_model(int mode);
void draw_local_axes (void);
void set_click_frame (OBJECT *o);
void draw_3D_bb (void);

/* texture generation */
void get_tex_coords (void);
char *tex_gen_name (int mode);

/* lighting */
void calculate_light_vectors (void);


/* obj io */
void read_obj_file (char *file_name, MAT4 *model);
void write_obj_file (char *file_name);
#endif
