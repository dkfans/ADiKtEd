/******************************************************************************/
/** @file arr_utils.h
 * Dungeon Keeper Array Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from arr_utils.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     15 Nov 2007 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef BULL_ARRUTILS_H
#define BULL_ARRUTILS_H

#include "globals.h"

DLLIMPORT void strip_crlf(char *string_in);
DLLIMPORT char *prepare_short_fname(const char *fname, unsigned int maxlen);
DLLIMPORT char *filename_from_path(char *pathname);
DLLIMPORT char *strdup_noquot(char *src);
DLLIMPORT char *strdup_trim(char *src);

DLLIMPORT short format_map_fname(char *fname, const char *usrinput,const char *levels_path);
DLLIMPORT short format_data_fname(char **fullname, const char *data_path, const char *format, ...);

int arr_ushort_pos(const unsigned short *arr,unsigned short arr_item,int array_count);

#endif /* BULL_ARRUTILS_H */
