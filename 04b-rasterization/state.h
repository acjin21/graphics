#ifndef STATE_H
#define STATE_H

typedef struct render_state {
    /* > 2 options; note: must be EITHER/OR */
    int projection_mode;
    int manipulator_mode;
    /* note: combined texturing and texturing_mode into one var */
    int texturing_mode;     // NONE, NAIVE, CYLINDRICAL, SPHERICAL, ENVIRONMENTAL, CUBE_MAP
    int modulation_mode;    // NONE, LIGHT, COLOR
    int shading_mode;       // NONE, FLAT, PHONG
    int reflection_mode;    // DIFFUSE, SPECULAR
    int framebuffer_source; // COLOR, DEPTH, STENCIL
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
    
    
} RENDER_STATE;
#endif
