#include "frustum.h"

#include <math.h>

#include "vector.h"
/************************/
/* global variables */
/************************/
PLANE frustum[6];        //view frustum for clipping

/************************/
/* functions */
/************************/
/* initialize our global frustum */
/*  NOTE: hard coded for FOV = 90 deg */
void setup_clip_frustum(float near, float far)
{
    float inv_sqrt = 1.0 / sqrt(2.0);
    
    /* left */
    set_vec4(frustum[0].normal, inv_sqrt, 0, inv_sqrt, 0);
    frustum[0].distance = 0.0;
    
    /* right */
    set_vec4(frustum[1].normal, -inv_sqrt, 0, inv_sqrt, 0);
    frustum[1].distance = 0.0;
    
    /* top */
    set_vec4(frustum[2].normal, 0, -inv_sqrt, inv_sqrt, 0);
    frustum[2].distance = 0.0;
    
    /* bottom */
    set_vec4(frustum[3].normal, 0, inv_sqrt, inv_sqrt, 0);
    frustum[3].distance = 0.0;
    
    /* near */
    set_vec4(frustum[4].normal, 0, 0, 1, 0);
    frustum[4].distance = near;
    
    /* bottom */
    set_vec4(frustum[5].normal, 0, 0, -1, 0);
    frustum[5].distance = far;
}

