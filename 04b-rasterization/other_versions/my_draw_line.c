#define SWAP(a,b)\
{                   \
POINT *tmp = a;  \
a = b;          \
b = tmp;\
}

#define ABS(_a) (((_a) < 0.0) ? -(_a) : (_a))


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
        }
        vector_add(p.color, step.color, p.color);
        vector_add(p.position, step.position, p.position);
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
            end_p.position[Y] = start_p.position[Y];
            draw_line_modal(&start_p, &end_p, DRAW);
        }
    }
}
