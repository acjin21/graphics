
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
int depth_test = ON;
int texturing = OFF;
int modulate = OFF;
int perspective_correct = OFF;
int shading_mode = FLAT;
extern float light[4];
extern float eye[4];
extern float shinyness;
extern int modulate_type;

IMAGE bump_map; //bump/normal map
int drawing_normal = OFF;

int bump_mapping = OFF; //bump mapping for specular lighting
int material = OFF; //material properties

float fog_color[4] = {0, 0, 0, 1};
int fog = OFF;

extern float material_diffuse[4], material_specular[4], material_ambient[4];
extern float light_diffuse[4], light_specular[4], light_ambient[4];

/* calculate diffuse term for current material and light types, and
 store vector result in diffuse_term */
void set_diffuse_term (float normal[4], float diffuse_term[4])
{
    float diffuse;
    normalize(light);
    diffuse = MAX(vector_dot(normal, light), 0);
    if(material)
    {
        scalar_multiply(diffuse, material_diffuse, diffuse_term);
        vector_multiply(diffuse_term, light_diffuse, diffuse_term);
    }
    else
    {
        scalar_multiply(diffuse, light_diffuse, diffuse_term);
    }

}

/* calculate specular term for current material and light types, and
 store vector result in specular_term */
void set_specular_term (float normal[4], float spec_term[4])
{
    float specular, refl[4];
    normalize(light);
    vector_reflect(light, normal, refl);
    specular = MAX(vector_dot(eye, refl), 0);
    specular = pow(specular, shinyness);
    if(material)
    {
        scalar_multiply(specular, material_specular, spec_term);
        vector_multiply(spec_term, light_specular, spec_term);
    }
    else
    {
        scalar_multiply(specular, light_specular, spec_term);
    }
}

/* shade point p with diffuse, specular, and ambient components using the given
 diffuse and specular vectors */
void shade_point (float diffuse[4], float spec[4], POINT *p)
{
    vector_multiply(diffuse, p->color, p->color);
    vector_add(spec, p->color, p->color);
    
    float ambient[4] = {0, 0 , 0 , 0};
    if(material)
    {
        vector_multiply(light_ambient, material_ambient, ambient);
    }
    else
    {
        scalar_add(0.5, ambient, ambient);
    }
    vector_add(p->color, ambient, p->color);
}

/*************************************************************************/
/* draw a point into color_buffer */
/*************************************************************************/
void draw_point (POINT *p)
{
    int r = (int) (p->position[Y] + WIN_H / 2);
    int c = (int) (p->position[X] + WIN_W / 2);
    if(r >= WIN_H || r < 0 || c >= WIN_W || c < 0) return;
    float blend_weight = 0.50;

    if(depth_test && p->position[Z] > depth_buffer[r][c] &&
       !(perspective_correct && texturing))
    {
        return;
    }
    
    if(!drawing_normal && bump_mapping)
    {
        float bump[4];
        int u, v;
        
        u = (int) (p->tex[S] * texture.width);
        v = (int) (p->tex[T] * texture.height);
        
        //fix for texel conversion
        if(p->tex[S] == 1 || p->tex[T] == 1)
        {
            u = p->tex[S] == 1 ? texture.width - 1 : u;
            v = p->tex[T] == 1 ? texture.height - 1 : v;
        }
        bump[X] = bump_map.data[v][u][R] / 255.0;
        bump[Y] = bump_map.data[v][u][G] / 255.0;
        bump[Z] = bump_map.data[v][u][B] / 255.0;

        bump[W] = 1;
        
        scalar_add(-0.5, bump, bump);
        normalize(bump);
        vector_multiply(bump, p->v_normal, p->v_normal);
        
    }
    
    if(shading_mode == PHONG)
    {
        float tmp_diff[4], tmp_spec[4];
        set_diffuse_term(p->v_normal, tmp_diff);
        set_specular_term(p->v_normal, tmp_spec);
        
        //modulate texture with color and brightness
        if(!modulate || (modulate && modulate_type == MOD_COLOR))
        {
            shade_point(tmp_diff, tmp_spec, p);
        }
        //don't incorporate point's interpolated color; just modulate texture with lighting/brightness
        if(modulate && modulate_type == MOD_LIGHT)
        {
            cpy_vec4(p->color, tmp_diff);
            vector_add(p->color, tmp_spec, p->color);
            float ambient[4] = {0, 0 , 0 , 0};
            if(material)
            {
                vector_multiply(light_ambient, material_ambient, ambient);
            }
            else
            {
                scalar_add(0.5, ambient, ambient);
            }
            vector_add(p->color, ambient, p->color);
        }
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
            int u = (int) s;
            int v = (int) t;


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
    if(fog)
    {
        float z = p->position[Z];
        color_buffer[r][c][R] = z * fog_color[R] + (1 - z) * color_buffer[r][c][R];
        color_buffer[r][c][G] = z * fog_color[G] + (1 - z) * color_buffer[r][c][G];
        color_buffer[r][c][B] = z * fog_color[B] + (1 - z) * color_buffer[r][c][B];

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


