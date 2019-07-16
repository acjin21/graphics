#ifndef MATERIAL_H
#define MATERIAL_H

#define EMERALD             0
#define JADE                1
#define OBSIDIAN            2
#define PEARL               3
#define RUBY                4
#define TURQUOISE           5
#define BRASS               6
#define BRONZE              7
#define CHROME              8
#define COPPER              9
#define GOLD                10
#define SILVER              11
#define BLACK_PLASTIC       12
#define CYAN_PLASTIC        13
#define GREEN_PLASTIC       14
#define RED_PLASTIC         15
#define WHITE_PLASTIC       16
#define YELLOW_PLASTIC      17
#define BLACK_RUBBER        18
#define CYAN_RUBBER         19
#define GREEN_RUBBER        20
#define RED_RUBBER          21
#define WHITE_RUBBER        22
#define YELLOW_RUBBER       23

#define NUM_MATERIALS 24

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
