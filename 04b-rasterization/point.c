#include "point.h"

#include <stdio.h>
#include <math.h>
#include "macros.h"
#include "vector.h"

#include "app.h"
#include "model.h"
#include "light.h"
#include "camera.h"
#include "color.h"
#include "depth.h"
#include "material.h"

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
int drawing_bounding_box = OFF;
int drawing_axes = OFF;

int bump_mapping = OFF;         // bump mapping for specular lighting
int material = OFF;             // material properties
int specular_highlight = OFF;
int fog = OFF;

/* data */
IMAGE bump_map;
float fog_color[4] = {1, 1, 1, 1};

IMAGE cube_map[6];
IMAGE texture;
IMAGE *texture_ptr = &texture;
int cube_map_index;

/*************************************************************************/
/* helper functions                                                      */
/*************************************************************************/
/*
    calculate diffuse term for current material and light types, and
        store vector result in diffuse_term
 */
void set_diffuse_term (float normal[4], float light[4], float diffuse_term[4])
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
void set_specular_term (float normal[4], float light[4], float spec_term[4])
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
    int r = (int) (p->position[Y]);//+ WIN_H / 2);
    int c = (int) (p->position[X]);// + WIN_W / 2);
    
    /* if not within window size, do not draw */
    if(r >= WIN_H || r < 0 || c >= WIN_W || c < 0) return;
    
    float blend_weight = 0.50; //for alpha blending
    
    /* early return if fail depth test */
    if(depth_test && p->position[Z] > depth_buffer[r][c]) return;
    
    /* bump map */
    if(normal_type != V_NORMALS && bump_mapping)
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
        bump[Z] = (bump_map.data[v][u][B] / 255.0);
        bump[W] = 1;
        scalar_multiply(2, bump, bump);
        scalar_add(-1, bump, bump);
        bump[Z] = -1 * bump[Z]; //flip for our left-handed coord system
        normalize(bump);
        cpy_vec4(p->v_normal, bump);
    }
    
    /* phong shading */
    if(!drawing_normals && !drawing_bounding_box && !drawing_axes && shading_mode == PHONG)
    {
        float tmp_diff[4], tmp_spec[4];
        if(light_type == LOCAL_L)
        {
            set_diffuse_term(p->v_normal, p->light, tmp_diff);
            set_specular_term(p->v_normal, p->light, tmp_spec);
        }
        else if (light_type == GLOBAL_L)
        {
            set_diffuse_term(p->v_normal, light, tmp_diff);
            set_specular_term(p->v_normal, light, tmp_spec);
        }
        
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
    texture_ptr = &texture;

    /* texture mapping */
    if(!drawing_normals && !drawing_bounding_box && !drawing_axes && tex_gen_mode == CUBE_MAP)
    {
        normalize(p->v_normal);
        cube_map_vec(p->v_normal, p->tex, &cube_map_index);
        texture_ptr = &cube_map[cube_map_index];

    }
    
    if(!drawing_normals && !drawing_bounding_box && !drawing_axes && texturing)
    {
        float s, t;
        int u, v;

        if (perspective_correct)
        {
            s = p->tex[S];
            t = p->tex[T];
        
            // unwrap s and t coords using interpolated 1/w value:
            //  (s/w, t/w) -> (s, t)
            s /= p->position[W];
            t /= p->position[W];
            
            u = (int) (s * texture_ptr->width);
            v = (int) (t * texture_ptr->height);
        }
        else
        {
            s = (p->tex[S] * texture_ptr->width);
            t = (p->tex[T] * texture_ptr->height);
            
            if(p->tex[S] == 1 || p->tex[T] == 1)
            {
                s = p->tex[S] == 1 ? texture_ptr->width - 1 : s;
                t = p->tex[T] == 1 ? texture_ptr->width - 1 : t;
            }
            
            u = (int) s;
            v = (int) t;
        }
        
        color_buffer[r][c][R] = texture_ptr->data[v][u][R] / 255.0;
        color_buffer[r][c][G] = texture_ptr->data[v][u][G] / 255.0;
        color_buffer[r][c][B] = texture_ptr->data[v][u][B] / 255.0;
        color_buffer[r][c][A] = texture_ptr->data[v][u][A] / 255.0;
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



