#include "light.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
float light[4]      = {1, -1, 0, 0};
float light_eye[4]  = {-10, 10, 5, 1};

float light_pos_screen[4];
/* point lights */
float light_pos[4] = {2, 2, 0, 1};

