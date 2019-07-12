#ifndef COMMAND_H
#define COMMAND_H

typedef struct draw_mode {
    int projection;
    int alpha_blend;
    int depth_test;
    
    int texturing;
    int texture_idx;
    int uv_generation;
    int bump_map;
    
    int shading;
    int lighting;
    
    int fog;
    int material;
    int post_processing;
    int img_fx;
} DRAW_MODE;


#endif COMMAND_H
