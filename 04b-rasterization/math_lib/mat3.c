#include "mat3.h"
#include <math.h>
#include <stdio.h>

void print_mat3 (MAT3 *out)
{
    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            printf("%.4f\t", out->data[r * 3 + c]);
        }
        printf("\n");
    }
}

void clear_mat3 (MAT3 *out)
{
    for(int i = 0; i < 9; i++)
    {
        out->data[i] = 0;
    }
}

void set_mat3 (MAT3 *out,
         float m00, float m01, float m02,
         float m10, float m11, float m12,
         float m20, float m21, float m22)
{
    out->data[0] = m00;
    out->data[1] = m01;
    out->data[2] = m02;
    out->data[3] = m10;
    out->data[4] = m11;
    out->data[5] = m12;
    out->data[6] = m20;
    out->data[7] = m21;
    out->data[8] = m22;
}

void set_mat3_idx (MAT3 *out, int i, float val)
{
    out->data[i] = val;
}

/* cofactor assuming r = 0 */
float cofactor (MAT3 *m, int r, int c)
{
//    int sign = (int) pow(-1, r + c);
    float minor;
    switch (c)
    {
        case 0:
            return m->data[4] * m->data[8] - m->data[5] * m->data[7];
        case 1:
            return m->data[5] * m->data[6] - m->data[3] * m->data[8];
        case 2:
            return m->data[3] * m->data[7] - m->data[4] * m->data[6];
        default:
            return -1;
    }
}

/* | m00 m01 m02 | */
/* | m10 m11 m12 | */
/* | m20 m21 m22 | */
float determinant (MAT3 *m)
{
    float det = 0;
    for(int i = 0; i < 3; i++)
    {
        det += (m->data[i] * cofactor(m, 0, i));
    }
    return det;
}

