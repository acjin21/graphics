#include "image.h"

#include "macros.h"
#include <math.h>
#include <stdio.h>

/*************************************************************************/
/* image processing */
/*************************************************************************/
/*
 * fill image *img with solid color (r, g, b).
 */
void fill (IMAGE *img, float r, float g, float b)
{
    //    img->width = 256;
    //    img->height = 256;
    for (int j = 0; j < img->height; j++)
    {
        for (int i = 0; i < img->width; i++)
        {
            img->data[j][i][R] = r;
            img->data[j][i][G] = g;
            img->data[j][i][B] = b;
            img->data[j][i][A] = 1;
        }
    }
}

/*
 * copy image *input into *output.
 */
void copy (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            output->data[j][i][R] = input->data[j][i][R];
            output->data[j][i][G] = input->data[j][i][G];
            output->data[j][i][B] = input->data[j][i][B];
            output->data[j][i][A] = input->data[j][i][A];
        }
    }
}

/*
 * compute the negative of input image *input and store the result in *output.
 */
void negative (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            output->data[j][i][R] = 255 - input->data[j][i][R];
            output->data[j][i][G] = 255 - input->data[j][i][G];
            output->data[j][i][B] = 255 - input->data[j][i][B];
            output->data[j][i][A] = 255 - input->data[j][i][A];
        }
    }
}

/*
 * flip the input image *input vertically and store result in *output.
 */
void flip_vertical (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    
    for (int j = 0; j < output->height; j++)
    {
        int inv_j = input->height - 1 - j;
        for (int i = 0; i < output->width; i++)
        {
            output->data[j][i][R] = input->data[inv_j][i][R];
            output->data[j][i][G] = input->data[inv_j][i][G];
            output->data[j][i][B] = input->data[inv_j][i][B];
            output->data[j][i][A] = input->data[inv_j][i][A];
        }
    }
}

/*
 * flip the input image *input horizontally and store result in *output.
 */
void flip_horizontal (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int inv_i = input->width - 1 - i;
            output->data[j][i][R] = input->data[j][inv_i][R];
            output->data[j][i][G] = input->data[j][inv_i][G];
            output->data[j][i][B] = input->data[j][inv_i][B];
            output->data[j][i][A] = input->data[j][inv_i][A];
        }
    }
}

/*
 * compute the luminosity of input pixels and
 *  write results to the corresponding pixel in *output.
 */
void luminosity (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            float luminosity = (input->data[j][i][R] + input->data[j][i][G] +
                                input->data[j][i][B]) / 3.0;
            output->data[j][i][R] = (int) (luminosity);
            output->data[j][i][G] = (int) (luminosity);
            output->data[j][i][B] = (int) (luminosity);
//            output->data[j][i][A] = 1;
        }
    }
}

/*
 * compute the sepia tone of input pixel rgb values and
 *  write results to the corresponding pixel in *output.
 */
void sepia (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int r = input->data[j][i][R];
            int g = input->data[j][i][G];
            int b = input->data[j][i][B];
            
            float sr = 0.393 * r + 0.769 * g + 0.189 * b;
            float sg = 0.349 * r + 0.686 * g + 0.168 * b;
            float sb = 0.272 * r + 0.534 * g + 0.131 * b;
            
            output->data[j][i][R] = sr > 255 ? 255 : sr;
            output->data[j][i][G] = sg > 255 ? 255 : sg;
            output->data[j][i][B] = sb > 255 ? 255 : sb;
            output->data[j][i][A] = input->data[j][i][A];
        }
    }
}

/*
 * compute the average of 3x3 neighborhood of input pixel rgb values and
 *  write results to the corresponding pixel in *output.
 */
void avg (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int total_r = 0;
            int total_g = 0;
            int total_b = 0;
            int total_a = 0;
//            printf("===========(%i, %i)===========\n", i, j);
            float count = 0;

            /* visit 3x3 neighborhood and average the rgb values */
            for(int n = j - 1; n <= j + 1; n++)
            {
                for(int m = i - 1; m <= i + 1; m++)
                {
                    if(m >= 0 && m < input->width && n >= 0 && n < input->height)
                    {
//                        printf("(%i, %i)\n", m, n);
                        total_r += input->data[n][m][R];
                        total_g += input->data[n][m][G];
                        total_b += input->data[n][m][B];
                        total_a += input->data[n][m][A];
                        count++;
                    }
                }
            }
            output->data[j][i][R] = (int) (total_r / count);
            output->data[j][i][G] = (int) (total_g / count);
            output->data[j][i][B] = (int) (total_b / count);
            output->data[j][i][A] = (int) (total_a / count);
        }
    }
}

