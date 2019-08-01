#ifndef MACROS_H
#define MACROS_H

/* screen */
#define DEFAULT_WIN_W 800
#define DEFAULT_WIN_H 800
#define MAX_WIN_H 1000
#define MAX_WIN_W 1000
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
#define NA -1
#define ON 1
#define OFF 0


/* functions */
#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))
#define MAX(_a,_b) ((_a) < (_b) ? (_b) : (_a))
#define MIN(_a,_b) ((_a) > (_b) ? (_b) : (_a))
#define CLAMP(a,l,h) ((a) < (l) ? (l) : ((a) > (h) ? (h) : (a)))

#define CLAMP_IMG(val,min_val,max_val)\
{                   \
val = (val >= max_val) ? max_val - 1 : val; \
val = (val < min_val) ? min_val : val; \
}

#endif
