#include "app.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

//#include "macros.h"
#include "util.h"       // random_float
//#include "time.h"
#include "vector.h"

#include "material.h"
#include "texture.h"    // IMAGE typedef, texture and ppm functions
#include "post-processing.h"

#include "point.h"
#include "model.h"
#include "scene.h"
#include "camera.h"
#include "frustum.h" // for setup_clip_frustum
#include "g_buffer.h" //just for mode_deferred_render
#include "opengl.h"
/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern FILE *cb_file;
extern int object_type;
extern int counter;
extern float window_size;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int framebuffer_src = COLOR;             // which buffer to draw from (COLOR/DEPTH)

/* reading in different inputs: OBJ, SCENE (persistence) */
int program_type = 0;

/* for render pipeline */
/* -- vtx stage */
float init_scale = 1.0;         // for obj files from the internet

int manip_mode = ROTATE;        // manipulator: ROTATE, TRANSLATE, or SCALE

MAT4 camera_mat;
float lookat[4] = {0, 0, 100, 0};
float k[4] = {0, 0, 1, 0};
float world_up[4] = {0, 1, 0, 0};

int proj_mode = ORTHO;          // projection type (ORTHO/PERSPECT)
float ortho_vp_scale = 50;
float dz = INIT_DZ;             // init dz in world space for persp projection

/* -- pixel stage (drawing) */
int draw_mode = FRAME;          // draw model as wireframe/filled (FRAME/FILL)
IMAGE texture;                  // final display texture
int texture_idx = 0;            // determine which texture to read from
int material_type = EMERALD;    // material type for each object

/* for post processing */
int post_processing = OFF;
int dof_mode = OFF;             // depth_of_field

/* manipulating objects one at a time */
int curr_objectID = 0;            //ID of current object

/* misc */
//todo: diff texture_idx and material_types for each object in a scene
int obj_has_vnorms = FALSE;     // whether obj supplies vn's
int reading_obj = FALSE;        // whether reading in an OBJ file
int debugging_mode = OFF;       // for testing different features, 1 at a time

/* for drawing peripheral components */
int normal_type = NO_NORMALS;   // if drawing normals (and which type)
int draw_coord_axes = OFF;      // draw object space coord axes
int draw_bounding_box = OFF;    // draw 3D bounding box

int calculate_all_vns = OFF;


char obj_file[MAX_FILE_NAME];   // when program_type == OBJ, "obj_name.obj"
char scene_file[MAX_FILE_NAME];

int light_type = LOCAL_L;         // LOCAL or GLOBAL lights

float near = 1;
float far = 40.0;
int skip;
float camera_transl = 0.3;

int draw_peripherals = OFF;

/*************************************************************************/
/* helper functions                                                    */
/*************************************************************************/
/*
 * gl_printf
 */
void gl_printf( int x, int y, char *s )
{
    int len = strlen( s );
    
    glDisable( GL_LIGHTING );
    
    glColor4f( 0, 0, 0, 1 );
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    
    gluOrtho2D( 0, WIN_W, 0, WIN_H );
    
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    
    glRasterPos2i( x, y );
    
    for( int i = 0; i < len; i++ )
    {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, s[i] );
    }
    
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    
}
/* console logging for some of the more ambiguous knobs */
void print_settings(void)
{
    printf("\n============================\nNEW DISPLAY: %i\n", counter);
    printf("Projection Mode (p):\t%s\n",
           proj_mode ? "PERSPECTIVE" : "ORTHOGRAPHIC");
    printf("Perspect. Correct (c):\t%s\n",
           perspective_correct ? "ON" : "OFF");
    printf("Manip Mode ([tab]):\t%s\n",
           manip_mode ? (manip_mode == 1 ? "TRANSLATE" : "SCALE") : "ROTATE" );
    printf(".....................\n");
    printf("Alpha Blending (b):\t%s\n",
           alpha_blend ? "ON" : "OFF");
    printf("Depth Testing (d):\t%s\n",
           depth_test ? "ON" : "OFF");\
    printf(".....................\n");
    printf("......TEXTURING......\n");
    printf("Texturing (t/P):\t%s\n",
           texturing ? "ON" : "OFF");
    printf("Tex Gen mode (0):\t%s\n",
           tex_gen_name(tex_gen_mode));
    printf("Modulate Type (m/M):\t%s\n",
           modulate ? (modulate_type ? "LIGHT" : "COLOR") : "OFF");
    printf("Bump Mapping (5):\t%s\n",
           bump_mapping ? "ON" : "OFF");
    printf(".....................\n");
    printf(".......LIGHTING......\n");
    printf("Shading Type (h):\t%s\n",
           shading_mode ? (shading_mode == 1 ? "FLAT" : "PHONG") : "NONE");
    printf("Spec Highlight (S):\t%s\n",
           specular_highlight ? "ON" : "OFF");
    printf(".....................\n");
    printf(".........MISC........\n");
    printf("Fog Mode (F):\t\t%s\n",
           fog ? "ON" : "OFF");
    printf("Material Type (1/2):\t%s\n",
           material ? material_name(material_type) : "OFF");
    printf("Post-Processing (3):\t%s\n",
           post_processing ? "ON" : "OFF");
    printf("DOF (4):\t\t%s\n",
           dof_mode ? "ON" : "OFF");
    printf("Buffer (B):\t\t%s\n",
           framebuffer_src ? "COLOR" : "DEPTH");
    printf(".....................\n");
    printf("Debug Mode ([enter]):\t%s\n",
           debugging_mode ? "ON" : "OFF");
    printf("\n============================\n");
}
char settings_str[1000000000];

