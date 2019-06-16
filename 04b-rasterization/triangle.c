#include "triangle.h"
#include "vector.h"
#include "texture.h" //IMAGE typedef, texture and ppm functions
#include "image.h" //uses IMAGE typedef, image processing funcs
#include "util.h" //random_float
#include "color.h"
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

POINT span[800][2];
int edge_counts[800];

#define ON 1
#define OFF 0

IMAGE texture;
IMAGE texture_in;
extern float color_buffer[800][800][4];
float depth_buffer[800][800];
float alpha_blend = OFF;
float depth_test = OFF;
float texturing = ON;
/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/

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

void set_tex (POINT *p, float s, float t)
{
    p->tex[S] = s;
    p->tex[T] = t;
    p->tex[2] = 0;
    p->tex[3] = 0;
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

/*************************************************************************/
/* buffer util funcs */
/*************************************************************************/


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

void draw_color_buffer (void)
{
    glBegin(GL_POINTS);
    for(int y = -window_size; y < window_size; y++)
    {
        for(int x = -window_size; x < window_size; x++)
        {
            int row = (int) (y + 400);
            int col = (int) (x + 400);
            glColor4f(color_buffer[row][col][R], 0, 0, color_buffer[row][col][A]);
            glVertex2f( x, y);
        }
    }
    glEnd();
}
/*************************************************************************/
/* draw a point */
/*************************************************************************/
void draw_point (POINT *p)
{
    glBegin(GL_POINTS);
        int row = (int) (p->position[Y] + 400);
        int col = (int) (p->position[X] + 400);
        float blend_weight = 0.50;
    
        if(texturing)
        {
            int s, t;
            s = (int) (p->tex[S] * texture.width);
            t = (int) (p->tex[T] * texture.height);
            
            if(p->tex[S] == 1 || p->tex[T] == 1)
            {
                s = p->tex[S] == 1 ? texture.width - 1 : s;
                t = p->tex[T] == 1 ? texture.width - 1 : t;
            }
            color_buffer[row][col][R] = texture.data[t][s][R] / 255.0;
            color_buffer[row][col][G] = texture.data[t][s][G] / 255.0;
            color_buffer[row][col][B] = texture.data[t][s][B] / 255.0;
            color_buffer[row][col][A] = texture.data[t][s][A] / 255.0;
        }
        else
        {
            if((depth_test && p->position[Z] < depth_buffer[row][col]) || !depth_test)
            {
                if(alpha_blend)
                {
                    float new_r, new_g, new_b, new_a;
//                    if (texturing)
//                    {
//                          int s, t;
//                    }
                    new_r = (1 - blend_weight) * color_buffer[row][col][R] +
                        blend_weight * p->color[R];
                    new_g = (1 - blend_weight) * color_buffer[row][col][G] +
                        blend_weight * p->color[G];
                    new_b = (1 - blend_weight) * color_buffer[row][col][B] +
                        blend_weight * p->color[B];
                    new_a = (1 - blend_weight) * color_buffer[row][col][A] +
                        blend_weight * p->color[A];

                    /* write blended color to color_buffer */
                    color_buffer[row][col][R] = new_r;
                    color_buffer[row][col][G] = new_g;
                    color_buffer[row][col][B] = new_b;
                    color_buffer[row][col][A] = new_a;
                }
                else
                {
                    /* write p.color to color_buffer */
                    color_buffer[row][col][R] = p->color[R];
                    color_buffer[row][col][G] = p->color[G];
                    color_buffer[row][col][B] = p->color[B];
                    color_buffer[row][col][A] = p->color[A];
                }
                if(depth_test)
                {
                    depth_buffer[row][col] = p->position[Z];
                }
            }
    }
    /* draw point on screen */
    glColor4f(color_buffer[row][col][R], color_buffer[row][col][G],
              color_buffer[row][col][B], color_buffer[row][col][A]);
    glVertex2f( p->position[X],  p->position[Y]);
    glEnd();
}

/* store point in span */
void store_point (POINT *p)
{
    int row = (int) (p->position[Y] + 400);
    int count = edge_counts[row];
    
    /* sanity check */
    if(count < 0 || count > 2) {
        printf("ERR221: row %i count %i out of bounds\n", row, count);
    }
    span[row][count] = *p;
    edge_counts[row]++;
}

/*************************************************************************/
/* draw_line from Chris */
/*************************************************************************/
void draw_line( POINT *start, POINT *end, int mode )
{
    POINT delta, step, p;
    int i;
    
    /*
     * calculate deltas in position, color
     */
    vector_subtract(end->position, start->position, delta.position);
    vector_subtract(end->color, start->color, delta.color);
    vector_subtract(end->tex, start->tex, delta.tex);

    
    /*
     * determine whether line is x-major or y-major
     */
    i = (ABS(delta.position[X]) >= ABS(delta.position[Y]) && mode == DRAW )
        ? X : Y;
    
    /*
     * calculate slope (i.e. increment per iteration) for position, color
     *
     * for x-major divide by deltax, for y-major divide by deltay
     */
    scalar_divide(ABS(delta.position[i]), delta.position, step.position);
    scalar_divide(ABS(delta.position[i]), delta.color, step.color);
    scalar_divide(ABS(delta.position[i]), delta.tex, step.tex);

    if( step.position[i] > 0 )
    {
        for( p = *start; (int) p.position[i] < (int) end->position[i]; )
        {
            if( mode == DRAW )
            {
                draw_point(&p);
            }
            else
            {
                store_point(&p);
            }
            vector_add(p.position, step.position, p.position);
            vector_add(p.color, step.color, p.color);
            vector_add(p.tex, step.tex, p.tex);
        }
    }
    else
    {
        for( p = *start; (int) p.position[i] > (int) end->position[i]; )
        {
            if( mode == DRAW )
            {
                draw_point(&p);
            }
            else
            {
                store_point(&p);
            }
            vector_add(p.position, step.position, p.position);
            vector_add(p.color, step.color, p.color);
            vector_add(p.tex, step.tex, p.tex);
        }
    }
}

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
        else if(count == 2){
            POINT end_p = span[r][1];
            draw_line (&start_p, &end_p, DRAW);
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
    draw_spans();
    
}
int counter = 0;
int file_index = 0;

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
        glutReshapeWindow(2 * window_size,2 * window_size);
        //  Necessary for Mojave.
        //  Has to be different dimensions than in glutInitWindowSize();
        Mojave_WorkAround = 0;
    }
    if( draw_one_frame == 0 )
        return;
	
    /* test reading in texture files */
    char file_names[7][100] =
    {
        "ppm/blackbuck.ascii.ppm",
        "ppm/out.ppm",
        "ppm/feep.ascii.ppm",
        "ppm/pbmlib.ascii.ppm",
        "ppm/sines.ascii.ppm",
        "ppm/snail.ascii.ppm",
        "ppm/star_field.ascii.ppm"
    };
    file_index = (file_index + 1) % 7;
    char *ppm_file = file_names[file_index];
    read_ppm(ppm_file, &texture_in);
    
    /* test image processing */
//    fill (&texture, 255, 0, 0);
//    luminosity(&texture_in, &texture);
//    negative(&texture_in, &texture);
//    flip_vertical(&texture_in, &texture);
//    flip_horizontal(&texture_in, &texture);
//    luminosity(&texture_in, &texture);
//    sepia(&texture_in, &texture);
//    avg(&texture_in, &texture);
//    min(&texture_in, &texture);
//    max(&texture_in, &texture);
    clear_texture(&texture, 0, 0, 0, 1);
    rotate_ccw(&texture_in, &texture, 45);
    
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
//    glClearColor(0, 0, 0, 1);
    glClearColor(255/255.0, 255/255.0,  153/255.0,  1); //light yellow
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    glPointSize(1.0);
    
    /*
     * textures
     */
//    random_texture(&texture);
//    checkerboard_texture(&texture);
    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}

