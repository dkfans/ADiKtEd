/******************************************************************************/
// scr_thing.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   KeeperFX Team
// Created:  08 Dec 2020

// Purpose:
//   Header file. Defines exported routines from scr_tileset.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_TILESET_H
#define ADIKT_TILESET_H

extern void actions_mdtileset(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
extern void draw_mdtileset(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
extern short start_mdtileset(struct SCRMODE_DATA *,struct WORKMODE_DATA *);
extern void end_mdtileset(struct SCRMODE_DATA *,struct WORKMODE_DATA *);
extern short color_mdtileset(struct SCRMODE_DATA *scrmode, struct MAPMODE_DATA *mapmode, struct LEVEL *lvl, int tx, int ty);

#endif