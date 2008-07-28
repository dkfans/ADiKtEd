/******************************************************************************/
/** @file draw_map.h
 * Graphics draw of Dungeon Keeper map.
 * @par Purpose:
 *     Header file. Defines exported routines from draw_map.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     12 Jan 2008 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_DRAWMAP_H
#define ADIKT_DRAWMAP_H

#include "globals.h"

struct LEVEL;
struct CUBES_DATA;
struct IMAGELIST;

struct PALETTE_ENTRY {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char o;
};

struct MAPDRAW_DATA {
    struct CUBES_DATA *cubes;
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

// Disk bitmap drawing

DLLIMPORT short generate_map_bitmap(const char *bmpfname,const struct LEVEL *lvl,
    const struct MAPDRAW_OPTIONS *opts);
DLLIMPORT short generate_map_bitmap_mapfname(struct LEVEL *lvl);

// Memory buffer drawing

DLLIMPORT short load_draw_data(struct MAPDRAW_DATA **draw_data,const struct MAPDRAW_OPTIONS *opts,
    const struct IPOINT_2D bmp_size,int textr_idx);
DLLIMPORT short free_draw_data(struct MAPDRAW_DATA *draw_data);
DLLIMPORT short draw_map_on_buffer(char *dest,const struct LEVEL *lvl,
    const struct MAPDRAW_DATA *draw_data);
DLLIMPORT short set_draw_data_rect(struct MAPDRAW_DATA *draw_data,
    const int startx,const int starty,const int endx,const int endy,const short rescale);
DLLIMPORT short set_draw_data_rect_ip(struct MAPDRAW_DATA *draw_data,
    const struct IPOINT_2D start,const struct IPOINT_2D end,const short rescale);

#endif // ADIKT_DRAWMAP_H
