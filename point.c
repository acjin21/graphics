/*
 *
 * point.c - simple GLUT app that draws one frame with a single point at origin
 *
 * To build:  gcc -framework OpenGL -framework GLUT point.c -o point
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
#define SIGN(_a)        (((_a) < 0.0) ? -1 : (((_a) > 0.0) ? 1 : 0))
#define ABS(_a)            (((_a) < 0.0) ? -(_a) : (_a))
#define SQR(_a)            ((_a) * (_a))
#define CLAMP(_a,_l,_h)        (((_a) <= (_l)) ? (_l) : (((_a) >= (_h)) ? (_h) : (_a)))

#define PI 3.1415926
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

int draw_prog = 0;

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

void draw_gradient (void)
{
    for(int y = 0; y < 2 * window_size; y++)
    {
        float end_r = random_float(0, 1);
        float end_g = random_float(0, 1);
        float end_b = random_float(0, 1);
        for(int x = 0; x < 2 * window_size; x++)
        {
            float scale = x / ((float) 2 * window_size);
            draw_point(x - window_size, y - window_size);
            float r = scale * end_r;
            float g = scale * end_g;
            float b = scale * end_b;
            glColor4f(r, g, b, 1.0);
            
        }
    }

}
 
#define LIMIT   21000
 
double sgn(double x)
{
        if (x == 0.0) return x;
        if (x < 0.0)  return -1.0;
        return 1.0 ;
}
 
void draw_hop_fract (void)
{
    double x, y ;
    // double a = -200, b = .1, c = -80 ;
    // double a = 0.4, b = 1, c = 0 ;
    // double a = -3.14, b = 0.3, c = 0.3 ;
//    double a = -1000., b = random_float(.1, 1), c = -10 ;
    double    a    = random_float(-20,20);
    double    b    = (random_float(-1000,1000))/1000.0;
    double    c    = (random_float(-1000,1000))/1000.0;
    double    scale    = 10.0;
    double nx, ny ;
    int i ;
    
    x = y = 0.0 ;
    
    for (i=0; i<LIMIT; i++)
    {
        if( i % 1000 == 0)
        {
            printf("new color\n");
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

void draw_sine (void)
{
    for(int x = 0; x < 400; x++)
    {
        float temp = (x / 400.0) * 2 * PI;
        float y = sin(temp);
        draw_point(x, y * 100);
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
    
   
    switch (draw_prog)
    {
        case 'm':
            draw_mc_circle();   break;
        case 't':
            draw_tv_screen();   break;
        case 'p':
            draw_all_pixels();  break;
        case 'g':
            draw_gradient();    break;
        case 'h':
            draw_hop_fract();   break;
            

    }

    draw_sine();
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
            
        case 'm':
            draw_prog = 'm';
            draw_one_frame = 1;
            glutPostRedisplay();
            break;
        case 't':
            draw_prog = 't';
            draw_one_frame = 1;
            glutPostRedisplay();
            break;
        
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
    glPointSize(2.0);
    glColor4f(1.0,0.0,0.0,1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}
