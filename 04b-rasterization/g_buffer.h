#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "point.h"

extern POINT g_buffer[DEFAULT_WIN_H][DEFAULT_WIN_W];
extern int mode_deferred_render;

void clear_g_buffer(float r, float g, float b, float a);
void draw_g_buffer(void);

#endif
