#ifndef MACROS_H
#define MACROS_H

#define R 0
#define G 1
#define B 2
#define A 3

#define X 0
#define Y 1
#define Z 2
#define W 3

#define PI 3.1415926
#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))

typedef struct point {
    float position[4];
    float color[4];
    float tex[4];
} POINT;
#endif
