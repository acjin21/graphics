
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
#include "texture.h"    //IMAGE typedef, texture and ppm functions
#include "util.h"       //random_float
#include "color.h" 
#include "depth.h"
#include "raster.h"
#include "point.h"
#include "model.h"
#include "scene.h"
#include "material.h"
#include "post-processing.h"

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

#define INIT_DZ 5

#define N_TYPES 11
#define QUAD 0
#define CUBE 1
#define MESH 2
#define CYLINDER 3
#define CONE 4
#define SPHERE 5
#define TORUS 6
#define TEAPOT 7
#define CAT 8
#define DEER 9
#define LAPTOP 10

#define NA -1

#define GLOBAL 0
#define LOCAL 1

#define MAX_FILE_NAME 100
#define OBJ 0
#define SCENE 1
#define BASIC 2

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
extern int drawing_normals;     // TODO: this may belong here or in model.c
extern int bump_mapping;
extern int material;
extern int specular_highlight;
extern int fog;
extern IMAGE bump_map;

/* from model.c */
extern int modulate_type;
extern int tex_gen_mode;

/* from scene.c */              // for drawing multiple 3d objects on screen
extern OBJECT objects[MAX_N_OBJECTS];
extern int num_objects;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int counter = 0;

int buffer = COLOR;             // which buffer to draw from (COLOR/DEPTH)

/* reading in different inputs: OBJ, SCENE (persistence) */
int input_type = 0;
char scene_file[MAX_FILE_NAME]; // when input_type == SCENE, "scene_name.txt"
char obj_file[MAX_FILE_NAME];   // when input_type == OBJ, "obj_name.obj"

/* for render pipeline */
int object_type = QUAD;         // model shape (CUBE/MESH/QUAD)
float init_scale = 1.0;         // for obj files from the internet
float mesh_da = 0;              // flowing mesh animation

int rot_mode = LOCAL;
//float dx_angle = 0;             // init 3D rotation angle about the x axis
//float dy_angle = 0;             // init 3D rotation angle about the y axis
//float dz_angle = 0;             // init 3D rotation angle about the z axis

int proj_mode = ORTHO;          // projection type (ORTHO/PERSPECT)
float ortho_scale = 50;
float dz = INIT_DZ;             // init dz in world space for perspective projection

int draw_mode = FRAME;          // draw model as wireframe or filled (FRAME/FILL)


/* for post processing */
int post_processing = OFF;
int dof_mode = OFF;             // depth_of_field

/* manipulating objects one at a time */
int curr_objectID = 0;            //ID of current object

/* misc */
//todo: diff texture_idx and material_types for each object in a scene
IMAGE texture;                  // final display texture
int texture_idx = 0;            // determine which texture to read from
int material_type = EMERALD;    // material type for each object
int normal_type = NO_NORMALS;
int obj_has_vnorms = FALSE;
int reading_obj = FALSE;

