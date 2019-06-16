#include "texture.h"

#include <stdio.h>
#include "macros.h"
#include "util.h"

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
    int color;
    for(int j = 0; j < 256; j++)
    {
        color = ((j / 32) % 2 == 0) ? 0 : 255;
        
        for(int i = 0; i < 256; i++)
        {
            if(i % 32 == 0 && i / 32 != 0)
            {
                //                printf("switch color at column %i\n", i);
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

/* read a .ppm file into texture */
void read_ppm (char *file_name, IMAGE *img)
{
    FILE *fp;
    fp = fopen(file_name, "r");
    int width, height;
    float max;
    
    fscanf(fp, "P3\n%i %i\n", &width, &height);
    printf("width = %i, height = %i\n", width, height);
    
    fscanf(fp, "%f\n", &max);
    printf("max = %f\n", max);
    
    img->width = width;
    img->height = height;
    int r, g, b, a;
    
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
    fclose(fp);
}
