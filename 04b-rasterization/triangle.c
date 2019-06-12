#include "triangle.h"
#include "vector.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

POINT span[800][2];
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
        glVertex2f( p->position[X],  p->position[Y]);
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
    printf("{%.5f, %.5f, %.5f, %.5f}, ",
           p->position[X], p->position[Y], p->position[Z], p->position[W]);
}

void print_tri_vertices (POINT *v0, POINT *v1, POINT *v2)
{
    print_point(v0);
    print_point(v1);
    print_point(v2);
    printf("\n");
}
void sprint_point (char *s, POINT *p)
{
    sprintf(s, "pos: (%.2f, %.2f)  color: (%.2f, %.2f, %.2f, %.2f)",
            p->position[X], p->position[Y],
            p->color[R], p->color[G], p->color[B], p->color[A]);
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
void print_span (int row_start, int row_end)
{
    char start[100];
    char end[100];
    printf("\n=====================================================\n");
    for(int i = row_start; i < row_end; i++)
    {
        sprint_point(start, &span[i][0]);
        sprint_point(end, &span[i][1]);
        
        printf("- row = %i, [0] = %s, [1] = %s, count = %i\n",
               i, start, end,
               edge_counts[i]);
        
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
    /*
     * CHANGE ONE
     */
//    start.position[Y] = (int) start.position[Y];
//    end.position[Y] = (int) end.position[Y];

    POINT delta, step;
    
    // overall change in position
    vector_subtract(end.position, start.position, delta.position);
    
    int x_major = ABS(delta.position[X]) > ABS(delta.position[Y]);
    
    if(mode == DRAW && ((delta.position[X] < 0 && x_major) ||
        (delta.position[Y] < 0 && !x_major )))
    {
        SWAP(p1, p2);

        /* recalculate delta.position */
        start = *p1;
        end = *p2;
        vector_subtract(end.position, start.position, delta.position);
    }
    // calculate delta color
    vector_subtract(end.color, start.color, delta.color);

    POINT p;
    
    /*******************/
    /* horizontal line */
    /*******************/
    if(mode == DRAW && (int) delta.position[Y] == 0)
    {
//        printf("horiz draw mode\n");
//        printf("y = (start) %f, (end) %f\n", start.position[Y], end.position[Y]);
        scalar_divide(ABS(delta.position[X]), delta.position, step.position);
        scalar_divide(ABS(delta.position[X]), delta.color, step.color);
        for(p = start; p.position[X] < end.position[X];)
        {
            printf("x = %i; y =  %i; rgb = (%.3f, %.3f, %.3f)\n", (int) p.position[X], (int)p.position[Y], p.color[0], p.color[1], p.color[2]);
            /*
             * CHANGE TWO
             */
            p.position[Y] = (int) p.position[Y];
//            printf("%.2f, (int) %i, (floor) %f\n", p.position[Y], (int) p.position[Y], floor(p.position[Y]));
//            p.position[Y] = (int)floor(p.position[Y]);
            glColor4f(p.color[0], p.color[1], p.color[2], 1.0);
            draw_point(&p);
            
            vector_add(p.position, step.position, p.position);
            vector_add(p.color, step.color, p.color);
        }
        return;
    }
    /* x-major diagonal line, i.e. 0 < |slope| < 1 */
    else if(x_major && mode == DRAW)
    {
        
        printf("xmajor draw mode\n");
        scalar_divide(ABS(delta.position[X]), delta.position, step.position); // dy/dx
        scalar_divide(ABS(delta.position[X]), delta.color, step.color);

        for(p =  start; (int)p.position[X] < (int)end.position[X]; )
        {
            glColor4f(p.color[0], p.color[1], p.color[2], 1.0);
            draw_point(&p);

            vector_add(p.position, step.position, p.position);
            vector_add(p.color, step.color, p.color);
        }
        return;
    }
    
    /*******************/
    /* WALK */
    /*******************/

    int prev_row;
    if ((int) delta.position[Y] == 0) {
        scalar_divide(ABS(delta.position[X]), delta.position, step.position);
        scalar_divide(ABS(delta.position[X]), delta.color, step.color);
    }
    else {
        scalar_divide(ABS(delta.position[Y]), delta.position, step.position);
        scalar_divide(ABS(delta.position[Y]), delta.color, step.color);
    }

    for(p = start; (int) p.position[Y] < (int) end.position[Y];)
    {
        /* calculate scale based on Y coordinate for color interp. */
        if (mode == DRAW)
        {
            /* should be y major and draw */
            glColor4f(p.color[0], p.color[1], p.color[2], 1.0);
            draw_point(&p);
        }
        
        else if (mode == WALK)
        {
            int row = (int) p.position[Y] + 400;
            int count = edge_counts[row];
            
            /* sanity check */
            if(count < 0 || count > 2) {
                printf("ERR196: row %i count %i out of bounds\n", row, count);
            }
            if(row != prev_row){
                span[row][count] = p;
                edge_counts[row]++;
            }
            prev_row = row;


        }
        vector_add(p.color, step.color, p.color);
        vector_add(p.position, step.position, p.position);
    }

    for(p = start; (int) p.position[Y] > (int) end.position[Y];)
    {
        if (mode == DRAW)
        {
            /* should be y major and draw */

            glColor4f(p.color[0], p.color[1], p.color[2], 1.0);
            draw_point(&p);
        }

        else if (mode == WALK)
        {
            int row = (int) p.position[Y] + 400;
            int count = edge_counts[row];

            /* sanity check */
            if(count < 0 || count > 2) {
                printf("ERR221: row %i count %i out of bounds\n", row, count);
            }
            if(row != prev_row){
                span[row][count] = p;
                edge_counts[row]++;
            }
            prev_row = row;
        }
        vector_add(p.color, step.color, p.color);
        vector_add(p.position, step.position, p.position);
    }
//    }
}

/* draw horizontal scan lines of triangle */
void draw_spans(void)
{
    int prev_end_x = -window_size;
    for(int r = 0; r < 800; r++)
    {
        POINT start_p = span[r][0];
        int count = edge_counts[r];
        if(count == 0) continue;
        if(count == 1) {
            glColor4f(start_p.color[0], start_p.color[1], start_p.color[2], 1.0);
            draw_point(&start_p);
        }
        else if(count == 2){
            
            POINT end_p = span[r][1];
    
            POINT *p1 = &start_p;
            POINT *p2 = &end_p;

            if (start_p.position[X] > end_p.position[X]) {
                SWAP(p1, p2);
            }
            /*
             * CHANGE THREE
             */
            //start_p should be to left of end_p now (avoid gaps)
//            if(p1->position[X] <= prev_end_x){
                //draw horizontal span line
                p2->position[Y] = p1->position[Y];
                draw_line_modal(p1, p2, DRAW);
//            }
//            prev_end_x = p2->position[X];
        
        }
    }
}

/* draw triangle with vertices *v0, *v1, *v2 */
void draw_triangle(POINT *v0, POINT *v1, POINT *v2)
{
    printf("====================================\ndrawing new triangle\n");
    reset_edge_counts();
    print_tri_vertices(v0, v1, v2);

//    draw_line_modal(v0, v1, DRAW);
//    draw_line_modal(v1, v2, DRAW);
//    draw_line_modal(v2, v0, DRAW);
    
//    glPointSize(1.0);

    draw_line_modal(v0, v1, WALK);
    print_span(0, 300);

    draw_line_modal(v1, v2, WALK);
    print_span(0, 300);
//
    draw_line_modal(v2, v0, WALK);
    print_span(0, 300);

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
    int y = 0;
    for(int j = -window_size; j < window_size; j+=50)
    {
        for(int x = -window_size; x < window_size; x++)
        {
            POINT p = {
                {x, j, 0, 0},
                {1, 0, 0, 1}
            };
            glColor4f(1, 1, 1, 1);
            draw_point(&p);
        }
    }
    
    draw_tri_test();
//    printf("%i\n", (int) -1.89);

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
    glutInitWindowSize(window_size, window_size);
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
    glClearColor(0, 0, 0, 1);
//    glClearColor(1.0,1.0,224.0/256.0,1.0);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
//    glPointSize(4.0);
//    glColor4f(1.0,0.0,0.0,1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}

