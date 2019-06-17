/*
 * To build:  gcc -framework OpenGL -framework GLUT line.c -o line
 *
 */
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#include "macros.h"
#include "vector.h"
#include "texture.h" //IMAGE typedef, texture and ppm functions
#include "image.h" //uses IMAGE typedef, image processing funcs
#include "util.h" //random_float
#include "color.h" 
#include "depth.h"
#include "raster.h"
#include "point.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

/* for textures and image processing */
IMAGE texture;
IMAGE texture0;
IMAGE texture1;

/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
extern void draw_tri_test(void);
int counter = 0;
int file_index = 0;

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
    
/*************************************************************************/
/* test reading in texture files */
/*************************************************************************/

    char file_names[8][100] =
    {
        "ppm/blackbuck.ascii.ppm",
        "ppm/out.ppm",
        "ppm/feep.ascii.ppm",
        "ppm/feep2.ascii.ppm",
        "ppm/pbmlib.ascii.ppm",
        "ppm/sines.ascii.ppm",
        "ppm/snail.ascii.ppm",
        "ppm/star_field.ascii.ppm"
    };
    file_index %= 8;
    char *ppm_file = file_names[file_index];
    file_index++;
    read_ppm(ppm_file, &texture0);

    
/*************************************************************************/
/* test image processing */
/*************************************************************************/
    
//    fill (&texture, 255, 0, 0);
//    luminosity(&texture0, &texture);
//    negative(&texture0, &texture);
//    flip_vertical(&texture0, &texture);
//    flip_horizontal(&texture0, &texture);
//    luminosity(&texture0, &texture);
//    sepia(&texture0, &texture);
//    avg(&texture0, &texture);
//    min(&texture0, &texture);
//    max(&texture0, &texture);
    
    clear_texture(&texture, 100, 100, 100, 1);
    rotate_ccw(&texture0, &texture, 90);
    
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

