#include "color.h"
#include "macros.h"

void clear_color_buffer (float r, float g, float b, float a)
{
    for(int row = 0; row < WIN_H; row++)
    {
        for(int col = 0; col < WIN_W; col++)
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
    for(int y = - WIN_H / 2; y < WIN_H / 2; y++)
    {
        for(int x = - WIN_W / 2; x < WIN_W / 2; x++)
        {
            int r = (int) (y + WIN_H / 2);
            int c = (int) (x + WIN_W / 2);
            glColor4f(color_buffer[r][c][R], color_buffer[r][c][G], color_buffer[r][c][B], color_buffer[r][c][A]);
            glVertex2f(x, y);
        }
    }
    glEnd();
}
