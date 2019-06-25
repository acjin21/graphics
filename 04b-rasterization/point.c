
#include "point.h"
#include <stdio.h>
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
int phong_shading = OFF;
extern float light[4];
float ambient[4] = {0.3, 0.3, 0.3, 0};

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

    if(texturing)
    {
        float s, t;
        int u, v;
        
        if( perspective_correct )
        {
//            printf("foo\n");
            s = p->tex[S];
            t = p->tex[T];
            
            float z = 1.0 / p->position[Z];
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
        vector_add(color_buffer[r][c], ambient, color_buffer[r][c]);
    }
    if(depth_test)
    {
        depth_buffer[r][c] = p->position[Z];
    }
    if(phong_shading)
    {
        float brightness = vector_dot(p->v_normal, light);
        color_buffer[r][c][R] *= brightness;
        color_buffer[r][c][G] *= brightness;
        color_buffer[r][c][B] *= brightness;
        color_buffer[r][c][A] = p->color[A];
        vector_add(color_buffer[r][c], ambient, color_buffer[r][c]);
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

