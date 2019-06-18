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
#include "model.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define IMG_PROC 0
#define MODEL 1

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

int main_mode = MODEL;
int draw_mode = FRAME;

float dx_angle = 0;
float dy_angle = 0;
float dz_angle = 0;
int axis = Z;

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
#define NUM_FILES 12

    if(main_mode == IMG_PROC)
    {
        char file_names[NUM_FILES][100] =
        {
            "ppm/blackbuck.ascii.ppm",
            "ppm/out.ppm",
            "ppm/feep.ascii.ppm",
            "ppm/feep2.ascii.ppm",
            "ppm/pbmlib.ascii.ppm",
            "ppm/sines.ascii.ppm",
            "ppm/snail.ascii.ppm",
            "ppm/star_field.ascii.ppm",
            "ppm/apollonian_gasket.ascii.pgm",
            "ppm/mona_lisa.ascii.pgm",
            "ppm/stop01.ppm",
            "ppm/me.ppm"
        };
        file_index %= NUM_FILES;
        char *ppm_file = file_names[6];
        file_index++;
        
        /* to rotate between ppm files */
        read_ppm(ppm_file, &texture0);
        
        /* for one specific ppm file */
//        read_ppm("ppm/me_square.ppm", &texture0);
//        read_ppm("ppm/mona_lisa.ascii.pgm", &texture0);
//        luminosity(&texture0, &texture1);
//        oil_transfer(&texture0, &texture);
//        tiling(&texture0, &texture);
    }

/*************************************************************************/
/* test image processing */
/*************************************************************************/
    if (main_mode == IMG_PROC)
    {
        //    fill (&texture, 255, 0, 0);
        //    copy(&texture0, &texture);
        switch(counter % 14)
        {
            case 0:
                luminosity(&texture0, &texture);
                write_ppm("out_ppm/luminosity.ppm", &texture);
                break;
            case 1:
                negative(&texture0, &texture);
                write_ppm("out_ppm/negative.ppm", &texture);
                break;
            case 2:
                flip_vertical(&texture0, &texture);
                break;
            case 3:
                flip_horizontal(&texture0, &texture);
                break;
            case 4:
                sepia(&texture0, &texture);
                write_ppm("out_ppm/sepia.ppm", &texture);
                break;
            case 5:
                avg(&texture0, &texture);
                break;
            case 6:
                min(&texture0, &texture);
                break;
            case 7:
                max(&texture0, &texture);
                break;
            case 8:
                clear_texture(&texture, 100, 100, 100, 1);
                rotate_ccw(&texture0, &texture, 90);
                break;
            case 9:
                lincoln(&texture0, &texture, 3);
                write_ppm("out_ppm/lincoln.ppm", &texture);
                break;
            case 10:
                clear_texture(&texture, 100, 100, 100, 1);
                fisheye(&texture0, &texture);
                write_ppm("out_ppm/fisheye.ppm", &texture);
                break;
            case 11:
                clear_texture(&texture, 100, 100, 100, 1);
                einstein(&texture0, &texture);
                write_ppm("out_ppm/einstein.ppm", &texture);
                break;
            case 12:
                clear_texture(&texture, 100, 100, 100, 1);
                luminosity(&texture0, &texture1);
                oil_transfer(&texture1, &texture);
                write_ppm("out_ppm/oil_paint.ppm", &texture);
                break;
            case 13:
                clear_texture(&texture, 100, 100, 100, 1);
                tiling(&texture0, &texture);
                break;
        }
        draw_tri_test();
    }
    
    /*
     * clear color buffer
     */
    glClear(GL_COLOR_BUFFER_BIT );
    clear_color_buffer(1, 1, 1, 1);
    clear_depth_buffer(1.0);
    glPointSize(2.0);
    printf("\nNEW DISPLAY: %i\n", counter);
    counter++;
    
    if(main_mode == MODEL)
    {
        init_cube();
        rotate_model(dx_angle, dy_angle, dz_angle, axis);
        xform_model();
        draw_model(draw_mode);
    }
    
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
        case 'd':       draw_mode = 1 - draw_mode;      break;
        case 'x':       axis = X;   dx_angle += 10;                  break;
        case 'y':       axis = Y;   dy_angle += 10;                  break;
        case 'z':       axis = Z;   dz_angle += 10;                  break;


        case 'a':       draw_one_frame = 1;             break;
        case 'q':       exit(0);                        break;
        case '\033':    exit(0);                        break;
    }
    
    draw_one_frame = 1;
    glutPostRedisplay();
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
    glClearColor(0.7, 0.7, 0.7, 1);
//    glClearColor(255/255.0, 255/255.0,  153/255.0,  1); //light yellow
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

