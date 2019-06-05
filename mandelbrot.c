#include "point.h"

void mandelbrot (float, float, float, float);

/* draw the mandelbrot set */
void draw_mdb (void)
{
    mandelbrot(-2, 1.5, 3, 3);
}

void mandelbrot( float a_corner, float b_corner, float x_side, float y_side )
{
    float a, b;
    float x, y, next_x, next_y;
    float shade;
    float x_inc = x_side / 800;
    float y_inc = y_side / 800;
    int count, j, k;
    
    a = a_corner;
    for( j = -400; j < 400; j++ )
    {
        b = b_corner;
        for( k = -400; k < 400; k++ )
        {
            x = 0.0;
            y = 0.0;
            count = 0;
            
            while(count < 50 && (x * x + y * y) < 4)
            {
                next_x = x * x - y * y + a;
                next_y = 2 * x * y + b;
                
                x = next_x;
                y = next_y;
                
                count = count + 1;
            }
            
            shade = count/50.0;
            glColor4f(shade, shade, shade, 1.0);
            draw_point(j+0.5, k+0.5);
            
            b -= y_inc;
        }
        a +=  x_inc;
    }
}
