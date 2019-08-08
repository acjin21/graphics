#include "state.h"
#include "macros.h"
void set_default_render_state (RENDER_STATE *rs)
{
    rs->draw_mode               = FRAME;
    rs->object_type             = QUAD;
    rs->texturing_mode          = TEXTURE_OFF;
    rs->modulation_mode         = MOD_OFF;
    rs->shading_mode            = FLAT;
    rs->reflection_mode         = DIFFUSE;
    rs->light_source            = POINT_LIGHT;
    
    rs->perspective_correction  = OFF;
    rs->depth_testing           = ON;
    rs->alpha_blending          = OFF;
    rs->bump_mapping            = OFF;
    rs->fog_fx                  = OFF;
    rs->material_properties     = OFF;
    rs->post_processing         = OFF;
    rs->depth_of_field          = OFF;
    rs->backface_culling        = OFF;
    
    rs->material_type           = EMERALD;
    rs->render_mode             = FORWARD;
    rs->render_pass_type        = COLOR_PASS;
}

void set_default_app_state (APP_STATE *as)
{
    as->framebuffer_source      = COLOR;
    as->program_type            = BASIC;
    as->selected_objectID       = 0;
    as->stencil_bufferID        = 0;
    as->draw_normals_mode       = NO_NORMALS;
    as->manipulator_mode        = ROTATE;
    as->projection_mode         = PERSPECT;
    as->post_processing_mode    = NO_FX;
    as->renderer                = ALL_SW;
}

void set_benchmarking_state (APP_STATE *as, RENDER_STATE *rs)
{
    
}
