
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
#include <string.h>

#include "macros.h"
#include "vector.h"
#include "texture.h"    // IMAGE typedef, texture and ppm functions
#include "util.h"       // random_float
#include "color.h" 
#include "depth.h"
#include "raster.h"
#include "point.h"
#include "model.h"
#include "scene.h"
#include "material.h"
#include "image.h"

#include "post-processing.h"
#include "time.h"
#include "command.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

#define INIT_DZ 8

/* manipulator modes */
#define NUM_MANIP_MODES 3
#define ROTATE      0
#define TRANSLATE   1
#define SCALE       2

#define GLOBAL 0
#define LOCAL 1

#define MAX_FILE_NAME 100
/* program types */
#define OBJ 0
#define SCENE 1
#define BASIC 2
#define ANIMATE 3

/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
/* from point.c */
extern int alpha_blend;         // whether alpha blending is turned on (OFF/ON)
extern int depth_test;          // whether depth testing turned on (OFF/ON)
extern int texturing;           // whether texturing is turned on (OFF/ON)
extern int modulate;            // whether modulating is turned on (OFF/ON)
extern int perspective_correct;
extern int shading_mode;        // FLAT / PHONG / NONE
extern int bump_mapping;
extern int material;
extern int specular_highlight;
extern int fog;
extern IMAGE bump_map;
extern IMAGE cube_map[6];

/* from model.c */
extern int modulate_type;
extern int tex_gen_mode;

/* from scene.c */              // for drawing multiple 3d objects on screen
extern OBJECT objects[MAX_N_OBJECTS];
extern int num_objects;

/* from time.c */
extern TIMER sw_renderer_timer;
extern TIMER gl_timer;
extern TIMER vtx_timer;          // start and end in render_object()
extern TIMER px_timer;           // start and end in model.c

/* from command.c */
extern DISPLAY_MODE benchmark;
/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int counter = 0;

int buffer = COLOR;             // which buffer to draw from (COLOR/DEPTH)

/* reading in different inputs: OBJ, SCENE (persistence) */
int program_type = 0;
char scene_file[MAX_FILE_NAME]; // when program_type == SCENE, "scene_name.txt"
char obj_file[MAX_FILE_NAME];   // when program_type == OBJ, "obj_name.obj"

/* for render pipeline */
/* -- vtx stage */
int object_type = QUAD;         // model shape (CUBE/MESH/QUAD)
float init_scale = 1.0;         // for obj files from the internet
float mesh_da = 0;              // flowing mesh animation

int manip_mode = ROTATE;        // manipulator: ROTATE, TRANSLATE, or SCALE
int rot_mode = LOCAL;

int proj_mode = ORTHO;          // projection type (ORTHO/PERSPECT)
float ortho_vp_scale = 50;
float dz = INIT_DZ;             // init dz in world space for perspective projection

/* -- pixel stage (drawing) */
int draw_mode = FRAME;          // draw model as wireframe or filled (FRAME/FILL)
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
int debugging_mode = OFF;       // for testing different features, one at a time

/* for drawing peripheral components */
int normal_type = NO_NORMALS;   // if drawing normals (and which type)
int draw_coord_axes = OFF;      // draw object space coord axes
int draw_bounding_box = OFF;    // draw 3D bounding box

int calculate_all_vns = OFF;
int display_timer = 0;
//OBJECT *anim_curr_obj;
FILE *cb_file;


/*************************************************************************/
/* helper functions                                                    */
/*************************************************************************/
char *tex_gen_name (int mode);

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
    printf("Rotation Mode (R):\t%s\n",
           rot_mode ? "LOCAL" : "GLOBAL");
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
    printf("Shading Type (s):\t%s\n",
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
           buffer ? "COLOR" : "DEPTH");
    printf(".....................\n");
    printf("Debug Mode ([enter]):\t%s\n",
           debugging_mode ? "ON" : "OFF");
    printf("\n============================\n");

}

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
    fog =                   display_mode->fog;
    material =              display_mode->material;
    post_processing =       display_mode->post_processing;
    dof_mode =              display_mode->depth_of_field;
    calculate_all_vns =     display_mode->calculate_all_vns;
}

