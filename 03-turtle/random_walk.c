

#include "turtle.h"

/*************************************************************************/
/* extern                                                               */
/*************************************************************************/
extern int window_size;
extern float curr_x;
extern float curr_y;

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
struct step {
    float theta;
    float dist;
};

/*************************************************************************/
/* globals                                                               */
/*************************************************************************/
struct step steps_hist[100];

/* generates a random walk of 100 steps */
void do_random_walk (void)
{
//    printf("generating random walk\n");
    for(int i = 0; i < 100; i++)
    {
        float rand_theta = random_float(-360, 360);
        float rand_steps = random_float(20, 150);
        struct step s = {rand_theta, rand_steps};
        steps_hist[i] = s;
        
    }
}

/* draw all random_walk iterations up to and including steps_hist[idx] */
void draw_random_walk (int idx)
{
    home();
    for(int i = 0; i <= idx; i++)
    {
        right(steps_hist[i].theta);
        forward(steps_hist[i].dist);
        /*
         * if any step in steps_hist makes the turtle go offscreen,
         *  fix the current step data for the next draw iteration by
         *  setting the dist for that step = 0.
         */
        if(fabsf(curr_x) > window_size || fabsf(curr_y) > window_size)
        {
//            steps_hist[i].theta = 180;
            steps_hist[i].dist = 0;
        }
    }
    
}
