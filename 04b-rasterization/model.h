#ifndef MODEL_H
#define MODEL_H

#include "math_lib/mat4.h"
#include "scene.h"
#include "camera.h"

/****************************************************************/
/* types */
/****************************************************************/
typedef struct face
{
    /* indices into respective lists */
    int vertices[3];
    int colors[3];
    int tex[3];
    int v_normals[3]; //only used if obj has vns
    
    float f_normal[4]; //face normal in world coordinates
    int clip_flag;
    float backface_factor;
} FACE;

/****************************************************************/
/* variable externs */
/****************************************************************/
/* for app.c, scene.c */
extern int modulate_type;
extern int tex_gen_mode;

/* for point.c */
extern int modulate_type;
extern int drawing_backside;
extern int tex_gen_mode;

/****************************************************************/
/* functions */
/****************************************************************/
/* model init functions (set initial world space coords) */
void init_quad(MAT4 *model);
void init_cube (MAT4 *model);
void init_mesh (float scale, float cx, float cy, float cz);
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
void translate_model_mat (float tx, float ty, float tz);
void scale_model_mat (float sx, float sy, float sz);

/* calculate/set normals */
void calculate_face_normals (void);
void calculate_vertex_normals (void);

void set_backface_flags (CAMERA *c);
void camera_xform (CAMERA *c);


/* orthographic transformations */
void xform_model(float x_min, float x_max,
                 float y_min, float y_max,
                 float z_min, float z_max);

/* perspective transformations */
void translate_model (float distance);
int cull_model (float near, float far);
void perspective_xform(float near, float far, float x_min, float x_max, float y_min, float y_max);
void viewport_xform(float scale);

void viewport_mat_xform (int vp_w, int vp_h);
void set_triangle_clip_flags (void);

/* draw functions */
void draw_model(int mode);
void draw_local_axes (void);
void set_click_frame (OBJECT *o);
void draw_3D_bb (float bb_color[4]);

/* texture generation */
void get_tex_coords (void);
char *tex_gen_name (int mode);

/* lighting */
void calculate_light_vectors (void);


/* obj io */
void read_obj_file (char *file_name, MAT4 *model);
void write_obj_file (char *file_name);
#endif
