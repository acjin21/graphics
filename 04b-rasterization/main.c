
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
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;
int draw_prog = 1;
char scene_file[MAX_FILE_NAME];
char obj_file[MAX_FILE_NAME];
int input_type = 0;
float init_scale = 1.0; //for obj files from the internet

IMAGE texture;          /* final display texture */

/* toggles/knobs */
extern int draw_mode;  /* draw model as wireframe or filled (FRAME/FILL) */
extern int proj_mode;  /* projection type (ORTHO/PERSPECT) */
extern int buffer;     /* which buffer to draw from (COLOR/DEPTH) */

float ortho_scale = 50;
int rot_mode = LOCAL;
float dz = INIT_DZ;     /* init dz in world space for perspective projection */

extern int depth_test;  /* whether depth testing turned on (OFF/ON) */
extern int texturing;   /* whether texturing is turned on (OFF/ON) */
extern int modulate;    /* whether modulating is turned on (OFF/ON) */
extern int alpha_blend; /* whether alpha blending is turned on (OFF/ON) */
extern int shading_mode;
extern int perspective_correct;
extern int modulate_type;

extern int bump_mapping;
extern int material;
extern IMAGE bump_map;
extern int drawing_normals;
extern int fog;
extern int specular_highlight;
int dof_mode = OFF;
/* more knobs */
int texture_idx = 0;            //todo
int material_type = EMERALD;    //todo  //have a diff material type for each object

int object_type = QUAD;       /* model shape (CUBE/MESH/QUAD) //already recorded in scene files */
int normal_type = NO_NORMALS;            //todo or omit-- more of a debugging tool

/* for drawing multiple 3d objects on screen */
extern OBJECT objects[MAX_N_OBJECTS];
extern int num_objects;

/* offset vars */
float dx_angle = 0;     /* init 3D rotation angle about the x axis */
float dy_angle = 0;     /* init 3D rotation angle about the y axis */
float dz_angle = 0;     /* init 3D rotation angle about the z axis */

int post_processing = OFF;
int obj_has_vnorms = OFF;
int reading_obj = OFF;
float mesh_da = 0;      /* flowing mesh animation */

int counter = 0;

