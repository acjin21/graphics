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



void draw_point(POINT *p);

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
               i, start, end, edge_counts[i]);
        
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

        scalar_divide(ABS(delta.position[X]), delta.position, step.position);
        scalar_divide(ABS(delta.position[X]), delta.color, step.color);
        for(p = start; p.position[X] < end.position[X];)
        {
//            printf("x = %i; y =  %i; rgb = (%.3f, %.3f, %.3f)\n",
//            (int) p.position[X], (int)p.position[Y],
//            p.color[0], p.color[1], p.color[2]);
            
            /*
             * CHANGE TWO
             */
            p.position[Y] = (int) p.position[Y];
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

        for(p =  start; p.position[X] < end.position[X]; )
        {
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
}
void draw_line(POINT *v1, POINT *v2, int mode);
/* draw horizontal scan lines of triangle */
void draw_spans(void)
{
    for(int r = 0; r < 800; r++)
    {
        POINT start_p = span[r][0];
        int count = edge_counts[r];
        if(count == 0) continue;
        if(count == 1) {
            draw_point(&start_p);
        }
        else if(count != 1 && count >= 2){
            POINT end_p = span[r][1];
//            end_p.position[Y] = start_p.position[Y];
//            draw_line_modal(&start_p, &end_p, DRAW);
            draw_line (&start_p, &end_p, DRAW);

        }
    }
}

/*************************************************************************/
 /* draw_line from Chris*/
/*************************************************************************/
#define ON 1
#define OFF 0

float color_buffer[800][800][4];
float depth_buffer[800][800];
float alpha_blend = OFF;
float depth_test = ON;

void clear_color_buffer (float r, float g, float b, float a)
{
    for(int row = 0; row < 800; row++)
    {
        for(int col = 0; col < 800; col++)
        {
            color_buffer[row][col][R] = r;
            color_buffer[row][col][G] = g;
            color_buffer[row][col][B] = b;
            color_buffer[row][col][A] = a;
        }
    }
}

void clear_depth_buffer (float value)
{
    printf("\ndepth buffer cleared\n");
    for(int row = 0; row < 800; row++)
    {
        for(int col = 0; col < 800; col++)
        {
            depth_buffer[row][col] = value;
        }
    }
}

/*
 * draw_point()
 */
//void draw_point (POINT *p)
//{
//    glBegin(GL_POINTS);
//        int row = (int) (p->position[Y] + 400);
//        int col = (int) (p->position[X] + 400);
//        float blend_weight = 0.50;
//        if((depth_test && p->position[Z] < depth_buffer[row][col]) || !depth_test)
//        {
//            if(alpha_blend)
//            {
////                float new_r = blend_weight * color_buffer[row][col][R] + blend_weight * p->color[R];
////                float new_g = blend_weight * color_buffer[row][col][G] + blend_weight * p->color[G];
////                float new_b = blend_weight * color_buffer[row][col][B] + blend_weight * p->color[B];
////                float new_a = blend_weight * color_buffer[row][col][A] + blend_weight * p->color[A];
////                /* write blended color to color_buffer */
//                color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][R] = blend_weight * color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][R] + blend_weight * p->color[R];
//                color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][G] = blend_weight * color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][G] + blend_weight * p->color[G];
//                color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][B] = blend_weight * color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][G] + blend_weight * p->color[B];
//                color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][A] = blend_weight * color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][G] + blend_weight * p->color[B];
//            }
//            else
//            {
//                /* write p.color to color_buffer */
//                color_buffer[row][col][R] = p->color[R];
//                color_buffer[row][col][G] = p->color[G];
//                color_buffer[row][col][B] = p->color[B];
//                color_buffer[row][col][A] = p->color[A];
//            }
//
//            if(depth_test)
//            {
//                depth_buffer[row][col] = p->position[Z];
//            }
//    }
//    /* draw point on screen */
//    glColor4f(color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][R],
//              color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][G],
//              color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][B],
//              color_buffer[(int) (p->position[Y] + 400)][(int) (p->position[X] + 400)][A]);
//    glVertex2f( p->position[X],  p->position[Y]);
//    glEnd();
//
//}
void draw_point(POINT *p)
{
    glBegin(GL_POINTS);
        glColor4f(1, 0, 0, 1);
    glVertex2f( p->position[X],  p->position[Y]);
    glEnd();
}

void store_point (POINT *p)
{
    int row = (int) (p->position[Y] + 400);
    int count = edge_counts[row];
    
    /* sanity check */
    if(count < 0 || count > 2) {
        printf("ERR221: row %i count %i out of bounds\n", row, count);
    }
    if(count <= 1)
    {
        span[row][count] = *p;
        edge_counts[row]++;
    }
}

void draw_line( POINT *start, POINT *end, int mode )
{
    POINT   delta;
    POINT   step;
    POINT   p;
    int     i;
    
    /*
     * calculate deltas in position, color
     */
    vector_subtract( end->position,    start->position,    delta.position   );
    vector_subtract( end->color,       start->color,       delta.color );
    
    /*
     * determine whether line is x-major or y-major
     */
    i = (ABS(delta.position[X]) >= ABS(delta.position[Y]) && mode == DRAW ) ? X : Y;
    
    /*
     * calculate slope (i.e. increment per iteration) for position, color
     *
     * for x-major divide by deltax, for y-major divide by deltay
     */
    scalar_divide( ABS(delta.position[i]), delta.position,    step.position   );
    scalar_divide( ABS(delta.position[i]), delta.color,      step.color );
    
    if( step.position[i] > 0 )
    {
        for( p = *start; (int)p.position[i] < (int)end->position[i]; )
        {
            if( mode == DRAW )
            {
                draw_point( &p );
            }
            else
            {
                store_point( &p );
            }
            vector_add( p.position,    step.position,   p.position   );
            vector_add( p.color,  step.color, p.color );
        }
    }
    else
    {
        for( p = *start; (int)p.position[i] > (int)end->position[i]; )
        {
            if( mode == DRAW )
            {
                draw_point( &p );
            }
            else
            {
                store_point( &p );
            }
            vector_add( p.position,    step.position,   p.position   );
            vector_add( p.color,  step.color, p.color );
        }
    }
}



/* draw triangle with vertices *v0, *v1, *v2 */
void draw_triangle(POINT *v0, POINT *v1, POINT *v2)
{

    printf("====================================\ndrawing new triangle\n");
    reset_edge_counts();
    
    print_tri_vertices(v0, v1, v2);
    
        draw_line (v0, v1, WALK);
        draw_line (v1, v2, WALK);
        draw_line (v2, v0, WALK);
    
//    draw_line_modal(v0, v1, WALK);
//    draw_line_modal(v1, v2, WALK);
//    draw_line_modal(v2, v0, WALK);
    draw_spans();
    
}
int counter = 0;
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
    clear_color_buffer(0, 0, 0, 1);
    clear_depth_buffer(1.0);
    glPointSize(2.0);
    printf("\nNEW DISPLAY: %i\n", counter);
    counter++;
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
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
//    glPointSize(1.0);

    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}

