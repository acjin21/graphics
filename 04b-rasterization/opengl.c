#include "opengl.h"
#include "point.h"
#include "texture.h"
#include "app.h"
#include <stdlib.h>
int renderer = ALL_SW;

/*
 * draw_triangle_gl()
 */
void draw_triangle_gl( POINT *v0, POINT *v1, POINT *v2 )
{
    glBegin( GL_TRIANGLES );
    glColor4fv( v0->color );
    if( texturing ) glTexCoord4fv( v0->tex );
    if( shading_mode == PHONG ) glNormal3fv( v0->v_normal );
    if( renderer == SW_HW )
        glVertex3f( v0->position[X] + 0.5, v0->position[Y] + 0.5, v0->position[Z] );
    else
        glVertex3f( v0->world[X], v0->world[Y], -v0->world[Z] );
    
    glColor4fv( v1->color );
    if( texturing ) glTexCoord4fv( v1->tex );
    if( shading_mode == PHONG ) glNormal3fv( v1->v_normal );
    if( renderer == SW_HW )
        glVertex3f( v1->position[X] + 0.5, v1->position[Y] + 0.5, v1->position[Z] );
    else
        glVertex3f( v1->world[X], v1->world[Y], -v1->world[Z] );
    
    glColor4fv( v2->color );
    if( texturing ) glTexCoord4fv( v2->tex );
    if( shading_mode == PHONG ) glNormal3fv( v2->v_normal );
    if( renderer == SW_HW )
        glVertex3f( v2->position[X] + 0.5, v2->position[Y] + 0.5, v2->position[Z] );
    else
        glVertex3f( v2->world[X], v2->world[Y], -v2->world[Z] );
    glEnd();
}

/*
 * draw_line_gl()
 */
void draw_line_gl( POINT *start, POINT *end )
{
    glBegin( GL_LINES );
    /*
     * start vertex
     */
    glColor4fv( start->color );
    if( texturing ) glTexCoord4fv( start->tex );
    if( shading_mode == PHONG ) glNormal3fv( start->v_normal );
    if( renderer == SW_HW )
        glVertex3f( start->position[X] + 0.5, start->position[Y] + 0.5, start->position[Z] );
    else
        glVertex3f( start->world[X], start->world[Y], -start->world[Z] );
    
    /*
     * end vertex
     */
    glColor4fv( end->color );
    if( texturing ) glTexCoord4fv( end->tex );
    if( shading_mode == PHONG ) glNormal3fv( end->v_normal );
    if( renderer == SW_HW )
        glVertex3f( end->position[X] + 0.5, end->position[Y] + 0.5, end->position[Z] );
    else //ALL_HW
        glVertex3f( end->world[X], end->world[Y], -end->world[Z] );
    glEnd();
}

void passthrough_gl_state(void)
{
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

void change_gl_state(void)
{
    if (depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    
    if (shading_mode == FLAT) glShadeModel (GL_FLAT);
    else if (shading_mode == PHONG) glShadeModel (GL_SMOOTH);
    
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        if(modulate)
        {
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        }
        else
        {
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        }
        if (perspective_correct)
        {
            glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        }
        else
        {
            glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        }
    }
    
    if (alpha_blend) glEnable (GL_BLEND);
    else glDisable(GL_BLEND);

}

/*
 * convert_image_to_gl
 */
void convert_image_to_gl( IMAGE *input, GL_IMAGE *output )
{
    int w = input->width;
    int h = input->height;
    
    output->width   = w;
    output->height  = h;
    
    output->data = (unsigned char *)malloc( w * h * 4 );
    
    for( int j = 0; j < h; j++ )
    {
        for( int i = 0; i < w; i++ )
        {
            output->data[((j*w)+i)*4+0] = input->data[j][i][R];
            output->data[((j*w)+i)*4+1] = input->data[j][i][G];
            output->data[((j*w)+i)*4+2] = input->data[j][i][B];
            output->data[((j*w)+i)*4+3] = input->data[j][i][A];
        }
    }
}

/*
 * init_gl_state
 */
void init_gl_state( void )
{
    float clear_color[4] = { 0, 0, 0, 1 };
    float window_size = 400;
    int textureID = 0;
    
    /*
     * GL draw state
     */
    glClearColor(clear_color[R], clear_color[G], clear_color[B], clear_color[A] );
    glPointSize(1.0);
    glColor4f(1.0,0.0,0.0,1.0);
    
    /*
     * GL depth state
     */
    glClearDepth( 1.0 );
    glDisable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    /*
     * GL view state
     */
    glViewport( 0, 0, WIN_W, WIN_H );
    
    if( renderer == SW_HW )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( -window_size, window_size, -window_size, window_size, -40, 40 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        if( proj_mode == PERSPECT )
        {
            glFrustum( -1, 1, -1, 1, near, far );
        }
        else
        {
            glOrtho( -near, near, -near, near, near, far );
        }
        
        glMatrixMode( GL_MODELVIEW );
    }
    
    /*
     * GL texture state
     */
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,   0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,    0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    
    convert_image_to_gl( &texture, &gl_texture );
    
    glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 gl_texture.width,
                 gl_texture.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 gl_texture.data );
    
    glBindTexture( GL_TEXTURE_2D, 0 );
    glDisable( GL_TEXTURE_2D );
    
    free( gl_texture.data );
    
    /*
     * cube map state
     */
    glEnable( GL_TEXTURE_CUBE_MAP );
    glGenTextures( 1, &cubemapID );
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapID );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    for( int i = 0; i < 6; i++ )
    {
        convert_image_to_gl( &mipmap[i], &gl_texture );
        
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGBA8,
                     gl_texture.width,
                     gl_texture.height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     gl_texture.data );
        
        free( gl_texture.data );
    }
    
    glDisable( GL_TEXTURE_CUBE_MAP );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    
    /*
     * texgen state
     */
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    
    /*
     * culling state
     */
    glDisable( GL_CULL_FACE );
    
    /*
     * GL fog state
     */
    glFogfv( GL_FOG_COLOR, clear_color );
    glDisable( GL_FOG );
    
    /*
     * GL blend state
     */
    glBlendEquation( GL_FUNC_ADD );
    glBlendColor( 0.5, 0.5, 0.5, 1.0 );
    glBlendFunc( GL_CONSTANT_COLOR, GL_CONSTANT_COLOR );
}

