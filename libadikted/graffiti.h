/******************************************************************************/
// graffiti.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from graffiti.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_GRAFFITI_H
#define ADIKT_GRAFFITI_H

struct LEVEL;
struct COLUMN_REC;
struct DK_GRAFFITI;

#include "globals.h"

// Imports from graffiti.c
DLLIMPORT short level_free_graffiti(struct LEVEL *lvl);
DLLIMPORT int place_graffiti_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty);

DLLIMPORT unsigned int get_graffiti_count(struct LEVEL *lvl);
DLLIMPORT struct DK_GRAFFITI *get_graffiti(struct LEVEL *lvl, int graf_idx);
DLLIMPORT char *get_graffiti_text(struct LEVEL *lvl,unsigned int num);
DLLIMPORT int graffiti_idx(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT int graffiti_idx_next(struct LEVEL *lvl, int tx, int ty,int prev_idx);
DLLIMPORT struct DK_GRAFFITI *create_graffiti(int tx, int ty, char *text, const struct LEVEL *lvl, int orient);
DLLIMPORT int graffiti_add_obj(struct LEVEL *lvl,struct DK_GRAFFITI *graf);
DLLIMPORT int graffiti_add(struct LEVEL *lvl,int tx, int ty,int height, char *text,int font,
      unsigned short orient,unsigned short cube);
DLLIMPORT void graffiti_del(struct LEVEL *lvl,unsigned int num);
DLLIMPORT short place_graffiti_on_column(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,unsigned short cube);
DLLIMPORT short place_graffiti_on_clm_top(struct COLUMN_REC *clm_rec,unsigned short font,
        unsigned short height,char *text,int graf_subtl,int graf_subtl_h,
        unsigned short cube);
DLLIMPORT int compute_graffiti_subtl_length(unsigned short font,char *text);
DLLIMPORT void graffiti_update_columns(struct LEVEL *lvl,int graf_idx);
DLLIMPORT void graffiti_clear_from_columns(struct LEVEL *lvl,int graf_idx);
DLLIMPORT short set_graffiti_orientation(struct DK_GRAFFITI *graf,const struct LEVEL *lvl,unsigned short orient);
DLLIMPORT int set_graffiti_height(struct DK_GRAFFITI *graf,int height);

DLLIMPORT int get_graffiti_cube_height(unsigned short font,char *text);
DLLIMPORT const unsigned char *get_font_char(unsigned short font,char chr);


// Import the font from graffiti_font.c
extern const unsigned char font_any_chnull[];
extern const unsigned char *font_adiclssc_chars[];
extern const int font_adiclssc_height;
extern const unsigned char *font_adisize8_chars[];
extern const int font_adisize8_height;

#endif // ADIKT_GRAFFITI_H