int draw_coord_axes = OFF;    // draw object space coord axes
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
    printf("Buffer (B):\t\t%s\n",
           buffer ? "COLOR" : "DEPTH");
    printf("Rotation Mode (R):\t%s\n",
           rot_mode ? "LOCAL" : "GLOBAL");
    printf(".....................\n");
    printf("Alpha Blending (b):\t%s\n",
           alpha_blend ? "ON" : "OFF");
    printf("Depth Testing (d):\t%s\n",
           depth_test ? "ON" : "OFF");
    printf("Texturing (t/P):\t%s\n",
           texturing ? "ON" : "OFF");
    printf("Modulate Type (M):\t%s\n",
           modulate ? (modulate_type ? "LIGHT" : "COLOR") : "OFF");
    printf("Normals Type (n):\t%s\n",
           normal_type ? (normal_type == 1 ? "FACE" : "VTX") : "NONE" );
    printf("Shading Type (s):\t%s\n",
           shading_mode ? (shading_mode == 1 ? "FLAT" : "PHONG") : "NONE");
    printf("Fog Mode (F):\t%s\n",
           fog ? "ON" : "OFF");
    printf("Spec Highlight (S):\t%s\n",
           specular_highlight ? "ON" : "OFF");
    printf(".....................\n");
    printf("Material Type (1/2):\t%s\n",
           material ? material_name(material_type) : "OFF");
    printf("Post-Processing (3):\t%s\n",
           post_processing ? "ON" : "OFF");
    printf("DOF (4):\t%s\n",
           dof_mode ? "ON" : "OFF");
    printf("Bump Mapping (5):\t%s\n",
           bump_mapping ? "ON" : "OFF");
    printf(".....................\n");
    printf("Drawing Coord Axes (a):\t%s\n",
           draw_coord_axes ? "ON" : "OFF");
    printf("\n============================\n");
    printf("Tex Gen mode (0):\t%s\n",
           tex_gen_mode ? (tex_gen_mode == 1 ? "NAIVE" : "CYLINDRICAL") : "NONE");
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
    float cx, cy, cz, scale, r0, r1;
    cx = o->center[X];
    cy = o->center[Y];
    cz = o->center[Z];
    scale = o->scale;
    r0 = o->radii[0];
    r1 = o->radii[1];
    set_material(material_type);
    switch (o->type)
    {
        case CUBE:      init_cube(scale, cx, cy, cz);                       break;
        case MESH:      init_mesh(scale, cx, cy, cz, mesh_da);              break;
        case QUAD:      init_quad();                                        break;
        case CYLINDER:  init_cylinder(r0, scale, cx, cy, cz);               break;
        case CONE:      init_cone (r0, scale, cx, cy, cz);                  break;
        case SPHERE:    init_sphere (r0, cx, cy, cz);                       break;
        case TORUS:     init_torus(r0, r1, cx, cy, cz);                     break;
        case TEAPOT:    read_obj_file("obj/teapot.obj", scale, cx, cy, cz); break;
        case CAT:       init_scale = 0.01;
                        read_obj_file("obj/cat.obj", init_scale * scale, 0, 0, 0);  break;
        case DEER:      init_scale = 0.005;
                        read_obj_file("obj/deer.obj", init_scale * scale, 0, 0, 0); break;
        case LAPTOP:    read_obj_file("obj/laptop.obj", scale, 0, 0, 0); break;
    }
    if(o->type == TEAPOT || o->type == CAT || o->type == DEER || o->type == LAPTOP)
    {
        reading_obj = TRUE;
        calculate_face_normals();
        if(!reading_obj || (reading_obj && !obj_has_vnorms))
        {
            calculate_vertex_normals();
        }
        get_tex_coords();
    }
    else
    {
        reading_obj = FALSE;
    }
    
    if(draw_coord_axes)
    {
        insert_coord_axes(cx, cy, cz, scale);
    }
    if(rot_mode == LOCAL)
    {
        rotate_model(cx, cy, cz,
                     o->init_orientation[X] + o->rotation[X],
                     o->init_orientation[Y] + o->rotation[Y],
                     o->init_orientation[Z] + o->rotation[Z]);
       

    }
    else
    {
        rotate_model(0, 0, 0,
                     o->init_orientation[X] + o->rotation[X],
                     o->init_orientation[Y] + o->rotation[Y],
                     o->init_orientation[Z] + o->rotation[Z]);
    }
    calculate_face_normals();
    if(!reading_obj || (reading_obj && !obj_has_vnorms))
    {
        calculate_vertex_normals();
    }
    else {
        printf("not calculating vertex normals\n");
    }
    if(normal_type == F_NORMALS)
    {
        insert_normal_coords();
    }



    switch(proj_mode)
    {
        case ORTHO:
            xform_model(ortho_scale);
            break;
        case PERSPECT:
            translate_model(dz);
            perspective_xform(3.0, 40.0);
            viewport_xform(30);
            break;
    }

    draw_model(draw_mode);
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
    if(bump_mapping)
    {
        read_ppm("ppm/rocks_bump.ppm", &bump_map);
    }
    print_settings();
    set_texture();
    
    /*
     * clear color and depth buffers
     */
    clear_color_buffer(1, 1, 1, 1);
    clear_depth_buffer(1.0);
    glPointSize(2.0);
    counter++;
    /*******************************************************/
    /* 3D MODELING */
    /*******************************************************/

    if(input_type == BASIC)
    {
        OBJECT *o = &objects[0];
        o->type = object_type;
        o->scale = 1;
        o->radii[0] = 0.5;
        o->radii[1] = 1;
        render_object(o);
    }
    
    else if(input_type == SCENE)
    {
        printf("scene: %i objects\n", num_objects);
        for(int i = 0; i < num_objects; i++)
        {
            render_object(&objects[i]);
        }
    }
    else //OBJ
    {
//        printf("FROM OBJ\n");
        OBJECT *o = &objects[0];
        o->type = NA;
        read_obj_file(obj_file, init_scale, 0, 0, 0);
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
    char scene_name[MAX_FILE_NAME - 4] = "";
    OBJECT *curr_object = get_curr_object(curr_objectID);
    
    switch (key)
    {
        /* draw wire frame or fill */
        case 'f':       draw_mode = 1 - draw_mode;                      break;
        
        /* toggle object_type or curr_object */
        case ' ':       if(input_type == BASIC)
                        {
                            object_type = (object_type + 1) % N_TYPES;
                        }
                        else if(input_type == SCENE)
                        {
                            curr_objectID = (curr_objectID + 1) % num_objects;
                        }
                        break;
        
        /* rotations */
        case 'x':       curr_object->rotation[X] += 10;                 break;
        case 'y':       curr_object->rotation[Y] += 10;                 break;
        case 'z':       curr_object->rotation[Z] += 10;                 break;

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
        
        /* Z translation */
        case '+':       if(proj_mode == PERSPECT) dz -= 0.20;
                        if(proj_mode == ORTHO) ortho_scale += 10;
                        break;
        case '-':       if(proj_mode == PERSPECT) dz += 0.20;
                        if(proj_mode == ORTHO) ortho_scale -= 10;
                        break;
        
        /* point drawing modes */
        case 't':       texturing = 1 - texturing;                      break;
        case 'T':       texture_idx = (texture_idx + 1) % N_TEXTURES;   break;
            
        case 'b':       alpha_blend = 1 - alpha_blend;                  break;
        case 'd':       depth_test = 1 - depth_test;                    break;
            
        case 'm':       modulate = 1 - modulate;                        break;
        case 'M':       modulate_type = 1 - modulate_type;              break;
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

        
        /* write out scene objects with initial orientation to a scene file */
        case 'W':
            strcat(scene_name, strtok(scene_file, "."));
            write_scene(strcat(scene_name,"_out.txt"));
            break;
       /* write obj file */
        case 'O': write_obj_file("obj/out.obj");                        break;
        
        case '1': material = 1 - material;                              break;
        case '2': material_type = (material_type + 1) % NUM_MATERIALS;  break;
        case '3': post_processing = 1 - post_processing;                break;
        case '4': dof_mode = 1 - dof_mode;                              break;

        case '5': bump_mapping = 1 - bump_mapping;                      break;
        
        case '0': tex_gen_mode = (tex_gen_mode + 1) % NUM_TEX_MODES;    break;
            
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
    if(argc == 2 && !strcmp("BASIC", argv[1]))
    {
        input_type = BASIC;
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

    /*
     * setup OpenGL state
     */
    glClearColor(0.7, 0.7, 0.7, 1);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    
    //reading in scenes from command line argument
    
    //get type of file we're reading from (obj vs scene file)
    if(input_type != BASIC)
    {
        if(!strcmp("OBJ", argv[1]))
        {
            if(argc < 4)
            {
                printf("Please specify scale for OBJ file.\n");
                return -1;
            }
            strcat(obj_file, argv[2]); //get .obj file name
            input_type = OBJ;
            init_scale = atof(argv[3]);
        }
        else if(!strcmp("SCENE", argv[1]))
        {
            strcat(scene_file, argv[2]);
            read_scene(scene_file);
            input_type = SCENE;
        }
        else
        {
            printf("Invalid input file type. Should be either \"OBJ\" or \"SCENE\"\n");
            return -1;
        }
    }

    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();


    return 0;
}

