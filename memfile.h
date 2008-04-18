/******************************************************************************/
// memfile.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  05 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from memfile.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_MEMFILE_H
#define ADIKT_MEMFILE_H

//max. acceptable file size for allreading operations (set to 700MB)
#define MAX_FILE_SIZE 734003200

//first 16 errors are reserved to RNC support
#define MFILE_OK             0
#define MFILE_CANNOT_OPEN  -17
#define MFILE_MALLOC_ERR   -18
#define MFILE_SIZE_ERR     -19
#define MFILE_READ_ERR     -20


struct MEMORY_FILE
{
    unsigned long len;
    unsigned char *content;
    int errcode;
};

struct MEMORY_FILE read_file(const char *iname);
char *read_file_error(int errcode);

#endif // ADIKT_MEMFILE_H
