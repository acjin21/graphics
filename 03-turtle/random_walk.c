

#include "turtle.h"
#define NUM_WALKS 100
/*************************************************************************/
/* extern                                                               */
/*************************************************************************/
extern int window_size;
extern float curr_x;
extern float curr_y;
extern float heading;

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
struct step {
    float pos[2];
    float heading;
    float theta;
    float dist;
    float color[4];
};

/*************************************************************************/
/* globals                                                               */
/*************************************************************************/
struct step steps_hist[NUM_WALKS];

/* generates a random walk of 100 steps */
void do_random_walk (void)
{
//    printf("generating random walk\n");
    for(int i = 0; i < NUM_WALKS; i++)
    {
        float rand_theta = random_float(-360, 360);
        float rand_steps = random_float(20, 150);
        
        float rand_r = random_float(0, 1);
        float rand_g = random_float(0, 1);
        float rand_b = random_float(0, 1);
        
        struct step s =
        {
            .pos = {curr_x, curr_y},
            .heading = heading,
            .theta = rand_theta,
            .dist = rand_steps,
            .color = {rand_r, rand_g, rand_b, 1.0}
        };
        steps_hist[i] = s;
//        printf("step %i: theta = %f; steps = %f\n", i, rand_theta, rand_steps);


        
    }
}

/* draw all random_walk iterations up to and including steps_hist[idx] */
void draw_random_walk (int start_idx, int end_idx)
{
    set_xy(steps_hist[start_idx].pos[0],
           steps_hist[start_idx].pos[1]);
    set_heading(steps_hist[start_idx].heading);
//    home();

    
    for(int i = start_idx; i <= end_idx; i++)
    {
//        printf("step %i: pos = (%f, %f); heading = %f\n", i, curr_x, curr_y, heading);
//        i %= 10;
        glColor4f(steps_hist[i].color[0],
                  steps_hist[i].color[1],
                  steps_hist[i].color[2],
                  steps_hist[i].color[3]);
        right(steps_hist[i].theta);
        forward(steps_hist[i].dist);

        
        steps_hist[i+1].pos[0] = curr_x;
        steps_hist[i+1].pos[1] = curr_y;
        steps_hist[i+1].heading = heading;
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
