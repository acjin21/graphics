#include "opengl.h"
#include "point.h"
#include "texture.h"
#include "app.h"
#include "model.h"
#include "vector.h"
#include "light.h"
#include "material.h"
#include "window.h"

#include <stdlib.h>
#include <stdio.h>

extern float window_size;
float zero_vect[4] = {0, 0, 0, 1};
float one_vect[4] = {1, 1, 1, 1};
int renderer = ALL_SW;

typedef struct gl_image {
    int             width;
    int             height;
    unsigned char   *data;
} GL_IMAGE;

GLuint textureID;
GLuint cubemapID;
GL_IMAGE gl_texture;


/*
 * draw_triangle_gl()
 */
void draw_triangle_gl( POINT *v0_ptr, POINT *v1_ptr, POINT *v2_ptr )
{
    POINT v0 = *v0_ptr;
    POINT v1 = *v1_ptr;
    POINT v2 = *v2_ptr;
    int need_v_normals = shading_mode == FLAT   ||
                         shading_mode == PHONG  || tex_gen_mode;
    
    /* draw the light's CAM space position */
    glBegin( GL_POINTS );
    glColor4f(1, 1, 1, 1);
    glPointSize(3.0);
//    light_pos_screen[Z] = 0;
    glVertex4fv(light_pos_screen);
    glEnd();
    
    glBegin( GL_TRIANGLES );
    
    glColor4fv( v0.color );
    v0.v_normal[Z] *= -1;
    normalize(v0.v_normal);
    if( texturing )         glTexCoord4fv( v0.tex );
    if( need_v_normals )    glNormal3fv( v0.v_normal );
    if( renderer == SW_HW )
    {
        glVertex3f(v0.position[X] - window_width / 2 - 0.5,
                   v0.position[Y] - window_height / 2 - 0.5,
                   -v0.position[Z]);
    }
    else    glVertex3f( v0.world[X], v0.world[Y], -v0.world[Z] );
    
    glColor4fv( v1.color );
    v1.v_normal[Z] *= -1;
    normalize(v1.v_normal);

    if( texturing )         glTexCoord4fv( v1.tex );
    if( need_v_normals )    glNormal3fv( v1.v_normal );
    if( renderer == SW_HW )
    {
        glVertex3f(v1.position[X] - window_width / 2 - 0.5,
                   v1.position[Y] - window_height / 2 - 0.5,
                   -v1.position[Z] );
    }
    else    glVertex3f( v1.world[X], v1.world[Y], -v1.world[Z] );
    

    glColor4fv( v2.color );
    v2.v_normal[Z] *= -1;
    normalize(v2.v_normal);

    if(texturing)           glTexCoord4fv( v2.tex );
    if(need_v_normals)      glNormal3fv( v2.v_normal );
    if(renderer == SW_HW)
    {
        glVertex3f(v2.position[X] - window_width / 2 - 0.5,
                   v2.position[Y] - window_height / 2 - 0.5,
                   -v2.position[Z] );
    }
    else    glVertex3f( v2.world[X], v2.world[Y], -v2.world[Z] );

    glEnd();
}

/*
 * draw_line_gl()
 */
void draw_line_gl( POINT *start_ptr, POINT *end_ptr )
{
    POINT start = *start_ptr;
    POINT end = *end_ptr;
    int need_v_normals = shading_mode == FLAT   ||
                         shading_mode == PHONG  || tex_gen_mode;
    
    glBegin( GL_LINES );
    /*
     * start vertex
     */
    glColor4fv( start.color );
    start.v_normal[Z] *= -1;
    if(texturing)         glTexCoord4fv( start.tex );
    if(need_v_normals)    glNormal3fv( start.v_normal );
    if(renderer == SW_HW)
        glVertex3f(start.position[X] - window_width / 2 - 0.5,
                   start.position[Y] - window_height / 2 - 0.5,
                   -start.position[Z]);
    else
        glVertex3f( start.world[X], start.world[Y], -start.world[Z] );
    
    /*
     * end vertex
     */
    glColor4fv(end.color);
    end.v_normal[Z] *= -1;
    if(texturing)         glTexCoord4fv(end.tex);
    if(need_v_normals)    glNormal3fv(end.v_normal);
    if(renderer == SW_HW)
        glVertex3f(end.position[X] - window_width / 2 - 0.5,
                   end.position[Y] - window_height / 2 - 0.5,
                   -end.position[Z]);
    else //ALL_HW
        glVertex3f( end.world[X], end.world[Y], -end.world[Z] );
    glEnd();
}

void passthrough_gl_state(void)
{
    /*
     * reset GL state to "pass-through" defaults
     */
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDisable( GL_DEPTH_TEST );
    glShadeModel( GL_FLAT );
    glDisable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    glDisable( GL_TEXTURE_CUBE_MAP );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    glDisable( GL_CULL_FACE );
    glDisable( GL_FOG );
    glDisable( GL_BLEND );
    glDisable( GL_LIGHTING );
    
    glDisable( GL_NORMALIZE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   zero_vect           );
    glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  0.0                 );
}

void change_gl_state(void)
{
    printf("CHANGE GL\n");
    /*
     * GL polygon state
     */

    /*
     * GL depth state
     */
    glPolygonMode(GL_FRONT_AND_BACK, draw_mode == FRAME ? GL_LINE : GL_FILL);
    glShadeModel(shading_mode == FLAT ? GL_FLAT : GL_SMOOTH);
    depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    float light_amb[4] = {0.5, 0.5, 0.5, 1};
    float light_diff[4] = {1, 1, 1, 1};
    float light_spec[4] = {1, 1, 1, 1};
    
    /*
     * GL texturing state
     */
    if( texturing )
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, textureID );
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (modulate) ?
                  GL_MODULATE : GL_DECAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, (perspective_correct) ?
               GL_NICEST : GL_FASTEST);
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
    
    fog         ? glEnable(GL_FOG)  : glDisable(GL_FOG);
    alpha_blend ? glEnable(GL_BLEND): glDisable(GL_BLEND);

    /* GL lighting state */
    if( shading_mode == FLAT || shading_mode == PHONG )
    {
        float gl_light[4];

        if( light_type == LOCAL_L )
        {
            cpy_vec4( gl_light, light_pos_screen );
//            cpy_vec4(gl_light, light_pos);
            gl_light[X] *= -1;
            gl_light[Y] *= -1;
        }
        else
        {
            cpy_vec4( gl_light, light );
            gl_light[W] = 0;
        }
        glLightfv( GL_LIGHT0, GL_POSITION, gl_light );
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_NORMALIZE );
        
        glLightfv( GL_LIGHT0, GL_AMBIENT,   light_amb  );
        glLightfv( GL_LIGHT0, GL_DIFFUSE,   light_diff   );
        glLightfv( GL_LIGHT0, GL_SPECULAR,  light_spec );
        
        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    }
    else
    {
        glDisable( GL_LIGHTING );
    }
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
    float clear_color[4] = { 0.5, 0.5, 0.5, 1 };
    float window_size = 400;

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

