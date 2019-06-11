#include "triangle.h"
#include "vector.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

int span[800][2];
int edge_counts[800];
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

void draw_point (POINT *p)
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

/*
 * print point *p
 */
void print_point (POINT *p)
{
    printf("POINT: (%.2f, %.2f)\n", p->position[X], p->position[Y]);
}

/*
 * print table of edge counts
 */
void print_edge_counts (void)
{
    for(int i = 0; i < 800; i++)
    {
        printf("row %i, y = %i, count = %i\n", i, i - 400, edge_counts[i]);
    }
}

/*
 * print entire span
 */
void print_span (void)
{
    printf("\n=====================================================\n");
    for(int i = 0; i < 800; i++)
    {
        printf("- row = %i, [0] = %i, [1] = %i, count = %i\n",
               i, span[i][0], span[i][1], edge_counts[i]);
        
    }
}

/*************************************************************************/
/* draw_line                                                             */
/*************************************************************************/

/* reset all edge counts to 0 */
void reset_edge_counts (void)
{
    for(int i = 0; i < 800; i++)
    {
        edge_counts[i] = 0;
    }
}

/* draw_line with DRAW and WALK modes */
void draw_line_modal (POINT *p1, POINT *p2, int mode)
{
    POINT start = *p1;
    POINT end = *p2;
    POINT delta;
    POINT step;
    
    // overall change in position and in rgb values
    vector_subtract(end.position, start.position, delta.position);
    int x_major = ABS(delta.position[X]) > ABS(delta.position[Y]);
    
    printf("\nstart: (%.2f, %.2f) || end: (%.2f, %.2f)\n",
           start.position[X], start.position[Y],
           end.position[X], end.position[Y]);
    
    if((delta.position[X] < 0 && x_major ) ||
       (delta.position[Y] < 0 && !x_major) )
    {
        printf("SWAP\n");
        SWAP(start.position[X], end.position[X]);
        SWAP(start.position[Y], end.position[Y]);
        SWAP(start.color[R], end.color[R]);
        SWAP(start.color[G], end.color[G]);
        SWAP(start.color[B], end.color[B]);
        
        /* recalculate delta */
        vector_subtract(end.position, start.position, delta.position);
    }
    vector_subtract(end.color, start.color, delta.color);
    
    printf("after swap- start: (%.2f, %.2f) || end: (%.2f, %.2f)\n", start.position[X], start.position[Y],
           end.position[X], end.position[Y]);
    
    //scale for color interpolation
    float scale[4];
    POINT p;
    
    glColor4f(start.color[R], start.color[G], start.color[B], 1.0);
    
    /* horizontal line */
    if(delta.position[Y] == 0) {
        scalar_divide(fabsf(delta.position[X]), delta.position, step.position);
    }
    else {
        scalar_divide(fabsf(delta.position[Y]), delta.position, step.position);
    }
    
    for(p = start; (int) p.position[Y] < (int) end.position[Y];)
    {
        /* calculate scale based on Y coordinate for color interp. */
        vector_subtract(p.position, start.position, scale);
        scalar_divide(fabsf(delta.position[Y]), scale, scale);
        scalar_multiply(scale[Y], delta.color, step.color);
        vector_add(p.color, step.color, p.color);
        
        if (mode == DRAW)
        {
            glColor4f(p.color[0], p.color[1], p.color[2], 1.0);
            draw_point(&p);
        }
        
        else if (mode == WALK)
        {
            int row = (int) p.position[Y] + 400;
            int count = edge_counts[row];
            
            /* sanity check */
            if(count < 0 || count > 2) printf("ERR: count out of bounds\n");

            //fill in the walk data
            span[row][count] = (int) p.position[X];
            edge_counts[row]++;
        }
        vector_add(p.position, step.position, p.position);
    }
}

/* draw horizontal scan lines of triangle */
void draw_spans(void)
{
    for(int r = 0; r < 800; r++)
    {
        int start_x = span[r][0];
        int count = edge_counts[r];
        
        POINT point;
        
        if(count == 0) continue;
//        if(count == 1) {
//            draw_point(span[r][0], r-400);
//        }
        else if(count == 2){
            int end_x = span[r][count-1];
            //start_x <= end_x
            for(int c = start_x; c <= end_x; c++)
            {
                set_position(&point, c, r-400, 0, 0);
                draw_point(&point);
            }
            //start_x > end_x
            for(int c = start_x; c >= end_x; c--)
            {
                set_position(&point, c, r-400, 0, 0);
                draw_point(&point);
            }
        }
    }
}

/* draw triangle with vertices *v0, *v1, *v2 */
void draw_triangle(POINT *v0, POINT *v1, POINT *v2)
{
    reset_edge_counts();
    print_point(v0);
    print_point(v1);
    print_point(v2);

    draw_line_modal(v0, v1, 1);
    print_span();
    draw_line_modal(v1, v2, 1);
    print_span();
    draw_line_modal(v2, v0, 1);
    print_span();

    int row;
    
    row = (int) v0->position[Y] + 400;
    if(edge_counts[row] == 0)
    {
        edge_counts[row] = 2;
        span[row][0] = (span[row][1] = v0->position[X]);
    }
    row = (int) v1->position[Y] + 400;
    if(edge_counts[row] == 0)
    {
        edge_counts[row] = 2;
        span[row][0] = (span[row][1] = v1->position[X]);
    }
    row = (int) v2->position[Y] + 400;
    if(edge_counts[row] < 1)
    {
        edge_counts[row] = 2;
        span[row][0] = (span[row][1] = v2->position[X]);
    }
    print_span();

    draw_spans();
}

/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
extern void draw_tri_test(void);
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
    
    draw_tri_test();

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
