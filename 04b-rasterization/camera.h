#ifndef CAMERA_H
#define CAMERA_H

typedef struct camera
{
    float orig_pos[4];
    float orig_u[4];
    float orig_v[4];
    float orig_up[4];
    
    float pos[4]; //current translation of camera and also used in cam matrix
    float u[4];
    float v[4]; // view/lookat vec
    float up[4];
    
    float rot[3]; //current orientation of camera
} CAMERA;

extern float eye[4];
extern CAMERA camera;

void set_camera (CAMERA *c, float pos[4], float lookat[4], float up[4]);
void rotate_camera (CAMERA *c, float rx, float ry, float rz);
void reset_camera (CAMERA *c);
void translate_camera (CAMERA *c, float tx, float ty, float tz);


#endif
