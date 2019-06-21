#include "texture.h"

#include <stdio.h>
#include <string.h>

/*************************************************************************/
/* textures */
/*************************************************************************/
/* generate a random tv screen texture */
void random_texture (IMAGE *img)
{
    img->width = 256;
    img->height = 256;
    for(int j = 0; j < 256; j++)
    {
        for(int i = 0; i < 256; i++)
        {
            img->data[j][i][R] = random_float(0, 255);
            img->data[j][i][G] = random_float(0, 255);
            img->data[j][i][B] = random_float(0, 255);
            img->data[j][i][A] = 1;
        }
    }
}

/* generate checkerboard texture */
void checkerboard_texture (IMAGE *img)
{

    img->width = 256;
    img->height = 256;
    
    int num_squares = 16;
    int px_per_sq = img->width / num_squares;
    
    int color;
    for(int j = 0; j < 256; j++)
    {
        color = ((j / px_per_sq) % 2 == 0) ? 0 : 255;
        
        for(int i = 0; i < 256; i++)
        {
            if(i % px_per_sq == 0 && i / px_per_sq != 0)
            {
                /* if start new block and not 0, switch color */
                color = (color == 255 ? 0 : 255);
            }
            img->data[j][i][R] = color;
            img->data[j][i][G] = color;
            img->data[j][i][B] = color;
            img->data[j][i][A] = 1;
        }
    }
}

/* read a .ppm or .pgm file with option comment into texture */
void read_ppm (char *file_name, IMAGE *img)
{
    FILE *fp;
    char ppm_type[5];

    fp = fopen(file_name, "r");
    int width, height;
    float max;
    
    fscanf(fp, "%s\n", ppm_type);
    
    /* handle any possible comments */
    int next_ch;
    char comment[500];
    next_ch = getc(fp);
    if(next_ch == '#')
    {
        fgets(comment, 500, fp);
    }
    else
    {
        ungetc(next_ch, fp);
    }
    
    fscanf(fp, "%i %i\n" , &width, &height);
//    printf("%i, %i\n", width, height);
    fscanf(fp, "%f\n", &max);
    
    img->width = width;
    img->height = height;
    int r, g, b, a, c;
    if(strcmp(ppm_type, "P3") == 0)
    {
        for (int t = 0; t < height; t++)
        {
            for (int s = 0; s < width; s++)
            {
                fscanf(fp, "%i %i %i", &r, &g, &b);
                img->data[t][s][R] = (float) r / max * 255.0;
                img->data[t][s][G] = (float) g / max * 255.0;
                img->data[t][s][B] = (float) b / max * 255.0;
                img->data[t][s][A] = 1;
            }
        }
    }
    else if (!strcmp(ppm_type, "P2"))
    {
        for (int t = 0; t < height; t++)
        {
            for (int s = 0; s < width; s++)
            {
                fscanf(fp, "%i", &c);
                img->data[t][s][R] = (float) c / max * 255.0;
                img->data[t][s][G] = (float) c / max * 255.0;
                img->data[t][s][B] = (float) c / max * 255.0;
                img->data[t][s][A] = 1;
            }
        }
    }
    fclose(fp);
}

void write_ppm (char *file_name, IMAGE *img)
{
    FILE *fp;
    fp = fopen(file_name, "w");

    
    if (fp == NULL)
    {
        printf("Unable to open file %s\n", file_name);
    }
    else
    {
        printf("%s has been opened and its contents overwritten.\n", file_name);
        fputs("P3\n", fp);
        fprintf(fp, "%i %i\n", img->width, img->height);
        fprintf(fp, "%i\n", 255);
        for(int j = 0; j < img->height; j++)
        {
            for(int i = 0; i < img->width; i++)
            {
                int r = img->data[j][i][R];
                int g = img->data[j][i][G];
                int b = img->data[j][i][B];
                fprintf(fp, "%i %i %i\n", r, g, b);
            }
        }
        printf("Done writing ppm file to %s\n", file_name);
        fclose(fp);
    }
}

/* clear texture image to specified rgba values */
void clear_texture (IMAGE *texture, float r, float g, float b, float a)
{
    for(int i = 0; i < 2000; i++)
    {
        for(int j = 0; j < 2000; j++)
        {
            texture->data[j][i][R] = r;
            texture->data[j][i][G] = g;
            texture->data[j][i][B] = b;
            texture->data[j][i][A] = a;
        }
    }
}
