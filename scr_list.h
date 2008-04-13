/******************************************************************************/
// scr_list.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  05 Jan 2008

// Purpose:
//   Header file. Defines exported routines from scr_list.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_SCRLIST_H
#define ADIKT_SCRLIST_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

#define USRINPUT_ROWS 4

//Variables for navigating in list screen

struct LIST_DATA {
    int prevmode;
    int y; //the first visible row
    int pos; // selected item position (rel. to screen top)
    int items; //number of items in the list
    int cols; //number of columns in the item list
    // Parameters for specific lists
    int val1;
    int val2;
    int val3;
    unsigned char *ptr;
  };

//Functions - init and free
short init_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

short start_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

short start_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop, lower level
short start_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int lstmode);
void end_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int lstmode);
void end_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void draw_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);

void draw_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);

void draw_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);

//Functions - actions and screen, lower level
short actions_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
short actions_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_numbered_list(char *(*itemstr)(unsigned short),
        struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width);
void draw_rpanel_list(char *(*itemstr)(unsigned short),
        struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LIST_DATA *list,
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width);

//Functions - internal

#endif // ADIKT_SCRLIST_H
