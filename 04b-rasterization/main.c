
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#ifndef WINDOWS
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#ifdef WINDOWS
#include <GL/glut.h>
#endif 

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

#include "fourier.h"
#include "model.h"
#include "vector.h"
#include "g_buffer.h"
#include "opengl.h"
//#include "texture.h" //for IMAGE typedef
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
IMAGE texture;
/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
/* display callback */
void display(void)
{
    if( Mojave_WorkAround )
    {
        glutReshapeWindow(2 * window_size, 2 * window_size);
        Mojave_WorkAround = 0;
        
        //eliminate weird resizing in the beginning
        draw_one_frame = 1;
        glutPostRedisplay();
        return;
    }

    if( draw_one_frame == 0 ) return;
    
    /*******************************************************/
    /* animation switching */
    /*******************************************************/
    if(program_type == BENCHMARK)
    {
        if(counter % num_samples == 0) /* every 360 display() calls */
        {
            OBJECT *o = &objects[0];
            object_type = (object_type + 1) % N_TYPES; /* cycle through object types */
            if(object_type == 0 && counter > 360) /* done cycling */
            {
                draw_one_frame = 0;
                return;
            }
            o->rotation[Y] = 0;
            draw_one_frame = 1;
            glutPostRedisplay();
        }
    }
    if(renderer == ALL_SW)
    {
        clear_color_buffer(1, 1, 1, 1);
        if(mode_deferred_render)    clear_g_buffer(1, 1, 1, 1);
        clear_depth_buffer(1.0);
    }
    else //gl takes care of pixel processing
    {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glPointSize(2.0);
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
/* START SW_RENDERER_TIMER */
    start_timer(&sw_renderer_timer);
    switch (program_type)
    {
        case BASIC: display_basic_mode();                       break;
        case BENCHMARK: display_benchmark_mode(num_samples);    break;
        case SCENE: display_scene_mode();                       break;
        case OBJ: display_obj_mode();                           break;
        default: display_basic_mode();                          break;
    }
    if(renderer == ALL_SW)     apply_post_pipeline_fx();
    stop_timer(&sw_renderer_timer);
/* STOP SW_RENDERER_TIMER */
    
/* START GL_TIMER */
    start_timer(&gl_timer);
    if(renderer == ALL_SW)
    {
        if(mode_deferred_render)    draw_g_buffer();
        framebuffer_src == COLOR ? draw_color_buffer() : draw_depth_buffer();
    }
    stop_timer(&gl_timer);
/* STOP GL_TIMER */
    /*******************************************************/
    /* print on screen */
    /*******************************************************/
    gl_print_settings();
    
    char res[100];
    gl_printf(650, 775, "BENCHMARK:");
    sprintf(res, "sw renderer: %.5f", elapsed_time(&sw_renderer_timer));
    gl_printf(655, 760, res);
    sprintf(res, "gl: %.5f", elapsed_time(&gl_timer));
    gl_printf(655, 745, res);
            
    sprintf(res, "vertex: %.5f", elapsed_time(&vtx_timer));
    gl_printf(655, 730, res);
    sprintf(res, "pixel: %.5f", elapsed_time(&px_timer));
    gl_printf(655, 715, res);

    /*******************************************************/
    /* write to cb file */
    /*******************************************************/
    if(program_type == BENCHMARK)
    {
        fprintf(cb_file, "%s ", object_name(object_type));
        fprintf(cb_file, "%.5f ", elapsed_time(&sw_renderer_timer));
        fprintf(cb_file, "%.5f\n", elapsed_time(&gl_timer));
    }

    /* show results */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    if(program_type != BENCHMARK) draw_one_frame = 0;
}

/* key callback */
static void key(unsigned char key, int x, int y)
{
    key_callback(key);
    draw_one_frame = 1;
    glutPostRedisplay();
}

/* mouse motion callback */
void motion(int x, int y)
{
    OBJECT *curr_object = get_curr_object(curr_objectID);

    y = WIN_H - y;
    stop[X] = x;
    stop[Y] = y;
    
    float vp_start[4], vp_stop[4], ws_start[4], ws_stop[4];
    set_vec4(vp_start, start[X], start[Y], 0, 1);
    set_vec4(vp_stop, stop[X], stop[Y], 0, 1);

    //convert vp_start to ws_start
    unproject_screen_to_world (ws_start, vp_start, curr_object->w);
    //convert vp_stop to ws_stop
    unproject_screen_to_world (ws_stop, vp_stop, curr_object->w);

    //calculate ws_dx, ws_dy, call translate_object_mouse
    float ws_dx = ws_stop[X] - ws_start[X];
    float ws_dy = ws_stop[Y] - ws_start[Y];

    translate_object_mouse(ws_dx, ws_dy);
    start[X] = x;
    start[Y] = y;
    
    draw_one_frame = 1;
    glutPostRedisplay();
}

/* whether mouse click position is in the 2D screen bounding box of object o */
int click_in_bb (float x, float y, OBJECT *o)
{
//    printf("type: %i\nbb_bl: %.2f, %.2f\nbb_tr:%.2f, %.2f\n", o->type,
//           o->bb_bl.world[X], o->bb_bl.world[Y],
//           o->bb_tr.world[X], o->bb_tr.world[Y]);
    return (x > o->bb_bl.world[X] &&
            x < o->bb_tr.world[X] &&
            y > o->bb_bl.world[Y] &&
            y < o->bb_tr.world[Y]);
}

/* mouse click callback */
void mouse (int button, int state, int x, int y)
{
    y = WIN_H - y;
    if(state == GLUT_DOWN)
    {
        float screen_coords[4] = {(float)x, (float)y, 0, 0};
        float res[4];

        start[X] = x;
        start[Y] = y;
        
        float closest_z = FLT_MAX;

        for(int i = 0; i < num_objects; i++)
        {
//            printf("screen coords (%.2f, %.2f)\n", screen_coords[X], screen_coords[Y]);
            unproject_screen_to_camera (res, screen_coords, objects[i].w);
//            printf("unprojected cam space (%.2f, %.2f)\n", res[X], res[Y]);

            if(click_in_bb(res[X], res[Y], &objects[i]))
            {
                if(objects[i].center[Z] < closest_z)
                {
                    closest_z = objects[i].center[Z];
                    curr_objectID = i;
                    printf("curr object %i\n", curr_objectID);
                }
            }
        }
        draw_one_frame = 1;
        glutPostRedisplay();
    }
}

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
    else if(argc == 2 && !strcmp("FOURIER", argv[1]))
    {
        //        read_ppm ("ppm/rocks_color.ppm", &texture);
        //        texture.width = 64;
        //        texture.height = 64;
        checkerboard_texture(&texture);
        
        convert_to_float_image(&texture, &x);
        dft_2D(&x, &r, &i);
        power_spectrum(&r, &i, &p);
        
        convert_to_image( &r, &texture);
        write_ppm ("dft/checkerboard_raw.ppm", &texture);
        convert_to_image( &p, &texture);
        write_ppm ("dft/checkerboard_power.ppm", &texture);
        
        invdft_2D(&r, &i, &x, &y);
        convert_to_image( &x, &texture);
        write_ppm ("dft/checkerboard_dft.ppm", &texture);
        
        program_type = BASIC;
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

    
    /* init OpenGL */
    init_gl_state();
    passthrough_gl_state();
    
    if(program_type == BASIC)
    {
        init_basic_program();
    }
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

