
#include "benchmark.h"

#include "macros.h" // ON/OFF, PHONG/FLAT/NO_SHADING
#include "model.h" // FILL vs FRAME
#include "point.h" // uv_generation

DISPLAY_MODE benchmark =
{
    .raster_mode = FILL,
    .projection = PERSPECT,
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

BENCHMARK_CONTEXT bench_rot1;

void init_bench_ctx (BENCHMARK_CONTEXT *bench_ctx, DISPLAY_MODE *display_m, int num_samples)
{
    bench_ctx->display_mode = display_m;
    bench_ctx->num_samples = num_samples;
}

//void init_object_list (void)
//{
//
//}
