#ifndef FRUSTUM_H
#define FRUSTUM_H

typedef struct plane {
    float normal[4];
    float distance;
} PLANE;

void setup_clip_frustum(float near, float far);

#endif
