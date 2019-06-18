
#include "point.h"

extern IMAGE texture;
extern float color_buffer[800][800][4];
extern float depth_buffer[800][800];

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define ON 1
#define OFF 0

/* modes */
int alpha_blend = OFF;
int depth_test = OFF;
int texturing = OFF;
int modulate = OFF;

/*************************************************************************/
/* draw a point */
/*************************************************************************/
void draw_point (POINT *p)
{
    glBegin(GL_POINTS);
    int r = (int) (p->position[Y] + 400);
    int c = (int) (p->position[X] + 400);
    float blend_weight = 0.50;
    
    if(texturing)
    {
        int s, t;
        s = (int) (p->tex[S] * texture.width);
        t = (int) (p->tex[T] * texture.height);
        
        if(p->tex[S] == 1 || p->tex[T] == 1)
        {
            s = p->tex[S] == 1 ? texture.width - 1 : s;
            t = p->tex[T] == 1 ? texture.width - 1 : t;
        }
        color_buffer[r][c][R] = texture.data[t][s][R] / 255.0;
        color_buffer[r][c][G] = texture.data[t][s][G] / 255.0;
        color_buffer[r][c][B] = texture.data[t][s][B] / 255.0;
        color_buffer[r][c][A] = texture.data[t][s][A] / 255.0;
        if(modulate)
        {
            color_buffer[r][c][R] *= p->color[R];
            color_buffer[r][c][G] *= p->color[G];
            color_buffer[r][c][B] *= p->color[B];
            color_buffer[r][c][A] *= p->color[A];
            
        }
    }
    else
    {
        if((depth_test && p->position[Z] < depth_buffer[r][c]) || !depth_test)
        {
            if(alpha_blend)
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
    }
    /* draw point on screen */
    glColor4f(color_buffer[r][c][R], color_buffer[r][c][G],
              color_buffer[r][c][B], color_buffer[r][c][A]);
    glVertex2f(p->position[X], p->position[Y]);
    glEnd();
}
