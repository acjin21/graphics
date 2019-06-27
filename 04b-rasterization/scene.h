#ifndef SCENE_H
#define SCENE_H

#define MAX_N_OBJECTS 10


typedef struct object {
    int type;
    float center[3];
    float scale;
    float radii[2];
} OBJECT;

void write_scene (char *file_name);
void read_scene (char *file_name);
#endif
