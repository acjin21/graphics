
#include "turtle.h"

/*************************************************************************/
/* extern                                                               */
/*************************************************************************/
extern int window_size;
extern float curr_x;
extern float curr_y;
extern float heading;

/*
 * draw a square with side length len.
 */
void draw_square(float len)
{
    for(int i = 0; i < 4; i++)
    {
        forward(len);
        right(90);
    }
}

/* draw a pinwheel of squares length 100 */
void draw_pinwheel(void)
{
    for(int i = 0; i < 360; i+= 10)
    {
        draw_square(100);
        right(10);
    }
}

/* draw a recursive square pattern */
void draw_square_rec (float len)
{
    if (len < 10) return;
    for(int i = 0; i < 360; i+= 10)
    {
        forward(len);
        draw_square_rec(len / 2.0);
        right(90);
    }
}

/* draw a circle with the specified radius */
void draw_circle (float radius)
{
    float circumference = 2 * PI * radius;
    float theta = 360.0 / circumference;
    set_xy(radius, 0);
    for(float i = 0; i < circumference; i++)
    {
        forward(1);
        left(theta);
    }
}

/* draw a spiral with the specified radius */
void draw_spiral (float radius, int num_spirals)
{
    float circumference = 2 * PI * radius;
    float theta = 360.0 / circumference;
    set_xy(radius, 0);
    float d_step = 1.0/(num_spirals * circumference);
    for(float step = 1; step >= 0; step -= d_step)
    {
        forward(step);
        left(theta);
    }
}

/* draw a star with length 'len' */
void draw_star (float len)
{
    for(int i = 0; i < 5; i++)
    {
        forward(len);
        right(144);
    }
}

/* draw a V with 60 deg angle between two sides of length 'len' */
void draw_V (float len)
{
    float theta = 30;
    left(theta);
    forward(len);
    backward(len);
    right(2 * theta);
    forward(len);
    backward(len);
}

void draw_V_rec (float len)
{
    if(len < 1.0) return;
    float theta = 30;
    // heading = 90
    left(theta);
    forward(len);
    right(theta);
    //now at top left vertex, heading = 90.

    draw_V_rec (len / 2.0);
    
    //heading = 90 + theta (pointing to upper left)
    left(theta);
    backward(len);
    right(2 * theta);
    //at vertex, pointing upper left
    forward(len);
    // @ vertex, pointing upper right
    left(theta);
     // @ top right vertex, heading = 90
    draw_V_rec (len / 2.0);
    // @ top right vertex, pointing upper right
    right(theta);
    backward(len);
    left(theta);
}

/* draw witch hat centered (horizontally) at the origin, with edges length 'len' */
void draw_witch_hat (float len)
{
    float dx = -1.5 * len;
    set_xy(dx, 0);
    //    printf("(%f, %f)\n", curr_x, curr_y);
    right(90);
    forward(len);
    left(60);
    forward(len);
    right(120);
    forward(len);
    left(60);
    forward(len);
}

void draw_witch_hat_rec (float len)
{
    if(len < 1) return;
    set_heading(90);
    right(90);
    forward(len);
    left(60);
    forward(len);
//    left(30);
    draw_witch_hat_rec (len / 2);
    right(150);
    forward(len);
    left(60);
    forward(len);
    set_heading(90);
}

void draw_wh_rec (float len)
{
    if(len < 2) {
        forward(len);
        return;
    }
    draw_wh_rec(len/2.0);
    left(60);
    draw_wh_rec(len/2.0);
    right(120);
    draw_wh_rec(len/2.0);
    left(60);
    draw_wh_rec(len/2.0);
}


/* draw triangle with side length 'len' */
void draw_equil_tri (float len)
{
    right(30);
    for(int i = 0; i < 3; i++)
    {
        forward(len);
        right(120);
    }
    left(30);//heading back to 90
    
}
/* draw pentagon with side length 'len' */
void draw_pent (float len)
{
    float d_theta = 108 - heading;
    left(d_theta); //should be 108 - 90
    for(int i = 0; i < 5; i++)
    {
        forward(len);
        right(72);
    }
    right(d_theta);
}

/* draw n-sided polygon with side length 'len' */
void draw_n_gon (int n, float len)
{
    float extern_angle = 360.0 / (float) n;
    float intern_angle = 180 - extern_angle;
    
    float d_theta = intern_angle - heading;
    left(d_theta);
    for(int i = 0; i < n; i++)
    {
        forward(len);
        right(extern_angle);
    }
    right(d_theta);
    
}

/* draw a random shape with random start (x, y), random heading, random length, random num sides, and random color */
void draw_random_shape (void)
{
    float start_x = random_float(-window_size/2.0, 0);
    float start_y = random_float(-window_size/2.0, 0);
    float rand_heading = random_float(0, 360);
    
    set_xy(start_x, start_y);
    heading = rand_heading;
    
    float rand_len = random_float(50, 150);
    int rand_n = (int) random_float(3,21);
    
    float rand_r = random_float(0, 1);
    float rand_g = random_float(0, 1);
    float rand_b = random_float(0, 1);
    
    glColor4f(rand_r, rand_g, rand_b, 1.0);
    draw_n_gon(rand_n, rand_len);
}
