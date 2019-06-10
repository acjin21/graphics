#include "line.h"
#include "vector.h"
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
        SWAP_POINTS(start, end);
        
        /* recalculate delta */
        vector_subtract(end->position, start->position, delta.position);
    }
    vector_subtract(end->color, start->color, delta.color);
    
    float step[4];
    //scale for color interpolation
    float scale[4];
    float color[4];
    POINT p;
    
    glColor4f(start->color[R], start->color[G], start->color[B], 1.0);
    /* degenerate line = point */
    if(delta.position[X] == 0 && delta.position[Y] == 0)
    {
        draw_point_2(&p);
    }
    /* horizontal line */
    else if(delta.position[Y] == 0)
    {
        for(p = *start; p.position[X] < end->position[X];)
        {
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[X]), scale, scale);
            scalar_multiply(scale[X], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            
            draw_point_2(&p);
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
            
            draw_point_2(&p);
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
            /* calculate scale based on X coordinate for color interp. */
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[X]), scale, scale);
            scalar_multiply(scale[X], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            
            draw_point_2(&p);
            vector_add(p.position, step, p.position);
        }
    }
    /* y-major diagonal line, i.e. |slope| > 1 */
    else if (!x_major)
    {
        //        printf("ymajor\n");
        //        step = dx / dy;
        scalar_divide(fabsf(delta.position[Y]), delta.position, step);
        
        for(p = *start; p.position[Y] < end->position[Y];)
        {
            /* calculate scale based on Y coordinate for color interp. */
            vector_subtract(p.position, start->position, scale);
            scalar_divide(fabsf(delta.position[Y]), scale, scale);
            scalar_multiply(scale[Y], delta.color, color);
            vector_add(p.color, color, color);
            glColor4f(color[0], color[1], color[2], 1.0);
            
            draw_point_2(&p);
            vector_add(p.position, step, p.position);
        }
    }
}
