#ifndef APP_H
#define APP_H

#include "command.h"
#include "scene.h"

#define INIT_DZ 8

/* manipulator modes */
#define NUM_MANIP_MODES 3
//#define ROTATE      0
//#define TRANSLATE   1
//#define SCALE       2
enum Manipulator {ROTATE = 0, TRANSLATE, SCALE};

/* rotation mode */
enum Rotation_Mode {GLOBAL = 0, LOCAL = 1};
//#define GLOBAL 0
//#define LOCAL 1

#define MAX_FILE_NAME 100
/* program types */
//#define OBJ 0
//#define SCENE 1
//#define BASIC 2
//#define ANIMATE 3

enum Program {BASIC = 0, SCENE, OBJ, ANIMATE};


/*************/
/* Functions */
/*************/
/* UI */
void print_settings(void);

/* for benchmarking */
void set_display_mode (DISPLAY_MODE *display_mode);
char *object_name (int object_type);

/* textures */
void set_texture (void);

/* rendering */
void render_object(OBJECT *o);
void display_basic_mode (void);
void display_scene_mode (void);
void display_obj_mode (void);

void key_callback (unsigned char key);
int init_obj_program (int argc, char **argv);
int init_scene_program (int argc, char **argv);

void apply_post_pipeline_fx (void);
#endif
