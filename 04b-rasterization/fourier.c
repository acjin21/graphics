#include "fourier.h"
#include "vector.h"
#include <math.h>

IMAGE_F     x;
IMAGE_F     y;
IMAGE_F     p;
IMAGE_F     r;
IMAGE_F     i;


/*
 * convert_to_float_image
 */
void convert_to_float_image( IMAGE *input, IMAGE_F *output )
{
    float   r;
    float   g;
    float   b;
    float   a;
    
    output->width   = input->width;
    output->height  = input->height;
    
    for( int j = 0; j < input->height; j++ )
    {
        for( int i = 0; i < input->width; i++ )
        {
            r = input->data[j][i][R] / 255.0;
            g = input->data[j][i][G] / 255.0;
            b = input->data[j][i][B] / 255.0;
            a = input->data[j][i][A] / 255.0;
            
            output->data[j][i][R] = CLAMP( r, 0, 1);
            output->data[j][i][G] = CLAMP( g, 0, 1);
            output->data[j][i][B] = CLAMP( b, 0, 1);
            output->data[j][i][A] = CLAMP( a, 0, 1);
        }
    }
}

/*
 * convert_to_image
 */
void convert_to_image( IMAGE_F *input, IMAGE *output )
{
    float   r;
    float   g;
    float   b;
    float   a;
    
    output->width   = input->width;
    output->height  = input->height;
    
    for( int j = 0; j < input->height; j++ )
    {
        for( int i = 0; i < input->width; i++ )
        {
            r = input->data[j][i][R] * 255.0;
            g = input->data[j][i][G] * 255.0;
            b = input->data[j][i][B] * 255.0;
            a = input->data[j][i][A] * 255.0;
            
            output->data[j][i][R] = CLAMP( r, 0, 255);
            output->data[j][i][G] = CLAMP( g, 0, 255);
            output->data[j][i][B] = CLAMP( b, 0, 255);
            output->data[j][i][A] = CLAMP( a, 0, 255);
        }
    }
}

/* 2d discrete fourier transform */
void dft_2D(IMAGE_F *x, IMAGE_F *real, IMAGE_F *imaginary)
{
    int M = x->height;
    int M_2 = M / 2;
    int N = x->width;
    int N_2 = N / 2;

    int u, v, m, n;
    float theta, c, s;
    float tmp[4], x_sum[4], y_sum[4];

    real->height = imaginary->height = M;
    real->width = imaginary->width = N;

    for(v = 0; v < M; v++) //rows
    {
        for(u = 0; u < N; u++) //cols
        {
            set_vec4(x_sum, 0, 0, 0, 0);
            set_vec4(y_sum, 0, 0, 0, 0);

            for(m = 0; m < M; m++)
            {
                for(n = 0; n < N; n++)
                {
                    theta = (2.0 * PI) / (M * N) * ((n - N_2) * (u - N_2) * M + ((m - M_2) * (v - M_2) * N));
                    c = cos(theta);
                    s = sin(theta);

                    scalar_multiply(c, x->data[m][n], tmp);
                    vector_add(x_sum, tmp, x_sum);

                    scalar_multiply(s, x->data[m][n], tmp);
                    vector_subtract(y_sum, tmp, y_sum);
                }
            }
            cpy_vec4(real->data[v][u], x_sum);
            cpy_vec4(imaginary->data[v][u], y_sum);
        }
    }
}

/* inverse 2d discrete fourier transform */
void invdft_2D(IMAGE_F *x, IMAGE_F *y, IMAGE_F *real, IMAGE_F *imaginary)
{
    int M = x->height;
    int N = x->width;

    int M_2 = M / 2;
    int N_2 = N / 2;
    
    int u, v, m, n;
    float theta, c, s;
    float tmp[4], x_sum[4], y_sum[4];
    
    real->height = imaginary->height = M;
    real->width = imaginary->width = N;
    
    for(v = 0; v < M; v++) //rows
    {
        for(u = 0; u < N; u++) //cols
        {
            set_vec4(x_sum, 0, 0, 0, 0);
            set_vec4(y_sum, 0, 0, 0, 0);
            
            for(m = 0; m < M; m++)
            {
                for(n = 0; n < N; n++)
                {
                    theta  = 2.0 * PI * (-1 * (n-N_2) * (u-N_2) * M - (m-M_2) * (v-M_2) * N) / (N * M);
                    c = cos(theta);
                    s = sin(theta);
                    
                    scalar_multiply(c, x->data[m][n], tmp);
                    vector_add(x_sum, tmp, x_sum);
                    scalar_multiply(s, y->data[m][n], tmp);
                    vector_add(x_sum, tmp, x_sum);
                    
                    scalar_multiply(c, y->data[m][n], tmp);
                    vector_add(y_sum, tmp, y_sum);
                    scalar_multiply(s, x->data[m][n], tmp);
                    vector_subtract(y_sum, tmp, y_sum);
                }
            }
            scalar_multiply(1.0 / (M * N), x_sum, real->data[v][u]);
            scalar_multiply(1.0 / (M * N), y_sum, imaginary->data[v][u]);
        }
    }
}

void power_spectrum (IMAGE_F *real, IMAGE_F *imaginary, IMAGE_F *output)
{
    float tmp_r[4], tmp_i[4];
    int m, n;
    output->width = real->width;
    output->height = real->height;
    
    for(m = 0; m < real->height; m++)
    {
        for(n = 0; n < real->width; n++)
        {
            cpy_vec4 (tmp_r, real->data[m][n]);
            vector_multiply (tmp_r, tmp_r, tmp_r);
            
            cpy_vec4 (tmp_i, imaginary->data[m][n]);
            vector_multiply (tmp_i, tmp_i, tmp_i);
            
            vector_add (tmp_r, tmp_i, tmp_r);
            
            output->data[m][n][R] = sqrt(tmp_r[R]);
            output->data[m][n][G] = sqrt(tmp_r[G]);
            output->data[m][n][B] = sqrt(tmp_r[B]);
            output->data[m][n][A] = sqrt(tmp_r[A]);

        }
    }
}
