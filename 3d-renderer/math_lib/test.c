#include "mat3.h"
#include "mat4.h"
#include "../vector.h"
#include <stdio.h>

#ifdef TESTING
int main()
{
#if 0
    MAT3 test1;
    set_mat3(&test1,
             3, 0, 10,
             4, -4, 6,
             2, 3, -1.2);
    float det = determinant(&test1);
    printf("Matrix: \n");
    print_mat3(&test1);
    printf("Determinant = %.2f\n", determinant(&test1));
    
    MAT4 test2;
    set(&test2,
            1, 2, 3, 4,
            7, 8, 9, 10,
            -1, -3.2, 1, 4,
            8, 2, 5.5, 1);
    printf("Matrix: \n");
    print_mat4(&test2);
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            printf("%.2f\t", cofactor_mat4(&test2, r, c));
        }
        printf("\n");
    }
#endif
    
    MAT4 test3, test3_inv;
    set(&test3,
        5, 6, 6, 8,
        2, 2, 2, 8,
        6, 6, 2, 8,
        2, 3, 6, 7);
    printf("Matrix: \n");
    print_mat4(&test3);
    invert_mat4(&test3_inv, &test3);
    printf("Inverse: \n");

    print_mat4(&test3_inv);

    return 0;

}
#endif
