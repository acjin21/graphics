
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
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "scene.h" //OBJECT
#include "time.h"
#include "command.h"
#include "app.h"
#include "color.h"
#include "depth.h"
#include "macros.h"
#include "texture.h"
#include "post-processing.h"
#include "model.h"

/* from app.c */
extern int program_type;
extern int buffer;
extern int curr_objectID;

/* from scene.c */              // for drawing multiple 3d objects on screen
extern OBJECT objects[MAX_N_OBJECTS];
extern int num_objects;

/* from time.c */
extern TIMER sw_renderer_timer;
extern TIMER gl_timer;

/* from command.c */
extern DISPLAY_MODE benchmark;
extern IMAGE cube_map[6];
extern IMAGE bump_map;
extern int bump_mapping;

/* for animating / benchmarking */
int display_timer = 0;
FILE *cb_file;

int object_type = QUAD;         // model shape (CUBE/MESH/QUAD)

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int counter = 0;

/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
/*
 * display routine
 */
void display(void)
{
    if( Mojave_WorkAround )
    {
        glutReshapeWindow(2 * window_size,2 * window_size);
        Mojave_WorkAround = 0;
    }
    if( draw_one_frame == 0 ) return;
    
    /*******************************************************/
    /* animation switching */
    /*******************************************************/
    if(program_type == ANIMATE)
    {
        display_timer++;
        if(display_timer % 360 == 0) /* every 360 display() calls */
        {
            OBJECT *o = &objects[0]; /* get one object */
            object_type = (object_type + 1) % N_TYPES; /* cycle through object types for ANIMATE program */
            if(object_type == 0 && display_timer > 360)
            {
                draw_one_frame = 0;
                return;
            }
            o->rotation[Y] = 0;
            
            draw_one_frame = 1;
            glutPostRedisplay();
            print_settings();
        }
    }
    if(program_type != ANIMATE)
    {
        print_settings();
    }

    if(program_type == ANIMATE)
    {
        clear_color_buffer(0.5, 0.5, 0.5, 1);
    }
    else
    {
        clear_color_buffer(1, 1, 1, 1);
    }
    clear_depth_buffer(1.0);
    glPointSize(2.0);
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
    start_timer(&sw_renderer_timer);            /* START SW_RENDERER_TIMER */
    
    if(program_type == BASIC || program_type == ANIMATE)
    {
        display_basic_mode();
    }
    else if(program_type == SCENE)
    {
        display_scene_mode();
    }
    else if(program_type == OBJ)
    {
        display_obj_mode();
    }
    apply_post_pipeline_fx();
    
    stop_timer(&sw_renderer_timer);         /* STOP SW_RENDERER_TIMER */
    fprintf(cb_file, "%s ", object_name(object_type));
    fprintf(cb_file, "%.5f ", elapsed_time(&sw_renderer_timer));
    start_timer(&gl_timer);
    
    //draw color or depth buffer            /* START GL_TIMER */
    buffer == COLOR ? draw_color_buffer() : draw_depth_buffer();
    stop_timer(&gl_timer);                  /* STOP GL_TIMER */
    fprintf(cb_file, "%.5f\n", elapsed_time(&gl_timer));

    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    if(program_type != ANIMATE)
    {
        draw_one_frame = 0;
    }
}

/*
 * Key routine
 */
static void key(unsigned char key, int x, int y)
{
    key_callback(key);
    draw_one_frame = 1;
    glutPostRedisplay();
}

int click_in_bb (int x, int y, OBJECT *o)
{
    return (x > o->bb_bl.position[X] &&
            x < o->bb_tr.position[X] &&
            y > o->bb_bl.position[Y] &&
            y < o->bb_tr.position[Y]);
}

//void glutMouseFunc(void (*func)(int button, int state, int x, int y));
void mouse (int button, int state, int x, int y)
{
    if(program_type == SCENE)
    {
        int screen_x = x - 400;
        int screen_y = y - 400;
        screen_y *= -1; // (flip)
        if(state == GLUT_DOWN)
        {
            for(int i = 0; i < num_objects; i++)
            {
                if(click_in_bb(screen_x, screen_y, &objects[i]))
                {
                    curr_objectID = i;
                }
            }
            draw_one_frame = 1;
            glutPostRedisplay();
        }
    }
}

/*
 * main function
 */
int main(int argc, char **argv)
{
    if(argc == 2 && !strcmp("BASIC", argv[1]))
    {
        program_type = BASIC;
    }
    else if(argc == 2 && !strcmp("ANIMATE", argv[1]))
    {
        program_type = ANIMATE;
    }
    else if(argc < 3)
    {
        printf("Too few arguments.\n");
        return -1;
    }
    glutInit(&argc, argv);
    srand(time(NULL));

    /*
     * create window
     */
    glutInitWindowSize(window_size, window_size);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Software Renderer");

    /*
     * setup display(), key(), and mouse() funtions
     */
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);

    /*
     * setup OpenGL state
     */
    glClearColor(0.7, 0.7, 0.7, 1);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    
    /* get type of file we're reading from (obj vs scene file) */
    if(program_type != BASIC && program_type != ANIMATE)
    {
        if(!strcmp("OBJ", argv[1]))
        {
            init_obj_program(argc, argv);
        }
        else if(!strcmp("SCENE", argv[1]))
        {
            init_scene_program(argc, argv);
        }
        else
        {
            printf("Invalid input file type. Should be either \"OBJ\" or \"SCENE\"\n");
            return -1;
        }
    }
    /************************/
    /* pre-renderloop setup */
    /************************/
    if(program_type == ANIMATE)
    {
        set_display_mode(&benchmark);
    }

    /* load necessary resources */
    if(1)//tex_gen_mode == CUBE_MAP)
    {
        read_ppm("ppm/lmcity_rt.ppm", &cube_map[0]);
        read_ppm("ppm/lmcity_lf.ppm", &cube_map[1]);
        read_ppm("ppm/lmcity_up.ppm", &cube_map[2]);
        read_ppm("ppm/lmcity_dn.ppm", &cube_map[3]);
        read_ppm("ppm/lmcity_bk.ppm", &cube_map[4]);
        read_ppm("ppm/lmcity_ft.ppm", &cube_map[5]);
    }
    if(bump_mapping || program_type == BASIC)
    {
        read_ppm("ppm/rocks_bump.ppm", &bump_map);
    }
    set_texture();
    
    /* open output file for benchmarking */
    char file_name[MAX_FILE_NAME] = "rotate_cb1.txt";
    cb_file = fopen(file_name, "w");
    
    if (cb_file == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        glutMainLoop();

    }
    printf("Done writing cb file to %s\n", file_name);
    fclose(cb_file);
    return 0;
}

