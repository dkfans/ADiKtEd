/******************************************************************************/
// scr_txtgen.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  05 Apr 2008

// Purpose:
//   Header file. Defines exported routines from scr_txtgen.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_SCRTXTGEN_H
#define ADIKT_SCRTXTGEN_H

// This must be equal to PLAYERS_COUNT
#define GEN_PLAYERS_COUNT        6

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;
struct workmenu;
struct _menu;

enum SCRIPTGEN_FLAGS {
     SGF_ENABLED   = 0x01,
     SGF_CHANGED   = 0x02,
     SGF_STARTNEW  = 0x04,
   };

struct TXTGEN_DATA {
    struct workmenu *currmnu;
    char **script;
    int snum;

    long mpool[17]; // Pool for monsters
    long hpool[14]; // Pool for heroes

    long mavail[17][GEN_PLAYERS_COUNT]; // Availability of heroes/monsters
    long havail[14][GEN_PLAYERS_COUNT];

    long davail[4][GEN_PLAYERS_COUNT]; // Availability of doors
    long tavail[6][GEN_PLAYERS_COUNT]; // Availability of traps
    long savail[18][GEN_PLAYERS_COUNT]; // Availability of spells
    long ravail[16][GEN_PLAYERS_COUNT]; // Availability of rooms

    // NEW- not in menu yet
    long tamount[6][GEN_PLAYERS_COUNT]; // Amount of traps at start
    long damount[4][GEN_PLAYERS_COUNT]; // Amount of traps at start
    long portal_genspd;
    long plcrtrmax[GEN_PLAYERS_COUNT];
    long plgold[GEN_PLAYERS_COUNT];
    long plcomput[GEN_PLAYERS_COUNT];
    long pally[GEN_PLAYERS_COUNT][GEN_PLAYERS_COUNT];
    
  };


//Functions - init and free
short init_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
struct _menu *init_scrptgen_menus(struct WORKMODE_DATA *workdata);
void free_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_scrptgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
short scrptgen_load_mnuparams(struct TXTGEN_DATA *tgen,struct LEVEL *lvl);
short scrptgen_store_mnuparams(struct LEVEL *lvl,const struct TXTGEN_DATA *tgen);

//Functions - internal

#endif // ADIKT_SCRTXTGEN_H
