#include "post-processing.h"

#include <stdio.h>

#include "image.h"
#include "window.h"

#include "buffers.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
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

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
/* copy rgba vals [0, 1] in color_buffer to byte channels in image [0, 255] */
void color_buffer_to_image (IMAGE *img)
{
    img->width = window_width;
    img->height = window_height;
    
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
    img->width = window_width;
    img->height = window_height;
    
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
    img->width = window_width;
    img->height = window_height;
    
    for(int j = 0; j < img->height; j++)
    {
        for(int i = 0; i < img->width; i++)
        {
            int z = (int) (depth_buffer[j][i] * 255.0);
            img->data[j][i][R] = CLAMP(z, 0, 255);
            img->data[j][i][G] = CLAMP(z, 0, 255);
            img->data[j][i][B] = CLAMP(z, 0, 255);
        }
    }
}

/* store gaussian 5x5 blur of IMAGE in into IMAGE out.
    assume that size of IMAGE in == size of IMAGE out */
void blur (IMAGE *in, IMAGE *out, float kernel[5][5])
{
    out->height = window_height;
    out->width = window_width;
    for(int j = 2; j < out->height - 2; j++)
    {
        for(int i = 2; i < out->width - 2; i++)
        {
            float accum[4] = {0, 0, 0, 0};
            int kx = 0;
            int ky = 0;
            for(int y = j - 2; y <= j + 2; y++, ky++)
            {
                kx = 0;
                for(int x = i - 2; x <= i + 2; x++, kx++)
                {
                    if(y >= 0 && y < window_height && x >= 0 && x < window_width)
                    {
                        accum[R] += (kernel[ky][kx] * in->data[y][x][R]);
                        accum[G] += (kernel[ky][kx] * in->data[y][x][G]);
                        accum[B] += (kernel[ky][kx] * in->data[y][x][B]);
                    }
                }
            }
            out->data[j][i][R] = CLAMP(accum[R], 0, 255);
            out->data[j][i][G] = CLAMP(accum[G], 0, 255);
            out->data[j][i][B] = CLAMP(accum[B], 0, 255);
        }
    }
}

/* blend sharp + blurry imgs w/ mask as blend weight and store result in out */
void blend_with_mask (IMAGE *blur, IMAGE *sharp, IMAGE *mask, IMAGE *out)
{
    out->height = window_height;
    out->width = window_width;
    for(int j = 0; j < out->height; j++)
    {
        for(int i = 0; i < out->width; i++)
        {
            float z = mask->data[j][i][R] / 255.0; //z value [0, 1] blend weight
            out->data[j][i][R] = CLAMP( z * blur->data[j][i][R] +
                                       (1 - z) * sharp->data[j][i][R], 0, 255);
            out->data[j][i][G] = CLAMP(z * blur->data[j][i][G] +
                                       (1 - z) * sharp->data[j][i][G], 0, 255);
            out->data[j][i][B] = CLAMP(z * blur->data[j][i][B] +
                                (1 - z) * sharp->data[j][i][B], 0, 255);

        }
    }
}

/* implement depth_of_field post proc effect using textures 00-03 */
void depth_of_field (void)
{
    texture03.width = window_width;
    texture03.height = window_height;

    int num_blurs = 3;
    color_buffer_to_image(&texture00); //sharp
    
    color_buffer_to_image(&texture01); //pingpong 1
    depth_buffer_to_image(&texture02);

    for(int i = 0; i < num_blurs; i++)
    {
        blur(&texture01, &texture03, gaussian_kernel);
        blur(&texture03, &texture01, gaussian_kernel);

    }
    //blend sharp and blurry images using depth buffer values
    blend_with_mask(&texture01, &texture00, &texture02, &texture03);

    image_to_color_buffer (&texture03);
}

/* apply post processing on color buffer */
void apply_post_processing (int mode)
{
    if(mode == NO_FX) return;
    IMAGE *input = &texture00;
    IMAGE *output = &texture01;

    color_buffer_to_image(input);
    switch (mode)
    {
        case NEGATIVE:
            negative(input, output);
            break;
        case FLIP_VERTICAL:
            flip_vertical(input, output);
            break;
        case FLIP_HORIZONTAL:
            flip_horizontal(input, output);
            break;
        case LUMINOSITY:
            luminosity(input, output);
            break;
        case SEPIA:
            sepia(input, output);
            break;
        case AVG:
            avg(input, output);
            break;
        case MIN:
            min(input, output);
            break;
        case MAX:
            max(input, output);
            break;
        case ROTATE_CCW:
            rotate_ccw (input, output, 90);
            break;
        case LINCOLN:
            lincoln(input, output, 2);
            break;
        case FISHEYE:
            fisheye(input, output);
            break;
        case EINSTEIN:
            einstein(input, output);
            break;
        case OIL_TRANSFER:
            oil_transfer(input, output);
            break;
        case TILING:
            tiling(input, output);
            break;
        case EDGE:
            edge_detection_wrapper(input, output);
            break;
        default:
            return; // don't do anything
    }
    
    image_to_color_buffer(output);
}


