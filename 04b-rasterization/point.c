#include "point.h"

#include <stdio.h>
#include <math.h>
#include "macros.h"
#include "texture.h"
#include "vector.h"

/*************************************************************************/
/* externs                                                               */
/*************************************************************************/
/* modes */
extern int modulate_type;
extern int drawing_backside;

/* data */
extern IMAGE texture;
extern float color_buffer[WIN_H][WIN_W][4];
extern float depth_buffer[WIN_H][WIN_W];

extern float light[4];
extern float eye[4];
extern float shinyness;
extern float material_diffuse[4], material_specular[4], material_ambient[4];
extern float light_diffuse[4], light_specular[4], light_ambient[4];

/*************************************************************************/
/* global vars                                                          */
/*************************************************************************/
/* draw modes */
int alpha_blend = OFF;
int depth_test = ON;
int texturing = OFF;
int modulate = OFF;
int perspective_correct = OFF;

int shading_mode = FLAT;
int drawing_normals = OFF;
int bump_mapping = OFF;         // bump mapping for specular lighting
int material = OFF;             // material properties
int specular_highlight = OFF;
int fog = OFF;

/* data */
IMAGE bump_map;
float fog_color[4] = {1, 1, 1, 1};

/*************************************************************************/
/* helper functions                                                      */
/*************************************************************************/
/*
    calculate diffuse term for current material and light types, and
        store vector result in diffuse_term
 */
void set_diffuse_term (float normal[4], float diffuse_term[4])
{
    float diffuse;
    normalize(light);
    diffuse = MAX(vector_dot(normal, light), 0);
    if(material)
    {
        // include material properties
        scalar_multiply(diffuse, material_diffuse, diffuse_term);
        vector_multiply(diffuse_term, light_diffuse, diffuse_term);
    }
    else
    {
        // only use light properties
        scalar_multiply(diffuse, light_diffuse, diffuse_term);
    }

}

/*
    calculate specular term for current material and light types, and
        store vector result in specular_term
 */
void set_specular_term (float normal[4], float spec_term[4])
{
    float specular, refl[4];
    normalize(light);
    vector_reflect(light, normal, refl);
    specular = MAX(vector_dot(eye, refl), 0);
    specular = pow(specular, shinyness);
    if(material)
    {
        // include material properties
        scalar_multiply(specular, material_specular, spec_term);
        vector_multiply(spec_term, light_specular, spec_term);
    }
    else
    {
        // include material properties
        scalar_multiply(specular, light_specular, spec_term);
    }
}

/* shade point p with diffuse, specular, and ambient components using the given
 diffuse and specular vectors */
void shade_point (float diffuse[4], float spec[4], POINT *p)
{
    vector_multiply(diffuse, p->color, p->color);
    if(specular_highlight)
    {
        vector_add(spec, p->color, p->color);
    }
    
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
    
    /* if not within window size, do not draw */
    if(r >= WIN_H || r < 0 || c >= WIN_W || c < 0) return;
    
    float blend_weight = 0.50; //for alpha blending
    
    /* for now, we only implement perspective correct texture mapping */
    int persp_correct_texturing = perspective_correct && texturing;
    
    /* calculate fail condition for depth test */
    int fails_z =
        (!persp_correct_texturing && p->position[Z] > depth_buffer[r][c]) ||
        (persp_correct_texturing && 1.0/p->position[Z] > depth_buffer[r][c]);
    
    /* early return if fail depth test */
    if(depth_test && fails_z)
    {
        return;
    }
    
    /* bump map */
    if(!drawing_normals && bump_mapping)
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
    
    /* phong shading */
    if(!drawing_normals && shading_mode == PHONG)
    {
        float tmp_diff[4], tmp_spec[4];
        set_diffuse_term(p->v_normal, tmp_diff);
        set_specular_term(p->v_normal, tmp_spec);
        
        //modulate texture with color and brightness
        if(!modulate || (modulate && modulate_type == MOD_COLOR))
        {
            shade_point(tmp_diff, tmp_spec, p);
        }
        
        //don't incorporate point's interpolated color;
        //  just modulate texture with lighting/brightness
        if(modulate && modulate_type == MOD_LIGHT)
        {
            cpy_vec4(p->color, tmp_diff);
            if(specular_highlight)
            {
                vector_add(p->color, tmp_spec, p->color);
            }
            float ambient[4] = {0.5, 0.5 , 0.5 , 0};
            if(material)
            {
                vector_multiply(light_ambient, material_ambient, ambient);
            }

            vector_add(p->color, ambient, p->color);
        }
    }
    
    /* texture mapping */
    if(!drawing_normals && texturing)
    {
        float s, t;
        int u, v;
        
        if (perspective_correct)
        {
            s = p->tex[S];
            t = p->tex[T];
            
            float z = 1.0 / p->position[Z];
            p->position[Z] = 1.0 / p->position[Z];
            s *= z;
            t *= z;
            
            u = (int) (s * texture.width);
            v = (int) (t * texture.height);
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
            
            u = (int) s;
            v = (int) t;
        }

        color_buffer[r][c][R] = texture.data[v][u][R] / 255.0;
        color_buffer[r][c][G] = texture.data[v][u][G] / 255.0;
        color_buffer[r][c][B] = texture.data[v][u][B] / 255.0;
        color_buffer[r][c][A] = texture.data[v][u][A] / 255.0;
        // if drawing reverse side of triangles, ignore the texel channels
        if(drawing_backside)
        {
            set_vec4(color_buffer[r][c], 0, 0, 0, 1);
        }
        //any form of modulating (with color or with light)
        if(modulate)
        {
            vector_multiply(color_buffer[r][c], p->color, color_buffer[r][c]);
        }
    }
    
    /* alpha blending */
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
        set_vec4(color_buffer[r][c], new_r, new_g, new_b, new_a);
    }
    
    /* just write interpolated point color into color buffer */
    else
    {
        cpy_vec4(color_buffer[r][c], p->color);
    }
    
    /* fog effect */
    if(fog)
    {
        float z = p->position[Z];
        float new_r, new_g, new_b, new_a;
        
        new_r = z * fog_color[R] + (1 - z) * color_buffer[r][c][R];
        new_g = z * fog_color[G] + (1 - z) * color_buffer[r][c][G];
        new_b = z * fog_color[B] + (1 - z) * color_buffer[r][c][B];

        set_vec4(color_buffer[r][c], new_r, new_g, new_b, 1);

    }
    
    if(depth_test)
    {
        depth_buffer[r][c] = p->position[Z];
    }
}



