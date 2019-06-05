
#include "point.h"

/*
 * Basic Lorenz Attractor code
 */
void draw_lorenz (void)
{
    double x = 0.1;
    double y = 0;
    double z = 0;
    double a = 10.0;
    double b = 28.0;
    double c = 8.0 / 3.0;
    double t = 0.01;
    int lorenzIterationCount = 10000;
    int i;

    // Iterate and update x, y and z locations
    //  based upon the Lorenz equations
    for ( i = 0; i < lorenzIterationCount; i++ ){
        double xt = x + t * a * (y - x);
        double yt = y + t * (x * (b - z) - y);
        double zt = z + t * (x * y - c * z);
        x = xt;
        y = yt;
        z = zt;
        draw_point(10* x,10* y);
    }
}
