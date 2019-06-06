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
#define PI 3.1415926
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

float curr_x = 0;
float curr_y = 0;
float heading = 90;

/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/
/*
 * random_float()
 */
float random_float( float low, float high )
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

/*
 * draw_point()
 */
void draw_line( float startx, float starty, float endx, float endy )
{
    /*
     * draw points
     */
    glBegin(GL_LINES);
        glVertex2f( startx, starty );
        glVertex2f( endx, endy );
    glEnd();
    
}

/*
 * return turtle to origin (0, 0) with heading 90 deg.
 */
void home()
{
    curr_x = 0;
    curr_y = 0;
    heading = 90;
}

/*
 * rotate the turtle towards the right, 'angle' degrees
 */
void right(float angle)
{
    heading -= angle;
    if(heading > 360) {
        heading -= 360;
    }
    else if(heading < 0) {
        heading += 360;
    }
//    printf("angle: %f\n", heading);
}

/*
 * rotate the turtle towards the left, 'angle' degrees
 */
void left(float angle)
{
    right(-1 * angle);
}

/*
 * move the turtle in direction of heading, 'length' steps
 */
void forward(float length)
{
    float dx = length * cos(PI * heading/180.0);
    float dy = length * sin(PI * heading/180.0);
    draw_line(curr_x, curr_y, curr_x + dx, curr_y + dy);
    curr_x += dx;
    curr_y += dy;
}

/*
 * move the turtle in opposite direction of heading, 'length' steps
 */
void backward(float length)
{
    forward(length * -1);
}

/* change turtle's current coordinates to (x, y). */
void set_xy(float x, float y)
{
    curr_x = x;
    curr_y = y;
}

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
//    printf("C = %f\ntheta = %f\n", circumference, theta);
//    int counter = 0;
    float d_step = 1.0/(num_spirals * circumference);
    for(float step = 1; step >= 0; step -= d_step)
    {
        forward(step);
        left(theta);
//        counter++;
//        if(heading < 90.3 && heading > 89.7)
//        {
//            printf("counter: %i\n step: %f\n", counter, step);
//            counter = 0;
//        }
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
    float d_x = len / -2.0;
    set_xy(d_x, 0);
    left(d_theta);
    for(int i = 0; i < n; i++)
    {
        forward(len);
        right(extern_angle);
    }
    right(d_theta);

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
    home();
//    draw_square(100);
//    draw_pinwheel();
//    draw_square_rec(100);
//    draw_circle(100);
//    draw_spiral(100, 10);
//    draw_star(100);
//    draw_V(100);
//    draw_witch_hat(100);
//    draw_equil_tri(100);
//    draw_pent(100);
    for(int i = 6; i < 10; i++) {
        draw_n_gon(i, 100);
    }
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
    glPointSize(2.0);
    glColor4f(1.0,0.0,0.0,1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}
