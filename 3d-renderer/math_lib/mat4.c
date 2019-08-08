#include "mat4.h"

#include <math.h>
#include <stdio.h>
#include "../macros.h"
#include "../vector.h"
#include "mat3.h"

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
}

void set_one_col (MAT4 *out, int c, float col[4])
{
    for(int i = 0; i < 4; i++)
    {
        out->data[c + 4 * i] = col[i];
    }
}

void set_one_row (MAT4 *out, int r, float row[4])
{
    for(int i = 0; i < 4; i++)
    {
        out->data[4 * r + i] = row[i];
    }
}

void set_all_cols (MAT4 *out,
                   float col0[4], float col1[4], float col2[4], float col3[4])
{
    set_one_col(out, 0, col0);
    set_one_col(out, 1, col1);
    set_one_col(out, 2, col2);
    set_one_col(out, 3, col3);
}

void set_all_rows (MAT4 *out,
                   float row0[4], float row1[4], float row2[4], float row3[4])
{
    set_one_row(out, 0, row0);
    set_one_row(out, 1, row1);
    set_one_row(out, 2, row2);
    set_one_row(out, 3, row3);
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
    float temp[4];
    temp[X] = m->data[0]  * in[X] + m->data[1]  * in[Y] + m->data[2]  * in[Z] + m->data[3]  * in[W];
    temp[Y] = m->data[4]  * in[X] + m->data[5]  * in[Y] + m->data[6]  * in[Z] + m->data[7]  * in[W];
    temp[Z] = m->data[8]  * in[X] + m->data[9]  * in[Y] + m->data[10] * in[Z] + m->data[11] * in[W];
    temp[W] = m->data[12]  * in[X] + m->data[13]  * in[Y] + m->data[14] * in[Z] + m->data[15] * in[W];
    cpy_vec4(out, temp);
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
    float dot_product = 0;
    for(int i = 0; i < 4; i++)
    {
        dot_product += (row[i] * col[i]);
    }
    return dot_product;
}

// left * right
void mat_mul (MAT4 *out, MAT4 *left, MAT4 *right)
{
    set(out,
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

void set_camera_mat (MAT4 *cam, CAMERA *c)
{
    float u[4], v[4];
    float col3[4] = {0, 0, 0, 1};
    MAT4 transl, rot;

    set_all_rows(&rot, c->u, c->up, c->v, col3);
    set_transl(&transl, -c->pos[X], -c->pos[Y], -c->pos[Z]);
    mat_mul (cam, &rot, &transl ); // M_cam = M_rot * M_transl
}

void set_perspective_mat (MAT4 *perspective,
                          float near, float far,
                          float x_min, float x_max,
                          float y_min, float y_max)
{
    set(perspective,
        2.0 * near / (x_max - x_min), 0, (x_max + x_min) / (x_min - x_max), 0,
        0, 2.0 * near / (y_max - y_min), (y_max + y_min) / (y_min - y_max), 0,
        0, 0, (near + far) / (far - near), - (2.0 * far * near) / (far - near),
        0, 0, 1, 0);
//    set(perspective,
//        near, 0, 0, 0,
//        0, near, 0, 0,
//        0, 0, 0, 1,
//        0, 0, far - near, 0);
}

void set_ortho_mat (MAT4 *ortho,
                    float x_min, float x_max,
                    float y_min, float y_max,
                    float z_min, float z_max)
{
    float dx = x_max - x_min;
    float dy = y_max - y_min;
    float dz = z_max - z_min;
    set(ortho,
       2.0 / dx,    0,          0,          (x_max + x_min) / (x_min - x_max),
       0,           2.0 / dy,   0,          (y_max + y_min) / (y_min - y_max),
       0,           0,          2.0 / dz,   (z_max + z_min) / (z_min - z_max),
       0,           0,          0,          1);
}

//TODO: what to do if view_w and/or view_h are odd numbers?
void set_viewport_mat (MAT4 *viewport, int view_w, int view_h)
{
    set(viewport,
        view_w / 2, 0, 0, 0,  //view_w / 2,
        0, view_h / 2, 0, 0,  //view_h / 2,
        0, 0, 1, 0,
        0, 0, 0, 1);
}



float cofactor_mat4 (MAT4 *m, int r, int c)
{
    int sign = ((r + c) % 2 == 0) ? 1 : -1;
    MAT3 minor;
    int i = 0;
    //create the minor
    for(int ri = 0; ri < 4; ri++)
    {
        if(ri == r) continue;
        for(int ci = 0; ci < 4; ci++)
        {
            if(ci == c) continue;
            set_mat3_idx(&minor, i++, m->data[ri * 4 + ci]);
        }
    }
//    printf("\n====== MINOR: =======\n");
//    print_mat3(&minor);
//    printf("=====================\n");
    return sign * determinant(&minor);
}

float determinant_mat4 (MAT4 *m)
{
    float det = 0;
    for(int i = 0; i < 4; i++)
    {
        det += (m->data[i] * cofactor_mat4(m, 0, i));
    }
    return det;
}

void scalar_multiply_mat4 (MAT4 *out, float s, MAT4 *in)
{
    for(int i = 0; i < 16; i++)
    {
        out->data[i] = s * in->data[i];
    }
}

void invert_mat4 (MAT4 *out, MAT4 *in)
{
    float cof00, cof01, cof02, cof03,
    cof10, cof11, cof12, cof13,
    cof20, cof21, cof22, cof23,
    cof30, cof31, cof32, cof33;
    // get cofactors
    cof00 = cofactor_mat4(in, 0, 0);
    cof01 = cofactor_mat4(in, 0, 1);
    cof02 = cofactor_mat4(in, 0, 2);
    cof03 = cofactor_mat4(in, 0, 3);
    cof10 = cofactor_mat4(in, 1, 0);
    cof11 = cofactor_mat4(in, 1, 1);
    cof12 = cofactor_mat4(in, 1, 2);
    cof13 = cofactor_mat4(in, 1, 3);
    cof20 = cofactor_mat4(in, 2, 0);
    cof21 = cofactor_mat4(in, 2, 1);
    cof22 = cofactor_mat4(in, 2, 2);
    cof23 = cofactor_mat4(in, 2, 3);
    cof30 = cofactor_mat4(in, 3, 0);
    cof31 = cofactor_mat4(in, 3, 1);
    cof32 = cofactor_mat4(in, 3, 2);
    cof33 = cofactor_mat4(in, 3, 3);


    // calculate determinant
    float det =
            in->data[0] * cof00 +
            in->data[1] * cof01 +
            in->data[2] * cof02 +
            in->data[3] * cof03;
    
    // check to see if matrix is invertible
    if(det == 0)
    {
        printf("invert_mat4 error: singular matrix\n");
        return;
    }
    // calculate 1 / det
    float factor = 1.0 / det;
    
    // set out (making sure to transpose adjugate matrix)
    set(out,
        cof00, cof10, cof20, cof30,
        cof01, cof11, cof21, cof31,
        cof02, cof12, cof22, cof32,
        cof03, cof13, cof23, cof33);
    scalar_multiply_mat4 (out, factor, out);
}






