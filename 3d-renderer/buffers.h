/* defines all buffers used in renderer */
#ifndef BUFFERS_H
#define BUFFERS_H

#include "macros.h"
#include "point.h"
#include "app.h"

extern float color_buffer[MAX_WIN_H][MAX_WIN_W][4];
extern float depth_buffer[MAX_WIN_H][MAX_WIN_W];
extern int stencil_buffer[MAX_WIN_H][MAX_WIN_W];
extern POINT g_buffer[MAX_WIN_H][MAX_WIN_W];

void clear_color_buffer(float r, float g, float b, float a);
void draw_color_buffer (void);
void clear_depth_buffer (float value);
void draw_depth_buffer (void);
void clear_stencil_buffer(void);
void draw_stencil_buffer(void);
void clear_g_buffer(float r, float g, float b, float a);
void draw_g_buffer(void);

#endif
