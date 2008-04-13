/******************************************************************************/
// lev_files.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from lev_files.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_LEVFILES_H
#define ADIKT_LEVFILES_H

#include "lev_data.h"

// No error condition need to be same as in read_file (MFILE_OK)
// There should be no more than 32 errors (up to -31)
// values to 23 are reserved by read_file and RNC library
#define ERR_NONE           0
#define ERR_FILE_TOOSMLL -24
#define ERR_FILE_BADDATA -25
#define ERR_INTERNAL     -26
#define WARN_BAD_COUNT    24

short save_map(struct LEVEL *lvl);

short load_map(struct LEVEL *lvl);
short load_map_preview(struct LEVEL *lvl);

unsigned short script_load_and_execute(struct LEVEL *lvl,char *fname,char *err_msg);
short write_text_file(char **lines,int lines_count,char *fname);

char *load_error(int errcode);

#endif // ADIKT_LEVFILES_H
