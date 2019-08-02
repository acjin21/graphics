
#include "point.h"
void draw_grid (void)
{
    glPointSize(2.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    for(int i = -window_size; i < window_size; i++)
    {
        draw_point(i, 0);
        draw_point(0, i);
    }
    glPointSize(2.0);

}
void draw_quadratic (void)
{
    draw_grid();
    glColor4f(1.0, 0.0, 0.0, 1.0);
    int max_x = window_size;
    int min_x = 0;

    int end_x = window_size;
    int start_x = -window_size;
    int x_range = end_x - start_x;

    for(int i = min_x; i < max_x; i++)
    {
//        int sx = (int)(window_size * i / max_x);
        
        float y = pow(i, 2);
        int sy = (int) ((y / max_x));
        draw_point(i, sy);
    }
}

void draw_pow2 (void)
{
    draw_grid();
    glColor4f(1.0, 0.0, 0.0, 1.0);
    //    int max_x = round(pow(window_size, 0.5));
    for(int i = -window_size; i < window_size; i++)
    {
        draw_point(i, pow(2, i));
    }
}
