#include "post-processing.h"

#include "image.h"
#include "color.h"

extern float color_buffer[800][800][4];

IMAGE texture00;
IMAGE texture01;

/* copy rgba vals [0, 1] in color_buffer to byte channels in image [0, 255] */
void color_buffer_to_image (IMAGE *img)
{
    img->width = WIN_W;
    img->height = WIN_H;
    
    for(int j = 0; j < img->height; j++)
    {
        for(int i = 0; i < img->width; i++)
        {
            int r, g, b;
            r = (int) (color_buffer[j][i][R] * 255.);
            g = (int) (color_buffer[j][i][G] * 255.);
            b = (int) (color_buffer[j][i][B] * 255.);
            
            img->data[j][i][R] = CLAMP(r, 0, 255);
            img->data[j][i][G] = CLAMP(g, 0, 255);
            img->data[j][i][B] = CLAMP(b, 0, 255);
        }
    }
}

/* copy byte channels in image [0, 255] to rgba vals [0, 1] in color_buffer */
void image_to_color_buffer (IMAGE *img)
{
    img->width = WIN_W;
    img->height = WIN_H;
    
    for(int j = 0; j < img->height; j++)
    {
        for(int i = 0; i < img->width; i++)
        {
            color_buffer[j][i][R] = img->data[j][i][R] / 255.0;
            color_buffer[j][i][G] = img->data[j][i][G] / 255.0;
            color_buffer[j][i][B] = img->data[j][i][B] / 255.0;
        }
    }
}

/* post processing on color buffer */
void apply_post_processing (void)
{
    color_buffer_to_image(&texture00);
    
    //some image processing
    lincoln(&texture00, &texture01, 2);
    
    image_to_color_buffer(&texture01);
}
