/******************************************************************************/
// var_utils.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  15 Nov 2007

// Purpose:
//   Header file. Defines exported routines from var_utils.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_VARUTILS_H
#define ADIKT_VARUTILS_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

#if defined(unix) && !defined(GO32)
// Externs for updating console on resize
extern volatile int safe_update, update_required;
#endif


void popup_show(const char *title,const char *format, ...);

void die(const char *format, ...);
void done(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata);

#endif // ADIKT_VARUTILS_H
