/*
 *
 * turtle.c - simple GLUT app that provides turtle graphics
 *
 * To build:  'make build' within the current directory.
 *
 */
#include "turtle.h"

#include <time.h>
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

//typedef Turtle = struct {
//    float curr_x;
//    float curr_y;
//    float heading;
//}

float curr_x = 0;
float curr_y = 0;
float heading = 90;

int counter = 0;
time_t clk;
float start;


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

/*************************************************************************/
/* turtle functions                                                     */
/*************************************************************************/
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

void set_heading(float theta)
{
    heading = theta;
}

/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
/*
 * display routine
 */
void display(void)
{
    /*
     * Necessary for Mojave.
     *  Input to glutReshapeWindow has to be different dimensions
     *  than in glutInitWindowSize();
    */
    if( Mojave_WorkAround )
    {
        glutReshapeWindow(2 * window_size,2 * window_size);
        Mojave_WorkAround = 0;
        
    }

    if( draw_one_frame == 0 ) return;

    /*
     * draw points
     */
    home();
    
    time_t seconds = time(NULL);
    double delta_s = difftime(seconds, clk);

    printf("%.3f seconds\n", (double) delta_s);
    if(delta_s == 1)

    {
    
        float tm = (float)clock()/(float)CLOCKS_PER_SEC;
//        printf("%.3f seconds\n", tm - start);

//        glColor4f(random_float(0, 1), random_float(0, 1), random_float(0, 1), 1.0);
        counter++;
    }
    /* draw the 'current' frame */
    glClear(GL_COLOR_BUFFER_BIT );
    draw_random_walk(counter);
    clk = seconds;
    /* done drawing */
    if(counter > 100)
    {
        printf("counter: %i\n", counter);
        draw_one_frame = 0;
    }
    
//    set_xy(-window_size/2.0, 0);
//    right(90);
//    draw_wh_rec(50);
    
//    set_xy(-300, 0);
//    draw_witch_hat_rec(100);

    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.

//    draw_one_frame = 0;
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

void my_glInit(void)
{
    
}
/*
 * main function
 */
int main(int argc, char **argv)
{
    clock_t stop;
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
     * generate random walk data once before main rendering loop
     */
    do_random_walk();

    /*
     * start loop that calls display() and Key() routines
     */
    clk = time(NULL);
    start = (float) clock() / (float) CLOCKS_PER_SEC;
    glutMainLoop();


    return 0;
}
