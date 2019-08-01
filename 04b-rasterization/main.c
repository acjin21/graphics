
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

#include "app.h"
#include "time.h"
#include "window.h"
#include "buffers.h"
#include "fourier.h"
#include "model.h"
#include "vector.h"
#include "opengl.h"
#include "state.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

int counter = 0;
//int object_type = QUAD;         // model shape (CUBE/MESH/QUAD)

/* for animating / benchmarking */
int display_timer = 0;
FILE *benchmark_output;
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
//    if(current_as.program_type == BENCHMARK)
//    {
//        if(counter % num_samples == 0) /* every 360 display() calls */
//        {
//            OBJECT *o = &objects[0];
//            object_type = (object_type + 1) % N_TYPES; /* cycle through object types */
//            if(object_type == 0 && counter > 360) /* done cycling */
//            {
//                draw_one_frame = 0;
//                return;
//            }
//            o->rotation[Y] = 0;
//            draw_one_frame = 1;
//            glutPostRedisplay();
//        }
//    }
    glClearDepth( 1.0 );
    glClearColor(1, 1, 1, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(renderer == ALL_SW)
    {
        clear_color_buffer(1, 1, 1, 1);
        if(current_rs.render_mode)    clear_g_buffer(1, 1, 1, 1);
        clear_depth_buffer(1.0);
    }

    glPointSize(2.0);
    counter++;
    
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/
    start_timer(&framerate_timer);
    switch (current_as.program_type)
    {
        case BASIC: display_basic_mode();                       break;
        case SCENE: display_scene_mode();                       break;
        default: display_basic_mode();                          break;
    }
    
    if(renderer == ALL_SW)
    {
        apply_post_pipeline_fx();
        if(current_rs.render_mode)    draw_g_buffer();
        switch(current_as.framebuffer_source)
        {
            case COLOR: draw_color_buffer();        break;
            case STENCIL: draw_stencil_buffer();    break;
            case DEPTH: draw_depth_buffer();        break;
        }

    }
    stop_timer(&framerate_timer);
    /*******************************************************/
    /* print on screen */
    /*******************************************************/
//    gl_print_settings();
//    
//    char res[100];
//    int next_line_y = 775;
//    gl_printf(640, next_line_y, "BENCHMARK:");
//    next_line_y -= 15;
//
//    sprintf(res, "Framerate: %.5f fps", 1.0 / elapsed_time(&framerate_timer));
//    gl_printf(650, next_line_y, res);
//    next_line_y -= 15;
//
//    sprintf(res, "Vertex: %.5f seconds", elapsed_time(&vtx_timer));
//    gl_printf(650, next_line_y, res);
//    next_line_y -= 15;
//
//    sprintf(res, "Pixel: %.5f seconds", elapsed_time(&px_timer));
//    gl_printf(650, next_line_y, res);
//    next_line_y -= 15;

    /* show results */
    glutSwapBuffers();
    glutPostRedisplay(); // Necessary for Mojave.
    draw_one_frame = 0;  // if not animating
    printf("======= END DISPLAY CALL =========\n");
}

/*******************************************************/
/* GLUT CALLBACKS */
/*******************************************************/
/* window resizing callback */
static void reshape(int w, int h)
{
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
    OBJECT *curr_object = get_curr_object(current_as.selected_objectID);

    y = window_height - y;
    stop[X] = x;
    stop[Y] = y;
    
    float vp_start[4], vp_stop[4], ws_start[4], ws_stop[4];
    set_vec4(vp_start, start[X], start[Y], 0, 1);
    set_vec4(vp_stop, stop[X], stop[Y], 0, 1);

    //convert vp_start to ws_start
    unproject_screen_to_world (ws_start, vp_start, curr_object->w, current_as.projection_mode);
    //convert vp_stop to ws_stop
    unproject_screen_to_world (ws_stop, vp_stop, curr_object->w, current_as.projection_mode);

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

/* mouse click callback */
void mouse (int button, int state, int x, int y)
{
    y = window_height - y;
    if(state == GLUT_DOWN)
    {
        start[X] = x;
        start[Y] = y;
        
        current_as.selected_objectID = stencil_buffer[y][x];
        draw_one_frame = 1;
        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{
    if(argc == 2 && !strcmp("BASIC", argv[1]))
    {
        current_as.program_type = BASIC;

    }
    else if(argc > 2 && !strcmp("SCENE", argv[1]))
    {
        current_as.program_type = SCENE;
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
        
        current_as.program_type = BASIC;
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

    if(current_as.program_type == BASIC)
    {
        init_basic_program();
    }
    else if(current_as.program_type == SCENE)
    {
        init_scene_program(argc, argv);
    }
    else
    {
        printf("Invalid input file type. Should be \"SCENE\"\n");
        return -1;
    }
    /************************/
    /* pre-renderloop setup */
    /************************/
    set_default_render_state (&current_rs);
    set_default_app_state (&current_as);
    /* load necessary resources */
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
    
    read_ppm("ppm/rocks_bump.ppm", &bump_map);
    
    set_texture();

    /* init OpenGL */
    init_gl_state();
    passthrough_gl_state();
    
    /* open output file for benchmarking */
    char file_name[MAX_FILE_NAME] = "benchmarking/rotate.txt";
    benchmark_output = fopen(file_name, "w");
    
    fprintf(benchmark_output, "%i\n\n", num_samples);
    if (benchmark_output == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        glutMainLoop();

    }
    printf("Done writing benchmark file to %s\n", file_name);
    fclose(benchmark_output);
    return 0;
}

