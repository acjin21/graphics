#include "color.h"
#include "macros.h"

extern int window_size;

void clear_color_buffer (float r, float g, float b, float a)
{
    for(int row = 0; row < 800; row++)
    {
        for(int col = 0; col < 800; col++)
        {
            color_buffer[row][col][R] = r;
            color_buffer[row][col][G] = g;
            color_buffer[row][col][B] = b;
            color_buffer[row][col][A] = a;
        }
    }
}

void draw_color_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = -window_size; y < window_size; y++)
    {
        for(int x = -window_size; x < window_size; x++)
        {
            int r = (int) (y + 400);
            int c = (int) (x + 400);
            glColor4f(color_buffer[r][c][R], color_buffer[r][c][G],
                      color_buffer[r][c][B], color_buffer[r][c][A]);
            glVertex2f( x, y);
        }
    }
    glEnd();
}
