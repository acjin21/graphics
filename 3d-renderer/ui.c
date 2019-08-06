#include "ui.h"

#include <string.h>
#include <stdio.h>
#include "app.h"
#include "state.h"
#include "window.h"

char settings_str[100];

/* print string *s starting at screen coordinates (x, y) */
void gl_printf( int x, int y, char *s, float color[4])
{
    int len = strlen( s );

    glDisable( GL_LIGHTING );

    glColor4fv( color);

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D( 0, window_width, 0, window_height );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glRasterPos2i( x, y );

    for( int i = 0; i < len; i++ )
    {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_10, s[i] );
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

}

char *ips_mode_to_string (int mode)
{
    switch (mode)
    {
        case NEGATIVE:
            return "NEGATIVE";
            break;
        case FLIP_VERTICAL:
            return "FLIP_VERTICAL";
            break;
        case FLIP_HORIZONTAL:
            return "FLIP_HORIZONTAL";
            break;
        case LUMINOSITY:
            return "LUMINOSITY";
            break;
        case SEPIA:
            return "SEPIA";
            break;
        case AVG:
            return "AVG";
            break;
        case MIN:
            return "MIN";
            break;
        case MAX:
            return "MAX";
            break;
        case ROTATE_CCW:
            return "ROTATE_CCW";
            break;
        case LINCOLN:
            return "LINCOLN";
            break;
        case FISHEYE:
            return "FISHEYE";
            break;
        case EINSTEIN:
            return "EINSTEIN";
            break;
        case OIL_TRANSFER:
            return "OIL_TRANSFER";
            break;
        case TILING:
            return "TILING";
            break;
        default:
            return "NO EFFECT";
            break;
    }
}

char *ips_mode_strings[N_IP_MODES] =
{
    "NO EFFECT",
    "NEGATIVE",
    "FLIP_VERTICAL",
    "FLIP_HORIZONTAL",
    "LUMINOSITY",
    "SEPIA",
    "AVG",
    "MIN",
    "MAX",
    "ROTATE_CCW",
    "LINCOLN",
    "FISHEYE",
    "EINSTEIN",
    "OIL_TRANSFER",
    "TILING"
};

char *material_strings[NUM_MATERIALS] =
{
    "EMERALD",
    "JADE",
    "OBSIDIAN",
    "PEARL",
    "RUBY",
    "TURQUOISE",
    "BRASS",
    "BRONZE",
    "CHROME",
    "COPPER",
    "GOLD",
    "SILVER",
    "BLACK_PLASTIC",
    "CYAN_PLASTIC",
    "GREEN_PLASTIC",
    "RED_PLASTIC",
    "WHITE_PLASTIC",
    "YELLOW_PLASTIC",
    "BLACK_RUBBER",
    "CYAN_RUBBER",
    "GREEN_RUBBER",
    "RED_RUBBER",
    "WHITE_RUBBER",
    "YELLOW_RUBBER"
};

/* print settings for image processing */
void gl_print_image_settings (IMAGE_PROCESSING_STATE *ips)
{
    float color[4] = {0, 0, 1, 1};
    
    int next_y = 100;
    sprintf(settings_str, "%s", ips_mode_strings[ips->processing_mode]);
    gl_printf(5, next_y, "Processing Mode:", color);
    gl_printf(120, next_y, settings_str, color);
    next_y += 15;
}

char *framebuffer_src_strings[3] =
{
    "COLOR",
    "DEPTH",
    "STENCIL"
};

char *manip_mode_strings[3] =
{
    "ROTATE",
    "TRANSLATE",
    "SCALE"
};

char *texture_mode_strings[NUM_TEX_MODES] =
{
    "TEXTURE_OFF",
    "MANUAL",
    "NAIVE",
    "CYLINDRICAL",
    "SPHERICAL",
    "REFLECTION",
    "CUBE_MAP"
};
/* print app settings */
void gl_print_app_settings (APP_STATE *as)
{
    float color[4] = {0, 0, 1, 1};
    
    int next_y = 100;
    sprintf(settings_str, "%s", framebuffer_src_strings[as->framebuffer_source]);
    gl_printf(5, next_y, "Framebuffer Source (B): ", color);
    gl_printf(130, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", manip_mode_strings[as->manipulator_mode]);
    gl_printf(5, next_y, "Manipulator ([tab]): ", color);
    gl_printf(130, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", as->projection_mode ? "PERSPECT" : "ORTHO");
    gl_printf(5, next_y, "Projection (p): ", color);
    gl_printf(130, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", ips_mode_strings[as->post_processing_mode]);
    gl_printf(5, next_y, "Image Proc. (3, #): ", color);
    gl_printf(130, next_y, settings_str, color);
    next_y -= 15;
    
    
    sprintf(settings_str, "%s", as->dof_mode ? "NEAR" : "FAR");
    gl_printf(5, next_y, "DOF (4): ", color);
    gl_printf(130, next_y, settings_str, color);
    next_y -= 15;

}

void gl_print_render_settings (RENDER_STATE *rs)
{
    float color[4] = {0, 0.4, 1, 1};

    int next_y = 100;
    
    sprintf(settings_str, "%s", texture_mode_strings[rs->texturing_mode]);
    gl_printf(220, next_y, "Texturing (t, T) / Mod (m): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", rs->light_source ? "GLOBAL" : "POINT");
    gl_printf(220, next_y, "Light Source (7): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", rs->perspective_correction ? "ON" : "OFF");
    gl_printf(220, next_y, "Perspective Correction (C): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s / %s", rs->depth_testing ? "ON" : "OFF", rs->alpha_blending ? "ON" : "OFF");
    gl_printf(220, next_y, "Depth Test (D) / Alpha Blend (b): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", rs->backface_culling ? "ON" : "OFF");
    gl_printf(220, next_y, "Backface Culling (*): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    
    gl_printf(220, next_y, "Material (1, 2): ", color);
    if(rs->material_properties)
    {
        sprintf(settings_str, "%s", material_strings[rs->material_type]);
        gl_printf(380, next_y, settings_str, color);
    }
    else
    {
        gl_printf(380, next_y, "OFF", color);
    }
    next_y -= 15;

    
    sprintf(settings_str, "%s", rs->render_mode ? "DEFERRED" : "FORWARD");
    gl_printf(220, next_y, "Render Mode (8): ", color);
    gl_printf(380, next_y, settings_str, color);
    next_y -= 15;
    

    
}


