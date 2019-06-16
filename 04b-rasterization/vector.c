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
        printf("error: div by zero\n");
        return;
    }
    scalar_multiply(1.0 / s, v, res);
}

float vector_length (float v[4])
{
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

void normalize (float v[4])
{
    float len = vector_length(v);
    scalar_divide(len, v, v);
}

void vec2f_rotate (float theta, float v[4], float res[4])
{
    float x = v[0];
    float y = v[1];
    /* res.x = xcos(theta) - ysin(theta) */
    res[0] = x * cos(theta) - y * sin(theta);
    /* res.y = xsin(theta) + ycos(theta) */
    res[1] = x * sin(theta) + y * cos(theta);
}

void vec2f_reflect_x (float v[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = v[i];
    }
    res[1] *= -1;
}
