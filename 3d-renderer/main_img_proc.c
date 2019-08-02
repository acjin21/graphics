//#include "image.h"      //uses IMAGE typedef, image processing funcs

#define N_FILTERS 14
extern void draw_tri_test(void);
int file_index = 0; // for image processing
int filter = 0;


//IMAGE texture;          /* final display texture */
IMAGE texture0;
IMAGE texture1;
char file_names[N_PPM_FILES][100] =
{
    "ppm/blackbuck.ascii.ppm",
    "ppm/out.ppm",
    "ppm/feep.ascii.ppm",
    "ppm/feep2.ascii.ppm",
    "ppm/pbmlib.ascii.ppm",
    "ppm/sines.ascii.ppm",
    "ppm/snail.ascii.ppm",
    "ppm/star_field.ascii.ppm",
    "ppm/apollonian_gasket.ascii.pgm",
    "ppm/mona_lisa.ascii.pgm",
    "ppm/stop01.ppm",
    "ppm/me_square.ppm"
};

int main(void)
{
    clear_texture(&texture0, 0, 0, 0, 1);
    file_index %= N_PPM_FILES;
    char *ppm_file = file_names[file_index];
    read_ppm(ppm_file, &texture0);
    
    //        //    fill (&texture, 255, 0, 0);
    //        //    copy(&texture0, &texture);
    //        switch(filter % N_FILTERS)
    //        {
    //            case 0:
    //                luminosity(&texture0, &texture);
    //                write_ppm("out_ppm/luminosity.ppm", &texture);
    //                break;
    //            case 1:
    //                negative(&texture0, &texture);
    //                write_ppm("out_ppm/negative.ppm", &texture);
    //                break;
    //            case 2:
    //                flip_vertical(&texture0, &texture);
    //                break;
    //            case 3:
    //                flip_horizontal(&texture0, &texture);
    //                break;
    //            case 4:
    //                sepia(&texture0, &texture);
    //                write_ppm("out_ppm/sepia.ppm", &texture);
    //                break;
    //            case 5:
    //                avg(&texture0, &texture);
    //                break;
    //            case 6:
    //                min(&texture0, &texture);
    //                break;
    //            case 7:
    //                max(&texture0, &texture);
    //                break;
    //            case 8:
    //                clear_texture(&texture, 100, 100, 100, 1);
    //                rotate_ccw(&texture0, &texture, 90);
    //                break;
    //            case 9:
    //                lincoln(&texture0, &texture, 3);
    //                write_ppm("out_ppm/lincoln.ppm", &texture);
    //                break;
    //            case 10:
    //                clear_texture(&texture, 100, 100, 100, 1);
    //                fisheye(&texture0, &texture);
    //                write_ppm("out_ppm/fisheye.ppm", &texture);
    //                break;
    //            case 11:
    //                clear_texture(&texture, 100, 100, 100, 1);
    //                einstein(&texture0, &texture);
    //                write_ppm("out_ppm/einstein.ppm", &texture);
    //                break;
    //            case 12:
    //                clear_texture(&texture, 100, 100, 100, 1);
    //                luminosity(&texture0, &texture1);
    //                oil_transfer(&texture1, &texture);
    //                write_ppm("out_ppm/oil_paint.ppm", &texture);
    //                break;
    //            case 13:
    //                clear_texture(&texture, 100, 100, 100, 1);
    //                tiling(&texture0, &texture);
    //                break;
    //        }
    //        glPointSize(2.0);
    //    draw_tri_test();
    //    draw_color_buffer();
}


