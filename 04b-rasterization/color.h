#ifndef COLOR_H
#define COLOR_H

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

float color_buffer[800][800][4];

void clear_color_buffer(float r, float g, float b, float a);
#endif
