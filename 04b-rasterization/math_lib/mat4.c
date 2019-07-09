#include "mat4.h"

#include <math.h>
#include <stdio.h>
#include "../macros.h"
#include "../vector.h"

void print_mat4 (MAT4 *mat)
{
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            printf("%.2f ", mat->data[r * 4 + c]);
        }
        printf("\n");
    }
    printf("\n");
    
}

/* set 4x4 matrix:
 *      [[m00, m01, m02, m03],
 *       [m10, m11, m12, m13],
 *       [m20, m21, m22, m23],
 *       [m30, m31, m32, m33]]
 */
void set(MAT4 *out,
         float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
{
    out->data[0] = m00;
    out->data[1] = m01;
    out->data[2] = m02;
    out->data[3] = m03;
    out->data[4] = m10;
    out->data[5] = m11;
    out->data[6] = m12;
    out->data[7] = m13;
    out->data[8] = m20;
    out->data[9] = m21;
    out->data[10] = m22;
    out->data[11] = m23;
    out->data[12] = m30;
    out->data[13] = m31;
    out->data[14] = m32;
    out->data[15] = m33;
//    printf("SET\n");
//    print_mat4(out);
}

/* set 4x4 matrix out to the identity matrix */
void set_identity (MAT4 *out)
{
    set(out,
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
}

/* uniform scale (scale factor s) mat4  */
void set_scale_uniform (MAT4 *out, float s)
{
    set(out,
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1);
}

/* non-uniform scale (scale factors x, y, z respectively) mat4  */
void set_scale_nonuniform (MAT4 *out, float x, float y, float z)
{
    set(out,
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
}

/* sets 4x4 matrix out to a translation matrix with
    translation amounts (x, y, z)*/
void set_transl (MAT4 *out, float x, float y, float z)
{
    set(out,
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1);
}

/* sets 4x4 matrix out to a 3D x-axis rotation matrix
 *     | 1   0       0       0 |
 *     | 0   cos     -sin    0 |
 *     | 0   sin     cos     0 |
 *     | 0   0       0       1 |
 *
 * rotation angle in degrees
 */
void set_rot_X (MAT4 *out, float angle){
    //convert deg->rad
    float t = angle * PI / 180.0;
    set(out,
        1,  0,          0,          0,
        0,  cos(t),     -sin(t),    0,
        0,  sin(t),     cos(t),     0,
        0,  0,          0,          1);
}

/* sets 4x4 matrix out to a 3D y-axis rotation matrix
 *     | cos    0   sin     0 |
 *     | 0      1   0       0 |
 *     | -sin   0   cos     0 |
 *     | 0      0   0       1 |
 *
 * rotation angle in degrees
 */
void set_rot_Y (MAT4 *out, float angle){
    //convert deg->rad
    float t = angle * PI / 180.0;
    set(out,
        cos(t),     0,      -sin(t),     0,
        0,          1,      0,          0,
        sin(t),    0,      cos(t),     0,
        0,          0,      0,          1);
}

/* sets 4x4 matrix out to a 3D z-axis rotation matrix
 *     | cos    -sin    0   0 |
 *     | sin    cos     0   0 |
 *     | 0      0       1   0 |
 *     | 0      0       0   1 |
 *
 * rotation angle in degrees
 */
void set_rot_Z (MAT4 *out, float angle){
    //convert deg->rad
    float t = angle * PI / 180.0;
    set(out,
        cos(t),     -sin(t),    0,  0,
        sin(t),     cos(t),     0,  0,
        0,          0,          1,  0,
        0,          0,          0,  1);
}

/* tranforms vector in and writes the resulting vector to out */
void mat_vec_mul (MAT4 *m, float in[4], float out[4])
{
    out[X] = m->data[0]  * in[X] + m->data[1]  * in[Y] + m->data[2]  * in[Z] + m->data[3]  * in[W];
    out[Y] = m->data[4]  * in[X] + m->data[5]  * in[Y] + m->data[6]  * in[Z] + m->data[7]  * in[W];
    out[Z] = m->data[8]  * in[X] + m->data[9]  * in[Y] + m->data[10] * in[Z] + m->data[11] * in[W];
    out[W] = 1;
//    printf("(%f, %f, %f, %f)\n", out[X], out[Y], out[Z], out[W]);
}

float get (MAT4 *m, int r, int c)
{
    return m->data[4 * r + c];
}

void get_row (float out[4], MAT4 *m, int r)
{
    for(int i = 0; i < 4; i++)
    {
        out[i] = get(m, r, i);
    }
}

void get_col (float out[4], MAT4 *m, int c)
{
    for(int i = 0; i < 4; i++)
    {
        out[i] = get(m, i, c);
    }
}


float inner_prod (MAT4* left, MAT4* right, int r, int c)
{
    float row[4], col[4];
    get_row(row, left, r);
    get_col(col, right, c);
    float dotProduct = 0;
    for(int i = 0; i < 4; i++)
    {
        dotProduct += (row[i] * col[i]);
    }
    return dotProduct;
}

// left * right
void mat_mul (MAT4 *out, MAT4 *left, MAT4 *right)
{
    return set(out,
               inner_prod(left, right, 0, 0),
               inner_prod(left, right, 0, 1),
               inner_prod(left, right, 0, 2),
               inner_prod(left, right, 0, 3),
               inner_prod(left, right, 1, 0),
               inner_prod(left, right, 1, 1),
               inner_prod(left, right, 1, 2),
               inner_prod(left, right, 1, 3),
               inner_prod(left, right, 2, 0),
               inner_prod(left, right, 2, 1),
               inner_prod(left, right, 2, 2),
               inner_prod(left, right, 2, 3),
               inner_prod(left, right, 3, 0),
               inner_prod(left, right, 3, 1),
               inner_prod(left, right, 3, 2),
               inner_prod(left, right, 3, 3));
}
// RyRxRz
void set_3d_rot (MAT4 *out, float rx, float ry, float rz)
{
    MAT4 tmp, Rz, Ry, Rx;
    set_rot_Z(&Rz, rz);
    set_rot_X(&Rx, rx);
    set_rot_Y(&Ry, ry);
    mat_mul(out, &Ry, &Rz);
    mat_mul(out, &Rx, out);
}

void set_model_mat (MAT4 *model, float s, float rx, float ry, float rz,
                    float tx, float ty, float tz)
{
    MAT4 tmp;
    set_identity (model);
    set_scale_nonuniform(&tmp, s, s, s); //scale
    mat_mul (model, &tmp, model);
    set_3d_rot(&tmp, rx, ry, rz); //rotate
    mat_mul (model, &tmp, model);
    set_transl (&tmp, tx, ty, tz);
    mat_mul (model, &tmp, model);
}









