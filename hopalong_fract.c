#include "point.h"

/* draw a hopalong fractal */
#define LIMIT   21000

double sgn(double x)
{
    if (x == 0.0) return x;
    if (x < 0.0)  return -1.0;
    return 1.0 ;
}
void draw_hop_fract (void)
{
    double x, y;
    double a = random_float(-20,20);
    double b = (random_float(-1000,1000))/1000.0;
    double c = (random_float(-1000,1000))/1000.0;
    double scale = 10.0;
    double nx, ny;
    int i;
    
    x = y = 0.0 ;
    
    for (i=0; i<LIMIT; i++)
    {
        if( i % 1000 == 0)
        {
            //            printf("new color\n");
            float r = random_float(0, 1);
            float g = random_float(0, 1);
            float b = random_float(0, 1);
            glColor4f(r, g, b, 1.0);
        }
        //        printf("%lf %lf\n", x, y) ;
        nx = y - SIGN(x) * ABS( b * x - c );
        ny = a - x ;
        x = nx ;
        y = ny ;
        draw_point(scale * x, scale * y);
    }
    
}
