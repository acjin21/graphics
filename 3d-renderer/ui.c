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

/* print app settings */
void gl_print_app_settings (APP_STATE *as)
{
    float color[4] = {0, 0, 1, 1};
    
    int next_y = 200;
    sprintf(settings_str, "%s", framebuffer_src_strings[as->framebuffer_source]);
    gl_printf(5, next_y, "Framebuffer Source: ", color);
    gl_printf(120, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", manip_mode_strings[as->manipulator_mode]);
    gl_printf(5, next_y, "Manipulator Mode: ", color);
    gl_printf(120, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", as->projection_mode ? "PERSPECT" : "ORTHO");
    gl_printf(5, next_y, "Projection Mode: ", color);
    gl_printf(120, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", ips_mode_strings[as->post_processing_mode]);
    gl_printf(5, next_y, "Image Proc. Mode: ", color);
    gl_printf(120, next_y, settings_str, color);
    next_y -= 15;
    
    sprintf(settings_str, "%s", current_rs.render_mode ? "DEFERRED" : "FORWARD");
    gl_printf(5, next_y, "Render Mode: ", color);
    gl_printf(120, next_y, settings_str, color);
    next_y -= 15;

}

