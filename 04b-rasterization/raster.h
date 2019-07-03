#ifndef RASTER_H
#define RASTER_H

#include "macros.h"
#include "vector.h"
#include "point.h"

#define DRAW 0
#define WALK 1

void draw_line(POINT *start, POINT *end, int mode);
void draw_triangle(POINT *v0, POINT *v1, POINT *v2);
void draw_triangle_barycentric( POINT *v0, POINT *v1, POINT *v2);

#endif
