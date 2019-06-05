/*
 *
 * point.c - simple GLUT app that draws one frame with a single point at origin
 *
 * To build:  gcc -framework OpenGL -framework GLUT point.c -o point
 *
 */


#include "point.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
static int draw_one_frame = 1;
static int draw_prog = 0;

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

/* set draw_prog to key; set draw_one_frame; and call GLUT redisplay func */
void update_draw_prog (int key)
{
    draw_prog = key;
    draw_one_frame = 1;
    glutPostRedisplay();
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
        case 'c':
            draw_mc_circle();   break;
        case 't':
            draw_tv_screen();   break;
        case 'p':
            draw_all_pixels();  break;
        case 'g':
            draw_gradient();    break;
        case 'h':
            draw_hop_fract();   break;
        case 's':
            draw_sine();        break;
        case 'm':
            draw_mdb();         break;
        case 'k':
            draw_sierp();       break;
        case 'b':
            draw_bifurcate();   break;
        case 'l':
            draw_lorenz();      break;
        case 'a':
            draw_cosine();      break;
        case 'u':
            draw_quadratic();      break;
        case 'w':
            draw_pow2();      break;
            
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
    // keys for various drawing programs
    if (key == 'c' || key == 't' || key == 'p' || key == 'g' || key == 'h' ||
        key == 's' || key == 'm' || key == 'k' || key == 'b' || key == 'l' ||
        key == 'a' || key == 'u' ||  key == 'w' )
    {
        update_draw_prog(key);
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
