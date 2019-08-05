#ifndef STATE_H
#define STATE_H

typedef struct render_state {
    /* > 2 options; note: must be EITHER/OR */
    int draw_mode;          //FRAME/FILL
    int object_type;
    /* note: combined texturing and texturing_mode into one var */
    int texturing_mode;     // NONE, NAIVE, CYLINDRICAL, SPHERICAL, ENVIRONMENTAL, CUBE_MAP
    int modulation_mode;    // NONE, LIGHT, COLOR
    int shading_mode;       // NONE, FLAT, PHONG
    int reflection_mode;    // DIFFUSE, SPECULAR
    int light_source;       // POINT, DIRECTIONAL
    
    /* ON/OFF */
    int perspective_correction;
    int depth_testing;
    int alpha_blending;
    int bump_mapping;
    int fog_fx;
    int material_properties;
    int post_processing;
    int depth_of_field;
    int backface_culling;
    
    int material_type;
    int render_mode;
    
    
} RENDER_STATE;

typedef struct app_state {
    int framebuffer_source;
    int program_type;
    int selected_objectID;  // SCENE: value only changes when user clicks on a diff obejct
    int stencil_bufferID;   // SCENE: diff for every object; value changes every display loop
    int draw_normals_mode;
    int manipulator_mode;
    int projection_mode;    // "app only"
    
    int post_processing_mode;
    int dof_mode;
    
    int texture_idx;
} APP_STATE;

typedef struct image_processing_state {
    int processing_mode;
    char *ppm_file;
} IMAGE_PROCESSING_STATE;

/*************************************************************************/
/* ENUMS */
/*************************************************************************/
enum Program_Type       { BASIC, SCENE, IMAGE_PROCESSING};

enum Projection_Mode    { ORTHO, PERSPECT };

enum Manipulator        { ROTATE, TRANSLATE, SCALE };

enum Rotation_Mode      { GLOBAL, LOCAL };

enum Reflection_Mode    { DIFFUSE, SPECULAR };

enum Draw_Mode          { FRAME, FILL };

enum Framebuffer_Source { COLOR, DEPTH, STENCIL};

enum Modulate_Mode      { MOD_OFF, MOD_COLOR, MOD_LIGHT };

enum Shading_Mode       { NO_SHADING, FLAT, PHONG };

enum Light_Type         { POINT_LIGHT, GLOBAL_LIGHT };

enum Draw_Normals_Mode  { NO_NORMALS,  F_NORMALS};

enum Render_Mode        { FORWARD, DEFERRED };


#define NUM_TEX_MODES 7
enum Texture_Mode {
    TEXTURE_OFF = 0,
    MANUAL, 
    NAIVE,
    CYLINDRICAL,
    SPHERICAL,
    REFLECTION,
    CUBE_MAP
};

#define N_TYPES 14
enum Object_Type {
    QUAD,
    CUBE,
    MESH,
    CYLINDER,
    CONE,
    SPHERE,
    TORUS,
    TEAPOT,
    CAT,
    DEER,
    BUNNY,
    BUDDHA,
    WOLF,
    TREE
};

#define N_IP_MODES 16
enum Image_Processing_Mode
{
    NO_FX,
    NEGATIVE,
    FLIP_VERTICAL,
    FLIP_HORIZONTAL,
    LUMINOSITY,
    SEPIA,
    AVG,
    MIN,
    MAX,
    ROTATE_CCW,
    LINCOLN,
    FISHEYE,
    EINSTEIN,
    OIL_TRANSFER,
    TILING,
    EDGE
};

#define EMERALD             0
#define JADE                1
#define OBSIDIAN            2
#define PEARL               3
#define RUBY                4
#define TURQUOISE           5
#define BRASS               6
#define BRONZE              7
#define CHROME              8
#define COPPER              9
#define GOLD                10
#define SILVER              11
#define BLACK_PLASTIC       12
#define CYAN_PLASTIC        13
#define GREEN_PLASTIC       14
#define RED_PLASTIC         15
#define WHITE_PLASTIC       16
#define YELLOW_PLASTIC      17
#define BLACK_RUBBER        18
#define CYAN_RUBBER         19
#define GREEN_RUBBER        20
#define RED_RUBBER          21
#define WHITE_RUBBER        22
#define YELLOW_RUBBER       23

#define NUM_MATERIALS 24

void set_default_render_state (RENDER_STATE *rs);
void set_default_app_state (APP_STATE *as);

#endif
