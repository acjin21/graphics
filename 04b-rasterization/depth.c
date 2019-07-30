#include "depth.h"

#include <stdio.h>
#include "macros.h"
#include "window.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
float depth_buffer[MAX_WIN_H][MAX_WIN_W];

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
/* clear depth buffer by setting each pixel's depth to value */
void clear_depth_buffer (float value)
{
    for(int row = 0; row < window_height; row++)
    {
        for(int col = 0; col < window_width; col++)
        {
            depth_buffer[row][col] = value;
        }
    }
}

/* draw depth buffer to screen using OpenGL */
void draw_depth_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = 0; y < window_height; y++)
    {
        for(int x = 0; x < window_width; x++)
        {
            glColor4f(depth_buffer[y][x], depth_buffer[y][x],
                      depth_buffer[y][x], 1);
            glVertex2f(x - window_width / 2, y - window_height / 2);
        }
    }
    glEnd();
}
