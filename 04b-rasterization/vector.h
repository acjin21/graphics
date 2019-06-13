void vector_add (float v1[4], float v2[4], float res[4]);
void vector_subtract (float v1[4], float v2[4], float res[4]);
void scalar_multiply (float s, float v[4], float res[4]);
void scalar_divide (float s, float v[4], float res[4]);
float vector_length (float v[4]);
void normalize (float v[4]);

void vec2f_rotate (float theta, float v[4], float res[4]);
void vec2f_reflect_x (float v[4], float res[4]);
