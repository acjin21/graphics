#ifndef MODEL_H
#define MODEL_H

#include "raster.h"
#include "vector.h"
#include "macros.h"

#define FRAME 0
#define FILL 1

void init_cube(void);
void xform_model(void);
void draw_model(int mode);
void rotate_model(float x_angle, float y_angle, float z_angle, int axis);

#endif
