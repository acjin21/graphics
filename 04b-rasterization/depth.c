#include "depth.h"
#include <stdio.h>
extern int window_size;

void clear_depth_buffer (float value)
{
    for(int row = 0; row < 800; row++)
    {
        for(int col = 0; col < 800; col++)
        {
            depth_buffer[row][col] = value;
        }
    }
}

void draw_depth_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = 0; y < 800; y++)
    {
        for(int x = 0; x < 800; x++)
        {
            int r = (int) (y + 400);
            int c = (int) (x + 400);
            
            
            glColor4f(depth_buffer[y][x], depth_buffer[y][x], depth_buffer[y][x], 1);
            glVertex2f(x - 400, y - 400);
        }
    }
    glEnd();
}
