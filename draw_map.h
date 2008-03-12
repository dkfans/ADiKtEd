/*
 * draw_map.h header file for draw_map.c
 */

#ifndef ADIKT_DRAWMAP_H
#define ADIKT_DRAWMAP_H

#include "globals.h"

struct LEVEL;

#define TEXTURE_SIZE_X 32
#define TEXTURE_SIZE_Y 32
#define TEXTURE_COUNT_X 8
#define TEXTURE_COUNT_Y 68

struct PALETTE_ENTRY {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char o;
};

struct CUBES_DATA {
    int count;
    struct CUBE_TEXTURES *data;
};

//Texture indices for every side of a cube
struct CUBE_TEXTURES {
       //north
    struct USPOINT_2D n;
       //south
    struct USPOINT_2D s;
       //west
    struct USPOINT_2D w;
       //east
    struct USPOINT_2D e;
       //top
    struct USPOINT_2D t;
       //bottom
    struct USPOINT_2D b;
};

struct MAPDRAW_DATA {
    struct CUBES_DATA cubes;
    unsigned char *texture;
    struct PALETTE_ENTRY *palette;
    struct PALETTE_ENTRY *ownerpal;
    struct PALETTE_ENTRY *intnspal;
    // Things and other objects
    struct IMAGELIST *images;
    // Where to start/end drawing
    struct IPOINT_2D start;
    struct IPOINT_2D end;
    short rescale;
};

extern short bitmap_rescale;

short generate_map_bitmap(const char *bmpfname,const struct LEVEL *lvl,const short rescale);
short generate_map_bitmap_mapfname(struct LEVEL *lvl);

#endif // ADIKT_DRAWMAP_H
