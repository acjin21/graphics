/****************************************************************/
 /* Vector library: vec4 operations on floating point elements */
/****************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

/* compute element-wise addition of v1 and v2, storing sum in res */
void vector_add (float v1[4], float v2[4], float res[4]);

/* compute element-wise subtraction v1 - v2, storing difference in res */
void vector_subtract (float v1[4], float v2[4], float res[4]);

/* compute element-wise multiplication v1 * v2, storing difference in res */
void vector_multiply (float v1[4], float v2[4], float res[4]);

/* compute scalar addition s + v, storing product vector in res */
void scalar_add (float s, float v[4], float res[4]);

/*  compute scalar multiplication s * v, storing product vector in res */
void scalar_multiply (float s, float v[4], float res[4]);

/* compute scalar division v / s, storing quotient vector in res */
void scalar_divide (float s, float v[4], float res[4]);

/* calculate the reflection vector of light and store in refl */
void vector_reflect (float light[4], float normal[4], float refl[4]);

/* calculate length of vector v */
float vector_length (float v[4]);

/* in-place normalization of vector v */
void normalize (float v[4]);

/* calculate cross product v1 x v2 and store in res */
void vector_cross (float v1[4], float v2[4], float res[4]);

/* dot product of v1 and v2 */
float vector_dot (float v1[4], float v2[4]);

/* component-wise average of 3 vectors: v1, v2, v3; result stored in res */
void avg_of_3_vecs (float v1[4], float v2[4], float v3[4], float res[4]);

void set_vec4 (float vec[4], float x, float y, float z, float w);

/* copy contents of val[4] into dest[4] */
void cpy_vec4 (float dest[4], float val[4]);

#endif
