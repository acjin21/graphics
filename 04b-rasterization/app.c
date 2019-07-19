#include "app.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

//#include "macros.h"
#include "util.h"       // random_float
#include "time.h"
#include "vector.h"

#include "material.h"
#include "texture.h"    // IMAGE typedef, texture and ppm functions
#include "post-processing.h"

#include "point.h"
#include "model.h"
#include "scene.h"
#include "camera.h"
#include "frustum.h" // for setup_clip_frustum
/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
extern FILE *cb_file;
extern int object_type;
extern int counter;

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

/*************************************************************************/
/* helper functions                                                    */
/*************************************************************************/
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
            return "to";
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
            return "ERROR: no name corresponding to this object type";
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
        case MESH:      init_mesh(scale, cx, cy, cz);                       break;
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

    rotate_model(cx, cy, cz, o->rotation[X], o->rotation[Y], o->rotation[Z]);

    /**********************/
    /* world space xforms */
    /**********************/

    //translate model
    translate_model_mat(o->translate[X], o->translate[Y], o->translate[Z]);
    
    //scale
    scale_model_mat(o->scale_vec[X], o->scale_vec[Y], o->scale_vec[Z]);
    
    /*******************/
    /* CAMERA SPACE */
    /*****************/
    
    /**********************/
    /* peripheral components: normals, bounding boxes */
    /**********************/
    calculate_face_normals();
    calculate_vertex_normals();
    
    camera_xform (&camera);
    /* lighting */
    if(light_type == LOCAL_L)
    {
        calculate_light_vectors(); /* generate all world points' light vecs */
    }

    float near = 5;
    float far = 20.0;
    int skip;
    setup_clip_frustum(near, far);
    set_triangle_clip_flags();
    insert_bb_coords();

    switch(proj_mode)
    {
        case ORTHO:
            xform_model(-10, 10, -10, 10, 3, 40);
            break;
            
        case PERSPECT:
            skip = cull_model(near, far);
            if(skip) return;
            perspective_xform(near, far, -2, 2, -2, 2);
            break;
    }
    viewport_mat_xform(WIN_W, WIN_H);
    set_click_frame (o);
    
    stop_timer(&vtx_timer);
    /*-------------------------------*/         /* end vertex processing */
    
    /*-------------------------------*/         /* start pixel processing */
    start_timer(&px_timer);
    draw_model(draw_mode);
    
    if((program_type != SCENE || (program_type == SCENE && o->ID == curr_objectID))
       && !skip)
    {
        draw_local_axes();
        float bb_color[4] = {1, 1, 1, 1};
        draw_3D_bb(bb_color);
    }
    stop_timer(&px_timer);
    /*-------------------------------*/         /* end pixel processing */
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
/* for BASIC mode */
/********************************************/
void init_basic_program (void)
{
    set_camera (&camera, eye, lookat, world_up);
}

void display_basic_mode (void)
{
    OBJECT *o = &objects[0];
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
/* for SCENE mode */
/********************************************/
int init_scene_program (int argc, char **argv)
{
    set_camera (&camera, eye, lookat, world_up);

    strcat(scene_file, argv[2]);
    read_scene(scene_file);
    program_type = SCENE;
    return 0;
}

void display_scene_mode (void)
{
    for(int i = 0; i < num_objects; i++)
    {
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

void translate_object(float screen_dx, float screen_dy)
{
    OBJECT *curr_object = get_curr_object(curr_objectID);
    curr_object->translate[X] += (5 * screen_dx / (WIN_W / 2.0));
    curr_object->translate[Y] += (5 * screen_dy / (WIN_H / 2.0));

}

float camera_transl = 0.1;
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
            if(manip_mode == ROTATE)
            {
                curr_object->rotation[X] += 10;
            }
            else if(manip_mode == TRANSLATE)
            {
                curr_object->translate[X] += 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[X] += 0.25;
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
                curr_object->translate[Y] += 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[Y] += 0.25;
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
                curr_object->translate[Z] += 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[Z] += 0.25;
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
                curr_object->translate[X] -= 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[X] -= 0.25;
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
                curr_object->translate[Y] -= 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[Y] -= 0.25;
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
                curr_object->translate[Z] -= 0.5;
            }
            else if(manip_mode == SCALE)
            {
                curr_object->scale_vec[Z] -= 0.25;
            }
            break;
        }
        
            
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
        case 'D':       depth_test = 1 - depth_test;                    break;
            
        case 'm':       modulate = 1 - modulate;                        break;
        case 'M':       if(modulate) modulate_type = 1 - modulate_type; break;
            
        case 'n':       normal_type = (normal_type + 1) % 3;            break;
        case 'h':       shading_mode = (shading_mode + 1) % 3;          break;
        case 'F':       fog = 1 - fog;                                  break;
        case 'S':       specular_highlight = 1 - specular_highlight;    break;
            
            
            /* toggle projection mode */
        case 'p':       proj_mode = 1 - proj_mode;                      break;
            /* toggle perspective-correct texturing */
        case 'c':       perspective_correct = 1 - perspective_correct;  break;
            
            /* toggle between color and depth buffer */
        case 'B':       framebuffer_src = 1 - framebuffer_src;                            break;
            
        case 'A':       draw_coord_axes = 1 - draw_coord_axes;          break;
        case 'u':       draw_bounding_box = 1 - draw_bounding_box;      break;
            
            /* write out scene objects with initial orientation to a scene file */
        case 'W':
            strcat(scene_name, strtok(scene_file, "."));
            write_scene(strcat(scene_name,"_out.txt"));
            break;
            /* write obj file */
        case 'O': write_obj_file("obj/out.obj");                        break;
        
        case 'a':
            rotate_camera (&camera, 0, 5, 0);
            break;
        case 'd':
            rotate_camera (&camera, 0, -5, 0);
            break;
        case 'w':
            rotate_camera (&camera, -5, 0, 0);
            break;
        case 's':
            rotate_camera (&camera, 5, 0, 0);
            break;
        case 'e':
            rotate_camera (&camera, 0, 0, -5);
            break;
        case 'r':
            rotate_camera (&camera, 0, 0, 5);
            break;

        case 'j':
            translate_camera (&camera, -camera_transl, 0, 0);
            break;
        case 'l':
            translate_camera (&camera, camera_transl, 0, 0);
            break;
        case 'i':
            translate_camera (&camera, 0, camera_transl, 0);
            break;
        case 'k':
            translate_camera (&camera, 0, -camera_transl, 0);
            break;
        case '+':
            translate_camera (&camera, 0, 0, camera_transl);
            break;
        case '-':
            translate_camera (&camera, 0, 0, -camera_transl);
            break;

        case '0': tex_gen_mode = (tex_gen_mode + 1) % NUM_TEX_MODES;    break;
        case '1': material = 1 - material;                              break;
        case '2': material_type = (material_type + 1) % NUM_MATERIALS;  break;
        case '3': post_processing = 1 - post_processing;                break;
        case '4': dof_mode = 1 - dof_mode;                              break;
            
        case '5': bump_mapping = 1 - bump_mapping;                      break;
        case '6':
            reset_camera(&camera);
            break;
        case '\t': manip_mode = (manip_mode + 1) % NUM_MANIP_MODES;     break;
        case '\r': debugging_mode = 1 - debugging_mode;                 break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
    }
}

/* after render pipeline */
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

