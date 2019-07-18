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
/* distance from point to view frustum plane */
float point_to_plane_dist (float point[4], PLANE *plane)
{
    float point_dist_from_orig = vector_dot(point, plane->normal);
    
    // havent checked if works when plane doesnt go through origin and is not near or far planes
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
/*  NOTE: hard coded for FOV = 90 deg and eye @ origin (camera space coords) */
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

void interp_intersection_point (float a[4], float b[4], float intersect[4], float t)
{
    for(int i = 0; i < 4; i++)
    {
        intersect[i] = a[i] + t * (b[i] - a[i]);
    }
}

/* clip line(a, b) against plane p to generate intersection point *intersect
 with properly interpolated POINT fields: normals, uvs, depth (z)*/
// NOTE: *a is inside, *b is outside
void clip_line (POINT *a, POINT *b, POINT *intersect, PLANE *p)
{
    // find dist a ( > 0)
    float dist_a = point_to_plane_dist(a->world, p);
    // find dist b (< 0)
    float dist_b = point_to_plane_dist(b->world, p);

    // find t factor ("blend weight") at the intersect
    float t = dist_a / (dist_a - dist_b);
    
    // set intersect's fields using t
    interp_intersection_point(a->world, b->world, intersect->world, t);
    interp_intersection_point(a->color, b->color, intersect->color, t);
    interp_intersection_point(a->tex, b->tex, intersect->tex, t);
    interp_intersection_point(a->v_normal, b->v_normal, intersect->v_normal, t);
    interp_intersection_point(a->light, b->light, intersect->light, t);
}

/* clips each edge of triangle by walking edges.
    sets the vertices of resulting clipped polygon (verts), and
    returns the number of elements in verts */
int clip_triangle (POINT *verts)
{
    POINT new, clipped[8];
    POINT *in, *out, *tmp;
    int num_in = 3;
    int num_out;
    
    in = verts;
    out = clipped;
    /* clip triangle against one plane at a time */
    for(int i = 0; i < 6; i++)
    {
        //reset num_out
        num_out = 0;
        //walk each edge that starts at POINT in[e]
        for(int e = 0; e < num_in; e++)
        {
            int next_e = (e + 1) % num_in;
            float dist1 = point_to_plane_dist(in[e].world, &frustum[i]);
            float dist2 = point_to_plane_dist(in[next_e].world, &frustum[i]);
            //if both out
            if (dist1 < 0 && dist2 < 0)
            {
                continue;
            }
            //if both in, add end
            else if (dist1 > 0 && dist2 > 0)
            {
                out[num_out] = in[next_e];
                num_out++;
            }
            //if out->in, add intersection, end
            else if (dist1 < 0 && dist2 > 0)
            {
                clip_line(&in[e], &in[next_e], &new, &frustum[i]);
                out[num_out] = new;
                num_out++;
                out[num_out] = in[next_e];
                num_out++;
            }
            //if in->out, add intersection
            else if (dist1 > 0 && dist2 < 0)
            {
                clip_line(&in[e], &in[next_e], &new, &frustum[i]);
                out[num_out] = new;
                num_out++;
            }
        }
        // swap in & out pointers
        tmp = in;
        in = out;
        out = in;
        
        // update num_in
        num_in = num_out;
    }
    // set out to the final output POINT list
    out = in;
    // move POINTs from in to verts
    for(int i = 0; i < num_out; i++)
    {
        verts[i] = out[i];
    }
    // return resulting num of verts of clipped polygon
    return num_out;
    
}


/* whether entire triangle is within frustum boundary */
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

/* whether entire triangle is outside frustum boundary */
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

