#include "stencil_buffer.h"

int stencil_buffer[MAX_WIN_H][MAX_WIN_W];

void clear_stencil_buffer(void)
{
    for(int r = 0; r < window_height; r++)
    {
        for(int c = 0; c < window_width; c++)
        {
            stencil_buffer[r][c] = -1;
        }
    }
}


void draw_stencil_buffer(void)
{
    glBegin(GL_POINTS);
    for(int y = 0; y < window_height; y++)
    {
        for(int x = 0; x < window_width; x++)
        {
            glColor4f(0.5 + stencil_buffer[y][x] / 3.0,
                      0.5 + stencil_buffer[y][x] / 3.0,
                      0.5 + stencil_buffer[y][x] / 3.0, 1);
            glVertex2f(x - window_width / 2, y - window_height / 2);
        }
    }
    glEnd();
}
