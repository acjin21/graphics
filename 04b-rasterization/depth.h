#ifndef DEPTH_H
#define DEPTH_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

float depth_buffer[800][800];

void clear_depth_buffer (float value);
void draw_depth_buffer (void);
#endif
