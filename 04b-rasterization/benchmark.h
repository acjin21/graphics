#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "scene.h"

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
    int light_type;
    
    int fog;
    int material;
    int post_processing;
    int depth_of_field;
    
    int calculate_all_vns;
} DISPLAY_MODE;

typedef struct benchmark_context {
    OBJECT object_list[10]; //objects to cycle through
    DISPLAY_MODE *display_mode;
    int num_samples; //num samples per object
} BENCHMARK_CONTEXT;

void init_bench_ctx (BENCHMARK_CONTEXT *bench_ctx, DISPLAY_MODE *display_m,
                     int num_samples);
#endif