/* console logging for some of the more ambiguous knobs */
void print_settings(void)
{
    printf("\n============================\nNEW DISPLAY: %i\n", counter);
    printf("Projection Mode (p):\t%s\n",
           proj_mode ? "PERSPECTIVE" : "ORTHOGRAPHIC");
    printf("Perspect. Correct (C):\t%s\n",
           perspective_correct ? "ON" : "OFF");
    printf("Buffer (c):\t\t%s\n",
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
    printf("Material Type (4/1):\t%s\n",
           material ? material_name(material_type) : "OFF");
    printf("Bump Mapping (3):\t%s\n",
           bump_mapping ? "ON" : "OFF");
    printf("Normals Type (n):\t%s\n",
           normal_type ? (normal_type == 1 ? "FACE" : "VTX") : "NONE" );
    printf("Shading Type (s):\t%s\n",
           shading_mode ? (shading_mode == 1 ? "FLAT" : "PHONG") : "NONE");
    printf("Fog Mode (F):\t%s\n",
           fog ? "ON" : "OFF");
    printf("Post-Processing (2):\t%s\n",
           post_processing ? "ON" : "OFF");
    printf("DOF (5):\t%s\n",
           dof_mode ? "ON" : "OFF");
    printf(".....................\n");
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
        reading_obj = ON;
    }
    else
    {
        reading_obj = OFF;
    }
    
    insert_coord_axes(cx, cy, cz, scale);

    if(rot_mode == LOCAL)
    {
        rotate_model(cx, cy, cz,
                     o->init_orientation[X] + dx_angle,
                     o->init_orientation[Y] + dy_angle,
                     o->init_orientation[Z] + dz_angle);
    }
    else
    {
        rotate_model(0, 0, 0,
                     o->init_orientation[X] + dx_angle,
                     o->init_orientation[Y] + dy_angle,
                     o->init_orientation[Z] + dz_angle);
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
    if(normal_type == V_NORMALS) drawing_normals = ON;
    else
    {
        drawing_normals = OFF;
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
//        printf("BASIC MODE\n");

        OBJECT *o = &objects[0];
        o->type = object_type;
        o->scale = 1;
        o->radii[0] = 0.5;
        o->radii[1] = 1;
        render_object(o);
    }
    
    else if(input_type == SCENE)
    {
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
    switch (key)
    {
        /* rotate between diff PPM files */
        case 'P':       texture_idx++;                                  break;
            
        /* draw wire frame or fill */
        case 'f':       draw_mode = 1 - draw_mode;                      break;
        /* toggle object_type between cube and mesh */
        case ' ':       object_type = (object_type + 1) % N_TYPES;      break;
            
        /* rotations */
        case 'x':       dx_angle += 10;                                 break;
        case 'y':       dy_angle += 10;                                 break;
        case 'z':       dz_angle += 10;                                 break;
        case 'X':       dx_angle -= 10;                                 break;
        case 'Y':       dy_angle -= 10;                                 break;
        case 'Z':       dz_angle -= 10;                                 break;
        case 'R':       rot_mode = 1 - rot_mode;                        break;
        /* 'tumble' around */
        case 'u':       dx_angle += 10;
                        dy_angle += 10;
                        dz_angle += 10;                                 break;
            
        /* reset rotations and any offsets */
        case 'r':       dx_angle = 0;
                        dy_angle = 0;
                        dz_angle = 0;
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
        case 'd':       depth_test = 1 - depth_test;                    break;
        case 'm':       modulate = 1 - modulate;                        break;
        case 'b':       alpha_blend = 1 - alpha_blend;                  break;
        case 's':       shading_mode = (shading_mode + 1) % 3;                break;
            
        case 'T':       texture_idx = (texture_idx + 1) % N_TEXTURES;   break;
        case 'M':       modulate_type = 1 - modulate_type;              break;
            
        /* toggle projection mode */
        case 'p':       proj_mode = 1 - proj_mode;                      break;
        /* toggle perspective-correct texturing */
        case 'C':       perspective_correct = 1 - perspective_correct;  break;
        
        /* toggle between color and depth buffer */
        case 'c':       buffer = 1 - buffer;                            break;
            
        /* flowing mesh animation */
        case 'w':       mesh_da += 0.5;                                 break;
        /* write out the normals */
        case 'n':       normal_type = (normal_type + 1) % 3;                break;
        /* write out scene objects with initial orientation to a scene file */
        case 'W':
            for(int i = 0; i < num_objects; i++)
            {
                OBJECT *o = &objects[i];
                o->init_orientation[X] += dx_angle;
                o->init_orientation[Y] += dy_angle;
                o->init_orientation[Z] += dz_angle;
            }
            strcat(scene_name, strtok(scene_file, "."));
            write_scene(strcat(scene_name,"_out.txt"));
            //temp fix so that writing object files doesnt cause another rotation
            for(int i = 0; i < num_objects; i++)
            {
                OBJECT *o = &objects[i];
                o->init_orientation[X] -= dx_angle;
                o->init_orientation[Y] -= dy_angle;
                o->init_orientation[Z] -= dz_angle;
            }
            break;
            
        case 'O': write_obj_file("obj/out.obj");                        break;

        case '1': material_type = (material_type + 1) % NUM_MATERIALS;  break;
        case '2': post_processing = 1 - post_processing;                break;
        case '3': bump_mapping = 1 - bump_mapping;                      break;
        case '4': material = 1 - material;                              break;
        case '5': dof_mode = 1 - dof_mode;                              break;

        case 'F': fog = 1 - fog;                                        break;
        case 'S': specular_highlight = 1 - specular_highlight;          break;
        case 'a':       draw_one_frame = 1;                             break;
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

