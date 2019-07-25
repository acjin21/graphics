#include "raster.h"
#include <stdio.h>
#include "opengl.h"
/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define MIN3(a,b,c) (((a) < (b)) ? (((a) < (c)) ? (a) : (c)) : (((b) < (c)) ? (b) : (c)))
#define MAX3(a,b,c) (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c)))

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
/* for rasterization */
POINT span[WIN_H][2];
int edge_counts[WIN_H];

/*************************************************************************/
/* method prototypes                                                     */
/*************************************************************************/
void store_point (POINT *p);
void reset_edge_counts (void);

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

/* store point in span */
void store_point (POINT *p)
{
    int r = (int) (p->position[Y] + WIN_H / 2);
    int count = edge_counts[r];
    
    /* sanity check */
    if(count < 0 || count > 2)
    {
        printf("ERR: row %i, count %i out of bounds\n", r, count);
    }
    span[r][count] = *p;
    edge_counts[r]++;
}


/* draw horizontal scan lines of triangle */
void draw_spans(void)
{
    for(int r = 0; r < WIN_H; r++)
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

/* reset all edge counts to 0 */
void reset_edge_counts (void)
{
    for(int i = 0; i < WIN_H; i++)
    {
        edge_counts[i] = 0;
    }
}
void print_tri_vertices (POINT *v0, POINT *v1, POINT *v2);
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

/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/
/*
 * print point *p
 */
void print_point (POINT *p)
{
    printf("position: {%.5f, %.5f, %.5f, %.5f}, ",
           p->position[X], p->position[Y], p->position[Z], p->position[W]);
}
void print_color (POINT *p)
{
    printf("color {%.2f, %.2f, %.2f, %.2f}, ",
           p->color[R], p->color[G], p->color[B], p->color[A]);
}

void print_tri_vertices (POINT *v0, POINT *v1, POINT *v2)
{
    print_point(v0);
    print_color(v0);
    print_point(v1);
    print_color(v1);
    print_point(v2);
    print_color(v2);

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
    for(int i = 0; i < WIN_H; i++)
    {
        printf("row %i, y = %i, count = %i\n", i, i - WIN_H / 2, edge_counts[i]);
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

/*
 * edgeFunction()
 */
float edgeFunction( float a[4], float b[4], float c[4] )
{
    return (c[X] - a[X]) * (b[Y] - a[Y]) - (c[Y] - a[Y]) * (b[X] - a[X] );
}

/* to choose between our draw_triangle and opengl's draw_triangle */
void draw_triangle_wrapper(POINT *v0, POINT *v1, POINT *v2)
{
    if(renderer == SW_HW)
    {
        draw_triangle_gl(v0, v1, v2);
    }
    else if(renderer == ALL_SW)
    {
        draw_triangle_barycentric(v0, v1, v2);
    }
}

/*
 * draw_triangle_barycentric()
 */
void draw_triangle_barycentric( POINT *v0, POINT *v1, POINT *v2 )
{
    int     minx = MIN3(v0->position[X],v1->position[X],v2->position[X]);
    int     miny = MIN3(v0->position[Y],v1->position[Y],v2->position[Y]);
    int     maxx = MAX3(v0->position[X],v1->position[X],v2->position[X]);
    int     maxy = MAX3(v0->position[Y],v1->position[Y],v2->position[Y]);
    float   area = edgeFunction(v0->position, v1->position, v2->position);
    int     x, y;
    float   w[4];
    POINT   p;
    
    /*
     * for all the points in triangle bounding box
     */
    for( y = miny-1; y < maxy+2; y++ )
    {
        for( x = minx-1; x < maxx+2; x++ )
        {
            set_vec4(p.position, x+0.5, y+0.5, 0, 1 );
            /*
             * compute barycentric weights
             */
            w[0] = edgeFunction(v1->position, v2->position, p.position);
            w[1] = edgeFunction(v2->position, v0->position, p.position);
            w[2] = edgeFunction(v0->position, v1->position, p.position);
            w[3] = 0.0;
            
            if( w[0] >= 0 && w[1] >= 0 && w[2] >= 0 )
            {
                /*
                 * if point is inside triangle, compute barycentric weighting of vertex attributes (e.g. z, color, s, t)
                 */
                scalar_divide(area, w, w);
                
                p.position[Z] = w[0] * v0->position[Z] + w[1] * v1->position[Z] + w[2] * v2->position[Z];

                //interpolate 1/w for perspective correct
                p.position[W] = w[0] * v0->position[W] + w[1] * v1->position[W] + w[2] * v2->position[W];
                
                p.color[R] = w[0] * v0->color[R] + w[1] * v1->color[R] + w[2] * v2->color[R];
                p.color[G] = w[0] * v0->color[G] + w[1] * v1->color[G] + w[2] * v2->color[G];
                p.color[B] = w[0] * v0->color[B] + w[1] * v1->color[B] + w[2] * v2->color[B];
                
                p.tex[S] = w[0] * v0->tex[S] + w[1] * v1->tex[S] + w[2] * v2->tex[S];
                p.tex[T] = w[0] * v0->tex[T] + w[1] * v1->tex[T] + w[2] * v2->tex[T];
                
                p.v_normal[X] = w[0] * v0->v_normal[X] + w[1] * v1->v_normal[X] + w[2] * v2->v_normal[X];
                p.v_normal[Y] = w[0] * v0->v_normal[Y] + w[1] * v1->v_normal[Y] + w[2] * v2->v_normal[Y];
                p.v_normal[Z] = w[0] * v0->v_normal[Z] + w[1] * v1->v_normal[Z] + w[2] * v2->v_normal[Z];
                
                p.light[X] = w[0] * v0->light[X] + w[1] * v1->light[X] + w[2] * v2->light[X];
                p.light[Y] = w[0] * v0->light[Y] + w[1] * v1->light[Y] + w[2] * v2->light[Y];
                p.light[Z] = w[0] * v0->light[Z] + w[1] * v1->light[Z] + w[2] * v2->light[Z];
                
                p.view[X] = w[0] * v0->view[X] + w[1] * v1->view[X] + w[2] * v2->view[X];
                p.view[Y] = w[0] * v0->view[Y] + w[1] * v1->view[Y] + w[2] * v2->view[Y];
                p.view[Z] = w[0] * v0->view[Z] + w[1] * v1->view[Z] + w[2] * v2->view[Z];
                
                draw_point(&p);
            }
        }
    }
}
