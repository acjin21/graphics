#include "app.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "vector.h"

#include "material.h"
#include "texture.h"    // IMAGE typedef, texture and ppm functions
#include "post-processing.h"
#include "buffers.h"

#include "point.h"
#include "model.h"
#include "scene.h"
#include "camera.h"
#include "frustum.h" // for setup_clip_frustum
#include "opengl.h"
#include "light.h"
#include "image.h"
#include "window.h"
/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern FILE *benchmark_output;
//extern int object_type;
extern int counter;
extern float window_size;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
/* for render pipeline */
/* -- vtx stage */
RENDER_STATE current_rs;
APP_STATE current_as;
IMAGE_PROCESSING_STATE current_ips;

float init_scale = 1.0;         // for obj files from the internet

MAT4 camera_mat;
float lookat[4] = {0, 0, 100, 0};
float k[4] = {0, 0, 1, 0};
float world_up[4] = {0, 1, 0, 0};

float ortho_vp_scale = 50;
float dz = INIT_DZ;             // init dz in world space for persp projection

IMAGE texture;                  // final display texture
int post_processing = OFF;
//int dof_mode = OFF;             // depth_of_field

/* misc */
//todo: diff texture_idx and material_types for each object in a scene
int reading_obj = FALSE;        // whether reading in an OBJ file

/* for drawing peripheral components */
int draw_coord_axes = OFF;      // draw object space coord axes
int draw_bounding_box = OFF;    // draw 3D bounding box

int calculate_all_vns = OFF;

char obj_file[MAX_FILE_NAME];   // when current_as.program_type == OBJ, "obj_name.obj"
char scene_file[MAX_FILE_NAME];

float near = 1;
float far = 40.0;
int skip;
float camera_transl = 0.05;

int draw_peripherals = OFF;

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
    current_as.texture_idx %= N_TEXTURES;
    if (current_as.texture_idx < N_PPM_FILES)
    {
        char *ppm_file = file_names[current_as.texture_idx];
        /* to rotate between ppm files */
        read_ppm(ppm_file, &texture);
    }
    else if (current_as.texture_idx == N_PPM_FILES)
    {
        random_texture(&texture);
    }
    else if (current_as.texture_idx == N_PPM_FILES + 1)
    {
        checkerboard_texture(&texture);
    }
}

/*******************************************************/
/* Render object using 3d graphics pipeline */
/*******************************************************/
void render_object(OBJECT *o)
{
    normalize(light);
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


    /* set material property vecs */
    set_material(current_rs.material_type);
    /* all OBJ models are at the end of the list */
    reading_obj = o->type >= TEAPOT;
    draw_peripherals = current_as.program_type != SCENE ||
                        (current_as.program_type == SCENE &&
                         o->ID == current_as.selected_objectID);
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
    
    
    if(current_rs.light_source == POINT_LIGHT) calculate_light_vectors();
    
    set_backface_flags(&camera);
    set_view_rays(&camera);
    if(current_as.draw_normals_mode == F_NORMALS)         insert_normal_coords();

    camera_xform (&camera);

    /*******************/
    /* CAMERA SPACE */
    /*******************/
    set_triangle_clip_flags();
    insert_bb_coords();

    switch(current_as.projection_mode)
    {
        case ORTHO:
            xform_model(-1, 1, -1, 1, near, far);                   break;
            
        case PERSPECT:
            skip = cull_model(near, far);
//            if(skip) return;
            o->w = perspective_xform(near, far, -1, 1, -1, 1);      break;
    }
    
    /*******************/
    /* NDC SPACE */
    /*******************/
    viewport_mat_xform(window_width, window_height);
    
    /*******************/
    /* SCREEN SPACE */
    /*******************/
    set_click_frame (o);
    stop_timer(&vtx_timer);
    /*-------------------------------*/         /* end vertex processing */
    
    /*-------------------------------*/         /* start pixel processing */
    start_timer(&px_timer);
    draw_model(current_rs.draw_mode);
    stop_timer(&px_timer);
    /*-------------------------------*/         /* end pixel processing */

    /* extra drawings -- do not include in benchmarking pixel processing */
    if(draw_peripherals && !skip)
    {
        draw_local_axes();
        float bb_color[4] = {0, 0, 0, 1};
        draw_3D_bb(bb_color);
        draw_face_normals();
    }
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
    o->type = current_rs.object_type;
    o->scale = 1;
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
    current_as.program_type = SCENE;
    return 0;
}

void display_scene_mode (void)
{
    clear_stencil_buffer();

    for(int i = 0; i < num_objects; i++)
    {

        /* set draw state */
        current_rs.texturing_mode = objects[i].texturing;
        current_as.texture_idx = objects[i].texture_idx;
        current_rs.perspective_correction = objects[i].persp_corr;
        current_rs.alpha_blending = objects[i].alpha_blend;
        
        objects[i].scale_vec[X] =
        (objects[i].scale_vec[X] ? objects[i].scale_vec[X] : 1);
        objects[i].scale_vec[Y] =
        (objects[i].scale_vec[Y] ? objects[i].scale_vec[Y] : 1);
        objects[i].scale_vec[Z] =
        (objects[i].scale_vec[Z] ? objects[i].scale_vec[Z] : 1);
        
        current_as.stencil_bufferID = i;
        render_object(&objects[i]);
    }
}

