#include "g_buffer.h"
#include "color.h"
#include "vector.h"
#include "window.h"

/* global vars */
POINT g_buffer[DEFAULT_WIN_H][DEFAULT_WIN_W];
int mode_deferred_render = 0;

/* functions */

/* clear every point in g buffer to r, g, b, a; and reset rendered_flag */
void clear_g_buffer(float r, float g, float b, float a)
{
    for(int r = 0; r < window_height; r++)
    {
        for(int c = 0; c < window_width; c++)
        {
            set_vec4(g_buffer[r][c].color, r, g, b, a);
            g_buffer[r][c].rendered_flag = 0;
        }
    }
}

/* for each px in g buffer, if wasn't rendered to, copy g buffer color to
    color buffer. else, perform lighting + other calculations */
void draw_g_buffer(void)
{
    int save_deferred = mode_deferred_render;
    mode_deferred_render = 0;

    for(int r = 0; r < window_height; r++)
    {
        for(int c = 0; c < window_width; c++)
        {
            if(g_buffer[r][c].rendered_flag == 0)
            {
                cpy_vec4(color_buffer[r][c], g_buffer[r][c].color);
            }
            else
            {
                draw_point(&g_buffer[r][c]);
            }
        }
    }
    mode_deferred_render = save_deferred;

}

