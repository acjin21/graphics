
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
#include <float.h>

//#include "macros.h"
#include "time.h"
#include "color.h"
#include "depth.h"

#include "app.h"
#include "benchmark.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

int counter = 0;

/* for animating / benchmarking */
int display_timer = 0;
FILE *cb_file;
int object_type = QUAD;         // model shape (CUBE/MESH/QUAD)

int num_samples = 360;
float start[2], stop[2];
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

    if(program_type == BENCHMARK)
    {
        display_timer++;
        
        if(display_timer % num_samples == 0) /* every 360 display() calls */
        {
            OBJECT *o = &objects[0]; /* get one object */
            object_type = (object_type + 1) % N_TYPES; /* cycle through object types for BENCHMARK program */
            if(object_type == 0 && display_timer > 360)
            {
                draw_one_frame = 0;
                return;
            }
            o->rotation[Y] = 0;
            
            draw_one_frame = 1;
            glutPostRedisplay();
        }
    }
    if(program_type != BENCHMARK)
    {
        print_settings();
    }

    if(program_type == BENCHMARK)
    {
        clear_color_buffer(0.5, 0.5, 0.5, 1);
    }
    else
    {
//        clear_color_buffer(1, 1, 1, 1);
        clear_color_buffer(0, 0, 0, 1);

    }

    clear_depth_buffer(1.0);
    glPointSize(2.0);
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
    start_timer(&sw_renderer_timer);            /* START SW_RENDERER_TIMER */
    
    if(program_type == BASIC)
    {
        display_basic_mode();
    }
    else if(program_type == BENCHMARK)
    {
        display_benchmark_mode(num_samples);
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
    framebuffer_src == COLOR ? draw_color_buffer() : draw_depth_buffer();
    stop_timer(&gl_timer);                  /* STOP GL_TIMER */
    fprintf(cb_file, "%.5f\n", elapsed_time(&gl_timer));

    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    if(program_type != BENCHMARK)
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
//    printf("bl: %.2f, %.2f || tr: %.2f, %.2f\n", o->bb_bl.position[X], o->bb_bl.position[Y],
//           o->bb_tr.position[X], o->bb_tr.position[Y]);
    return (x > o->bb_bl.position[X] &&
            x < o->bb_tr.position[X] &&
            y > o->bb_bl.position[Y] &&
            y < o->bb_tr.position[Y]);
}
//void glutMotionFunc(void (*func)(int x, int y));
void motion(int x, int y)
{
    y = WIN_H - y;// (flip)
    stop[X] = x;
    stop[Y] = y;
    float screen_dx = stop[X] - start[X];
    float screen_dy = stop[Y] - start[Y];
    translate_object(screen_dx, screen_dy);
    
    start[X] = x;
    start[Y] = y;
    
    draw_one_frame = 1;
    glutPostRedisplay();
}
//void glutMouseFunc(void (*func)(int button, int state, int x, int y));
void mouse (int button, int state, int x, int y)
{
    if(program_type == SCENE)
    {

        y = WIN_H - y;// (flip)
        if(state == GLUT_DOWN)
        {
            start[X] = x;
            start[Y] = y;
            
            float closest_z = FLT_MAX;
            for(int i = 0; i < num_objects; i++)
            {
                if(click_in_bb(x, y, &objects[i]))
                {
                    if(objects[i].center[Z] < closest_z)
                    {
                        closest_z = objects[i].center[Z];
                        curr_objectID = i;
                    }
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
    else if(argc == 2 && !strcmp("BENCHMARK", argv[1]))
    {
        program_type = BENCHMARK;
    }
    else if(argc > 2 && !strcmp("SCENE", argv[1]))
    {
        program_type = SCENE;
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
    glutMotionFunc(motion);
    /*
     * setup OpenGL state
     */
    glClearColor(0.7, 0.7, 0.7, 1);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);

    /* get type of file we're reading from (obj vs scene file) */
    if(program_type != BASIC && program_type != BENCHMARK)
    {
        if(!strcmp("OBJ", argv[1]))
        {
            init_obj_program(argc, argv);
        }
        else if(!strcmp("SCENE", argv[1]))
        {
            printf("SCENE\n");
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
    if(program_type == BENCHMARK)
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
    
    read_ppm("ppm/rocks_bump.ppm", &bump_map);
    
    set_texture();
    
    /* open output file for benchmarking */
    char file_name[MAX_FILE_NAME] = "benchmarking/rotate2_cb1.txt";
    cb_file = fopen(file_name, "w");
    
    fprintf(cb_file, "CB1\n");
    fprintf(cb_file, "%i\n\n", num_samples);
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

