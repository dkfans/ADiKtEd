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

#include "globals.h"

/**
 * Maximal acceptable file size for all reading operations.
 * Value set to 700MB.
 */
#define MAX_FILE_SIZE 734003200

//first 16 errors are reserved to RNC support
#define MFILE_OK             0
#define MFILE_CANNOT_OPEN  -17
#define MFILE_MALLOC_ERR   -18
#define MFILE_SIZE_ERR     -19
#define MFILE_READ_ERR     -20
#define MFILE_INTERNAL     -21

struct MEMORY_FILE
{
    unsigned long len;
    unsigned long alloc_len;
    unsigned long alloc_delta;
    unsigned long pos;
    unsigned char *content;
    short errcode;
};

DLLIMPORT short memfile_new(struct MEMORY_FILE **mfile, unsigned long alloc_len);
DLLIMPORT short memfile_free(struct MEMORY_FILE **mfile);
DLLIMPORT unsigned char *memfile_leave_content(struct MEMORY_FILE **mfile);
DLLIMPORT short memfile_read(struct MEMORY_FILE *mfile,const char *fname,unsigned long max_size);
DLLIMPORT short memfile_readnew(struct MEMORY_FILE **mfile,const char *fname,unsigned long max_size);
DLLIMPORT short memfile_add(struct MEMORY_FILE *mfile,
    const unsigned char *buf,unsigned long buf_len);
DLLIMPORT short memfile_set(struct MEMORY_FILE *mfile,
    unsigned char *buf,unsigned long len,unsigned long alloc_len);
DLLIMPORT short memfile_growalloc(struct MEMORY_FILE *mfile, unsigned long alloc_len);
DLLIMPORT char *memfile_error(int errcode);


//DLLIMPORT struct MEMORY_FILE read_file(const char *iname);

#endif // ADIKT_MEMFILE_H
