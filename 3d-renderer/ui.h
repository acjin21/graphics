#ifndef UI_H
#define UI_H

#include "state.h"

void gl_print_image_settings (IMAGE_PROCESSING_STATE *ips);
void gl_printf( int x, int y, char *s, float color[4]);
void gl_print_app_settings (APP_STATE *as);
void gl_print_render_settings (RENDER_STATE *rs);

#endif
