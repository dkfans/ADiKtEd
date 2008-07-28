/******************************************************************************/
/** @file lev_files.h
 * Functions for loading and writing levels from/to disk.
 * @par Purpose:
 *     Header file. Defines exported routines from lev_files.c.
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997 - 27 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_LEVFILES_H
#define ADIKT_LEVFILES_H

#include "lev_data.h"

// The "No error" constant need to be same as in read_file (MFILE_OK).
// There should be no more than 32 errors (up to -31).
// values to 23 and -23 are reserved by read_file and MEMORY_FILE support.
#define ERR_NONE           0
#define ERR_FILE_TOOSMLL -24
#define ERR_FILE_BADDATA -25
#define ERR_FILE_BADNAME -26
#define ERR_CANT_MALLOC  -27
#define ERR_CANT_OPENWR  -28
#define ERR_CANT_WRITE   -29
#define ERR_VERIF        -30
#define ERR_INTERNAL     -31
#define WARN_BAD_COUNT    24

/**
 * Flags to load extra objects when reading map.
 */
enum EXTRA_OBJ_LOAD {
   EXLD_THING  = 0x01,
   EXLD_ACTNPT = 0x02,
   EXLD_STLGHT = 0x04,
     };

/**
 * Flags to ignore errors when loading/saving.
 */
enum LEVEL_FILE_FLAGS {
    LFF_IGNORE_INTERNAL           = 0x0001,
    LFF_IGNORE_CANNOT_LOAD        = 0x0002,
    LFF_DONT_EVEN_WARN            = 0x0004,
    };

#define LFF_IGNORE_NONE (0)
#define LFF_IGNORE_ALL (LFF_IGNORE_INTERNAL|LFF_IGNORE_CANNOT_LOAD)
#define LFF_IGNORE_WITHOUT_WARN (LFF_IGNORE_INTERNAL|LFF_IGNORE_CANNOT_LOAD|LFF_DONT_EVEN_WARN)

DLLIMPORT short save_map(struct LEVEL *lvl);
DLLIMPORT short user_save_map(struct LEVEL *lvl,short prior_save);

DLLIMPORT short load_map(struct LEVEL *lvl);
DLLIMPORT short load_map_preview(struct LEVEL *lvl);
DLLIMPORT short user_load_map(struct LEVEL *lvl,short new_on_error);

DLLIMPORT short script_load_and_execute(struct LEVEL *lvl,char *fname,char *err_msg);
DLLIMPORT short load_text_file(char ***lines,int *lines_count,char *fname);
DLLIMPORT short write_text_file(char **lines,int lines_count,char *fname);

DLLIMPORT short write_def_clm_source(struct LEVEL *lvl,char *fname);
DLLIMPORT short write_def_tng_source(struct LEVEL *lvl,char *fname);

DLLIMPORT char *levfile_error(int errcode);

#endif // ADIKT_LEVFILES_H
