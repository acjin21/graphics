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
#include "texture.h"    //IMAGE typedef, texture and ppm functions
#include "image.h"      //uses IMAGE typedef, image processing funcs
#include "util.h"       //random_float
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

#define DEPTH 0
#define COLOR 1

#define INIT_DZ 5

#define ORTHO 0
#define PERSPECT 1

#define N_MODELS 6
#define QUAD 0
#define CUBE 1
#define MESH 2
#define CYLINDER 3
#define CONE 4
#define SPHERE 5
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

/* for textures and image processing */
IMAGE texture;          /* final display texture */
IMAGE texture0;
IMAGE texture1;

/* toggles/knobs */
int main_mode = MODEL;  /* demo type (IMG_PROC / MODEL) */
int draw_mode = FRAME;  /* draw model as wireframe or filled (FRAME/FILL) */
int proj_mode = ORTHO;  /* projection type (ORTHO/PERSPECT) */
int buffer = COLOR;     /* which buffer to draw from (COLOR/DEPTH) */
extern int depth_test;  /* whether depth testing turned on (OFF/ON) */
extern int texturing;   /* whether texturing is turned on (OFF/ON) */
extern int modulate;    /* whether modulating is turned on (OFF/ON) */
extern int alpha_blend; /* whether alpha blending is turned on (OFF/ON) */
extern int perspective_correct;
int model = QUAD;       /* model shape (CUBE/MESH/QUAD) */
int texture_idx = 0;
int filter = 0;
int normals = 0;
extern int perspective;
/* offset vars */
float dx_angle = 0;     /* init 3D rotation angle about the x axis */
float dy_angle = 0;     /* init 3D rotation angle about the y axis */
float dz_angle = 0;     /* init 3D rotation angle about the z axis */

float dz = INIT_DZ;     /* init dz in world space for perspective projection */

float mesh_da = 0;      /* flowing mesh animation */

/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
extern void draw_tri_test(void);
int counter = 0;
int file_index = 0; // for image processing

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
    {
        return;
    }
    
    /*******************************************************/
    /* adjustments to make demos look nice */
    /*******************************************************/

    /* if drawing depth buffer, turn on depth testing
        so there's something to look at. */
    if(buffer == DEPTH)
    {
        depth_test = ON;
    }
    /* if modulating, turn on texturing and fill the model. */
    if(modulate == ON)
    {
        texturing = ON;
        draw_mode = FILL;
    }
    /* since alpha blending does not blend textures, if alpha blending is on,
        turn off texturing and fill the model. */
    if(alpha_blend == ON)
    {
        draw_mode = FILL;
        texturing = OFF;
    }
    if(main_mode == IMG_PROC)
    {
        texturing = ON;
    }
    if(proj_mode == ORTHO)
    {
        perspective_correct = OFF;
    }
    /*******************************************************/
    /* Reading in texture files */
    /*******************************************************/
    #define N_PPM_FILES 12
    #define N_TEXTURES (N_PPM_FILES + 2)
    /* to rotate between ppm files */

    char file_names[N_PPM_FILES][100] =
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
        "ppm/me_square.ppm"
    };
//    /* only for image processing */
    if (main_mode == IMG_PROC)
    {
        clear_texture(&texture0, 0, 0, 0, 1);
        file_index %= N_PPM_FILES;
        char *ppm_file = file_names[file_index];
        read_ppm(ppm_file, &texture0);
    }
    else
    {
        texture_idx %= N_TEXTURES;
        if (texture_idx < N_PPM_FILES)
        {
            char *ppm_file = file_names[texture_idx];
            /* to rotate between ppm files */
            read_ppm(ppm_file, &texture);
        }
        else if (texture_idx == N_PPM_FILES)
        {
            random_texture(&texture);
        }
        else if (texture_idx == N_PPM_FILES + 1)
        {
            checkerboard_texture(&texture);
        }
    }
//    checkerboard_texture(&texture);
 

    /*
     * clear color and depth buffers
     */
    clear_color_buffer(1, 1, 1, 1);
    clear_depth_buffer(1.0);
    glPointSize(2.0);
//    draw_tri_test();
    /*******************************************************/
    /* Image processing */
    /*******************************************************/
