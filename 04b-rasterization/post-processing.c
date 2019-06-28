#include "post-processing.h"

#include "image.h"
#include "color.h"

extern float color_buffer[800][800][4];
extern float depth_buffer[800][800];

IMAGE texture00;
IMAGE texture01;
IMAGE texture02;
IMAGE texture03;



float gaussian_kernel[5][5] =
{
    {1 / 256.0, 4 / 256.0,  6 / 256.0,  4 / 256.0,  1 / 256.0},
    {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
    {6 / 256.0, 24 / 256.0, 36 / 256.0, 24 / 256.0, 6 / 256.0},
    {4 / 256.0, 16 / 256.0, 24 / 256.0, 16 / 256.0, 4 / 256.0},
    {1 / 256.0, 4 / 256.0,  6 / 256.0,  4 / 256.0,  1 / 256.0}
};

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

/* store z 'byte' in red channel of img->data */
void depth_buffer_to_image (IMAGE *img)
{
    img->width = WIN_W;
    img->height = WIN_H;
    
    for(int j = 0; j < img->height; j++)
    {
        for(int i = 0; i < img->width; i++)
        {
            int z;
            z = (int) (depth_buffer[j][i] * 255.);
            img->data[j][i][R] = CLAMP(z, 0, 255);
        }
    }
}

/* store gaussian 5x5 blur of IMAGE in into IMAGE out.
    assume that size of IMAGE in == size of IMAGE out */
void blur (IMAGE *in, IMAGE *out, float kernel[5][5])
{
    for(int j = 0; j < out->height; j++)
    {
        for(int i = 0; i < out->width; i++)
        {
            
            float accum[4] = {0, 0, 0, 0};
            int kx = 0;
            int ky = 0;
            for(int y = j - 2; y <= j + 2; y++, ky++)
            {
                for(int x = i - 2; i <= i + 2; x++, kx++)
                {
                    accum[R] += kernel[ky][kx] * in->data[y][x][R];
                    accum[G] += kernel[ky][kx] * in->data[y][x][G];
                    accum[B] += kernel[ky][kx] * in->data[y][x][B];

                }
            }
            out->data[j][i][R] = accum[R];
            out->data[j][i][G] = accum[G];
            out->data[j][i][B] = accum[B];
        }
    }
}


void blend_with_mask (IMAGE *blur, IMAGE *sharp, IMAGE *mask, IMAGE *out)
{
    for(int j = 0; j < out->height; j++)
    {
        for(int i = 0; i < out->width; i++)
        {
            float z = mask->data[j][i][R]; //z value [0, 1] is our blend weight
            out->data[j][i][R] = z * blur->data[j][i][R] +
                                (1 - z) * sharp->data[j][i][R];
            out->data[j][i][G] = z * blur->data[j][i][G] +
                                (1 - z) * sharp->data[j][i][G];
            out->data[j][i][B] = z * blur->data[j][i][B] +
                                (1 - z) * sharp->data[j][i][B];
        }
    }
}

void depth_of_field (void)
{
    int num_blurs = 4;
    color_buffer_to_image(&texture00);
    
    color_buffer_to_image(&texture01);
    depth_buffer_to_image(&texture02);
    for(int i = 0; i < num_blurs; i++)
    {
        blur(&texture01, &texture03, gaussian_kernel);
        blur(&texture03, &texture01, gaussian_kernel);
    }
    blend_with_mask (&texture01, &texture00, &texture02, &texture03);
    image_to_color_buffer (&texture03);
}



/* post processing on color buffer */
void apply_post_processing (void)
{
    color_buffer_to_image(&texture00);
    
    //some image processing
    lincoln(&texture00, &texture01, 2);
    
    image_to_color_buffer(&texture01);
}


