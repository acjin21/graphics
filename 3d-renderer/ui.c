#include "ui.h"

#include "app.h"
#include "state.h"

/*
 * gl_printf
 */
void gl_printf( int x, int y, char *s )
{
    int len = strlen( s );

    glDisable( GL_LIGHTING );

    glColor4f( 0, 0, 0, 1 );

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

//char settings_str[1000000000];
//
//void gl_print_settings (void)
//{
//    int next_y = 100;
//    sprintf(settings_str, "%s",
//            current_as.projection_mode ? "PERSPECTIVE" : "ORTHOGRAPHIC");
//    gl_printf(5, next_y, "Projection Mode (p):");
//    gl_printf(120, next_y, settings_str);
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            current_rs.perspective_correction ? "ON" : "OFF");
//    gl_printf(5, next_y, "Perspect. Correct (c):");
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            current_as.manipulator_mode ? (current_as.manipulator_mode == 1 ? "TRANSLATE" : "SCALE") : "ROTATE" );
//    gl_printf(5, next_y, "Manip Mode ([tab]):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            current_rs.render_mode ? "DEFER" : "FORWARD" );
//    gl_printf(5, next_y, "Render Mode ([8]):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            current_rs.light_source ? "GLOBAL" : "LOCAL");
//    gl_printf(5, next_y, "Light Mode ([7]):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            renderer ? (renderer == 1 ? "SW_HW" : "ALL_HW") : "ALL_SW" );
//    gl_printf(5, next_y, "Renderer ([9]):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            current_rs.shading_mode ? (current_rs.shading_mode == 1 ? "FLAT" : "PHONG") : "NONE" );
//    gl_printf(5, next_y, "Shading Type (h):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            tex_gen_name(current_rs.texturing_mode) );
//    gl_printf(5, next_y, "Tex Gen Mode (0):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//    sprintf(settings_str, "%s",
//            backface_culling ? "ON" : "OFF" );
//    gl_printf(5, next_y, "Backface culling (*):" );
//    gl_printf(120, next_y, settings_str );
//    next_y += 15;
//
//
//    printf("Manip Mode ([tab]):\t%s\n",
//           current_as.manipulator_mode ? (current_as.manipulator_mode == 1 ? "TRANSLATE" : "SCALE") : "ROTATE" );
//
//    printf("Alpha Blending (b):\t%s\n",
//           current_rs.alpha_blending ? "ON" : "OFF");
//    printf("Depth Testing (d):\t%s\n",
//           current_rs.depth_testing ? "ON" : "OFF");\
//
//    printf("Texturing (t/P):\t%s\n",
//           current_rs.texturing_mode ? "ON" : "OFF");
//
//    printf("Modulate Type (m/M):\t%s\n",
//           current_rs.modulation_mode ? (current_rs.modulation_mode == 2 ? "LIGHT" : "COLOR") : "OFF");
//    printf("Bump Mapping (5):\t%s\n",
//           current_rs.bump_mapping ? "ON" : "OFF");
//
//    printf("Spec Highlight (S):\t%s\n",
//           specular_highlight ? "ON" : "OFF");
//    printf("Fog Mode (F):\t\t%s\n",
//           fog ? "ON" : "OFF");
//    printf("Material Type (1/2):\t%s\n",
//           material ? material_name(current_rs.material_type) : "OFF");
//    printf("Post-Processing (3):\t%s\n",
//           post_processing ? "ON" : "OFF");
//    printf("DOF (4):\t\t%s\n",
//           dof_mode ? "ON" : "OFF");
//}
