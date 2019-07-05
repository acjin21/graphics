#ifndef MACROS_H
#define MACROS_H

/* screen */
#define WIN_W 800
#define WIN_H 800

/* coords */
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

/* general */
#define PI 3.1415926
#define EPSILON 0.0001
#define FALSE 0
#define TRUE 1

#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))
#define MAX(_a,_l) (_a < _l ? _l : _a)
#define CLAMP(a,l,h) ((a) < (l) ? (l) : ((a) > (h) ? (h) : (a)))

#define CLAMP_IMG(val,min_val,max_val)\
{                   \
val = (val >= max_val) ? max_val - 1 : val; \
val = (val < min_val) ? min_val : val; \
}

/* different modulation modes */
#define MOD_COLOR 0
#define MOD_LIGHT 1

/* shading techniques */
#define NO_SHADING 0
#define FLAT 1
#define PHONG 2

/* different types of normals to be drawn */
#define NO_NORMALS 0
#define F_NORMALS 1
#define V_NORMALS 2

#endif
