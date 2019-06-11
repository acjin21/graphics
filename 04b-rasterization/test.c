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

POINT v0, v1, v2;

float points[18][4] =
{
    {-250, 100, 0, 0},      {-350, 200, 0, 0},      {0, 300, 0, 0},
    {-400, -100, 0, 0},        {-200, -200, 0, 0},        {-200, 0, 0, 0},
    {200, -250, 0, 0},      {0, -350, 0, 0},        {-400, -400, 0, 0},
    {0, -100, 0, 0},        {200, -100, 0, 0},      {200, 200, 0, 0},
    {300, 300, 0, 0},       {400, 0, 0, 0},         {300, -300, 0, 0},
    {-75, 0, 0, 0},         {0, 200, 0, 0},         {-50, -50, 0, 0}
};

float colors[18][4] =
{
    {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
//    {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1},
    {1, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 1},
    
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
//    draw_triangle(&v0, &v2, &v1);
//    draw_triangle(&v1, &v0, &v2);
//    draw_triangle(&v1, &v2, &v0);
//    draw_triangle(&v2, &v0, &v1);
//    draw_triangle(&v2, &v1, &v0);
    
}

void draw_random_triangle (void)
{
    set_position(&v0, random_float(-400, 400), random_float(-400, 400), 0, 0);
    set_position(&v1, random_float(-400, 400), random_float(-400, 400), 0, 0);
    set_position(&v2, random_float(-400, 400), random_float(-400, 400), 0, 0);
    set_color(&v0, random_float(0, 1), random_float(0, 1), random_float(0, 1), 1);
    set_color(&v1, random_float(0, 1), random_float(0, 1), random_float(0, 1), 1);
    set_color(&v2, random_float(0, 1), random_float(0, 1), random_float(0, 1), 1);
    draw_triangle(&v0, &v1, &v2);
}

//float errata[1][4] =
//{
//    {208.05, -161.72, 0, 0},      {296.93, 25.39, 0, 0},      {298.56, 362.68, 0, 0}
//};

/* multiple triangles, one screen */
void draw_tri_test (void)
{
    
//    draw_one_tri_test(0, 1, 2);
//    draw_one_tri_test(3, 4, 5);
//    draw_one_tri_test(6, 7, 8);
//    draw_one_tri_test(9, 10, 11);
//    draw_one_tri_test(12, 13, 14);
//    draw_one_tri_test(15, 16, 17);
//    random_tests();
    
    draw_random_triangle();
    
    /*********************/
    /* draw calls        */
    /*********************/
    //    draw_triangle(&v0, &v2, &v1);
    //    draw_triangle(&v1, &v0, &v2);
    //    draw_triangle(&v1, &v2, &v0);
    //    draw_triangle(&v2, &v0, &v1);
    //    draw_triangle(&v2, &v1, &v0);

}
