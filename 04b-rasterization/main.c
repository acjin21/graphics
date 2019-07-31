
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
#include "window.h"

#include "time.h"
#include "buffers.h"

#include "app.h"

#include "fourier.h"
#include "model.h"
#include "vector.h"
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
        glutReshapeWindow(DEFAULT_WIN_W, DEFAULT_WIN_H);
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
    glClearDepth( 1.0 );
//    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClearColor(1, 1, 1, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(renderer == ALL_SW)
    {
        clear_color_buffer(1, 1, 1, 1);
        if(mode_deferred_render)    clear_g_buffer(1, 1, 1, 1);
        clear_depth_buffer(1.0);
    }

    glPointSize(2.0);
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
    /* START FRAMERATE TIMER */
    start_timer(&framerate_timer);
    switch (program_type)
    {
        case BASIC: display_basic_mode();                       break;
        case BENCHMARK: display_benchmark_mode(num_samples);    break;
        case SCENE: display_scene_mode();                       break;
        case OBJ: display_obj_mode();                           break;
        default: display_basic_mode();                          break;
    }
    if(renderer == ALL_SW)     apply_post_pipeline_fx();

    if(renderer == ALL_SW)
    {
        if(mode_deferred_render)    draw_g_buffer();
        
        framebuffer_src ?
        (framebuffer_src == COLOR ? draw_color_buffer() : draw_stencil_buffer())
        : draw_depth_buffer();
    }
    stop_timer(&framerate_timer);
    /* STOP FRAMERATE TIMER */

    /*******************************************************/
    /* print on screen */
    /*******************************************************/
    gl_print_settings();
    
    char res[100];
    int next_line_y = 775;
    gl_printf(640, next_line_y, "BENCHMARK:");
    next_line_y -= 15;

    sprintf(res, "Framerate: %.5f fps", 1.0 / elapsed_time(&framerate_timer));
    gl_printf(650, next_line_y, res);
    next_line_y -= 15;

    sprintf(res, "Vertex: %.5f seconds", elapsed_time(&vtx_timer));
    gl_printf(650, next_line_y, res);
    next_line_y -= 15;

    sprintf(res, "Pixel: %.5f seconds", elapsed_time(&px_timer));
    gl_printf(650, next_line_y, res);
    next_line_y -= 15;


    /*******************************************************/
    /* write to cb file */
//    /*******************************************************/
//    if(program_type == BENCHMARK)
//    {
//        fprintf(cb_file, "%s ", object_name(object_type));
//        fprintf(cb_file, "%.5f ", elapsed_time(&sw_renderer_timer));
//        fprintf(cb_file, "%.5f\n", elapsed_time(&gl_timer));
//    }

    /* show results */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    if(program_type != BENCHMARK) draw_one_frame = 0;
    printf("======= END DISPLAY CALL =========\n");
}

static void reshape(int w, int h)
{
    printf("RESHAPE: width = %i, height = %i\n", w, h);
    if(w < MAX_WIN_W && h < MAX_WIN_H)
    {
        window_width = w;
        window_height = h;
        draw_one_frame = 1;
        glutPostRedisplay();
    }
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

    y = window_height - y;
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
    
    curr_object->translate[X] += ws_dx;
    curr_object->translate[Y] += ws_dy;
    start[X] = x;
    start[Y] = y;
    
    draw_one_frame = 1;
    glutPostRedisplay();
}

/* whether mouse click position is in the 2D screen bounding box of object o */
int click_in_bb (float x, float y, OBJECT *o)
{
    return (x > o->bb_bl.world[X] &&
            x < o->bb_tr.world[X] &&
            y > o->bb_bl.world[Y] &&
            y < o->bb_tr.world[Y]);
}

/* mouse click callback */
void mouse (int button, int state, int x, int y)
{
    y = window_height - y;
    if(state == GLUT_DOWN)
    {
        float screen_coords[4] = {(float)x, (float)y, 0, 0};
        float res[4];

        start[X] = x;
        start[Y] = y;
        
        float closest_z = FLT_MAX;

//        for(int i = 0; i < num_objects; i++)
//        {
////            printf("screen coords (%.2f, %.2f)\n", screen_coords[X], screen_coords[Y]);
//            unproject_screen_to_camera (res, screen_coords, objects[i].w);
////            printf("unprojected cam space (%.2f, %.2f)\n", res[X], res[Y]);
//
//            if(click_in_bb(res[X], res[Y], &objects[i]))
//            {
//                if(objects[i].center[Z] < closest_z)
//                {
//                    closest_z = objects[i].center[Z];
//                    curr_objectID = i;
//                    printf("curr object %i\n", curr_objectID);
//                }
//            }
//        }
        curr_objectID = stencil_buffer[y][x];
        printf("object ID: %i\n", curr_objectID );
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
    glutReshapeFunc(reshape);
    
    gluOrtho2D(-window_size,window_size,-window_size,window_size);

    printf("%s", glGetString(GL_VERSION));

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

    /* load necessary resources */
    if(1)//tex_gen_mode == CUBE_MAP)
    {
        read_ppm("ppm/ashcanyon_rt.ppm", &cube_map[0]);
        read_ppm("ppm/ashcanyon_lf.ppm", &cube_map[1]);
        read_ppm("ppm/ashcanyon_up.ppm", &cube_map[2]);
        read_ppm("ppm/ashcanyon_dn.ppm", &cube_map[3]);
        read_ppm("ppm/ashcanyon_bk.ppm", &cube_map[4]);
        read_ppm("ppm/ashcanyon_ft.ppm", &cube_map[5]);
        
        read_ppm("ppm/test_right.ppm", &cube_map[0]);
        read_ppm("ppm/test_left.ppm", &cube_map[1]);
        read_ppm("ppm/test_top.ppm", &cube_map[2]);
        read_ppm("ppm/test_bottom.ppm", &cube_map[3]);
        read_ppm("ppm/test_back.ppm", &cube_map[4]);
        read_ppm("ppm/test_front.ppm", &cube_map[5]);
    }
    
    read_ppm("ppm/rocks_bump.ppm", &bump_map);
    
    set_texture();

    /* init OpenGL */
    init_gl_state();
    passthrough_gl_state();
    
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

