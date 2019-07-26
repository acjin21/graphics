#include "opengl.h"
#include "point.h"
#include "texture.h"
#include "app.h"
#include "model.h"
#include "vector.h"
#include "light.h"
#include "material.h"

#include <stdlib.h>
#include <stdio.h>

extern float window_size;
float zero_vect[4] = {0, 0, 0, 0};
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
void draw_triangle_gl( POINT *v0, POINT *v1, POINT *v2 )
{
    glBegin( GL_TRIANGLES );
    glColor4fv( v0->color );
    if( texturing ) glTexCoord4fv( v0->tex );
    if( shading_mode == FLAT || shading_mode == PHONG || tex_gen_mode ) glNormal3fv( v0->v_normal );
    if( renderer == SW_HW )
    {
        glVertex3f( v0->position[X] - WIN_W / 2 - 0.5, v0->position[Y] - WIN_H / 2 - 0.5, -v0->position[Z] );
    }
    else
        glVertex3f( v0->world[X], v0->world[Y], -v0->world[Z] );
    
    glColor4fv( v1->color );
    if( texturing ) glTexCoord4fv( v1->tex );
    if( shading_mode == FLAT || shading_mode == PHONG || tex_gen_mode ) glNormal3fv( v1->v_normal );
    if( renderer == SW_HW )
        glVertex3f( v1->position[X] - WIN_W / 2 - 0.5, v1->position[Y] - WIN_H / 2 - 0.5, -v1->position[Z] );
    else
        glVertex3f( v1->world[X], v1->world[Y], -v1->world[Z] );
    
    glColor4fv( v2->color );
    if( texturing ) glTexCoord4fv( v2->tex );
    if( shading_mode == FLAT || shading_mode == PHONG || tex_gen_mode ) glNormal3fv( v2->v_normal );
    if( renderer == SW_HW )
        glVertex3f( v2->position[X] - WIN_W / 2 - 0.5, v2->position[Y] - WIN_H / 2 - 0.5, -v2->position[Z] );
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
    if( texturing )
        glTexCoord4fv( start->tex );
    if( shading_mode == FLAT || shading_mode == PHONG || tex_gen_mode ) glNormal3fv( start->v_normal );
    if( renderer == SW_HW )
        glVertex3f( start->position[X] - WIN_W / 2 - 0.5, start->position[Y] - WIN_H / 2 - 0.5, -start->position[Z] );
    else
        glVertex3f( start->world[X], start->world[Y], -start->world[Z] );
    
    /*
     * end vertex
     */
    glColor4fv( end->color );
    if( texturing ) glTexCoord4fv( end->tex );
    if( shading_mode == FLAT || shading_mode == PHONG || tex_gen_mode ) glNormal3fv( end->v_normal );
    if( renderer == SW_HW )
        glVertex3f( end->position[X] - WIN_W / 2 - 0.5, end->position[Y] - WIN_H / 2 - 0.5, -end->position[Z] );
    else //ALL_HW
        glVertex3f( end->world[X], end->world[Y], -end->world[Z] );
    glEnd();
}

void passthrough_gl_state(void)
{
    printf("PASSTHROUGH GL\n");
    
#if 0
    if(renderer == ALL_SW)
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluOrtho2D(-window_size,window_size,-window_size,window_size);

    }
    if( renderer == SW_HW )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluOrtho2D(-window_size,window_size,-window_size,window_size);
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else if (renderer == ALL_HW)
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
#endif
    
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
     + sw_vertex_processing
     + sw_pixel_processing
     + modulate
     + perspective
     + correction
     + alpha_blending
     + depth_testing
     + texturing
     + tex_gen
     + env_mapping
     + wireframe
     bump_mapping
     + draw_specular
     + per_vertex_lighting
     + per_pixel_lighting
     local_light
     */

