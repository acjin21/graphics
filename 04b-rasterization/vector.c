#include "vector.h"

#include <stdio.h>
#include <math.h>
#include "macros.h"

extern float eye[4];
/****************************************************************/
/* vector utility functions */
/****************************************************************/
void vector_add (float v1[4], float v2[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = v1[i] + v2[i];
    }
}

void vector_subtract (float v1[4], float v2[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = v1[i] - v2[i];
    }
}

void vector_multiply (float v1[4], float v2[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = v1[i] * v2[i];
    }
}

void scalar_add (float s, float v[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = s + v[i];
    }
}
void scalar_multiply (float s, float v[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = s * v[i];
    }
}

void scalar_divide (float s, float v[4], float res[4])
{
    if (s < EPSILON)
    {
        s = EPSILON;
    }
    scalar_multiply(1.0 / s, v, res);
}

void vector_reflect (float light[4], float normal[4], float refl[4])
{
    float tmp[4];
    float dot = vector_dot (normal, light);
    scalar_multiply(2.0 * dot, normal, tmp);
    vector_subtract(tmp, light, refl);
    normalize(refl);
}

float vector_length (float v[4])
{
    return sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]);
}

void normalize (float v[4])
{
    float len = vector_length(v);
    if(len == 0)
    {
        len = EPSILON;
    }
    scalar_divide(len, v, v);
}

void vector_cross (float v1[4], float v2[4], float res[4])
{
    res[X] = (v1[Y] * v2[Z]) - (v1[Z] * v2[Y]);
    res[Y] = (v1[Z] * v2[X]) - (v1[X] * v2[Z]);
    res[Z] = (v1[X] * v2[Y]) - (v1[Y] * v2[X]);
    res[W] = 1.0;
}

float vector_dot (float v1[4], float v2[4])
{
    return v1[X] * v2[X] + v1[Y] * v2[Y] + v1[Z] * v2[Z];
}

void avg_of_3_vecs (float v1[4], float v2[4], float v3[4], float res[4])
{
    vector_add(v1, v2, res);
    vector_add(res, v3, res);
    scalar_divide(3, res, res);
}

void set_vec4 (float vec[4], float x, float y, float z, float w)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = w;
}

void cpy_vec4 (float dest[4], float val[4])
{
    for(int i = 0; i < 4; i++)
    {
        dest[i] = val[i];
    }
}

/****************************************************************/
/* normal-to-texture conversions */
/****************************************************************/
// assert normal is normalized
void naive_map (float normal[4], float tex[4])
{
    normalize(normal);
    tex[S] = (normal[X] + 1.0) / 2.0;
    tex[T] = (normal[Y] + 1.0) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
}

void cylindrical_map (float normal[4], float tex[4])
{
    normalize(normal);
    float r = sqrt(normal[X] * normal[X] + normal[Z] * normal[Z]);
    float theta = asin(normal[Z] / r);
    if(isnan(theta)) //NaN correction
    {
        theta = 0;
    }
    tex[S] = (theta + PI / 2.0) / (2.0 * PI) + (normal[Z] < 0 ? 0.5 : 0);
    tex[T] = (normal[Y] + 1.0) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}

void spherical_map (float normal[4], float tex[4])
{
    normalize(normal);
    float r = sqrt(normal[X] * normal[X] + normal[Z] * normal[Z]);
    float theta = asin(normal[Z] / r);
    if(isnan(theta)) //NaN correction
    {
        theta = 0;
    }
    float phi = asin(normal[Y]);
    tex[S] = (theta + PI / 2.0) / (2.0 * PI) + (normal[Z] < 0 ? 0.5 : 0);
    tex[T] = (phi + PI / 2.0) / PI;
    tex[2] = 0.0;
    tex[3] = 1.0;
}

void reflection_map (float normal[4], float tex[4])
{
    float reflect[4];
    vector_reflect(eye, normal, reflect);
    spherical_map(reflect, tex);
}

