/*
 *
 * point.c - simple GLUT app that draws one frame with a single point at origin
 *
 * To build:  gcc -framework OpenGL -framework GLUT line.c -o line
 *
 */
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/
/*
 * random_float()
 */
float random_float( int low, int high )
{
    return( (float)(low + (rand()/(float)RAND_MAX)*(high - low)) );
}

/*
 * draw_point()
 */
void draw_point( float x, float y )
{
    /*
     * draw points
     */
    glBegin(GL_POINTS);
        glVertex2f( x, y );
    glEnd();

}

/* draw_horizontal()
 */
void draw_horizontal(void)
{
    int y = 0;
    for(int x = 0; x < window_size; x++)
    {
        draw_point(x, y);
    }
}

void draw_vertical(void)
{
    int x = 0;
    for(int y = 0; y < window_size; y++)
    {
        draw_point(x, y);
    }
}

void draw_diagonal(void)
{
    int y;
    for(int x = 0; x < window_size; x++)
    {
        y = x;
        draw_point(-x, -y);
    }
}

#define SWAP(a, b)  \
{                   \
    float tmp = a;  \
    a = b;          \
    b = tmp;        \
}

void draw_line (float start_x, float start_y, float end_x, float end_y,
                float start_r, float start_g, float start_b,
                float end_r, float end_g, float end_b)
{
    float dy = end_y - start_y;
    float dx = end_x - start_x;
    
    if( end_r > 1.0 || start_r > 1.0 || end_g > 1.0 || start_g < 0.0 ||
        end_b > 1.0 || start_b < 0.0 )
    {
        printf("Please enter in valid rgb values in range [0.0, 1.0]\n");
        return;
    }
    
    int x_major = fabsf(dx) > fabsf(dy);
    
    if(dx < 0 && x_major || dy < 0 && !x_major )
    {
        SWAP(start_x, end_x);
        SWAP(start_y, end_y);
        SWAP(start_r, end_r);
        SWAP(start_g, end_g);
        SWAP(start_b, end_b);
        
        /* recalculate deltas */
        dy = end_y - start_y;
        dx = end_x - start_x;
    }
    
    float dr = end_r - start_r;
    float dg = end_g - start_g;
    float db = end_b - start_b;

    float step;
    float x = start_x;
    float y = start_y;
    glColor4f(start_r, start_g, start_b, 1.0);
    float scale;
    
    /* degenerate line = point */
    if(dx == 0 && dy == 0)
    {
        draw_point(x, y);
    }
    /* horizontal line */
    else if(dy == 0)
    {
        for(; x < end_x; x++)
        {
            scale = (x - start_x)/dx;
            glColor4f(start_r + dr * scale, start_g + dg * scale,
                      start_b + db * scale, 1.0);
            draw_point(x, y);
        }
    }
    /* vertical line */
    else if(dx == 0) // vertical
    {
        for(; y < end_y; y++)
        {
            scale = (y - start_y)/dy;
            glColor4f(start_r + dr * scale, start_g + dg * scale,
                      start_b + db * scale, 1.0);
            draw_point(x, y);
        }
    }
    /* x-major diagonal line, i.e. 0 < |slope| < 1 */
    else if(x_major)
    {
        printf("xmajor\n");
        step = dy / dx;
        float y = start_y;
        for(float x = start_x; x < end_x; x++)
        {
            scale = (x - start_x)/dx;
            glColor4f(start_r + dr * scale,
                      start_g + dg * scale,
                      start_b + db * scale, 1.0);
            draw_point(x, y);
            y += step;
        }
    }
    /* y-major diagonal line, i.e. |slope| > 1 */
    else if (!x_major) // y major
    {
        printf("ymajor\n");
        step = dx / dy;
        float x = start_x;
        for(float y = start_y; y < end_y; y++)
        {
            scale = (y - start_y)/dy;
            glColor4f(start_r + dr * scale,
                      start_g + dg * scale,
                      start_b + db * scale, 1.0);
            draw_point(x, y);
            x += step;
        }
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

    glPointSize(random_float(4, 10));
    draw_line(start_x, start_y, end_x, end_y,
              start_r, start_g, start_b, end_r, end_g, end_b);
}

/* draw coordinate grid filling the window */
void draw_coord_grid(void)
{
    /* set white background */
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT );

    glPointSize(1);
    float x, y;
    for(int i = 0; i < 2 * window_size; i+=20)
    {
        x = i - window_size;
        y = i - window_size;
        if(i == window_size) //draw red axes
        {
            glPointSize(2);
            draw_line(-400, y, 399, y, 1, 0, 0, 1, 0, 0);
            draw_line(x, -400, x, 399, 1, 0, 0, 1, 0, 0);
            glPointSize(1);
        }
        else
        {
            draw_line(-400, y, 399, y, 0, 0, 0, 0, 0, 0);
            draw_line(x, -400, x, 399, 0, 0, 0, 0, 0, 0);
        }
    }
}
/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/

/*
 * display routine
 */
void display(void)
{
    if( Mojave_WorkAround )
    {
        glutReshapeWindow(2 * window_size,2 * window_size);//Necessary for Mojave. Has to be different dimensions than in glutInitWindowSize();
        Mojave_WorkAround = 0;
    }

    if( draw_one_frame == 0 )
        return;
	
    /*
     * clear color buffer
     */
    glClear(GL_COLOR_BUFFER_BIT );

    /*
     * draw points
     */
//    draw_coord_grid();
//    draw_random_line();
    draw_line(100, 200, -100, -200, 1, 0, 0, 0, 0, 1);
    draw_line(-100, -300, 100, 300, 1, 0, 0, 0, 0, 1);

//    draw_line(50, -1, -1, -200, 1, 0, 0, 0, 0, 1);
    
    
    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay();//Necessary for Mojave.

    draw_one_frame = 0;
}

/*
 * Key routine
 */
static void Key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':       draw_one_frame = 1;     glutPostRedisplay();    break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
    }
}

/*
 * main function
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    srand(time(NULL));

    /*
     * create window
     */
    glutInitWindowSize(window_size,window_size);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("My OpenGL Window");

    /*
     * setup display(), Key() funtions
     */
    glutDisplayFunc(display);
    glutKeyboardFunc(Key);

    /*
     * setup OpenGL state
     */
    glClearColor(0.0,0.0,0.0,1.0);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    glPointSize(4.0);
    glColor4f(1.0,0.0,0.0,1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}
