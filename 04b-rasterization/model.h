#ifndef MODEL_H
#define MODEL_H

/****************************************************************/
/* defines */
/****************************************************************/
#define FRAME 0
#define FILL 1

#define NUM_TEX_MODES 3
#define NAIVE 1
#define CYLINDRICAL 2
#define SPHERICAL 3
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
void init_quad (void);
void init_cube (float scale, float cx, float cy, float cz);
void init_mesh (float scale, float cx, float cy, float cz, float mesh_da);
void init_cylinder (float radius, float scale, float cx, float cy, float cz);
void init_cone (float radius, float scale, float cx, float cy, float cz);
void init_sphere (float radius, float cx, float cy, float cz);
void init_torus (float tube_r, float hole_r, float cx, float cy, float cz);

void read_obj_file (char *name, float scale, float cx, float cy, float cz);
void write_obj_file (char *file_name);

void insert_normal_coords(void);
void insert_coord_axes (float cx, float cy, float cz, float scale);

void xform_model(float scale);
void draw_model(int mode);
void rotate_model(float cx, float cy, float cz,
                  float x_angle, float y_angle, float z_angle);
void calculate_face_normals (void);
void calculate_vertex_normals (void);

void translate_model (float distance);
void perspective_xform(float near, float far);
void viewport_xform(float scale);

void get_tex_coords (void);
#endif
