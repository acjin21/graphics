#include "point.h"

/* draw fuzzy grayscale tv screen */
void draw_tv_screen (void)
{
    for(int i = 0; i < 1000000; i++)
    {
        float rand_x = random_float(-400, 400);
        float rand_y = random_float(-400, 400);
        draw_point( rand_x, rand_y );
        
        float rand_r = random_float(0, 1);
        float rand_g = rand_r;
        float rand_b = rand_r;
        glColor4f(rand_r, rand_g, rand_b, 1.0);
        
    }
}

/* draw grayscale circle with radius r using monte carlo simulation */
void draw_mc_circle (void)
{
    for(int i = 0; i < 1000000; i++)
    {
        float rand_x = random_float(-400, 400);
        float rand_y = random_float(-400, 400);
        float r_squared = pow(rand_x, 2) + pow(rand_y, 2);
        if (r_squared <= 40000 )
        {
            draw_point( rand_x, rand_y );
            
            float rand_r = random_float(0, 1);
            float rand_g = rand_r;
            float rand_b = rand_r;
            glColor4f(rand_r, rand_g, rand_b, 1.0);
        }
    }
}

/* color all the pixels on the screen with random colors,
 hitting each exactly pixel once */
void draw_all_pixels (void)
{
    for(int y = -400; y < 400; y++)
    {
        for(int x = -400; x < 400; x++)
        {
            draw_point(x, y);
            float r = random_float(0, 1);
            float g = random_float(0, 1);
            float b = random_float(0, 1);
            glColor4f(r, g, b, 1.0);
        }
    }
}

/* draw a red color gradient*/
void draw_gradient (void)
{
    for(int y = 0; y < 2 * window_size; y++)
    {
        float end_r = 1;
        //        float end_g = random_float(0, 1);
        //        float end_b = random_float(0, 1);
        for(int x = 0; x < 2 * window_size; x++)
        {
            float scale = x / ((float) 2 * window_size);
            draw_point(x - window_size, y - window_size);
            float r = scale * end_r;
            float g = 0;
            float b = 0;
            glColor4f(r, g, b, 1.0);
            
        }
    }
}
