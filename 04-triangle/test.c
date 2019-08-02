#include "triangle.h"
void random_tests (void)
{
    /**************************/
    /* 1. two y-major edges */
    /**************************/

//    POINT v0 = {
//        {200, -200, 0, 0},
//        {1, 1, 0, 1}
//    };
//
//    POINT v1 = {
//        {-200, -100, 0, 0},
//        {1, 1, 0, 1}
//    };
//
//    POINT v2 = {
//        {-100, 400, 0, 0},
//        {1, 1, 0, 1}
//    };
    
    /**************************/
    /* 2. two y-major edges */
    /**************************/
    
//    POINT v0 = {
//        {0, 400, 0, 0},
//        {1, 0, 1, 1}
//    };
//
//    POINT v1 = {
//        {-100, 100, 0, 0},
//        {1, 0, 1, 1}
//    };
//
//    POINT v2 = {
//        {150, -300, 0, 0},
//        {1, 0, 1, 1}
//    };
    
    /**************************/
    /* 3. one horizontal edge */
    /**************************/
    
//        POINT v0 = {
//            {0, 400, 0, 0},
//            {.2, 0, .5, 1}
//        };
//
//        POINT v1 = {
//            {-100, 0, 0, 0},
//            {.2, 0, .5, 1}
//        };
//
//        POINT v2 = {
//            {100, 0, 0, 0},
//            {.2, 0, .5, 1}
//        };
    
    /*********************/
    /* 4. all x major    */
    /*********************/
    
        POINT v0 = {
            {400, 100, 0, 0},
            {.2, 0.1, .5, 1}
        };
    
        POINT v1 = {
            {200, -50, 0, 0},
            {.2, 0.1, .5, 1}
        };
    
        POINT v2 = {
            {-100, 0, 0, 0},
            {.2, 0.1, .5, 1}
        };
    
    draw_triangle(&v0, &v1, &v2);

}

/****************************************/
/* copy contents of val[4] into dest[4] */
/****************************************/
void cpy_vec4 (float dest[4], float val[4])
{
    for(int i = 0; i < 4; i++)
    {
        dest[i] = val[i];
    }
}

void set_vec4 (float dest[4], float x, float y, float z, float w)
{
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
    dest[3] = w;
}


POINT v0, v1, v2;

float points[18][4] =
{
    {-250, 100, 0, 0},      {-350, 200, 0, 0},      {0, 300, 0, 0},
    {-400, -100, 0, 0},     {-200, -200, 0, 0},     {-200, 0, 0, 0},
    {200, -250, 0, 0},      {0, -350, 0, 0},        {-400, -400, 0, 0},
    {0, -100, 0, 0},        {200, -100, 0, 0},      {200, 200, 0, 0},
    {300, 300, 0, 0},       {400, 0, 0, 0},         {300, -300, 0, 0},
    {-75, 0, 0, 0},         {0, 200, 0, 0},         {-50, -50, 0, 0}
};

float colors[18][4] =
{
    {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 1},
    {1, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1},
    {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 1},
    {1, 0, 1, 1}, {1, 0, 1, 1}, {1, 0, 1, 1},
    {0, 1, 1, 1}, {0, 1, 1, 1}, {0, 1, 1, 1},
    {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}
};

void draw_one_tri_test (int i0, int i1, int i2)
{
    cpy_vec4(v0.position, points[i0]);
    cpy_vec4(v1.position, points[i1]);
    cpy_vec4(v2.position, points[i2]);
    cpy_vec4(v0.color, colors[i0]);
    cpy_vec4(v1.color, colors[i1]);
    cpy_vec4(v2.color, colors[i2]);
    draw_triangle(&v0, &v1, &v2);

    
}

void draw_random_triangle (void)
{
    set_vec4(v0.position,
             random_float(-window_size, window_size),
             random_float(-window_size, window_size), 0, 0);
    set_vec4(v1.position,
             random_float(-window_size, window_size),
             random_float(-window_size, window_size), 0, 0);
    set_vec4(v2.position,
             random_float(-window_size, window_size),
             random_float(-window_size, window_size), 0, 0);
    
    set_vec4(v0.color,
             random_float(0, 1),
             random_float(0, 1),
             random_float(0, 1), 1);
    set_vec4(v1.color,
             random_float(0, 1),
             random_float(0, 1),
             random_float(0, 1), 1);
    set_vec4(v2.color, random_float(0, 1), random_float(0, 1),
             random_float(0, 1), 1);
    
    draw_triangle(&v0, &v1, &v2);
    
}

/* multiple triangles, one screen */
void draw_tri_test (void)
{
    draw_random_triangle();
}
