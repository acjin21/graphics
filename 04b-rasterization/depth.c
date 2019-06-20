#include "depth.h"

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
    for(int y = -window_size; y < window_size; y++)
    {
        for(int x = -window_size; x < window_size; x++)
        {
            int r = (int) (y + 400);
            int c = (int) (x + 400);
            glColor4f(depth_buffer[r][c], depth_buffer[r][c], depth_buffer[r][c], 1);
            glVertex2f(x, y);
        }
    }
    glEnd();
}
