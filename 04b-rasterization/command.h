#ifndef COMMAND_H
#define COMMAND_H

typedef struct display_mode {
    int raster_mode;
    int projection;
    int alpha_blend;
    int depth_test;
    
    int texturing;
    int modulate;
    int modulate_mode;
    int perspective_correct;
    int texture_idx;
    int uv_generation;
    int bump_map;
    
    int shading;
    int lighting;
    
    int fog;
    int material;
    int post_processing;
    int depth_of_field;
    
    int calculate_all_vns;
} DISPLAY_MODE;


#endif
