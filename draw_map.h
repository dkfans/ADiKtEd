/*
 * draw_map.h header file for draw_map.c
 */

#ifndef ADIKT_DRAWMAP_H
#define ADIKT_DRAWMAP_H

struct LEVEL;

#define TEXTURE_SIZE_X 32
#define TEXTURE_SIZE_Y 32
#define TEXTURE_COUNT_X 8
#define TEXTURE_COUNT_Y 68

struct MAPDRAW_POINT {
    int x;
    int y;
};

struct MAPDRAW_TXTRPOINT {
    unsigned short x;
    unsigned short y;
};

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
    struct MAPDRAW_TXTRPOINT n;
       //south
    struct MAPDRAW_TXTRPOINT s;
       //west
    struct MAPDRAW_TXTRPOINT w;
       //east
    struct MAPDRAW_TXTRPOINT e;
       //top
    struct MAPDRAW_TXTRPOINT t;
       //bottom
    struct MAPDRAW_TXTRPOINT b;
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
    struct MAPDRAW_POINT start;
    struct MAPDRAW_POINT end;
    short rescale;
};

short generate_map_bitmap(const char *bmpfname,const struct LEVEL *lvl,const short rescale);
short generate_map_bitmap_mapfname(struct LEVEL *lvl);

#endif // ADIKT_DRAWMAP_H