char *object_name (int object_type)
{
    switch (object_type)
    {
        case QUAD:
            return "qd";
        case CUBE:
            return "cb";
        case MESH:
            return "ms";
        case CYLINDER:
            return "cl";
        case CONE:
            return "cn";
        case SPHERE:
            return "sr";
        case TORUS:
            return "tr";
        case TEAPOT:
            return "tp";
        case CAT:
            return "ct";
        case DEER:
            return "de";
        case BUNNY :
            return "bn";
        case BUDDHA :
            return "bd";
        case WOLF :
            return "wl";
        case TREE :
            return "tr";
        default:
            return "ERROR";
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

char *tex_gen_name (int mode)
{
    switch (mode)
    {
        case OFF:
            return "NONE";
        case NAIVE:
            return "NAIVE";
        case CYLINDRICAL:
            return "CYLINDRICAL";
        case SPHERICAL:
            return "SPHERICAL";
        case REFLECTION:
            return "REFLECTION";
        case CUBE_MAP:
            return "CUBE MAP";
        default:
            return "ERROR";
    }
}

/*******************************************************/
/* Render object using 3d graphics pipeline */
/*******************************************************/
void render_object(OBJECT *o)
{
    /*-------------------------------*/         /* start vertex processing */
    start_timer(&vtx_timer);
    float cx, cy, cz, scale, r0, r1;
    cx = o->center[X];
    cy = o->center[Y];
    cz = o->center[Z];
    scale = o->scale;
    r0 = o->radii[0];
    r1 = o->radii[1];
    
    if(program_type == SCENE)
    {
        /* set draw state */
        texturing = o->texturing;
        texture_idx = o->texture_idx;
        tex_gen_mode = (o->cube_map ? CUBE_MAP : NAIVE);
        set_texture();
        printf("%i, %i, %i\n", texturing, texture_idx, tex_gen_mode);
    }

    float rx, ry, rz;
    rx = o->init_orientation[X];// + o->rotation[X];
    ry = o->init_orientation[Y];// + o->rotation[Y];
    rz = o->init_orientation[Z];// + o->rotation[Z];
    
    switch (o->type)
    {
        case CAT:
        case WOLF:
            scale *= 0.01;
            break;
        case DEER:
            scale *= 0.005;
            break;
    }
    /* construct model matrix */
    set_model_mat (&o->model_mat, scale, rx, ry, rz, cx, cy, cz);
    
    set_material(material_type);
    
    
    switch (o->type)
    {
        case QUAD:      init_quad(&o->model_mat);                           break;
        case CUBE:      init_cube (&o->model_mat);                          break;
        case MESH:      init_mesh(scale, cx, cy, cz, mesh_da);              break;
        case CYLINDER:  init_cylinder(r0, scale, cx, cy, cz);               break;
        case CONE:      init_cone (r0, scale, cx, cy, cz);                  break;
        case SPHERE:    init_sphere (r0, cx, cy, cz);                       break;
        case TORUS:     init_torus(r0, r1, cx, cy, cz);                     break;
        case TEAPOT:    read_obj_file("obj/teapot.obj", &o->model_mat);     break;
        case CAT:       read_obj_file("obj/cat.obj", &o->model_mat);        break;
        case DEER:      read_obj_file("obj/deer.obj", &o->model_mat);       break;
        case BUNNY:     read_obj_file("obj/bunnyNV.obj", &o->model_mat);    break;
        case BUDDHA:    read_obj_file("obj/buddha.obj", &o->model_mat);     break;
        case WOLF:      read_obj_file("obj/wolf.obj", &o->model_mat);       break;
        case TREE:      read_obj_file("obj/tree.obj", &o->model_mat);       break;
    }
    if(o->type == TEAPOT || o->type == CAT || o->type == DEER || o->type == BUNNY
       || o->type == BUDDHA || o->type == WOLF || o->type == TREE)
    {
        reading_obj = TRUE;
        calculate_face_normals();
        calculate_vertex_normals();

//        if(!obj_has_vnorms)
//        {
//            calculate_vertex_normals();
//        }
        get_tex_coords();
    }
    else
    {
        reading_obj = FALSE;
    }
    
 
    if(draw_coord_axes)
    {
        if(program_type != SCENE || (program_type == SCENE && o->ID == curr_objectID))
        {
            insert_coord_axes(cx, cy, cz, scale);
        }
    }


    if(rot_mode == LOCAL)
    {
        rotate_model(cx, cy, cz,
                     o->rotation[X],
                     o->rotation[Y],
                     o->rotation[Z]);

    }
    else
    {
        rotate_model(0, 0, 0,
                     o->rotation[X],
                     o->rotation[Y],
                     o->rotation[Z]);
    }

    calculate_face_normals();
    calculate_vertex_normals();

//    if( !reading_obj || (reading_obj && !obj_has_vnorms))
//    {
//
//    }
//    else {
//        printf("not calculating vertex normals\n");
//    }
    if(normal_type == F_NORMALS)
    {

        insert_normal_coords();
    }
    
    insert_bb_coords();

    switch(proj_mode)
    {
        case ORTHO:
            xform_model(ortho_vp_scale);
            break;
        case PERSPECT:
            translate_model(dz);
            int skip = cull_model(5.0, 40.0);
            if(skip)
            {
                return;
            }
            perspective_xform(5.0, 40.0);
            viewport_xform(30);
            break;
    }
    
    /* for detecting mouse clicks */
    set_click_frame (o);
    stop_timer(&vtx_timer);
//    printf("vtx_timer: %.5f\n", elapsed_time(&vtx_timer));
    /*-------------------------------*/         /* end vertex processing */
   
    /*-------------------------------*/         /* start pixel processing */
    start_timer(&px_timer);
    draw_model(draw_mode);

    if(program_type != SCENE || (program_type == SCENE && o->ID == curr_objectID))
    {
        draw_local_axes();
        draw_3D_bb();
    }
    stop_timer(&px_timer);
//    printf("px_timer: %.5f\n", elapsed_time(&px_timer));
    /*-------------------------------*/         /* end pixel processing */
}

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
        if(display_timer % 360 == 0)
        {
            OBJECT *o = &objects[0];
            object_type = (object_type + 1) % N_TYPES;
            if(object_type == 0 && display_timer > 360)
            {
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
    /*******************************************************/
    /* adjustments to make demos work */
    /*******************************************************/

    /* if drawing depth buffer, turn on depth testing
        so there's something to look at. */
//    if(buffer == DEPTH)
//    {
//        depth_test = ON;
//    }

    /* since alpha blending does not blend textures, if alpha blending is on,
        turn off texturing and fill the model. */
//    if(alpha_blend == ON)
//    {
//        draw_mode = FILL;
//        texturing = OFF;
//    }
//    if(proj_mode == ORTHO)
//    {
//        perspective_correct = OFF;
//    }

//    print_settings();
    
    /*
     * clear color and depth buffers
     */
    
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
        OBJECT *o = &objects[0];
        if(program_type == ANIMATE)
        {
            o->rotation[Y] += 1;
            printf("orientation: %f\n", o->rotation[Y]);
        }
        o->type = object_type;
        if(o->scale == 0)
        {
            o->scale = 1;
        }
        o->radii[0] = 0.5;
        o->radii[1] = 1;
        
        render_object(o);
    }
    
    else if(program_type == SCENE)
    {
        printf("scene: %i objects\n", num_objects);
        for(int i = 0; i < num_objects; i++)
        {
            render_object(&objects[i]);
        }
    }
    else //OBJ
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
    
    if(post_processing == ON)
    {
        apply_post_processing();
    }
    if (dof_mode == ON)
    {
        depth_of_field();

    }

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
static void Key(unsigned char key, int x, int y)
{
    if(program_type == ANIMATE)
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
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[X] += 10;
            }
            else if(manip_mode == TRANSLATE)
            {
                curr_object->center[X] += 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale += 0.5;
            }
            break;
        case 'y':
        {
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[Y] += 10;
            }
            else if(manip_mode == TRANSLATE)
            {
                curr_object->center[Y] += 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale += 0.5;
            }
            break;
        }
        case 'z':
        {
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[Z] += 10;
            }
            else if(manip_mode == TRANSLATE && proj_mode == PERSPECT)
            {
                curr_object->center[Z] += 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale += 0.5;
            }
            break;
        }
        case 'X':
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[X] -= 10;
            }
            else if(manip_mode == TRANSLATE)
            {
                curr_object->center[X] -= 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale -= 0.5;
            }
            break;
        case 'Y':
        {
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[Y] -= 10;
            }
            else if(manip_mode == TRANSLATE)
            {
                curr_object->center[Y] -= 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale -= 0.5;
            }
            break;
        }
        case 'Z':
        {
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[Z] -= 10;
            }
            else if(manip_mode == TRANSLATE && proj_mode == PERSPECT)
            {
                curr_object->center[Z] -= 1;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale -= 0.5;
            }
            break;
        }


        case 'R':       rot_mode = 1 - rot_mode;                        break;
        /* flowing mesh animation */
        case 'w':       mesh_da += 0.5;                                 break;
        /* reset rotations and any offsets */
        case 'r':
                        curr_object->rotation[X] = 0;
                        curr_object->rotation[Y] = 0;
                        curr_object->rotation[Z] = 0;
                        dz = INIT_DZ;
                        mesh_da = 0;                                    break;
        
        /* zooming: in (+) /out (-) */
        case '+':       if(proj_mode == PERSPECT) dz -= 0.20;
                        if(proj_mode == ORTHO) ortho_vp_scale += 10;
                        break;
        case '-':       if(proj_mode == PERSPECT) dz += 0.20;
                        if(proj_mode == ORTHO) ortho_vp_scale -= 10;
                        break;
        
        /* point drawing modes */
        case 't':
            if(program_type == SCENE)
            {
                curr_object->texturing = 1 - curr_object->texturing;
            }
            else
            {
                texturing = 1 - texturing;
            }
            break;
        case 'T':       texture_idx = (texture_idx + 1) % N_TEXTURES;
                        set_texture();
                        break;
            
        case 'b':       alpha_blend = 1 - alpha_blend;                  break;
        case 'd':       depth_test = 1 - depth_test;                    break;
            
        case 'm':       modulate = 1 - modulate;                        break;
        case 'M':       if(modulate) modulate_type = 1 - modulate_type; break;
            
        case 'n':       normal_type = (normal_type + 1) % 3;            break;
        case 's':       shading_mode = (shading_mode + 1) % 3;          break;
        case 'F':       fog = 1 - fog;                                  break;
        case 'S':       specular_highlight = 1 - specular_highlight;    break;
        
            
        /* toggle projection mode */
        case 'p':       proj_mode = 1 - proj_mode;                      break;
        /* toggle perspective-correct texturing */
        case 'c':       perspective_correct = 1 - perspective_correct;  break;
        
        /* toggle between color and depth buffer */
        case 'B':       buffer = 1 - buffer;                            break;
            
        case 'a':       draw_coord_axes = 1 - draw_coord_axes;          break;
        case 'u':       draw_bounding_box = 1 - draw_bounding_box;      break;
        
        /* write out scene objects with initial orientation to a scene file */
        case 'W':
            strcat(scene_name, strtok(scene_file, "."));
            write_scene(strcat(scene_name,"_out.txt"));
            break;
       /* write obj file */
        case 'O': write_obj_file("obj/out.obj");                        break;
            
        case '0': tex_gen_mode = (tex_gen_mode + 1) % NUM_TEX_MODES;    break;
        case '1': material = 1 - material;                              break;
        case '2': material_type = (material_type + 1) % NUM_MATERIALS;  break;
        case '3': post_processing = 1 - post_processing;                break;
        case '4': dof_mode = 1 - dof_mode;                              break;

        case '5': bump_mapping = 1 - bump_mapping;                      break;
        
        case '\t': manip_mode = (manip_mode + 1) % NUM_MANIP_MODES;     break;
        case '\r': debugging_mode = 1 - debugging_mode;                 break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
    }
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
            printf("mouse click: (%i, %i)\n", screen_x, screen_y);
            for(int i = 0; i < num_objects; i++)
            {
                if(click_in_bb(screen_x, screen_y, &objects[i]))
                {
                    curr_objectID = i;
                    printf("curr_objectID: %i\n", i);
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
    glutCreateWindow("My OpenGL Window");

    /*
     * setup display(), Key() funtions
     */
    glutDisplayFunc(display);
    glutKeyboardFunc(Key);
    glutMouseFunc(mouse);

    /*
     * setup OpenGL state
     */
    glClearColor(0.7, 0.7, 0.7, 1);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    
    //reading in scenes from command line argument
    
    //get type of file we're reading from (obj vs scene file)
    if(program_type != BASIC && program_type != ANIMATE)
    {
        if(!strcmp("OBJ", argv[1]))
        {
            if(argc < 4)
            {
                printf("Please specify scale for OBJ file.\n");
                return -1;
            }
            strcat(obj_file, argv[2]); //get .obj file name
            program_type = OBJ;
            init_scale = atof(argv[3]);
        }
        else if(!strcmp("SCENE", argv[1]))
        {
            strcat(scene_file, argv[2]);
            read_scene(scene_file);
            program_type = SCENE;
        }
        else
        {
            printf("Invalid input file type. Should be either \"OBJ\" or \"SCENE\"\n");
            return -1;
        }
    }
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
        
//        read_ppm("ppm/ashcanyon_rt.ppm", &cube_map[0]);
//        read_ppm("ppm/ashcanyon_lf.ppm", &cube_map[1]);
//        read_ppm("ppm/ashcanyon_up.ppm", &cube_map[2]);
//        read_ppm("ppm/ashcanyon_dn.ppm", &cube_map[3]);
//        read_ppm("ppm/ashcanyon_bk.ppm", &cube_map[4]);
//        read_ppm("ppm/ashcanyon_ft.ppm", &cube_map[5]);
        
//        read_ppm("ppm/right.ppm", &cube_map[0]);
//        read_ppm("ppm/left.ppm", &cube_map[1]);
//        read_ppm("ppm/top.ppm", &cube_map[2]);
//        read_ppm("ppm/bottom.ppm", &cube_map[3]);
//        read_ppm("ppm/back.ppm", &cube_map[4]);
//        read_ppm("ppm/front.ppm", &cube_map[5]);
        
//        read_ppm("ppm/test_right.ppm", &cube_map[0]);
//        read_ppm("ppm/test_left.ppm", &cube_map[1]);
//        read_ppm("ppm/test_top.ppm", &cube_map[2]);
//        read_ppm("ppm/test_bottom.ppm", &cube_map[3]);
//        read_ppm("ppm/test_back.ppm", &cube_map[4]);
//        read_ppm("ppm/test_front.ppm", &cube_map[5]);
    }
    if(bump_mapping || program_type == BASIC)
    {
        read_ppm("ppm/rocks_bump.ppm", &bump_map);
        //        read_ppm("ppm/metal_plate_rough.ppm", &bump_map);
        //        read_ppm("ppm/swirl_bump.ppm", &bump_map);
    }
    
    set_texture();
//    char file_name[MAX_FILE_NAME];
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
    /*
     * start loop that calls display() and Key() routines
     */


    printf("Done writing obj file to %s\n", file_name);
    fclose(cb_file);
    return 0;
}

