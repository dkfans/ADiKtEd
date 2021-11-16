/******************************************************************************/
// scr_help.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from scr_help.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_SCRHELP_H
#define ADIKT_SCRHELP_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

#define DHFLAG_USE_COLORS 1

struct HelpInfo
{
    int numrows;
    char **lines;
};

// Help variables
struct HELP_DATA {
    int formode;
    int y;
    int rows;
    char **text;
    int drows;
    char **desc;

    int clmrows;
    char **clm;
    int slbkeyrows;
    char **slbkey;
    int tngkeyrows;
    char **tngkey;
    int crtkeyrows;
    char **crtkey;
    int itmtkeyrows;
    char **itmtkey;
    int txtrkeyrows;
    char **txtrkey;
    int cclmkeyrows;
    char **cclmkey;
    int cubekeyrows;
    char **cubekey;
    int slblkeyrows;
    char **slblkey;
    int srchkeyrows;
    char **srchkey;
    int grftkeyrows;
    char **grftkey;
    int slbrows;
    char **slb;
    int tngrows;
    char **tng;
    int crtrows;
    char **crt;
    int itmtrows;
    char **itmt;
    int scrprows;
    char **scrp;
    int txtrrows;
    char **txtr;
    int cclmrows;
    char **cclm;
    int cuberows;
    char **cube;
    int slblrows;
    char **slbl;
    int srchrows;
    char **srch;
    int rwrkrows;
    char **rwrk;
    int lmaprows;
    char **lmap;
    int grftrows;
    char **grft;
    int smaprows;
    char **smap;
    int tiprows;
    char **tips;
    int compassrows;
    char **compass;
    int mcompassrows;
    char **mcompass;
    int cubedescrows;
    char **cubedesc;
    int slabdescrows;
    char **slabdesc;
    int crtrdescrows;
    char **crtrdesc;
    int itmtdescrows;
    char **itmtdesc;
    int txtrdescrows;
    char **txtrdesc;
    int efctrows;
    char **efct;
    int efctkeyrows;
    char **efctkey;
    int efctdescrows;
    char **efctdesc;
    int traprows;
    char **trap;
    int trapkeyrows;
    char **trapkey;
    int trapdescrows;
    char **trapdesc;
    int cclmdescrows;
    char **cclmdesc;

    struct HelpInfo tileset;
  };

//Functions - init and free
short init_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short init_key_help(struct HELP_DATA *help,int mode);
short init_item_desc(struct HELP_DATA *help,int mode,int itm_idx);

//Functions - for displaying list items
char *get_cubedesc_head(struct WORKMODE_DATA *workdata,unsigned short idx);

//Functions - start and stop
short start_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
void draw_help_line(int posy,int posx,char *text,short flags);
char *get_random_tip(const struct HELP_DATA *help);

//Functions - internal
char ***match_title(struct HELP_DATA *help,const char *title, const int n);

#endif // ADIKT_SCRHELP_H