/********************************************/
/* for IMAGE mode */
/********************************************/
void init_image_program (void)
{
    set_camera (&camera, eye, lookat, world_up);
    setup_clip_frustum(near, far);
}
void display_image_mode (void)
{
    clear_texture(&texture, 0, 0, 0, 1);
    char *ppm_file = "ppm/mona_lisa.ascii.pgm";
    read_ppm(ppm_file, &texture);
    
    current_rs.texturing_mode = MANUAL;
    current_as.projection_mode = ORTHO;
    current_rs.draw_mode = FILL;

    OBJECT *o = &objects[0];
    o->type = QUAD;
    o->scale = 1;
    o->rotation[X] = 0;
    
    o->scale_vec[X] = (o->scale_vec[X] ? o->scale_vec[X] : 1);
    o->scale_vec[Y] = (o->scale_vec[Y] ? o->scale_vec[Y] : 1);
    o->scale_vec[Z] = (o->scale_vec[Z] ? o->scale_vec[Z] : 1);
    o->center[Z] = near + 2.0;
    
    render_object(o);
}
/********************************************/
/* IO */
/********************************************/
void image_key_callback (unsigned char key)
{

    switch(key)
    {
        case '0':
            current_ips.processing_mode = NO_FX;
            break;
        case '1':
            current_ips.processing_mode = NEGATIVE;
            break;
        case '2':
            current_ips.processing_mode = FLIP_VERTICAL;
            break;
        case '3':
            current_ips.processing_mode = FLIP_HORIZONTAL;
            break;
        case '4':
            current_ips.processing_mode = LUMINOSITY;
            break;
        case '5':
            current_ips.processing_mode = SEPIA;
            break;
        case '6':
            current_ips.processing_mode = AVG;
            break;
        case '7':
            current_ips.processing_mode = MIN;
            break;
        case '8':
            current_ips.processing_mode = MAX;
            break;
        case '9':
            current_ips.processing_mode = ROTATE_CCW;
            break;
        case 'a':
            current_ips.processing_mode = LINCOLN;
            break;
        case 'b':
            current_ips.processing_mode = FISHEYE;
            break;
        case 'c':
            current_ips.processing_mode = EINSTEIN;
            break;
        case 'd':
            current_ips.processing_mode = OIL_TRANSFER;
            break;
        case 'e':
            current_ips.processing_mode = TILING;
            break;
        case 'f':
            current_ips.processing_mode = EDGE;
            break;
        case 'q':
        case '\033':
            exit(0);
            break;
    }
}
void key_callback (unsigned char key)
{
    char scene_name[MAX_FILE_NAME - 4] = "";
    OBJECT *curr_object = get_curr_object(current_as.selected_objectID);
    if(current_as.program_type == IMAGE_PROCESSING)
    {
            image_key_callback(key);
            return;
    }
    
    switch (key)
    {
            /* draw wire frame or fill */
        case 'f':
            current_rs.draw_mode = 1 - current_rs.draw_mode;
            break;
            
            /* toggle object_type or curr_object */
        case ' ':
            current_rs.object_type = (current_rs.object_type + 1) % N_TYPES;
            break;
            
            /* manipulation */
        case 'x':
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[X] += 10;
            else if(current_as.manipulator_mode == TRANSLATE) curr_object->translate[X] += 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[X] += 0.25;
            break;
        case 'y':
        {
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[Y] += 10;
            else if(current_as.manipulator_mode == TRANSLATE) curr_object->translate[Y] += 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[Y] += 0.25;
            break;
        }
        case 'z':
        {
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[Z] += 10;
            else if(current_as.manipulator_mode == TRANSLATE && current_as.projection_mode == PERSPECT) curr_object->translate[Z] += 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[Z] += 0.25;
            break;
        }
        case 'X':
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[X] -= 10;
            else if(current_as.manipulator_mode == TRANSLATE) curr_object->translate[X] -= 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[X] -= 0.25;
            break;
        case 'Y':
        {
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[Y] -= 10;
            else if(current_as.manipulator_mode == TRANSLATE) curr_object->translate[Y] -= 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[Y] -= 0.25;
            break;
        }
        case 'Z':
        {
            if(current_as.manipulator_mode == ROTATE) curr_object->rotation[Z] -= 10;
            else if(current_as.manipulator_mode == TRANSLATE && current_as.projection_mode == PERSPECT) curr_object->translate[Z] -= 0.5;
            else if(current_as.manipulator_mode == SCALE) curr_object->scale_vec[Z] -= 0.25;
            break;
        }
        
            /* texturing */
        case 't':
            if(current_as.program_type == SCENE) curr_object->texturing = (curr_object->texturing + 1) % (NUM_TEX_MODES);
            else current_rs.texturing_mode = (current_rs.texturing_mode + 1) % (NUM_TEX_MODES);
            break;
            
        case 'T':
            if(current_as.program_type == SCENE) curr_object->texture_idx = (curr_object->texture_idx + 1) % N_TEXTURES;
            else current_as.texture_idx = (current_as.texture_idx + 1) % N_TEXTURES;
            set_texture();
            break;
            
            /* alpha blending */
        case 'b':
            current_rs.alpha_blending = 1 - current_rs.alpha_blending;
            break;
            /* depth testing */
        case 'D':
            current_rs.depth_testing = 1 - current_rs.depth_testing;
            break;
            
        case 'm':
            current_rs.modulation_mode = (current_rs.modulation_mode + 1) % 3;
            break;
            
        case 'n':
            current_as.draw_normals_mode = 1 - current_as.draw_normals_mode;
            break;
        case 'h':
            current_rs.shading_mode = (current_rs.shading_mode + 1) % 3;
            break;
        case '0':
            current_rs.fog_fx = 1 - current_rs.fog_fx;
            break;
        case 'S':
            current_rs.reflection_mode = 1 - current_rs.reflection_mode;
            break;
            
        case 'p':
            current_as.projection_mode = 1 - current_as.projection_mode;
            break;
        case 'c':
            if(current_as.program_type == SCENE) curr_object->persp_corr = 1 - curr_object->persp_corr;
            else current_rs.perspective_correction = 1 - current_rs.perspective_correction;
            break;
            
        case 'B':
            current_as.framebuffer_source = (current_as.framebuffer_source + 1) % 3;
            break;
            
        case 'A':
            draw_coord_axes = 1 - draw_coord_axes;
            break;
        case 'u':
            draw_bounding_box = 1 - draw_bounding_box;
            break;
        
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

        case 'I':
            if(current_rs.light_source == POINT_LIGHT) light_pos[Y] += 10;
            else light[Y] += 0.5;
            break;
        case 'K':
            if(current_rs.light_source == POINT_LIGHT) light_pos[Y] -= 10;
            else light[Y] -= 0.5;
            break;
        case 'J':
            if(current_rs.light_source == POINT_LIGHT) light_pos[X] -= 10;
            else light[X] -= 0.5;
            break;
        case 'L':
            if(current_rs.light_source == POINT_LIGHT) light_pos[X] += 10;
            else light[X] += 0.5;
            break;
            
        case 'F':
            if(current_rs.light_source == POINT_LIGHT) light_pos[Z] -= .5;
            else light[Z] -= 0.5;
            break;

        case 'N':
            if(current_rs.light_source == POINT_LIGHT) light_pos[Z] += .5;
            else light[Z] += 0.5;
            break;
            
        case '1':
            current_rs.material_properties = 1 - current_rs.material_properties;
            break;
        case '2':
            current_rs.material_type = (current_rs.material_type + 1) % NUM_MATERIALS;
            break;
        case '3':
            current_as.post_processing_mode = NO_FX;
            break;
        case '#':
            current_as.post_processing_mode = (current_as.post_processing_mode + 1) % N_IP_MODES;
            break;
        case '4':
            current_as.dof_mode = (current_as.dof_mode + 1) % 3;
            break;
        case '5':
            current_rs.bump_mapping = 1 - current_rs.bump_mapping;
            break;
        case '6':
            reset_camera(&camera);
            break;
        case '7':
            current_rs.light_source = 1 - current_rs.light_source;
            break;
        case '8':
            current_rs.render_mode = 1 - current_rs.render_mode;
            break;
        case '9':
            renderer = (renderer + 1) % 2;
            break;
        case '*':
            current_rs.backface_culling = 1 - current_rs.backface_culling;
            break;
        case '!':
            /* mode 1 */
            current_as.projection_mode = PERSPECT;
            current_rs.perspective_correction = OFF;
            current_rs.depth_testing = ON;
            current_rs.draw_mode = FILL;
            current_rs.backface_culling = OFF;
            break;
        case '@':
            current_rs.object_type = TEAPOT;
            current_as.projection_mode = PERSPECT;
            current_rs.perspective_correction = OFF;
            current_rs.depth_testing = ON;
            current_rs.draw_mode = FILL;
            current_rs.backface_culling = OFF;
            current_rs.texturing_mode = ON;
            current_rs.texturing_mode = CUBE_MAP;
            break;
        case '\t':
            current_as.manipulator_mode = (current_as.manipulator_mode + 1) % NUM_MANIP_MODES;
            break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
    }
    if(renderer != ALL_SW)
    {
        change_gl_state();
    }
    else
    {
        passthrough_gl_state();
    }
}

/* after render pipeline, called in main */
void apply_post_pipeline_fx (void)
{
    if(current_as.program_type == IMAGE_PROCESSING)
    {
        apply_post_processing(current_ips.processing_mode);
        return;
    }
    if(current_as.post_processing_mode != NO_FX)
    {
        apply_post_processing(current_as.post_processing_mode);
    }
    if(current_as.dof_mode != DOF_OFF)
    {
        depth_of_field(current_as.dof_mode);
    }
}

