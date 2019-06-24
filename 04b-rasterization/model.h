#ifndef MODEL_H
#define MODEL_H

#include "raster.h"
#include "vector.h"
#include "macros.h"

#define FRAME 0
#define FILL 1


void init_quad (void);
void init_cube (float scale, float center[4]);
void init_mesh (float scale, float center[4], float mesh_da);
void init_cylinder (float radius, float scale, float center[4]);
void init_cone (float radius, float scale, float center[4]);

void insert_normal_coords(void);

void xform_model(float scale);
void draw_model(int mode);
void rotate_model(float center[4], float x_angle, float y_angle, float z_angle);
void calculate_face_normals (void);
void calculate_vertex_normals (void);

void translate_model (float distance);
void perspective_xform(float near, float far);
void viewport_xform(float scale);
#endif