/*
 * compute the min of 3x3 neighborhood of input pixel rgb values and
 *  write results to the corresponding pixel in *output.
 */
void min (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int min_r = 255;
            int min_g = 255;
            int min_b = 255;
            /* visit 3x3 neighborhood and find the min rgb values */
            for(int n = j - 1; n <= j + 1; n++)
            {
                for(int m = i - 1; m <= i + 1; m++)
                {
                    if(m >= 0 && m < input->width && n >= 0 && n < input->height)
                    {
                        min_r = (input->data[n][m][R] < min_r) ?
                            input->data[n][m][R] : min_r;
                        min_g = (input->data[n][m][G] < min_g) ?
                            input->data[n][m][G] : min_g;
                        min_b = (input->data[n][m][B] < min_b) ?
                            input->data[n][m][B] : min_b;
                    }
                }
            }
            output->data[j][i][R] = min_r;
            output->data[j][i][G] = min_g;
            output->data[j][i][B] = min_b;
            output->data[j][i][A] = input->data[j][i][A];
        }
    }
}

/*
 * compute the max of 3x3 neighborhood of input pixel rgb values and
 *  write results to the corresponding pixel in *output.
 */
void max (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int max_r = 0;
            int max_g = 0;
            int max_b = 0;
            /* visit 3x3 neighborhood and find the min rgb values */
            for(int n = j - 1; n <= j + 1; n++)
            {
                for(int m = i - 1; m <= i + 1; m++)
                {
                    if(m >= 0 && m < input->width && n >= 0 && n < input->height)
                    {
                        max_r = (input->data[n][m][R] > max_r) ?
                        input->data[n][m][R] : max_r;
                        max_g = (input->data[n][m][G] > max_g) ?
                        input->data[n][m][G] : max_g;
                        max_b = (input->data[n][m][B] > max_b) ?
                        input->data[n][m][B] : max_b;
                    }
                }
            }
            output->data[j][i][R] = max_r;
            output->data[j][i][G] = max_g;
            output->data[j][i][B] = max_b;
            output->data[j][i][A] = input->data[j][i][A];
        }
    }
}

/*
 * convert pixel cartesian coordinates (x, y) to
 *  polar coordinates (*radius, *angle) with *angle in degrees
 */
void cartesian_to_polar (float x, float y, float *radius, float *angle,
                         int width, int height)
{
    x -= ((float) width / 2.0);
    y -= ((float) height / 2.0);
    float r = sqrt(x * x + y * y);
    float a = atan2(y, x);
    
    *radius = r;
    /* convert from radians to degrees */
    *angle = a * 180.0 / PI;
}

/*
 * convert polar coordinates (radius, angle) with angle in degrees to
 *   pixel cartesian coordinates (*x, *y)
 */
void polar_to_cartesian (float radius, float angle, float *x, float *y,
                         int width, int height)
{
    float rad = angle * PI / 180.0;
    *x = radius * cos(rad) + ((float) width / 2.0) ;
    *y = radius * sin(rad) + ((float) height / 2.0) ;
}

/* rotates an image by angle degrees, counter-clockwise */
void rotate_ccw (IMAGE *input, IMAGE *output, float angle)
{
    int width = (output->width = input->width);
    int height = (output->height = input->height);
    
    //for each output texel, find its corresponding input texel
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            float radius, curr_angle;
            int in_i, in_j;
            
            /* get center of output texel */
            float centered_i = i + 0.5;
            float centered_j = j + 0.5;
            
            cartesian_to_polar(centered_i, centered_j, &radius, &curr_angle,
                               input->width, input->height);
            curr_angle += angle;
            polar_to_cartesian(radius, curr_angle, &centered_i, &centered_j,
                               input->width, input->height);
            in_i = (int) centered_i;
            in_j = (int) centered_j;
            
            if(in_i >= 0 && in_i < input->width && in_j >= 0 && in_j < input->height)
            {
                output->data[j][i][R] = input->data[in_j][in_i][R];
                output->data[j][i][G] = input->data[in_j][in_i][G];
                output->data[j][i][B] = input->data[in_j][in_i][B];
                output->data[j][i][A] = input->data[in_j][in_i][A];
            }
        }
    }
}

