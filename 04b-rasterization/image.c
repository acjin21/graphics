#include "image.h"

#include "macros.h"
#include <math.h>

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
            int color = (int) (luminosity * 255);
            output->data[j][i][R] = color;
            output->data[j][i][G] = color;
            output->data[j][i][B] = color;
            output->data[j][i][A] = color;
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
            //            float r_scale[3] = {0.393, 0.769, 0.189};
            //            float g_scale[3] = {0.349, 0.686, 0.168};
            //            float b_scale[3] = {0.272, 0.534, 0.131};
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
            /* visit 3x3 neighborhood and average the rgb values */
            for(int n = j - 1; n <= j + 1; n++)
            {
                for(int m = i - 1; m <= i + 1; m++)
                {
                    if(m >= 0 && m < input->width && n >= 0 && n < input->height)
                    {
                        total_r += input->data[n][m][R];
                        total_g += input->data[n][m][G];
                        total_b += input->data[n][m][B];
                    }
                }
            }
            output->data[j][i][R] = (int) (total_r / 3.0);
            output->data[j][i][G] = (int) (total_g / 3.0);
            output->data[j][i][B] = (int) (total_b / 3.0);
            output->data[j][i][A] = input->data[j][i][A];
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
    x -= (width / 2);
    y -= (height / 2);
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
    *x = radius * cos(rad) + (width / 2.0);
    *y = radius * sin(rad) + (height / 2.0);
}

/* rotates an image by angle degrees, counter-clockwise */
void rotate_ccw (IMAGE *input, IMAGE *output, float angle)
{
    output->width = input->width;
    output->height = input->height;
    for (int j = 0; j < output->height; j++)
    {
        for (int i = 0; i < output->width; i++)
        {
            float radius, curr_angle, new_i, new_j;
            
            cartesian_to_polar(i, j, &radius, &curr_angle,
                               input->width, input->height);
            printf("i: %i, j: %i; r = %f, angle = %f\n", i, j, radius, curr_angle);
            curr_angle += angle;
            polar_to_cartesian(radius, curr_angle, &new_i, &new_j, input->width, input->height);
            
            if( new_j >= 0 && new_j < output->height &&
               new_i >= 0 && new_i < output->width )
            {
                output->data[(int) new_j][(int) new_i][R] = input->data[j][i][R];
                output->data[(int) new_j][(int) new_i][G] = input->data[j][i][G];
                output->data[(int) new_j][(int) new_i][B] = input->data[j][i][B];
                output->data[(int) new_j][(int) new_i][A] = input->data[j][i][A];
            }
        }
    }
}