void gl_print_settings (void)
{
    int next_y = 100;
    sprintf(settings_str, "%s",
            proj_mode ? "PERSPECTIVE" : "ORTHOGRAPHIC");
    gl_printf(5, next_y, "Projection Mode (p):");
    gl_printf(120, next_y, settings_str);
    next_y += 15;
    
    sprintf(settings_str, "%s",
            perspective_correct ? "ON" : "OFF");
    gl_printf(5, next_y, "Perspect. Correct (c):");
    gl_printf(120, next_y, settings_str );
    next_y += 15;

    sprintf(settings_str, "%s",
            manip_mode ? (manip_mode == 1 ? "TRANSLATE" : "SCALE") : "ROTATE" );
    gl_printf(5, next_y, "Manip Mode ([tab]):" );
    gl_printf(120, next_y, settings_str );
    next_y += 15;

    sprintf(settings_str, "%s",
            mode_deferred_render ? "DEFER" : "FORWARD" );
    gl_printf(5, next_y, "Render Mode ([8]):" );
    gl_printf(120, next_y, settings_str );
    next_y += 15;

    sprintf(settings_str, "%s",
            light_type ? "LOCAL" : "GLOBAL" );
    gl_printf(5, next_y, "Light Mode ([7]):" );
    gl_printf(120, next_y, settings_str );
    next_y += 15;
    
    sprintf(settings_str, "%s",
            renderer ? (renderer == 1 ? "SW_HW" : "ALL_HW") : "ALL_SW" );
    gl_printf(5, next_y, "Renderer ([9]):" );
    gl_printf(120, next_y, settings_str );
    next_y += 15;

}

/* for running benchmarks */
void set_display_mode (DISPLAY_MODE *display_mode)
{
    draw_mode =             display_mode->raster_mode;
    proj_mode =             display_mode->projection;
    alpha_blend =           display_mode->alpha_blend;
    depth_test =            display_mode->depth_test;
    texturing =             display_mode->texturing;
    modulate =              display_mode->modulate;
    modulate_type =         display_mode->modulate_mode;
    perspective_correct =   display_mode->perspective_correct;
    texture_idx =           display_mode->texture_idx;
    tex_gen_mode =          display_mode->uv_generation;
    bump_mapping =          display_mode->bump_map;
    
    shading_mode =          display_mode->shading;
    specular_highlight =    display_mode->lighting;
    light_type =            display_mode->light_type;
    
    fog =                   display_mode->fog;
    material =              display_mode->material;
    post_processing =       display_mode->post_processing;
    dof_mode =              display_mode->depth_of_field;
    calculate_all_vns =     display_mode->calculate_all_vns;
}

