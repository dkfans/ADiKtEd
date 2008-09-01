/******************************************************************************/
// scr_clm.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from scr_clm.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_SCRCLM_H
#define ADIKT_SCRCLM_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

//Functions - init and free
short init_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void draw_mdclm_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

// Single actions
void action_update_all_datclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_delele_custclm_and_update(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
int display_dat_subtiles(const struct SCRMODE_DATA *scrmode,
    const struct WORKMODE_DATA *workdata,int scr_row, int scr_col,
    const short compressed,const int ty,const int tx);
int display_column(unsigned char *clmentry,int clm_idx, int scr_row, int scr_col);

//Functions - internal

#endif // ADIKT_SCRCLM_H
