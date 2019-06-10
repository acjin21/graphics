#include "line.h"

/* draw a horizontal line from origin to right side of screen. */
void draw_horizontal(void)
{
    int y = 0;
    for(int x = 0; x < window_size; x++)
    {
        draw_point(x, y);
    }
}

/* draw a vertical line from origin to top of screen. */
void draw_vertical(void)
{
    int x = 0;
    for(int y = 0; y < window_size; y++)
    {
        draw_point(x, y);
    }
}

/* draw a diagonal line from origin to top right corner of screen */
void draw_diagonal(void)
{
    int y;
    for(int x = 0; x < window_size; x++)
    {
        y = x;
        draw_point(x, y);
    }
}

/* draw a fan of lines */
void draw_fan (void)
{
    glPointSize(1.0);
    float r = window_size;
    for(float i = 0; i < 800; i++)
    {
        float theta = (i/800.0) * PI;
        float end_x = r * cos(theta);
        float end_y = r * sin(theta);
        float rand_r = random_float(0, 1);
        float rand_g = random_float(0, 1);
        float rand_b = random_float(0, 1);
        
        POINT p1 =
        {
            {0,0,0,0},
            {rand_r, rand_g, rand_b}
        };
        
        POINT p2 =
        {
            {end_x, end_y, 0, 0},
            {rand_r, rand_g, rand_b}
            
        };
        draw_line_2(&p1, &p2);
        //        draw_line(0, 0, end_x, end_y, rand_r, rand_g, rand_b, rand_r, rand_g, rand_b);
    }
}

/* draw random line with random color and random size */
void draw_random_line (void)
{
    float start_x = random_float(-400, 400);
    float start_y = random_float(-400, 400);
    float end_x = random_float(-400, 400);
    float end_y = random_float(-400, 400);
    
    float start_r = random_float(0, 1);
    float start_g = random_float(0, 1);
    float start_b = random_float(0, 1);
    float end_r = random_float(0, 1);
    float end_g = random_float(0, 1);
    float end_b = random_float(0, 1);
    
    POINT p1 =
    {
        {start_x, start_y, 0, 0},
        {start_r, start_g, start_b, 1}
        
    };
    
    POINT p2 =
    {
        {end_x, end_y, 0, 0},
        {end_r, end_g, end_b, 1}
    };
    
    glPointSize(random_float(4, 10));
    draw_line_2(&p1, &p2);
}

/* draw coordinate grid filling the window */
void draw_coord_grid(void)
{
    /* set white background */
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT );
    
    glPointSize(2);
    float x, y;
    for(int i = 0; i < 2 * window_size; i+=20)
    {
        x = i - window_size;
        y = i - window_size;
        POINT p1 =
        {
            {0, 0, 0, 0},
            {0, 0, 0, 1}
        };
        
        POINT p2 =
        {
            {0, 0, 0, 0},
            {0, 0, 0, 1}
        };
        if(i == window_size) //draw red axes in middle of screen
        {
            set_color(&p1, 1, 0, 0, 1);
            set_color(&p2, 1, 0, 0, 1);
        }
        else //all other lines are black
        {
            set_color(&p1, 0, 0, 0, 1);
            set_color(&p2, 0, 0, 0, 1);
        }

        set_position(&p1, -400, y, 0, 0);
        set_position(&p2, 399, y, 0, 0);
        draw_line_2(&p1, &p2);
        
        set_position(&p1, x, -400, 0, 0);
        set_position(&p2, x, 399, 0, 0);
        draw_line_2(&p1, &p2);
    }
}
