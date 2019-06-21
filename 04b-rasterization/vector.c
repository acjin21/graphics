#include <stdio.h>
#include <math.h>

#include "vector.h"

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

void scalar_multiply (float s, float v[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = s * v[i];
    }
}

void scalar_divide (float s, float v[4], float res[4])
{
    if (s == 0)
    {
        //printf("error: div by zero\n");
        //return;
    }
    scalar_multiply(1.0 / s, v, res);
}

/* calculate length of vector v */
float vector_length (float v[4])
{
    return sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]);
}

/* in-place normalization of vector v */
void normalize (float v[4])
{
    float len = vector_length(v);
    if(len == 0)
    {
        len = EPSILON;
    }
    scalar_divide(len, v, v);
}

/* calculate cross product v1 x v2 and store in res */
void vector_cross (float v1[4], float v2[4], float res[4])
{
    res[X] = (v1[Y] * v2[Z]) - (v1[Z] * v2[Y]);
    res[Y] = (v1[Z] * v2[X]) - (v1[X] * v2[Z]);
    res[Z] = (v1[X] * v2[Y]) - (v1[Y] * v2[X]);
    res[W] = 1.0;
}

/* set vec4 */
void set_vec4 (float vec[4], float x, float y, float z, float w)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = w;
}

/* copy contents of val[4] into dest[4] */
void cpy_vec4 (float dest[4], float val[4])
{
    for(int i = 0; i < 4; i++)
    {
        dest[i] = val[i];
    }
}