#if 0
    /*
     * GL projection state
     */
    if(renderer == ALL_SW)
    {
        gluOrtho2D(-window_size,window_size,-window_size,window_size);
        return;
    }
    else if( renderer == SW_HW )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        gluOrtho2D(-window_size,window_size,-window_size,window_size);
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else if (renderer == ALL_HW)
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
#endif
    
    /*
     * GL polygon state
     */
    if( draw_mode == FRAME )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    
    /*
     * GL depth state
     */
    if( depth_test )
    {
        glEnable( GL_DEPTH_TEST );
    }
    else
    {
        glDisable( GL_DEPTH_TEST );
    }
    
    /*
     * GL shading state
     */
    if( shading_mode == FLAT )
    {
        glShadeModel( GL_FLAT );
    }
    
    if( shading_mode == PHONG )
    {
        glShadeModel( GL_SMOOTH );
    }
    
    /*
     * GL texturing state
     */
    if( texturing )
    {
        /*
         * cube map state
         */
        if( tex_gen_mode == CUBE_MAP )
        {
            glDisable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, 0 );
            
            glEnable( GL_TEXTURE_CUBE_MAP );
            glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
            glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapID );
        }
        else
        {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textureID );
            
            glDisable( GL_TEXTURE_CUBE_MAP );
            glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
            glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
        }

        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (modulate) ? GL_MODULATE : GL_DECAL );
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, (perspective_correct) ? GL_NICEST : GL_FASTEST );

//        if( tex_gen_mode  )
//        {
//            glEnable( GL_TEXTURE_GEN_S );
//            glEnable( GL_TEXTURE_GEN_T );
//            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
//            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
//        }
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, 0 );
        
        glDisable( GL_TEXTURE_CUBE_MAP );
        glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
        glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    }
    
    /*
     * GL fog state
     */
    if( fog )
    {
        glEnable( GL_FOG );
    }
    else
    {
        glDisable( GL_FOG );
    }
    
    /*
     * GL blend state
     */
    if( alpha_blend )
    {
        glEnable( GL_BLEND );
    }
    else
    {
        glDisable( GL_BLEND );
    }
    
    /*
     * GL lighting state
//     */
    if( shading_mode == FLAT || shading_mode == PHONG )
    {
        float gl_light[4];

        if( light_type == LOCAL_L )
        {
            cpy_vec4( gl_light, light_pos );
        }
        else
        {
            cpy_vec4( gl_light, light );
            gl_light[W] = 0;
//            gl_light[Z] *= -1;
//            gl_light[X] *= -1;
//            gl_light[Y] *= -1;
        }



        glLightfv( GL_LIGHT0, GL_POSITION, gl_light );
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_NORMALIZE );
//        glEnable(GL_COLOR_MATERIAL);
        glLightfv( GL_LIGHT0, GL_AMBIENT,   light_ambient   );
        glLightfv( GL_LIGHT0, GL_DIFFUSE,   light_diffuse   );
        glLightfv( GL_LIGHT0, GL_SPECULAR,  light_specular  );
        
        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
//        glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

        float diff[4];
        float amb[4] = {0.5, 0.5, 0.5, 0.5};
        
        if(material)
        {
            glDisable(GL_COLOR_MATERIAL);
            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    material_ambient    );
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,    material_diffuse    );
        }
//        else
//        {
//            glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    amb    );
//        }


        else
        {
            glEnable(GL_COLOR_MATERIAL);
        }

        

        if( specular_highlight )
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   material_specular   );
            glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  shinyness           );
        }
        else
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   zero_vect           );
            glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  0.0                 );
        }
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
    float clear_color[4] = { 1, 1, 1, 1 };
    float window_size = 400;

    
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
#if 0
    glViewport( 0, 0, WIN_W, WIN_H );
    if(renderer == ALL_SW)
    {
        gluOrtho2D(-window_size,window_size,-window_size,window_size);
    }
    else if( renderer == SW_HW )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( -window_size, window_size, -window_size, window_size, -40, 40 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else if (renderer == ALL_HW)
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
#endif
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
        convert_image_to_gl( &cube_map[i], &gl_texture );
        
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

