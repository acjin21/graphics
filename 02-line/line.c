#include "line.h"
#include "vector.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

#define SWAP(p1, p2)  \
{                   \
    float tmp = p1->position[X];  \
    p1->position[X] = p2->position[X];          \
    p2->position[X] = tmp;        \
    tmp = p1->position[Y];  \
    p1->position[Y] = p2->position[Y];          \
    p2->position[Y] = tmp;        \
    tmp = p1->color[R];  \
    p1->color[R] = p2->color[R];          \
    p2->color[R] = tmp;        \
    tmp = p1->color[G];  \
    p1->color[G] = p2->color[G];          \
    p2->color[G] = tmp;        \
    tmp = p1->color[B];  \
    p1->color[B] = p2->color[B];          \
    p2->color[B] = tmp;        \
}

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

/*
 * draw arbitrary slope line given the two endpoints *p1 and *p2
 *  with coordinate positions and rgba values
 */
void draw_line_3 (POINT *start, POINT *end)
{
    // overall change in position and in rgb values
    POINT delta;
    vector_subtract(end->position, start->position, delta.position);
    
    if( end->color[R] > 1.0 || start->color[R] < 0.0 ||
        end->color[G] > 1.0 || start->color[G] < 0.0 ||
        end->color[B] > 1.0 || start->color[B] < 0.0 )
    {
        printf("Please enter in valid rgb values in range [0.0, 1.0]\n");
        return;
    }
    
    int x_major = fabsf(delta.position[X]) > fabsf(delta.position[Y]);
    
    if((delta.position[X] < 0 && x_major ) ||
       (delta.position[Y] < 0 && !x_major) )
    {
        SWAP(start, end);
        
        /* recalculate delta */
        vector_subtract(end->position, start->position, delta.position);
    }
    vector_subtract(end->color, start->color, delta.color);
    
    float step[4];
    float scale[4];
    float color[4];
    POINT p;
    
    glColor4f(start->color[R], start->color[G], start->color[B], 1.0);
    /* degenerate line = point */
    if(delta.position[X] == 0 && delta.position[Y] == 0)
    {
        draw_point(p.position[X], p.position[Y]);
    }
    /* horizontal line */
    else if(delta.position[Y] == 0)
    {
        for(p = *start; p.position[X] < end->position[X];)
        {
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[X]), scale, scale);
            
//            float color[4];
            scalar_multiply(scale[X], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);

            draw_point(p.position[X], p.position[Y]);
            
            p.position[X]++;
        }
    }
    /* vertical line */
    else if(delta.position[X] == 0)
    {
        for(p = *start; p.position[Y] < end->position[Y];)
        {
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[Y]), scale, scale);
            
            
            scalar_multiply(scale[Y], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            draw_point(p.position[X], p.position[Y]);
            p.position[Y]++;
        }
    }
    /* x-major diagonal line, i.e. 0 < |slope| < 1 */
    else if(x_major)
    {
        //        printf("xmajor\n");
//        step = dy / dx;
        scalar_divide(fabsf(delta.position[X]), delta.position, step);
        
        for(p = *start; p.position[X] < end->position[X]; )
        {
//            scale = (p.position[X] - p1->position[X])/dx;
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[X]), scale, scale);
            
//            glColor4f(p.color[R] + dr * scale, p.color[G] + dg * scale,
//                      p.color[B] + db * scale, 1.0);
            scalar_multiply(scale[Y], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            
            draw_point(p.position[X], p.position[Y]);
            
            p.position[X] += step[X];
            p.position[Y] += step[Y];        }
    }
    /* y-major diagonal line, i.e. |slope| > 1 */
    else if (!x_major)
    {
        //        printf("ymajor\n");
//        step = dx / dy;
        scalar_divide(fabsf(delta.position[Y]), delta.position, step);

        for(p = *start; p.position[Y] < end->position[Y];)
        {
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[Y]), scale, scale);
            
            scalar_multiply(scale[Y], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            
            draw_point(p.position[X], p.position[Y]);
            p.position[X] += step[X];
            p.position[Y] += step[Y];
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
//    draw_line_3(&start, &end);
    
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