#define N_FILTERS 14
//    if (main_mode == IMG_PROC)
//    {
//        //    fill (&texture, 255, 0, 0);
//        //    copy(&texture0, &texture);
//        switch(filter % N_FILTERS)
//        {
//            case 0:
//                luminosity(&texture0, &texture);
//                write_ppm("out_ppm/luminosity.ppm", &texture);
//                break;
//            case 1:
//                negative(&texture0, &texture);
//                write_ppm("out_ppm/negative.ppm", &texture);
//                break;
//            case 2:
//                flip_vertical(&texture0, &texture);
//                break;
//            case 3:
//                flip_horizontal(&texture0, &texture);
//                break;
//            case 4:
//                sepia(&texture0, &texture);
//                write_ppm("out_ppm/sepia.ppm", &texture);
//                break;
//            case 5:
//                avg(&texture0, &texture);
//                break;
//            case 6:
//                min(&texture0, &texture);
//                break;
//            case 7:
//                max(&texture0, &texture);
//                break;
//            case 8:
//                clear_texture(&texture, 100, 100, 100, 1);
//                rotate_ccw(&texture0, &texture, 90);
//                break;
//            case 9:
//                lincoln(&texture0, &texture, 3);
//                write_ppm("out_ppm/lincoln.ppm", &texture);
//                break;
//            case 10:
//                clear_texture(&texture, 100, 100, 100, 1);
//                fisheye(&texture0, &texture);
//                write_ppm("out_ppm/fisheye.ppm", &texture);
//                break;
//            case 11:
//                clear_texture(&texture, 100, 100, 100, 1);
//                einstein(&texture0, &texture);
//                write_ppm("out_ppm/einstein.ppm", &texture);
//                break;
//            case 12:
//                clear_texture(&texture, 100, 100, 100, 1);
//                luminosity(&texture0, &texture1);
//                oil_transfer(&texture1, &texture);
//                write_ppm("out_ppm/oil_paint.ppm", &texture);
//                break;
//            case 13:
//                clear_texture(&texture, 100, 100, 100, 1);
//                tiling(&texture0, &texture);
//                break;
//        }
//        glPointSize(2.0);
//        draw_tri_test();
//
//    }
    

    
    /* console logging for some of the more ambiguous knobs */
    if (main_mode == MODEL)
    {
        printf("\n============================\nNEW DISPLAY: %i\n", counter);
        printf("Projection:\t%s\n", proj_mode ? "PERSPECTIVE" : "ORTHOGRAPHIC");
        printf("Perspective Correct:\t%s\n", perspective_correct ? "ON" : "OFF");
        printf("Buffer:\t\t%s\n", buffer ? "COLOR" : "DEPTH");
        printf(".....................\n");
        printf("Alpha blending:\t%s\n", alpha_blend ? "ON" : "OFF");
        printf("Depth testing:\t%s\n", depth_test ? "ON" : "OFF");
        printf("Texturing:\t%s\n", texturing ? "ON" : "OFF");
        printf("Modulating:\t%s\n", modulate ? "ON" : "OFF");
        printf(".....................\n");
    }
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
    if(main_mode == MODEL)
    {
        float center[4] = {0, 0, 0, 0};
        switch (model)
        {
            case CUBE:  init_cube(1, center);           break;
            case MESH:  init_mesh(1, center, mesh_da);  break;
            case QUAD:  init_quad();                    break;
            case CYLINDER: init_cylinder(0.5, 2, center); break;
            case CONE: init_cone (0.5, 2, center);      break;
            case SPHERE: init_sphere (0.5, center);      break;

        }
        
        rotate_model(center, dx_angle, dy_angle, dz_angle);
        calculate_face_normals(); //calculate normals of all the model faces
        calculate_vertex_normals();
        if(normals) insert_normal_coords();
        switch(proj_mode)
        {
            case ORTHO:
                xform_model(300);
                break;
            case PERSPECT:
                translate_model(dz);
                perspective_xform(3.0, 40.0);
                viewport_xform(300);
                break;
        }
        draw_model(draw_mode);
        
        if(buffer == COLOR)
        {
            draw_color_buffer();
        }
        else
        {
            draw_depth_buffer();
        }
    }
    else // IMG_PROC
    {
        draw_tri_test();
        draw_color_buffer();
    }
    
    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    draw_one_frame = 0;
}

/*
 * Key routine
 */
static void Key(unsigned char key, int x, int y)
{
    switch (key)
    {
        /* switch between image processing and 3D modeling modes */
        case 'M':       main_mode = 1 - main_mode;          break;
            
        /*******************************************************/
        /* COMMANDS FOR IMAGE PROC */
        /*******************************************************/
        /* rotate between diff PPM files */
        case 'P':       file_index++; texture_idx++;   filter = 0;         break;
        /* rotate between diff image processing filters */
        case 'F':       filter = (filter + 1) % N_FILTERS;  break;
            
        /*******************************************************/
        /* COMMANDS FOR 3D Modeling */
        /*******************************************************/
        /* draw wire frame or fill */
        case 'f':       draw_mode = 1 - draw_mode;          break;
        /* toggle model shape between cube and mesh */
        case ' ':       model = (model + 1) % N_MODELS;     break;
            
        /* rotations */
        case 'x':       dx_angle += 10;                     break;
        case 'y':       dy_angle += 10;                     break;
        case 'z':       dz_angle += 10;                     break;
            
        /* 'tumble' around */
        case 'u':       dx_angle += 10;
                        dy_angle += 10;
                        dz_angle += 10;                     break;
            
        /* reset rotations and any offsets */
        case 'r':       dx_angle = 0;
                        dy_angle = 0;
                        dz_angle = 0;
                        dz = INIT_DZ;
                        mesh_da = 0;                        break;
        
        /* Z translation */
        case '+':       if(PERSPECT) dz += 0.20;            break;
        case '-':       if(PERSPECT) dz -= 0.20;            break;
        
        /* point drawing modes */
        case 't':       texturing = 1 - texturing;          break;
        case 'd':       depth_test = 1 - depth_test;        break;
        case 'm':       modulate = 1 - modulate;            break;
        case 'b':       alpha_blend = 1 - alpha_blend;      break;
            
        case 'T':       texture_idx = (texture_idx + 1) % N_TEXTURES;   break;

            
        /* toggle projection mode */
        case 'p':       proj_mode = 1 - proj_mode;          break;
        /* toggle perspective-correct texturing */
        case 'C':       perspective_correct = 1 - perspective_correct;  break;
        
        /* toggle between color and depth buffer */
        case 'c':       buffer = 1 - buffer;                break;
            
        /* flowing mesh animation */
        case 'w':       mesh_da += 0.5;                     break;
        case 'n':       normals = 1 - normals;              break;
        case 'a':       draw_one_frame = 1;                 break;
        case 'q':       exit(0);                            break;
        case '\033':    exit(0);                            break;
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
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
//    glPointSize(1.0);

    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}

