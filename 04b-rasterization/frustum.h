#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "point.h"

typedef struct plane {
    float normal[4];
    float distance;
} PLANE;

void setup_clip_frustum(float near, float far);
int entire_tri_inside_frustum (POINT tri[3]);
int entire_tri_outside_frustum (POINT tri[3]);

#endif
