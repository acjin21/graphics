#include "buffers.h"
#include "window.h"
#include "vector.h"
#include "app.h"

float color_buffer[MAX_WIN_H][MAX_WIN_W][4];
float depth_buffer[MAX_WIN_H][MAX_WIN_W];
int stencil_buffer[MAX_WIN_H][MAX_WIN_W];
POINT g_buffer[MAX_WIN_H][MAX_WIN_W];
/****************************************************************************/
/* clear functions */
/****************************************************************************/
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

/* clear stencil buffer to -1 (invalid objectID) */
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

/****************************************************************************/
/* draw functions */
/****************************************************************************/
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

/* for each px in g buffer, if wasn't rendered to, copy g buffer color to
 color buffer. else, perform lighting + other calculations */
void draw_g_buffer(void)
{
    int save_deferred = current_rs.render_mode;
    current_rs.render_mode = 0;
    
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
    current_rs.render_mode = save_deferred;
    
}

/* draw stencil buffer in grayscale */
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
