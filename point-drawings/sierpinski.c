#include "point.h"
#include <assert.h>

/* type of float 2D point */
typedef struct {
    float x;
    float y;
} Point_2f;

/* draw 1000 points of the sierpinski gasket */
void draw_sierp (void)
{
    /*
     * initialize random vertices t0, t1, t2
     */
    Point_2f vertices[3];
    for(int i = 0; i < 3; i++)
    {
        float rand_x = random_float(-window_size,window_size);
        float rand_y = random_float(-window_size,window_size);
        Point_2f v = { rand_x, rand_y };
        vertices[i] = v;
    }
//    Point_2f t0 = {10, 10};
//    Point_2f t1 = {300, 30};
//    Point_2f t2 = {200, 300};
//    vertices[0] = t0;
//    vertices[1] = t1;
//    vertices[2] = t2;
    /*
     * choose init vtx *point and draw it
     */
    int rand_idx = (int) random_float(0, 3);
    Point_2f point = {vertices[rand_idx].x, vertices[rand_idx].y};
    draw_point(point.x, point.y);

    for(int i = 0; i < 1000; i++)
    {
        rand_idx = (int) random_float(0, 3);        // new idx
        point.x = (point.x + (vertices[rand_idx]).x) / 2.0;
        point.y = (point.y + (vertices[rand_idx]).y) / 2.0;
        draw_point(point.x, point.y);
    }
}




