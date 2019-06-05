#include "point.h"

/* draw a sine curve */
void draw_sine (void)
{
    glColor4f(1.0, 0.0, 0.0, 1.0);
    
    for(int x = 0; x < 400; x++)
    {
        float temp = (x / 400.0) * 2 * PI;
        float y = sin(temp);
        draw_point(x, y * 100);
    }
}

void draw_cosine (void)
{
    glColor4f(1.0, 0.0, 0.0, 1.0);
    
    for(int x = 0; x < 400; x++)
    {
        float temp = (x / 400.0) * 2 * PI;
        float y = cos(temp);
        draw_point(x, y * 100);
    }
}