/*************************************************************************/
/* beyond photography examples */
/*************************************************************************/
#define LINCOLN 5
/* lincoln effect: pixelate image */
void lincoln (IMAGE *input, IMAGE *output, int scale)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            int new_j = (j / (LINCOLN + scale)) * (LINCOLN + scale);
            int new_i = (i / (LINCOLN + scale)) * (LINCOLN + scale);
            output->data[j][i][R] = input->data[new_j][new_i][R];
            output->data[j][i][G] = input->data[new_j][new_i][G];
            output->data[j][i][B] = input->data[new_j][new_i][B];
            output->data[j][i][A] = input->data[new_j][new_i][A];
        }
    }
}

/* fisheye effect */
void fisheye (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    float x = input->width / 2.0;
    float y = input->height / 2.0;

    float new_radius, radius, curr_angle;
    float new_i, new_j;

    float max_radius = sqrt(x * x + y * y);
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            cartesian_to_polar(i, j, &radius, &curr_angle,
                               input->width, input->height);
            
            float new_r = radius * radius / max_radius;
            
            polar_to_cartesian(new_r, curr_angle, &new_i, &new_j,
                               input->width, input->height);
            
            output->data[j][i][R] = input->data[(int) new_j][(int) new_i][R];
            output->data[j][i][G] = input->data[(int) new_j][(int) new_i][G];
            output->data[j][i][B] = input->data[(int) new_j][(int) new_i][B];
            output->data[j][i][A] = input->data[(int) new_j][(int) new_i][A];
        }
    }
}

/* einstein caricature */
void einstein (IMAGE *input, IMAGE *output)
{
    output->width = input->width;
    output->height = input->height;
    float x = input->width / 2.0;
    float y = input->height / 2.0;
    
    float new_radius, radius, curr_angle;
    float new_i, new_j;
    
    float max_radius = sqrt(x * x + y * y);
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            cartesian_to_polar(i, j, &radius, &curr_angle,
                               input->width, input->height);
            
            float new_r = sqrt(radius * max_radius);
            
            polar_to_cartesian(new_r, curr_angle, &new_i, &new_j,
                               input->width, input->height);
            
            CLAMP_IMG(new_i, 0, input->width);
            CLAMP_IMG(new_j, 0, input->height);

            output->data[j][i][R] = input->data[(int) new_j][(int) new_i][R];
            output->data[j][i][G] = input->data[(int) new_j][(int) new_i][G];
            output->data[j][i][B] = input->data[(int) new_j][(int) new_i][B];
            output->data[j][i][A] = input->data[(int) new_j][(int) new_i][A];
        }
    }
}

#define N 5

/* oil painting transformation:
    for every dot, calculate a histogram of surrounding 2N x 2N pixels and
    assign the value of the most frequently occuring brightness.
    Assumes that rgb channels all hold the same value. */
void oil_transfer (IMAGE *input, IMAGE *output)
{
    int x, y;
    int dx, dy, largest_freq, mfp;
    int histogram[256];
    output->height = input->height;
    output->width = input->width;
    for(y = N; y < output->height - N; y++)
    {
        for(x = N; x < output->width - N; x++)
        {
            /* reset the histogram */
            for(dx = 0; dx < 256; dx++)
            {
                histogram[dx] = 0;
            }
            
            /* visit each pixel in the 2N x 2N neighborhood
             and update the histogram with counts */
            for(dy = y - N; dy <= y + N; dy++)
            {
                for(dx = x - N; dx <= x + N; dx++)
                {
                    //assuming rgb channels are all the same value
                    histogram[input->data[dy][dx][R]]++;
                }
            }
            /* find the most frequent 0-255 pixel
             and color the current pixel with that value */
            largest_freq = 0;
            for(int i = 0; i < 256; i++)
            {
                if(histogram[i] > largest_freq)
                {
                    largest_freq = histogram[i];
                    mfp = i;
                }
            }
            output->data[y][x][R] = mfp;
            output->data[y][x][G] = mfp;
            output->data[y][x][B] = mfp;
            output->data[y][x][A] = input->data[y][x][A];
        }
    }
}

