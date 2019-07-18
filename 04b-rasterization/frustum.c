#include "frustum.h"

#include <math.h>
#include "macros.h"
#include "vector.h"
/************************/
/* global variables */
/************************/
PLANE frustum[6];        //view frustum for clipping

/************************/
/* utility functions */
/************************/
float point_to_plane_dist (float point[4], PLANE *plane)
{
    float point_dist_from_orig = vector_dot(point, plane->normal);
    
    // doesn't work if the plane doent go through origin and is not near or far planes
    // assuming plane.normal is normalized
    if(plane->distance > 0)
    {
        if(point_dist_from_orig < 0) //far plane
        {
            return plane->distance - ABS(point_dist_from_orig);
        }
        else if (point_dist_from_orig > 0) //near plane
        {
            return point_dist_from_orig - plane->distance;
        }
    }
    return  point_dist_from_orig;
}

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

int entire_tri_inside_frustum (POINT tri[3])
{
    float d0, d1, d2;
    for(int i = 0; i < 6; i++)
    {
        d0 = point_to_plane_dist(tri[0].world, &frustum[i]);
        d1 = point_to_plane_dist(tri[1].world, &frustum[i]);
        d2 = point_to_plane_dist(tri[2].world, &frustum[i]);
        
        if(d0 < 0 || d1 < 0 || d2 < 0)
        {
            return 0;
        }
    }
    return 1;
}

int entire_tri_outside_frustum (POINT tri[3])
{
    float d0, d1, d2;
    for(int i = 0; i < 6; i++)
    {
        d0 = point_to_plane_dist(tri[0].world, &frustum[i]);
        d1 = point_to_plane_dist(tri[1].world, &frustum[i]);
        d2 = point_to_plane_dist(tri[2].world, &frustum[i]);
        
        if(d0 < 0 && d1 < 0 && d2 < 0)
        {
            return 1;
        }
    }
    return 0;
}

