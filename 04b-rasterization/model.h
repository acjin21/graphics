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
void build_model (OBJECT *o);
/* model init functions (set initial world space coords) */
void init_quad(MAT4 *model);
void init_cube (MAT4 *model);
void init_mesh (MAT4 *model);
void init_cylinder (float radius, float height_scale, MAT4 *model);
void init_cone (float radius, float height_scale, MAT4 *model);
void init_sphere (float radius, MAT4 *model);
void init_torus (float tube_radius, float hole_radius, MAT4 *model);

/* insert peripheral coordinates */
void insert_normal_coords(void);
void insert_coord_axes (float cx, float cy, float cz, float scale);
void insert_bb_coords (void);

/* world space transformations */
void world_xforms(OBJECT *o);

/* calculate/set normals */
void calculate_face_normals (void);
void calculate_vertex_normals (void);

/* functions that rely on world space coords */
void set_backface_flags (CAMERA *c);
void set_view_rays (CAMERA *c);

void camera_xform (CAMERA *c);

void set_triangle_clip_flags (void);

/* orthographic transformations */
void xform_model(float x_min, float x_max,
                 float y_min, float y_max,
                 float z_min, float z_max);

/* perspective transformations */
int cull_model (float near, float far);
float perspective_xform(float near, float far,
                       float x_min, float x_max, float y_min, float y_max);
void viewport_mat_xform (int vp_w, int vp_h);

/* inverse projections */
void unproject_screen_to_camera (float out[4], float in[4], float w);
void unproject_screen_to_world (float out[4], float in[4], float w);

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
