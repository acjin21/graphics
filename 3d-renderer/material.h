#ifndef MATERIAL_H
#define MATERIAL_H

/* material lighting properties type */
typedef struct material {
    float   ambient[4];
    float   diffuse[4];
    float   specular[4];
} MATERIAL;

/*************************************************************************/
/* variable externs                                                      */
/*************************************************************************/
extern float shinyness;
extern float material_diffuse[4], material_specular[4], material_ambient[4];
extern float light_diffuse[4], light_specular[4], light_ambient[4];

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
void set_material(int mat_type);
char *material_name(int mat_type);

#endif
