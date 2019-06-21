/****************************************************************/
 /* Vector library: vec4 operations on floating point elements */
/****************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include "macros.h"
/*
 * compute element-wise addition of v1 and v2, storing sum in res
 */
void vector_add (float v1[4], float v2[4], float res[4]);

/*
 * compute element-wise subtraction v1 - v2, storing difference in res
 */
void vector_subtract (float v1[4], float v2[4], float res[4]);

/*
 * compute scalar multiplication s * v, storing product vector in res
 */
void scalar_multiply (float s, float v[4], float res[4]);

/*
 * compute scalar division v / s, storing quotient vector in res
 */
void scalar_divide (float s, float v[4], float res[4]);

/*
 * compute length of v
 */
float vector_length (float v[4]);

/*
 * normalize v
 */
void normalize (float v[4]);

/*
 * vector cross product
 */
void vector_cross (float v1[4], float v2[4], float res[4]);

void set_vec4 (float vec[4], float x, float y, float z, float w);
void cpy_vec4 (float dest[4], float val[4]);
#endif
