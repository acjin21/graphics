#include "line.h"
#include "vector.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

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

void draw_point_2 (POINT *p)
{
    glBegin(GL_POINTS);
        glVertex2f( p->position[X], p->position[Y]);
    glEnd();
}

/*
 * set position of point *p to (x, y, z, w)
 */
void set_position (POINT *p, float x, float y, float z, float w)
{
    p->position[X] = x;
    p->position[Y] = y;
    p->position[Z] = z;
    p->position[W] = w;
}

/*
 * set color of point *p to (r, g, b, a)
 */
void set_color (POINT *p, float r, float g, float b, float a)
{
    p->color[R] = r;
    p->color[G] = g;
    p->color[B] = b;
    p->color[A] = a;
}
/*************************************************************************/
/* draw_line                                                             */
/*************************************************************************/




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
//    draw_random_line();
//    printf("line drawing\n");
    
    if (draw_prog == 'r' )
    {
        draw_random_line();
    }
    else if (draw_prog == 'g')
    {
        draw_coord_grid();
    }
    else if (draw_prog == 'f')
    {
        draw_fan();
    }
    
    POINT start = {
        {100, 200, 0, 0},
        {1, 0, 0, 1}
    };
    
    POINT end = {
        {-100, -200, 0, 0},
        {0, 0, 1, 1}
    };
    draw_line_3(&start, &end);
    
    POINT start2 = {
        {-300, -100, 0, 0},
        {1, 0, 0, 1}
    };
    
    POINT end2 = {
        {300, 100, 0, 0},
        {0, 0, 1, 1}
    };
    draw_line_3(&start2, &end2);
    
    
    
//    draw_line(-100, -300, 100, 300, 1, 0, 0, 0, 0, 1);
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
        case 'r':
            draw_prog = 'r';
            draw_one_frame = 1;
            glutPostRedisplay();
            break;
        case 'f':
            draw_prog = 'f';
            draw_one_frame = 1;
            glutPostRedisplay();
            break;
        case 'g':
            draw_prog = 'g';
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
    glPointSize(4.0);
    glColor4f(1.0,0.0,0.0,1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}
