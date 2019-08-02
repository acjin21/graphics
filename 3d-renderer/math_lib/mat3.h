#ifndef MAT3_H
#define MAT3_H

/****************************************************************/
/* types */
/****************************************************************/
typedef struct mat3 {
    float data[9];
} MAT3;

/****************************************************************/
/* functions */
/****************************************************************/
void print_mat3 (MAT3 *out);
void set_mat3 (MAT3 *out,
               float m00, float m01, float m02,
               float m10, float m11, float m12,
               float m20, float m21, float m22);
void clear_mat3 (MAT3 *out);
void set_mat3_idx (MAT3 *out, int i, float val);

float determinant (MAT3 *m);

#endif
