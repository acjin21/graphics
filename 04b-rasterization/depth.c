#include "depth.h"

#include <stdio.h>
#include "macros.h"

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
/* clear depth buffer by setting each pixel's depth to value */
void clear_depth_buffer (float value)
{
    for(int row = 0; row < WIN_H; row++)
    {
        for(int col = 0; col < WIN_W; col++)
        {
            depth_buffer[row][col] = value;
        }
    }
}

/* draw depth buffer using OpenGL */
void draw_depth_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = 0; y < WIN_H; y++)
    {
        for(int x = 0; x < WIN_H; x++)
        {
            glColor4f(depth_buffer[y][x], depth_buffer[y][x],
                      depth_buffer[y][x], 1);
            glVertex2f(x - WIN_W / 2, y - WIN_H / 2);
        }
    }
    glEnd();
}
