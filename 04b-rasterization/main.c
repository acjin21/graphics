
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
#include "util.h"       //random_float
#include "color.h" 
#include "depth.h"
#include "raster.h"
#include "point.h"
#include "model.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define DEPTH 0
#define COLOR 1

#define INIT_DZ 5

#define ORTHO 0
#define PERSPECT 1

#define N_MODELS 7
#define QUAD 0
#define CUBE 1
#define MESH 2
#define CYLINDER 3
#define CONE 4
#define SPHERE 5
#define TORUS 6

#define N_SHAPES 5
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;

IMAGE texture;          /* final display texture */

/* toggles/knobs */
int draw_mode = FRAME;  /* draw model as wireframe or filled (FRAME/FILL) */
int proj_mode = ORTHO;  /* projection type (ORTHO/PERSPECT) */
int buffer = COLOR;     /* which buffer to draw from (COLOR/DEPTH) */
extern int depth_test;  /* whether depth testing turned on (OFF/ON) */
extern int texturing;   /* whether texturing is turned on (OFF/ON) */
extern int modulate;    /* whether modulating is turned on (OFF/ON) */
extern int alpha_blend; /* whether alpha blending is turned on (OFF/ON) */
extern int phong_shading;
extern int perspective_correct;

int model = QUAD;       /* model shape (CUBE/MESH/QUAD) */
int texture_idx = 0;
int normals = 0;

/* offset vars */
float dx_angle = 0;     /* init 3D rotation angle about the x axis */
float dy_angle = 0;     /* init 3D rotation angle about the y axis */
float dz_angle = 0;     /* init 3D rotation angle about the z axis */

float dz = INIT_DZ;     /* init dz in world space for perspective projection */

float mesh_da = 0;      /* flowing mesh animation */

/* for drawing multiple 3d shapes on screen */
float shape_centers[N_SHAPES][3];
float shape_scales[N_SHAPES];
/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
int counter = 0;

/* randomly drawn shapes */
void init_shapes_random (void)
{
    for(int i = 0; i < N_SHAPES; i++)
    {
        shape_scales[i] = random_float(1, 5);
        shape_centers[i][X] = random_float(-2, 0);
        shape_centers[i][Y] = random_float(-3, 3);
        shape_centers[i][Z] = random_float(0, 3);
    }
}

/* non-overlapping shapes */
void init_shapes(void)
{
    shape_scales[0] = 2;
    shape_centers[0][X] = -3;
    shape_centers[0][Y] = 3;
    shape_centers[0][Z] = 0;
    
    shape_scales[1] = 2;
    shape_centers[1][X] = 0;
    shape_centers[1][Y] = 3;
    shape_centers[1][Z] = 0;
    
    shape_scales[2] = 2;
    shape_centers[2][X] = 3;
    shape_centers[2][Y] = 3;
    shape_centers[2][Z] = 0;
    
    shape_scales[3] = 2;
    shape_centers[3][X] = -3;
    shape_centers[3][Y] = -3;
    shape_centers[3][Z] = 0;
    
    shape_scales[4] = 2;
    shape_centers[4][X] = 0;
    shape_centers[4][Y] = -3;
    shape_centers[4][Z] = 0;
}

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
    if( draw_one_frame == 0 ) return;
    
    /*******************************************************/
    /* adjustments to make demos work */
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

    /*
     * clear color and depth buffers
     */
    clear_color_buffer(1, 1, 1, 1);
    clear_depth_buffer(1.0);
    glPointSize(2.0);
    
    /* console logging for some of the more ambiguous knobs */
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

    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
//        switch (model)
//        {
//            case CUBE:  init_cube(1, cx, cy, cz);               break;
//            case MESH:  init_mesh(1, cx, cy, cz, mesh_da);      break;
//            case QUAD:  init_quad();                        break;
//            case CYLINDER: init_cylinder(0.5, 2, cx, cy, cz);   break;
//            case CONE: init_cone (0.5, 2, cx, cy, cz);          break;
//            case SPHERE: init_sphere (0.5, cx, cy, cz);         break;
//            case TORUS: init_torus(0.5, 1, cx, cy, cz);         break;
//        }
    float cx, cy, cz, scale;
    for(int i = 0; i < N_SHAPES; i++)
    {
        cx = shape_centers[i][X];
        cy = shape_centers[i][Y];
        cz = shape_centers[i][Z];
        scale = shape_scales[i];
        switch(i)
        {
            case 0:
                init_cube(scale, cx, cy, cz);
                break;
            case 1:
                init_sphere(scale / 2, cx, cy, cz);
                break;
            case 2:
                init_torus(scale / 2, scale, cx, cy, cz);
                break;
            case 3:
                init_cone(scale / 2, scale, cx, cy, cz);
                break;
            case 4:
                init_cylinder(scale / 2, scale, cx, cy, cz);
                break;
        }

        rotate_model(cx, cy, cz, dx_angle, dy_angle, dz_angle);
        calculate_face_normals();
        calculate_vertex_normals();
        if(normals) insert_normal_coords();
        
        switch(proj_mode)
        {
            case ORTHO:
                xform_model(50);
                break;
            case PERSPECT:
                translate_model(dz);
                perspective_xform(3.0, 40.0);
                viewport_xform(30);
                break;
        }
        draw_model(draw_mode);
    }
    //draw color or depth buffer
    buffer == COLOR ? draw_color_buffer() : draw_depth_buffer();
   
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
        /* rotate between diff PPM files */
        case 'P':       texture_idx++;                                  break;
            
        /* draw wire frame or fill */
        case 'f':       draw_mode = 1 - draw_mode;                      break;
        /* toggle model shape between cube and mesh */
        case ' ':       model = (model + 1) % N_MODELS;                 break;
            
        /* rotations */
        case 'x':       dx_angle += 10;                                 break;
        case 'y':       dy_angle += 10;                                 break;
        case 'z':       dz_angle += 10;                                 break;
            
        /* 'tumble' around */
        case 'u':       dx_angle += 10;
                        dy_angle += 10;
                        dz_angle += 10;                                 break;
            
        /* reset rotations and any offsets */
        case 'r':       dx_angle = 0;
                        dy_angle = 0;
                        dz_angle = 0;
                        dz = INIT_DZ;
                        mesh_da = 0;                                    break;
        
        /* Z translation */
        case '+':       if(PERSPECT) dz += 0.20;                        break;
        case '-':       if(PERSPECT) dz -= 0.20;                        break;
        
        /* point drawing modes */
        case 't':       texturing = 1 - texturing;                      break;
        case 'd':       depth_test = 1 - depth_test;                    break;
        case 'm':       modulate = 1 - modulate;                        break;
        case 'b':       alpha_blend = 1 - alpha_blend;                  break;
        case 'h':       phong_shading = 1 - phong_shading;              break;
        case 'T':       texture_idx = (texture_idx + 1) % N_TEXTURES;   break;

            
        /* toggle projection mode */
        case 'p':       proj_mode = 1 - proj_mode;                      break;
        /* toggle perspective-correct texturing */
        case 'C':       perspective_correct = 1 - perspective_correct;  break;
        
        /* toggle between color and depth buffer */
        case 'c':       buffer = 1 - buffer;                            break;
            
        /* flowing mesh animation */
        case 'w':       mesh_da += 0.5;                                 break;
        case 'n':       normals = 1 - normals;                          break;
            
        case 'a':       draw_one_frame = 1;                             break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
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
    
    /*
     * Initialize centers and scales of 3D models
     */
    init_shapes();
    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();

    return 0;
}

