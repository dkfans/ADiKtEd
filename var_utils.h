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

void init_messages(void);
void free_messages(void);

void strip_crlf(char *string_in);
char *prepare_short_fname(char *fname, unsigned int maxlen);

void message_error(const char *format, ...);
void message_info(const char *format, ...);
void message_info_force(const char *format, ...);
short message_is_empty(void);
void message_release(void);
char *message_get(void);

void popup_show(const char *title,const char *format, ...);
void message_log(const char *format, ...);

int arr_ushort_pos(const unsigned short *arr,unsigned short slab_type,int array_count);

short set_msglog_fname(char *fname);
short format_map_fname(char *fname, const char *usrinput);
short format_data_fname(char **fullname, const char *format, ...);

int rnd(int range);

void die(const char *format, ...);
void done(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata);

#endif // ADIKT_VARUTILS_H
