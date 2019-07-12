
#include "command.h"
#include "macros.h" // ON/OFF, PHONG/FLAT/NO_SHADING
#include "scene.h" // projection #defines
#include "model.h" // FILL vs FRAME
#include "point.h" // uv_generation

DISPLAY_MODE benchmark =
{
    .raster_mode = FILL,
    .projection = ORTHO,
    .alpha_blend = OFF,
    .depth_test = ON,
    
    .texturing = ON,
    .modulate = ON,
    .modulate_mode = MOD_LIGHT,
    .perspective_correct = OFF,
    .texture_idx = 0,
    .uv_generation = NAIVE, // only for OBJ files
    .bump_map = ON,
    
    .shading = PHONG,
    .lighting = ON, // corresponds to specular highlight
    
    .fog = OFF,
    .material = OFF,
    .post_processing = OFF,
    .depth_of_field = OFF,
    .calculate_all_vns = ON
};
#define N_MODES 13