/* decompose image into randomly displaced tiles */
#define TILE_SIZE 50
void tiling (IMAGE *input, IMAGE *output)
{
//    clear_texture(output, 1, 1, 1, 1);

    int x, y, dx, dy;
    int ox, oy, nx, ny;
    
    output->height = input->height;
    output->width = input->width;
    
    for(y = 0; y < output->height - TILE_SIZE; y += TILE_SIZE)
    {
        for(x = 0; x < output->width - TILE_SIZE; x += TILE_SIZE)
        {
            /* calculate random displacement between tiles */
            dx = ((int) random_float(0, 20) & 31) - 16;
            dy = ((int) random_float(0, 20) & 31) - 16;
            
            /* for all the pixels of old image that correspond to one tile */
            for(oy = y; oy < y + TILE_SIZE; oy++)
            {
                for(ox = x; ox < x + TILE_SIZE; ox++)
                {
                    /* new pixel is old tile pixel + displacement */
                    nx = ox + dx;
                    ny = oy + dy;
                    
                    /* if new pixel out of bounds, do not color */
                    if(nx >= output->width || ny >= output->height
                       || nx < 0 || ny < 0)
                    {
                        continue;
                    }
                    output->data[ny][nx][R] = input->data[oy][ox][R];
                    output->data[ny][nx][G] = input->data[oy][ox][G];
                    output->data[ny][nx][B] = input->data[oy][ox][B];
                    output->data[ny][nx][A] = input->data[oy][ox][A];
                }
            }
                
        }
    }
}

unsigned char horizontal_mask[3][3] =
{
    {-1, -1, -1},
    {2, 2, 2},
    {-1, -1, -1}
};

unsigned char sobel_gx[3][3] =
{
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

unsigned char sobel_gy[3][3] =
{
    {1, 2, 1},
    {0, 0, 0},
    {-1, -2, 1}
};
void edge_detection (IMAGE *input, IMAGE  *output, unsigned char mask[3][3])
{
    output->width = input->width;
    output->height = input->height;
    
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            unsigned char new_c = 0;
            for(int dj = -1; dj <= 1; dj++)
            {
                for(int di = -1; di <= 1; di++)
                {
                    new_c += (mask[1 + dj][1 + di] * input->data[j + dj][i + di][R]);
                }
            }
            output->data[j][i][R] = new_c;
            output->data[j][i][G] = new_c;
            output->data[j][i][B] = new_c;
            output->data[j][i][A] = 1;
        }
    }
}
void combine_approx_magnitude (IMAGE *in1, IMAGE *in2, IMAGE *output)
{
    output->width = in1->width;
    output->height = in1->height;
    
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            unsigned char new_c = ABS(in1->data[j][i][R]) + ABS(in2->data[j][i][R]);
            
            output->data[j][i][R] = new_c;
            output->data[j][i][G] = new_c;
            output->data[j][i][B] = new_c;
            output->data[j][i][A] = 1;
        }
    }
}

void combine_absolute_magnitude (IMAGE *in1, IMAGE *in2, IMAGE *output)
{
    output->width = in1->width;
    output->height = in1->height;
    unsigned char gx, gy, new_c;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            gx = in1->data[j][i][R];
            gy = in2->data[j][i][R];
            new_c = ABS(gx) + ABS(gy);
//            new_c = sqrt(gx * gx + gy * gy);
            
            output->data[j][i][R] = new_c;
            output->data[j][i][G] = new_c;
            output->data[j][i][B] = new_c;
            output->data[j][i][A] = 1;
        }
    }
}
void edge_detection_wrapper (IMAGE *input, IMAGE  *output)
{
    IMAGE grayscale, sobel_x, sobel_y;
    luminosity (input, &grayscale);
    edge_detection (&grayscale, &sobel_x, sobel_gx);
    edge_detection (&grayscale, &sobel_y, sobel_gy);
    combine_approx_magnitude (&sobel_x, &sobel_y, output);

}


