#ifndef MACROS_H
#define MACROS_H

#define WIN_W 800
#define WIN_H 800

#define R 0
#define G 1
#define B 2
#define A 3

#define X 0
#define Y 1
#define Z 2
#define W 3

#define S 0
#define T 1

#define PI 3.1415926
#define EPSILON 0.0001
#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))
#define MAX(_a,_l) (_a < _l ? _l : _a)
#define CLAMP(a,l,h) ((a) < (l) ? (l) : ((a) > (h) ? (h) : (a)))

#define CLAMP_IMG(val,min_val,max_val)\
{                   \
val = (val >= max_val) ? max_val - 1 : val; \
val = (val < min_val) ? min_val : val; \
}


#define MOD_COLOR 0
#define MOD_LIGHT 1

#define FLAT 0
#define PHONG 1
#define NO_SHADING 2

#endif
