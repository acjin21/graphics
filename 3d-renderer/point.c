#include "point.h"

#include <stdio.h>
#include <math.h>
#include "macros.h"
#include "vector.h"

#include "app.h"
#include "model.h"
#include "light.h"
#include "camera.h"

#include "material.h"

#include "buffers.h"

#include "window.h"
#include "state.h"
/*************************************************************************/
/* global vars                                                          */
/*************************************************************************/
/* draw modes */
int drawing_normals         = OFF;
int drawing_bounding_box    = OFF;
int drawing_axes            = OFF;

/* data */
IMAGE bump_map;
float fog_color[4] = {1, 1, 1, 1};

IMAGE cube_map[6];
IMAGE texture;
IMAGE *texture_ptr = &texture;
int cube_map_index;

float max_z_from_light = 0.0;
/*************************************************************************/
/* helper functions                                                      */
/*************************************************************************/
/*
    calculate diffuse term for current material and light types, and
        store vector result in diffuse_term
 */
void set_diffuse_term
(float diffuse_term[4], float normal[4], float light[4], float world_pos[4])
{
    float diffuse, tmp[4];
    if(current_rs.light_source == GLOBAL_LIGHT)           // directional light
    {
        scalar_multiply(-1, light, tmp); //reverse so (pos -> light)
    }
    else //positional light
    {
        cpy_vec4(tmp, light);           //already (pos -> light) from calculate_vectors
    }
    normalize(tmp);
    diffuse = MAX(vector_dot(normal, tmp), 0);
    if(current_rs.material_properties)
    {
        scalar_multiply(diffuse, material_diffuse, diffuse_term);
        vector_multiply(diffuse_term, light_diffuse, diffuse_term);
    }
    else
    {
        scalar_multiply(diffuse, light_diffuse, diffuse_term);
    }
}

/*
    calculate specular term for current material and light types, and
        store vector result in specular_term
 */
