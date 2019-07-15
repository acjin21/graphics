#ifndef MAT4_H
#define MAT4_H

/****************************************************************/
/* types */
/****************************************************************/
typedef struct mat4 {
    float data[16];
} MAT4;

/****************************************************************/
/* functions */
/****************************************************************/
void print_mat4 (MAT4 *mat);

/* setter functions */
void set(MAT4 *out,
         float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33);

void set_identity (MAT4 *out);

void set_scale_uniform (MAT4 *out, float s);
void set_scale_nonuniform (MAT4 *out, float x, float y, float z);
void set_transl (MAT4 *out, float x, float y, float z);
void set_rot_X (MAT4 *out, float angle);
void set_rot_Y (MAT4 *out, float angle);
void set_rot_Z (MAT4 *out, float angle);
void set_3d_rot (MAT4 *out, float rx, float ry, float rz);

/****************************************************************/
/* to implement later */
/****************************************************************/
/* matrix operations */
void mat_vec_mul (MAT4 *m, float in[4], float out[4]);
void mat_mul (MAT4 *out, MAT4 *left, MAT4 *right);
void set_3d_rot (MAT4 *out, float rx, float ry, float rz);

void set_model_mat (MAT4 *model, float s,
                    float rot_x, float rot_y, float rot_z,
                    float tx, float ty, float tz);

void set_perspective_mat (MAT4 *perspective,
                          float near, float far,
                          float x_min, float x_max,
                          float y_min, float y_max);

void set_ortho_mat (MAT4 *ortho,
                    float x_min, float x_max,
                    float y_min, float y_max,
                    float z_min, float z_max);

void set_viewport_mat (MAT4 *viewport, int view_w, int view_h);

//void mat_mul_before (MAT4 *m1, MAT4 *m2, MAT4 *out);
//void mat_mul_after (MAT4 *m1, MAT4 *m2, MAT4 *out);

//// transforms and homogenizes a position vector
//void mul_pos (MAT4 *m, float in[4], float out[4]);
//
//// transforms a dir vec  (does not homogenize)
//void mul_dir (MAT4 *m, float in[4], float out[4]);
#endif
