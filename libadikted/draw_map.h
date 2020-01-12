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

#define ERR_DRAW_BADTXTR -33
#define ERR_DRAW_BADCUBE -34

enum BMFONT_OPTIONS {
  BMFONT_DONT_LOAD  = 0x00,
  BMFONT_LOAD_SMALL = 0x01,
  BMFONT_LOAD_LARGE = 0x02,
};

enum TNGFLG_OPTIONS {
  TNGFLG_NONE         = 0x00,
  TNGFLG_SHOW_CIRCLES = 0x01,
};

struct LEVEL;
struct CUBES_DATA;
struct IMAGELIST;

#define SIN_ACOS_SIZE 1024

struct PALETTE_ENTRY {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char o;
};

struct MAPDRAW_DATA {
    /* Level size, in subtiles */
    struct UPOINT_2D subsize;
    /* Definitions of cubes */
    struct CUBES_DATA *cubes;
    unsigned char *texture;
    struct PALETTE_ENTRY *palette;
    struct PALETTE_ENTRY *ownerpal;
    struct PALETTE_ENTRY *intnspal;
    /* TNG/LGT/APT flags */
    short tngflags;
    /* Things and other objects */
    struct IMAGELIST *images;
    /* Fonts for drawing text messages */
    struct IMAGELIST *font0;
    struct IMAGELIST *font1;
    /* Where to start/end drawing */
    struct IPOINT_2D start;
    struct IPOINT_2D end;
    short rescale;
    unsigned int dest_scanln;
    /* Random pool */
    unsigned char *rand_pool;
    struct IPOINT_2D rand_subtl;
    unsigned int rand_count;
    unsigned int rand_size;
    unsigned long sin_acos[SIN_ACOS_SIZE];
};

/* Disk bitmap drawing */

DLLIMPORT short generate_map_bitmap(const char *bmpfname,const struct LEVEL *lvl,
    const struct MAPDRAW_OPTIONS *opts);
DLLIMPORT short generate_map_bitmap_mapfname(struct LEVEL *lvl);

/* Memory buffer drawing */

DLLIMPORT short load_draw_data(struct MAPDRAW_DATA **draw_data,const struct MAPDRAW_OPTIONS *opts,
    const struct UPOINT_2D *subtl,const struct IPOINT_2D bmp_size,int textr_idx);
DLLIMPORT short free_draw_data(struct MAPDRAW_DATA *draw_data);
DLLIMPORT short draw_map_on_buffer(char *dest,const struct LEVEL *lvl,
    struct MAPDRAW_DATA *draw_datam,unsigned int anim);
DLLIMPORT short draw_map_on_buffer_fast(char *dest,const struct LEVEL *lvl,
    struct MAPDRAW_DATA *draw_data,unsigned int anim);
DLLIMPORT short draw_things_on_buffer(char *dest,const struct LEVEL *lvl,
    struct MAPDRAW_DATA *draw_data);
DLLIMPORT short draw_text_on_buffer(char *dest,const int px,const int py,
    const char *text,struct MAPDRAW_DATA *draw_data,short font);

/* Helper functions */

DLLIMPORT short change_draw_data_texture(struct MAPDRAW_DATA *draw_data,
    const struct MAPDRAW_OPTIONS *opts,const int textr_idx);
DLLIMPORT short set_draw_data_rect(struct MAPDRAW_DATA *draw_data,
    const int startx,const int starty,const int endx,const int endy,
    const unsigned int scanline,const short rescale);
short set_draw_data_rect_ip(struct MAPDRAW_DATA *draw_data,
    const struct IPOINT_2D start,const struct IPOINT_2D end,
    const unsigned int scanline,const short rescale);
short set_draw_data_rect_sl4(struct MAPDRAW_DATA *draw_data,
    const int startx,const int starty,const int endx,const int endy,
    const short dest_sizex,const short rescale);

DLLIMPORT short get_draw_tile_rect(struct IPOINT_2D *tstart,
    struct IPOINT_2D *tdim, const struct MAPDRAW_DATA *draw_data,
    unsigned int tx,unsigned int ty);
DLLIMPORT short get_draw_subtile_rect(struct IPOINT_2D *sstart,
    struct IPOINT_2D *sdim, const struct MAPDRAW_DATA *draw_data,
    unsigned int sx,unsigned int sy);
DLLIMPORT short get_subtile_from_draw_coords(unsigned int *sx,unsigned int *sy,
    const struct MAPDRAW_DATA *draw_data,
    unsigned int px,unsigned int py);
DLLIMPORT short get_subtile_from_draw_coords_ip(struct IPOINT_2D *subtl,
    struct IPOINT_2D *subpos,const struct MAPDRAW_DATA *draw_data,
    unsigned int px,unsigned int py);
DLLIMPORT short get_subtile_from_draw_coords_adv(unsigned int *ssx,unsigned int *ssy,
    const struct MAPDRAW_DATA *draw_data,unsigned int px,unsigned int py);
short get_subtile_from_draw_coords_adv_fast(unsigned int *ssx,unsigned int *ssy,
    const struct IPOINT_2D scaled_txtr_size,const struct IPOINT_2D start,
    unsigned int px,unsigned int py);
DLLIMPORT short get_full_draw_dimensions(struct IPOINT_2D *gdim,
    const struct MAPDRAW_DATA *draw_data);
DLLIMPORT short get_full_draw_dimensions_opt(struct IPOINT_2D *gdim,
    const struct UPOINT_2D *subtl,const struct MAPDRAW_OPTIONS *opts);
DLLIMPORT short get_thing_with_circle_at(unsigned int *sx,unsigned int *sy,unsigned int *num,
    const struct MAPDRAW_DATA *draw_data,const struct LEVEL *lvl,
    unsigned int px,unsigned int py);
DLLIMPORT short get_object_with_circle_at(unsigned int *sx,unsigned int *sy,unsigned int *z,
    const struct MAPDRAW_DATA *draw_data,const struct LEVEL *lvl,
    unsigned int px,unsigned int py);
#endif /* ADIKT_DRAWMAP_H */
