/* defines functions for high-level user-facing modeling app */

#ifndef APP_H
#define APP_H

#include "scene.h"
#include "time.h"

#define INIT_DZ 8
#define MAX_FILE_NAME 100

/* manipulator modes */
#define NUM_MANIP_MODES 3


enum Manipulator {
    ROTATE = 0,
    TRANSLATE = 1,
    SCALE = 2
};

/* rotation mode */
enum Rotation_Mode {
    GLOBAL = 0,
    LOCAL = 1
};

/* program types */
enum Program {
    BASIC = 0,
    SCENE = 1,
    OBJ = 2,
    BENCHMARK = 3
};

/****************/
/* Var Externs */
/****************/
extern int debugging_mode;

/* for main.c */
extern int framebuffer_src; /* which buffer to draw from (COLOR/DEPTH) */
extern int program_type;
extern int curr_objectID;
extern int stencil_buffer_ID;

/* needed in model.c */
extern int obj_has_vnorms;
extern int draw_coord_axes;
extern int draw_bounding_box;
extern int light_type;
extern int normal_type;         // whether drawing normals or not

/* needed in scene.c */
extern int draw_mode;    /* draw model as wireframe or filled (FRAME/FILL) */
extern int proj_mode;   /* projection type (ORTHO/PERSPECT) */

extern TIMER vtx_timer;
extern TIMER px_timer;

extern IMAGE texture;                  // final display texture
extern float near;
extern float far;
/*************/
/* Functions */
/*************/
/* UI */
void gl_printf( int x, int y, char *s );
void print_settings(void);
void gl_print_settings (void);

/* for benchmarking */
char *object_name (int object_type);

/* textures */
void set_texture (void);

/* rendering */
void render_object(OBJECT *o);

void display_benchmark_mode (int num_samples);
void display_basic_mode (void);
void display_scene_mode (void);
void display_obj_mode (void);

void translate_object_mouse (float ws_dx, float ws_dy);

void key_callback (unsigned char key);
void init_basic_program(void);
int init_obj_program (int argc, char **argv);
int init_scene_program (int argc, char **argv);

void apply_post_pipeline_fx (void);
#endif
