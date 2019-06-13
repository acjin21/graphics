#include "triangle.h"

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


#define N_VERT(n_err) (3 * (6 + n_err))

float points[N_VERT(14)][4] =
{
    {-250, 100, 0, 0},      {-350, 200, 0, 0},      {0, 300, 0, 0},
    {-400, -100, 0, 0},     {-200, -200, 0, 0},     {-200, 0, 0, 0},
    {200, -250, 0, 0},      {0, -350, 0, 0},        {-400, -400, 0, 0},
    {0, -100, 0, 0},        {200, -100, 0, 0},      {200, 200, 0, 0},
    {300, 300, 0, 0},       {400, 0, 0, 0},         {300, -300, 0, 0},
    {-75, 0, 0, 0},         {0, 200, 0, 0},         {-50, -50, 0, 0},
    
    //errata
    /* 6 */
    {-201.11, -30.68, 0, 0}, {-383.60, 77.15, 0, 0},{-114.23, 71.68, 0, 0},
    /* 7 */
    {-185.41, -259.62, 0, 0}, {-198.95, 288.77, 0, 0},{-294.44, 217.84, 0, 0},
    /* 8 */
    {-214.01, -142.39, 0, 0}, {-329.96, -12.21, 0, 0}, {-377.88, 208.04, 0, 0},
    /* 9 */
    {-254.24, -233.43, 0, 0}, {-21.22, 174.45, 0, 0}, {-100.82, -9.72, 0, 0},
    /* 10 */
    {-34.14, -227.82, 0, 0}, {-161.25, 204.45, 0, 0}, {207.78, 145.71, 0, 0},
    /* 11 */
    {-258.69571, 101.20181, 0, 0}, {99.10535, 64.10852, 0, 0}, {-128.20712, -377.51328, 0, 0},
    /* 12 */
    {-80.09656, 217.02515, 0, 0}, {341.61304, -110.46164, 0, 0}, {271.30670, -148.52766, 0, 0},
    
    //extremely x-major edges, point artifacts at vertices
    {357.75647, 12.73666, 0, 0}, {-334.76651, -20.72269, 0, 0}, {-286.50208, -40.47708, 0, 0}, //
    {-21.42807, -141.35403, 0, 0}, {262.61322, 140.53381, 0, 0}, {352.05750, 230.81219, 0, 0}, // no overlapping x's
    
    {125.88763, -206.78404, 0.00000, 0.00000}, {-219.37938, 90.89993, 0.00000, 0.00000}, {-244.46317, 107.54776, 0.00000, 0.00000},
    {-170.38037, -383.10754, 0.00000, 0.00000}, {311.37482, -323.64474, 0.00000, 0.00000}, {-297.09048, -399.91119, 0.00000, 0.00000},
    {-164.01883, 135.57135, 0.00000, 0.00000}, {147.58612, -320.29736, 0.00000, 0.00000}, {-37.67960, 318.80963, 0.00000, 0.00000},
    {300, 287, 0.00000, 0.00000}, {0, 0, 0.00000, 0.00000}, {-300, -299, 0.00000, 0.00000},
    {115.15228, 164.46851, 0.00000, 0.00000}, {221.35168, 257.58685, 0.00000, 0.00000}, {-337.54013, -237.13881, 0.00000, 0.00000}


    
};

float colors[9][4] =
{
    {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, // r, g, b
    {1, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, // w, blk, blk
    {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}  // all green
};

void draw_one_tri_test (int i0, int i1, int i2)
{
    cpy_vec4(v0.position, points[i0]);
    cpy_vec4(v1.position, points[i1]);
    cpy_vec4(v2.position, points[i2]);
    cpy_vec4(v0.color, colors[0]);
    cpy_vec4(v1.color, colors[1]);
    cpy_vec4(v2.color, colors[2]);
    draw_triangle(&v0, &v1, &v2);
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

/* multiple triangles, one screen */
void draw_tri_test (void)
{
    
//    draw_one_tri_test(0, 1, 2);
//    draw_one_tri_test(3, 4, 5);
//    draw_one_tri_test(6, 7, 8);
//    draw_one_tri_test(9, 10, 11);
//    draw_one_tri_test(12, 13, 14);
//    draw_one_tri_test(15, 16, 17);
    
    //bugs
    /* missing scan line at y = 0 */
//    draw_one_tri_test(18, 19, 20);
//    draw_one_tri_test(21, 22, 23);
//    draw_one_tri_test(24, 25, 26);
//    draw_one_tri_test(27, 28, 29);
//    draw_one_tri_test(30, 31, 32);
//    draw_one_tri_test(33, 34, 35);
//    draw_one_tri_test(36, 37, 38);
    
// missing some scan lines at end of very x major line
//    draw_one_tri_test(39, 40, 41);
//    draw_one_tri_test(42, 43, 44);
//
//    draw_one_tri_test(45, 46, 47);
//    draw_one_tri_test(48, 49, 50);
//    draw_one_tri_test(51, 52, 53);
//    draw_one_tri_test(54, 55, 56);
//    draw_one_tri_test(57, 58, 59); //slope ~= 1, almost line







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
