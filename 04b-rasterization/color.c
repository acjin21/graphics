#include "color.h"
#include "macros.h"
#include "window.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
float color_buffer[DEFAULT_WIN_H][DEFAULT_WIN_W][4];

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
/* clear color buffer with clear color (r, g, b, a) */
void clear_color_buffer (float r, float g, float b, float a)
{
    for(int row = 0; row < window_height; row++)
    {
        for(int col = 0; col < window_width; col++)
        {
            color_buffer[row][col][R] = r;
            color_buffer[row][col][G] = g;
            color_buffer[row][col][B] = b;
            color_buffer[row][col][A] = a;
        }
    }
}

/* draw color buffer to screen using OpenGL */
void draw_color_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = - window_height / 2; y < window_height / 2; y++)
    {
        for(int x = - window_width / 2; x < window_width / 2; x++)
        {
            int r = (int) (y + window_height / 2);
            int c = (int) (x + window_width / 2);
            glColor4f(color_buffer[r][c][R],
                      color_buffer[r][c][G],
                      color_buffer[r][c][B],
                      color_buffer[r][c][A]);
            glVertex2f(x, y);
        }
    }
    glEnd();
}
