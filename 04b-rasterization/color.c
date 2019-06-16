#include "color.h"
#include "macros.h"

void clear_color_buffer (float r, float g, float b, float a)
{
    for(int row = 0; row < 800; row++)
    {
        for(int col = 0; col < 800; col++)
        {
            color_buffer[row][col][R] = r;
            color_buffer[row][col][G] = g;
            color_buffer[row][col][B] = b;
            color_buffer[row][col][A] = a;
        }
    }
}
