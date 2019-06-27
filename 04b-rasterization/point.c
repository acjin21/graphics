
#include "point.h"
#include <stdio.h>
#include <math.h>

extern IMAGE texture;
extern float color_buffer[WIN_H][WIN_W][4];
extern float depth_buffer[WIN_H][WIN_W];

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/

/* modes */
int alpha_blend = OFF;
int depth_test = OFF;
int texturing = OFF;
int modulate = OFF;
int perspective_correct = OFF;
int shading_mode = FLAT;
extern float light[4];
extern float eye[4];
extern float shinyness;
extern int modulate_type;

extern float material_diffuse[4], material_specular[4], material_ambient[4];
extern float light_diffuse[4], light_specular[4], light_ambient[4];
extern int material_type;
/*************************************************************************/
/* draw a point into color_buffer */
/*************************************************************************/
void draw_point (POINT *p)
{
    int r = (int) (p->position[Y] + WIN_H / 2);
    int c = (int) (p->position[X] + WIN_W / 2);
    if(r >= WIN_H || r < 0 || c >= WIN_W || c < 0) return;
    float blend_weight = 0.50;

    if(depth_test && p->position[Z] > depth_buffer[r][c] && !(perspective_correct && texturing))
    {
        return;
    }
    
    if(shading_mode == PHONG)
    {
        float diffuse, tmp[4], specular, refl[4], tmp_spec[4], intensity[4];
        
        normalize(light);
        normalize(p->v_normal);
        diffuse = CLAMP_LOW(vector_dot(p->v_normal, light), 0);
        scalar_multiply(diffuse, material_diffuse, tmp);
        vector_multiply(tmp, light_diffuse, tmp);
        
        vector_reflect(light, p->v_normal, refl);
        specular = CLAMP_LOW(vector_dot(eye, refl), 0);
        specular = pow(specular, shinyness);
        scalar_multiply(specular, material_specular, tmp_spec);
        vector_multiply(tmp_spec, light_specular, tmp_spec);
        
        if(modulate_type == MOD_COLOR) //modulate texture with color and brightness
        {
            vector_multiply(p->color, tmp, tmp);
            vector_add(p->color, tmp_spec, p->color);
            
//            vector_multiply(p->color, tmp_spec, tmp_spec);
//            vector_add(tmp, tmp_spec, intensity);
//            cpy_vec4(p->color, intensity);
        }
        else if(modulate) //don't incorporate point's interpolated color
        {
//            p->color[R] = brightness;
//            p->color[G] = brightness;
//            p->color[B] = brightness;
//            p->color[A] = p->color[A];
            cpy_vec4(p->color, tmp);
            vector_add(p->color, tmp_spec, p->color);
        }
        
        float ambient[4] = {0, 0, 0, 0};
//        vector_add(ambient, material_ambient, ambient);
//        vector_add(ambient, light_ambient, ambient);
//        vector_add(p->color, material_ambient, p->color);
    }
    
    if(texturing)
    {
        float s, t;
        int u, v;
        
        if( perspective_correct )
        {
            s = p->tex[S];
            t = p->tex[T];
            
            float z = 1.0 / p->position[Z];
            p->position[Z] = 1.0 / p->position[Z];
            s *= z;
            t *= z;
            
            u = (int) (s * texture.width);
            v = (int) (t * texture.height);
            
            color_buffer[r][c][R] = texture.data[v][u][R] / 255.0;
            color_buffer[r][c][G] = texture.data[v][u][G] / 255.0;
            color_buffer[r][c][B] = texture.data[v][u][B] / 255.0;
            color_buffer[r][c][A] = texture.data[v][u][A] / 255.0;
        }
        
        else
        {
            s = (p->tex[S] * texture.width);
            t = (p->tex[T] * texture.height);
            
            if(p->tex[S] == 1 || p->tex[T] == 1)
            {
                s = p->tex[S] == 1 ? texture.width - 1 : s;
                t = p->tex[T] == 1 ? texture.width - 1 : t;
            }
            color_buffer[r][c][R] = texture.data[(int) t][(int) s][R] / 255.0;
            color_buffer[r][c][G] = texture.data[(int) t][(int) s][G] / 255.0;
            color_buffer[r][c][B] = texture.data[(int) t][(int) s][B] / 255.0;
            color_buffer[r][c][A] = texture.data[(int) t][(int) s][A] / 255.0;
            
        }
        if(modulate)
        {
            color_buffer[r][c][R] *= p->color[R];
            color_buffer[r][c][G] *= p->color[G];
            color_buffer[r][c][B] *= p->color[B];
            color_buffer[r][c][A] *= p->color[A];
        }
    }
    else if(alpha_blend)
    {
        float new_r, new_g, new_b, new_a;
        
        new_r = (1 - blend_weight) * color_buffer[r][c][R] +
        blend_weight * p->color[R];
        new_g = (1 - blend_weight) * color_buffer[r][c][G] +
        blend_weight * p->color[G];
        new_b = (1 - blend_weight) * color_buffer[r][c][B] +
        blend_weight * p->color[B];
        new_a = (1 - blend_weight) * color_buffer[r][c][A] +
        blend_weight * p->color[A];
        
        /* write blended color to color_buffer */
        color_buffer[r][c][R] = new_r;
        color_buffer[r][c][G] = new_g;
        color_buffer[r][c][B] = new_b;
        color_buffer[r][c][A] = new_a;
    }
    else
    {
        /* write p.color to color_buffer */
        color_buffer[r][c][R] = p->color[R];
        color_buffer[r][c][G] = p->color[G];
        color_buffer[r][c][B] = p->color[B];
        color_buffer[r][c][A] = p->color[A];
    }
    if(depth_test)
    {
        depth_buffer[r][c] = p->position[Z];
    }
    
    

  
}

/*
 * set position of point *p to (x, y, z, w)
 */
void set_position (POINT *p, float x, float y, float z, float w)
{
    set_vec4(p->position, x, y, z, w);
}

/*
 * set color of point *p to (r, g, b, a)
 */
void set_color (POINT *p, float r, float g, float b, float a)
{
    set_vec4(p->color, r, g, b, a);
}

/*
 * set tex coords of point *p to (s, t, 0, 0)
 */
void set_tex (POINT *p, float s, float t)
{
    set_vec4(p->color, s, t, 0, 0);
}

