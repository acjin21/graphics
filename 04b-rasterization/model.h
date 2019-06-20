#ifndef MODEL_H
#define MODEL_H

#include "raster.h"
#include "vector.h"
#include "macros.h"

#define FRAME 0
#define FILL 1



void init_cube(void);
void init_mesh (int n, float mesh_dx);

void xform_model(float scale);
void draw_model(int mode);
void rotate_model(float x_angle, float y_angle, float z_angle);

void translate_model (float distance);
void perspective_xform(float near, float far);
void viewport_xform(float scale);
#endif
