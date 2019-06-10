#include <stdio.h>
#include <math.h>

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

void scalar_mul (float s, float v[4], float res[4])
{
    for(int i = 0; i < 4; i++)
    {
        res[i] = s * v[i];
    }
}

void scalar_div (float s, float v[4], float res[4])
{
    if (s == 0)
    {
        printf("error: div by zero\n");
        return;
    }
    scalar_mul(1.0 / s, v, res);
}


float vector_length (float v[4])
{
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

void normalize (float v[4])
{
    float len = vector_length(v);
    scalar_div(len, v, v);
}