void set_specular_term
(float spec_term[4], float normal[4], float light[4], float view[4], float world_pos[4])
{
    float specular, refl[4], tmp[4];
    normalize(view);
    if(current_rs.light_source == GLOBAL_LIGHT) //directional light
    {
        scalar_multiply(-1, light, tmp);
    }
    else //positional light
    {
        cpy_vec4(tmp, light);
    }
    normalize(tmp);
    vector_reflect(tmp, normal, refl);
    specular = MAX(vector_dot(view, refl), 0);
    specular = pow(specular, shinyness);
    if(current_rs.material_properties)
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
void shade_point (float diffuse[4], float spec[4], POINT *p, int mod_type)
{
    if(current_rs.modulation_mode != MOD_LIGHT)
    {
        vector_multiply(diffuse, p->color, p->color);
    }
    else if(current_rs.modulation_mode == MOD_LIGHT)
    {
        cpy_vec4(p->color, diffuse);
    }
    if(current_rs.reflection_mode == SPECULAR)
    {
        vector_add(spec, p->color, p->color);
    }
    
    float ambient[4] = {0.5, 0.5, 0.5, 0};
    if(current_rs.material_properties)
    {
        vector_multiply(light_ambient, material_ambient, ambient);
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

    int shadow_buffer_r = (int) (p->light_space[Y]);
    int shadow_buffer_c = (int) (p->light_space[X]);
    /* if not within window size, do not draw */
    if(r >= window_height || r < 0 || c >= window_width || c < 0) return;
    float blend_weight = 0.50; //for alpha blending
    
    /* early return if fail depth test */
    if(current_rs.depth_testing && p->position[Z] > depth_buffer[r][c])
    {
        return;
    }
    if(current_rs.depth_testing)
    {
        depth_buffer[r][c] = p->position[Z];
    }
    if(current_rs.render_pass_type == SHADOW_PASS) return;
    
    if(shadow_buffer_r >= window_height || shadow_buffer_r < 0 || shadow_buffer_c >= window_width || shadow_buffer_c < 0)
    {
        return;
    }

    if(p->light_space[Z] > shadow_buffer[shadow_buffer_r][shadow_buffer_c])
    {
        set_vec4(color_buffer[r][c], 0.8,  0.8,  0.8, 1);
        return;
    }
    /* only need to write to depth buffer if in shadow pass of shadow map rendering */
    
    stencil_buffer[r][c] = current_as.stencil_bufferID;

    /* write to g buffer if deferred rendering */
    if(current_rs.render_mode)
    {
        p->rendered_flag = 1;
        g_buffer[r][c] = *p;
        return;
    }
    
    /* bump map */
    if(current_as.draw_normals_mode != F_NORMALS && current_rs.bump_mapping)
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
    int shading_surface = !(drawing_normals || drawing_bounding_box || drawing_axes);
    /* phong shading */
    if(shading_surface && current_rs.shading_mode == PHONG)
    {
        float tmp_diff[4], tmp_spec[4];
        if(current_rs.light_source == POINT_LIGHT)
        {
            set_diffuse_term(tmp_diff, p->v_normal, p->light, p->world_pos);
            set_specular_term(tmp_spec, p->v_normal, p->light, p->view, p->world_pos);
        }
        else if (current_rs.light_source == GLOBAL_LIGHT)
        {
            set_diffuse_term(tmp_diff, p->v_normal, light, p->world_pos);
            set_specular_term(tmp_spec, p->v_normal, light, p->view, p->world_pos);
        }
        shade_point(tmp_diff, tmp_spec, p, current_rs.modulation_mode);

    }
    texture_ptr = &texture;

    /* texture mapping */
    if(!drawing_normals && !drawing_bounding_box && !drawing_axes && current_rs.texturing_mode == CUBE_MAP)
    {
        normalize(p->v_normal);
        cube_map_vec(p->v_normal, p->tex, &cube_map_index);
        texture_ptr = &cube_map[cube_map_index];

    }
    
    if(!drawing_normals && !drawing_bounding_box && !drawing_axes && current_rs.texturing_mode != TEXTURE_OFF)
    {
//        printf("texture\n");
        float s, t;
        int u, v;

        if (current_rs.perspective_correction)
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
        if(u >= 0 && u < texture_ptr->width && v >= 0 && v < texture_ptr->height)
        {
            color_buffer[r][c][R] = texture_ptr->data[v][u][R] / 255.0;
            color_buffer[r][c][G] = texture_ptr->data[v][u][G] / 255.0;
            color_buffer[r][c][B] = texture_ptr->data[v][u][B] / 255.0;
            color_buffer[r][c][A] = texture_ptr->data[v][u][A] / 255.0;
        }
        
        //any form of modulating (with color or with light)
        if(current_rs.modulation_mode)
        {
            vector_multiply(color_buffer[r][c], p->color, color_buffer[r][c]);
        }
    }
    
    /* alpha blending */
    else if(current_rs.alpha_blending)
    {
        float new_r, new_g, new_b, new_a;
        
        new_r = (1 - blend_weight) * color_buffer[r][c][R] + blend_weight * p->color[R];
        new_g = (1 - blend_weight) * color_buffer[r][c][G] + blend_weight * p->color[G];
        new_b = (1 - blend_weight) * color_buffer[r][c][B] + blend_weight * p->color[B];
        new_a = (1 - blend_weight) * color_buffer[r][c][A] + blend_weight * p->color[A];
        
        /* write blended color to color_buffer */
        set_vec4(color_buffer[r][c], new_r, new_g, new_b, new_a);
    }
    
    /* just write interpolated point color into color buffer */
    else
    {
        cpy_vec4(color_buffer[r][c], p->color);
    }
    
    /* fog effect */
    if(current_rs.fog_fx)
    {
        float z = p->position[Z];
        float new_r, new_g, new_b, new_a;
        
        new_r = z * fog_color[R] + (1 - z) * color_buffer[r][c][R];
        new_g = z * fog_color[G] + (1 - z) * color_buffer[r][c][G];
        new_b = z * fog_color[B] + (1 - z) * color_buffer[r][c][B];

        set_vec4(color_buffer[r][c], new_r, new_g, new_b, 1);

    }
    

}



