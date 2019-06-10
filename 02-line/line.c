#include "line.h"
/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

#define SWAP(a, b)  \
{                   \
    float tmp = a;  \
    a = b;          \
    b = tmp;        \
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
 * draw arbitrary slope line given the two endpoints and
 *  starting/ending rgb values.
 */
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
    
    if((dx < 0 && x_major )|| (dy < 0 && !x_major) )
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
    else if(dx == 0)
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
//        printf("xmajor\n");
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
    else if (!x_major)
    {
//        printf("ymajor\n");
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

/*
 * draw arbitrary slope line given the two endpoints *p1 and *p2
 *  with coordinate positions and rgba values
 */
void draw_line_2 (POINT *p1, POINT *p2)
{
    
    
    float dy = p2->position[Y] - p1->position[Y];
    float dx = p2->position[X] - p1->position[X];
    
    if( p2->color[R] > 1.0 || p1->color[R] < 0.0 ||
        p2->color[G] > 1.0 || p1->color[G] < 0.0 ||
        p2->color[B] > 1.0 || p1->color[B] < 0.0 )
    {
        printf("Please enter in valid rgb values in range [0.0, 1.0]\n");
        return;
    }
    
    int x_major = fabsf(dx) > fabsf(dy);
    
    if((dx < 0 && x_major )|| (dy < 0 && !x_major) )
    {
        SWAP(p1->position[X], p2->position[X]);
        SWAP(p1->position[Y], p2->position[Y]);
        SWAP(p1->color[R], p2->color[R]);
        SWAP(p1->color[G], p2->color[G]);
        SWAP(p1->color[B], p2->color[B]);
        
        /* recalculate deltas */
        dy = p2->position[Y] - p1->position[Y];
        dx = p2->position[X] - p1->position[X];
    }
    
    float dr = p2->color[R] - p1->color[R];
    float dg = p2->color[G] - p1->color[G];
    float db = p2->color[B] - p1->color[B];
    
    float step;
//    float x = p1->position[X];
//    float y = p1->position[Y];
    glColor4f(p1->color[R], p1->color[G], p1->color[B], 1.0);
    float scale;
    
    POINT p = *p1;
    /* degenerate line = point */
    if(dx == 0 && dy == 0)
    {
        draw_point(p.position[X], p.position[Y]);
    }
    /* horizontal line */
    else if(dy == 0)
    {
        for(; p.position[X] < p2->position[X];)
        {
            scale = (p.position[X] - p1->position[X])/dx;
            glColor4f(p.color[R] + dr * scale, p.color[G] + dg * scale,
                      p.color[B] + db * scale, 1.0);
            draw_point(p.position[X], p.position[Y]);
            
            p.position[X]++;
        }
    }
    /* vertical line */
    else if(dx == 0)
    {
        for(; p.position[Y] < p2->position[Y];)
        {
            scale = (p.position[Y] - p1->position[Y])/dy;
            glColor4f(p.color[R] + dr * scale, p.color[G] + dg * scale,
                      p.color[B] + db * scale, 1.0);
            draw_point(p.position[X], p.position[Y]);
            
            p.position[Y]++;
        }
    }
    /* x-major diagonal line, i.e. 0 < |slope| < 1 */
    else if(x_major)
    {
        //        printf("xmajor\n");
        step = dy / dx;
        for(; p.position[X] < p2->position[X]; )
        {
            scale = (p.position[X] - p1->position[X])/dx;
            glColor4f(p.color[R] + dr * scale, p.color[G] + dg * scale,
                      p.color[B] + db * scale, 1.0);
            draw_point(p.position[X], p.position[Y]);
            
            p.position[Y] += step;
            p.position[X]++;
        }
    }
    /* y-major diagonal line, i.e. |slope| > 1 */
    else if (!x_major)
    {
        //        printf("ymajor\n");
        step = dx / dy;
        for(; p.position[Y] < p2->position[Y];)
        {
            scale = (p.position[Y] - p1->position[Y])/dy;
            glColor4f(p.color[R] + dr * scale, p.color[G] + dg * scale,
                      p.color[B] + db * scale, 1.0);
            draw_point(p.position[X], p.position[Y]);
            p.position[X] += step;
            p.position[Y]++;
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
    
    
//    draw_line(100, 200, -100, -200, 1, 0, 0, 0, 0, 1);
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
