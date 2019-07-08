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
/* getter functions */
//void get_row (int r, MAT4 *mat, float out[4]); // set a vector to row r of this matrix

/* matrix operations */
void mat_vec_mul (MAT4 *m, float in[4], float out[4]);
//void mat_mul_before (MAT4 *m1, MAT4 *m2, MAT4 *out);
//void mat_mul_after (MAT4 *m1, MAT4 *m2, MAT4 *out);

// transforms and homogenizes a position vector
void mul_pos (MAT4 *m, float in[4], float out[4]);

// transforms a dir vec  (does not homogenize)
void mul_dir (MAT4 *m, float in[4], float out[4]);

#endif
