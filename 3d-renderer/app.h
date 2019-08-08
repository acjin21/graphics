/* defines functions for high-level user-facing modeling app */

#ifndef APP_H
#define APP_H

#include "scene.h"
#include "time.h"
#include "state.h"

#define INIT_DZ 8
#define MAX_FILE_NAME 100
#define NUM_MANIP_MODES 3

extern RENDER_STATE current_rs;
extern APP_STATE current_as;
extern IMAGE_PROCESSING_STATE current_ips;

/****************/
/* Var Externs */
/****************/
extern int draw_coord_axes;
extern int draw_bounding_box;

extern TIMER vtx_timer;
extern TIMER px_timer;

extern IMAGE texture;                  // final display texture
/*************/
/* Functions */
/*************/
char *object_name (int object_type);

/* textures */
void set_texture (void);

/* rendering */
void render_object(OBJECT *o, CAMERA *c);

void display_image_mode (void);
void display_basic_mode (void);
void display_scene_mode (void);

void key_callback (unsigned char key);

void init_image_program (void);
void init_basic_program(void);
int init_scene_program (int argc, char **argv);

void apply_post_pipeline_fx (void);
#endif