/* 2-character string corresponding to object with type object_type */
char *object_name (int object_type)
{
    switch (object_type)
    {
        case QUAD:      return "qd";
        case CUBE:      return "cb";
        case MESH:      return "ms";
        case CYLINDER:  return "cl";
        case CONE:      return "cn";
        case SPHERE:    return "sr";
        case TORUS:     return "to";
        case TEAPOT:    return "tp";
        case CAT:       return "ct";
        case DEER:      return "de";
        case BUNNY :    return "bn";
        case BUDDHA :   return "bd";
        case WOLF :     return "wl";
        case TREE :     return "tr";
        default:        return "N/A";
    }
}
/*******************************************************/
/* Reading in texture files */
/*******************************************************/
#define N_PPM_FILES 13
#define N_TEXTURES (N_PPM_FILES + 2)
/* to rotate between ppm files */
char file_names[N_PPM_FILES][100] =
{
    "ppm/rocks_color.ppm",
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

void set_texture (void)
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

/*******************************************************/
/* Render object using 3d graphics pipeline */
/*******************************************************/
void render_object(OBJECT *o)
{
    /*-------------------------------*/         /* start vertex processing */
    start_timer(&vtx_timer);
    
    /* appropriate scaling for different OBJ files */
    switch (o->type)
    {
        case CAT:   o->scale *= 0.01;  break;
        case WOLF:  o->scale *= 0.01;  break;
        case DEER:  o->scale *= 0.005; break;
    }
    build_model(o);

    /* for per-object texturing in SCENE mode */

    /* set material property vecs */
    set_material(material_type);
    /* all OBJ models are at the end of the list */
    reading_obj = o->type >= TEAPOT;
    draw_peripherals = program_type != SCENE ||
                        (program_type == SCENE && o->ID == curr_objectID);
    /*******************/
    /* WORLD SPACE */
    /*******************/
    /* if reading OBJ, for better texture mapping, generate their uv coords */
    if(reading_obj)
    {
        /* need pre-rotation face and vertex normals to get texture coords */
        calculate_face_normals();
        calculate_vertex_normals();
        get_tex_coords();
    }
    if(draw_coord_axes && draw_peripherals)
    {
        insert_coord_axes(o->center[X], o->center[Y], o->center[Z], o->scale);
    }
    world_xforms(o);
    
    /* rotated face and vertex normals */
    calculate_face_normals();
    calculate_vertex_normals();
    if(light_type == LOCAL_L) calculate_light_vectors();
    
    set_backface_flags(&camera);
    set_view_rays(&camera);
    camera_xform (&camera);

    /*******************/
    /* CAMERA SPACE */
    /*******************/
    set_triangle_clip_flags();
    insert_bb_coords();

    switch(proj_mode)
    {
        case ORTHO:
            xform_model(-1, 1, -1, 1, near, far);           break;
            
        case PERSPECT:
            skip = cull_model(near, far);
            if(skip) return;
            o->w = perspective_xform(near, far, -1, 1, -1, 1);     break;
    }
    
    /*******************/
    /* NDC SPACE */
    /*******************/
    viewport_mat_xform(WIN_W, WIN_H);
    
    /*******************/
    /* SCREEN SPACE */
    /*******************/
    set_click_frame (o);
    
    stop_timer(&vtx_timer);
    
    /*-------------------------------*/         /* end vertex processing */
    
    /*-------------------------------*/         /* start pixel processing */
    start_timer(&px_timer);
    printf("draw model\n");
    draw_model(draw_mode);
    
    if(draw_peripherals && !skip)
    {
        draw_local_axes();
        float bb_color[4] = {0, 0, 0, 1};
        draw_3D_bb(bb_color);
    }
    stop_timer(&px_timer);
    
    /*-------------------------------*/         /* end pixel processing */
}

/********************************************/
/* for BASIC mode */
/********************************************/
void init_basic_program (void)
{
    set_camera (&camera, eye, lookat, world_up);
    setup_clip_frustum(near, far);
}

void display_basic_mode (void)
{
    OBJECT *o = &objects[0];
    o->type = object_type;
    o->scale = 0.5;
    o->radii[0] = 0.5;
    o->radii[1] = 1;
    o->scale_vec[X] = (o->scale_vec[X] ? o->scale_vec[X] : 1);
    o->scale_vec[Y] = (o->scale_vec[Y] ? o->scale_vec[Y] : 1);
    o->scale_vec[Z] = (o->scale_vec[Z] ? o->scale_vec[Z] : 1);
    o->center[Z] = near + 2.0;
    render_object(o);
}

/********************************************/
/* for SCENE mode */
/********************************************/
int init_scene_program (int argc, char **argv)
{
    set_camera (&camera, eye, lookat, world_up);
    setup_clip_frustum(near, far);

    strcat(scene_file, argv[2]);
    read_scene(scene_file);
    program_type = SCENE;
    return 0;
}

void display_scene_mode (void)
{
    for(int i = 0; i < num_objects; i++)
    {

        /* set draw state */
//        texturing = objects[i].texturing;
//        texture_idx = objects[i].texture_idx;
//        tex_gen_mode = (objects[i].cube_map ? CUBE_MAP : NAIVE);
//        set_texture();
        
        objects[i].scale_vec[X] =
        (objects[i].scale_vec[X] ? objects[i].scale_vec[X] : 1);
        objects[i].scale_vec[Y] =
        (objects[i].scale_vec[Y] ? objects[i].scale_vec[Y] : 1);
        objects[i].scale_vec[Z] =
        (objects[i].scale_vec[Z] ? objects[i].scale_vec[Z] : 1);
        render_object(&objects[i]);
    }
}

/********************************************/
/* for BENCHMARK mode */
/********************************************/
/* apply specific benchmark function (called once per frame) */
void apply_benchmark_animation (OBJECT *o, int num_samples)
{
    o->rotation[Y] += (360.0 / num_samples); //rotate total of 360
}

void display_benchmark_mode (int num_samples)
{
    OBJECT *o = &objects[0];
    apply_benchmark_animation(o, num_samples);
    o->type = object_type;
    o->scale = (o->scale ? o->scale : 0.5);
    o->radii[0] = 0.5;
    o->radii[1] = 1;
    o->scale_vec[X] = (o->scale_vec[X] ? o->scale_vec[X] : 1);
    o->scale_vec[Y] = (o->scale_vec[Y] ? o->scale_vec[Y] : 1);
    o->scale_vec[Z] = (o->scale_vec[Z] ? o->scale_vec[Z] : 1);
    render_object(o);
}

/********************************************/
/* for OBJ mode */
/********************************************/
int init_obj_program (int argc, char **argv)
{
    if(argc < 4)
    {
        printf("Please specify scale for OBJ file.\n");
        return -1;
    }
    strcat(obj_file, argv[2]); //get .obj file name
    program_type = OBJ;
    init_scale = atof(argv[3]);
    return 0;
}

void display_obj_mode (void)
{
    OBJECT *o = &objects[0];
    o->type = NA;
    o->scale = init_scale;
    o->center[X] = 0;
    o->center[Y] = 0;
    o->center[Z] = 0;
    o->init_orientation[X] = 0;
    o->init_orientation[Y] = 0;
    o->init_orientation[Z] = 0;
    
    set_model_mat (&o->model_mat, init_scale,
                   o->init_orientation[X], o->init_orientation[Y],
                   o->init_orientation[Z],
                   o->center[X], o->center[Y], o->center[Z]);
    
    read_obj_file(obj_file, &o->model_mat);
    render_object(o);
}

/* for mouse io in main.c */
void translate_object_mouse (float ws_dx, float ws_dy)
{
    OBJECT *curr_object = get_curr_object(curr_objectID);
    curr_object->translate[X] += ws_dx;
    curr_object->translate[Y] += ws_dy;
}

/********************************************/
/* IO */
/********************************************/
void key_callback (unsigned char key)
{
    if(program_type == BENCHMARK)
    {
        switch (key)
        {
            case 'q':
                printf("Quit writing file\n");
                fclose(cb_file);
                exit(0);
                break;
            case '\033':
                printf("Quit writing file\n");
                fclose(cb_file);
                exit(0);
                break;
        }
        return;
    }
    char scene_name[MAX_FILE_NAME - 4] = "";
    OBJECT *curr_object = get_curr_object(curr_objectID);
    
    switch (key)
    {
            /* draw wire frame or fill */
        case 'f':       draw_mode = 1 - draw_mode;                      break;
            
            /* toggle object_type or curr_object */
        case ' ':       object_type = (object_type + 1) % N_TYPES;      break;
            
            /* rotations */
        case 'x':
            if(manip_mode == ROTATE) curr_object->rotation[X] += 10;
            else if(manip_mode == TRANSLATE) curr_object->translate[X] += 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[X] += 0.25;
            break;
        case 'y':
        {
            if(manip_mode == ROTATE) curr_object->rotation[Y] += 10;
            else if(manip_mode == TRANSLATE) curr_object->translate[Y] += 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[Y] += 0.25;
            break;
        }
        case 'z':
        {
            if(manip_mode == ROTATE) curr_object->rotation[Z] += 10;
            else if(manip_mode == TRANSLATE && proj_mode == PERSPECT) curr_object->translate[Z] += 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[Z] += 0.25;
            break;
        }
        case 'X':
            if(manip_mode == ROTATE) curr_object->rotation[X] -= 10;
            else if(manip_mode == TRANSLATE) curr_object->translate[X] -= 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[X] -= 0.25;
            break;
        case 'Y':
        {
            if(manip_mode == ROTATE) curr_object->rotation[Y] -= 10;
            else if(manip_mode == TRANSLATE) curr_object->translate[Y] -= 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[Y] -= 0.25;
            break;
        }
        case 'Z':
        {
            if(manip_mode == ROTATE) curr_object->rotation[Z] -= 10;
            else if(manip_mode == TRANSLATE && proj_mode == PERSPECT) curr_object->translate[Z] -= 0.5;
            else if(manip_mode == SCALE) curr_object->scale_vec[Z] -= 0.25;
            break;
        }
        
            
        case 't':
            if(0)//program_type == SCENE)
            {
                curr_object->texturing = 1 - curr_object->texturing;
            }
            else texturing = 1 - texturing;
            break;
            
        case 'T':
            texture_idx = (texture_idx + 1) % N_TEXTURES;
            set_texture();
            break;
            
        case 'b':       alpha_blend = 1 - alpha_blend;                  break;
        case 'D':       depth_test = 1 - depth_test;                    break;
            
        case 'm':       modulate = 1 - modulate;                        break;
        case 'M':       if(modulate) modulate_type = 1 - modulate_type; break;
            
        case 'n':       normal_type = (normal_type + 1) % 3;            break;
        case 'h':       shading_mode = (shading_mode + 1) % 3;          break;
        case 'F':       fog = 1 - fog;                                  break;
        case 'S':       specular_highlight = 1 - specular_highlight;    break;
            
        case 'p':       proj_mode = 1 - proj_mode;                      break;
        case 'c':       perspective_correct = 1 - perspective_correct;  break;
            
        case 'B':       framebuffer_src = 1 - framebuffer_src;          break;
            
        case 'A':       draw_coord_axes = 1 - draw_coord_axes;          break;
        case 'u':       draw_bounding_box = 1 - draw_bounding_box;      break;
        
        case 'W':
            strcat(scene_name, strtok(scene_file, "."));
            write_scene(strcat(scene_name,"_out.txt"));
            break;
            
        case 'O': write_obj_file("obj/out.obj");                        break;
        
        case 'a':   rotate_camera (&camera, 0, 5, 0);   break;
        case 'd':   rotate_camera (&camera, 0, -5, 0);  break;
        case 'w':   rotate_camera (&camera, -5, 0, 0);  break;
        case 's':   rotate_camera (&camera, 5, 0, 0);   break;
        case 'e':   rotate_camera (&camera, 0, 0, -5);  break;
        case 'r':   rotate_camera (&camera, 0, 0, 5);   break;

        case 'j':   translate_camera (&camera, -camera_transl, 0, 0);   break;
        case 'l':   translate_camera (&camera, camera_transl, 0, 0);    break;
        case 'i':   translate_camera (&camera, 0, camera_transl, 0);    break;
        case 'k':   translate_camera (&camera, 0, -camera_transl, 0);   break;
        case '+':   translate_camera (&camera, 0, 0, camera_transl);    break;
        case '-':   translate_camera (&camera, 0, 0, -camera_transl);   break;

        case '0': tex_gen_mode = (tex_gen_mode + 1) % NUM_TEX_MODES;    break;
        case '1': material = 1 - material;                              break;
        case '2': material_type = (material_type + 1) % NUM_MATERIALS;  break;
        case '3': post_processing = 1 - post_processing;                break;
        case '4': dof_mode = 1 - dof_mode;                              break;
        case '5': bump_mapping = 1 - bump_mapping;                      break;
        case '6': reset_camera(&camera);                                break;
        case '7': light_type = 1 - light_type;                          break;
        case '8': mode_deferred_render = 1 - mode_deferred_render;      break;
        case '9': renderer = (renderer + 1) % 2;                        break;
            
        case '\t': manip_mode = (manip_mode + 1) % NUM_MANIP_MODES;     break;
        case '\r': debugging_mode = 1 - debugging_mode;                 break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
    }
    if(renderer != ALL_SW)
    {
        change_gl_state();
//        passthrough_gl_state();

    }
    else
    {
        passthrough_gl_state();
    }
}

/* after render pipeline, called in main */
void apply_post_pipeline_fx (void)
{
    if(post_processing == ON)
    {
        apply_post_processing();
    }
    if (dof_mode == ON)
    {
        depth_of_field();
    }
}

