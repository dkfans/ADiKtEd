/******************************************************************************/
/** @file xtabjty.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from xtabjty.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     13 Mar 2007
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef BULL_XTABJTY_H
#define BULL_XTABJTY_H

#include "globals.h"
#include "xtabdat8.h"
#include "lbfileio.h"


/* Error returns */

#define JTYTB_FILE_BADDATA  1
#define JTYTB_CANNOT_OPEN   2
#define JTYTB_MALLOC_ERR    3
#define JTYTB_INTERNAL      4
#define JTYTB_CANNOT_READ   5

#define JTYTAB_ENTRY_SIZE 16

/* Data types */

struct JTYTAB_ITEM {
    unsigned long offset;
    unsigned int width;
    unsigned int height;
    struct IMGTAB_ATTRIB attrib;
       };

struct JTYTABFILE {
    long count;
    unsigned long filelength;
    struct JTYTAB_ITEM *items;
       };

/* Routines */

DLLIMPORT short alloc_jtytabfile_data(struct JTYTABFILE *jtabf,long count);
DLLIMPORT short free_jtytabfile_data(struct JTYTABFILE *jtabf);

DLLIMPORT short read_jtytabfile_data(struct JTYTABFILE *jtabf,const char *srcfname);
DLLIMPORT short write_jtytabfile_data(const struct JTYTABFILE *jtabf,const char *dstfname);
DLLIMPORT short switch_jtytabfile_entries(struct JTYTABFILE *jtabf,unsigned long idx1,unsigned long idx2);

DLLIMPORT short create_images_jtytab_idx(struct IMAGELIST *images,const char *jtyfname,const char *tabfname,int verbose);
DLLIMPORT int read_jtytab_images(struct IMAGELIST *images,unsigned long *readcount,
    const struct JTYTABFILE *jtabf,const struct DATFILE *jtyf,const int verbose);

DLLIMPORT short create_images_jtytab_enc(struct ENCIMAGELIST *images,const char *jtyfname,
    const char *tabfname,const int verbose);
DLLIMPORT short write_jtytab_images_enc(const struct ENCIMAGELIST *images,const char *jtyfname,
    const char *tabfname,const int verbose);
DLLIMPORT int read_jtytab_encimages(struct ENCIMAGELIST *images,unsigned long *readcount,
    const struct JTYTABFILE *jtabf,const struct DATFILE *jtyf,const int verbose);
DLLIMPORT int write_encimages_jtytab(const struct ENCIMAGELIST *images,unsigned long *writecount,
    struct JTYTABFILE *jtabf,struct DATFILE *jtyf);
DLLIMPORT short free_dattab_encimages(struct ENCIMAGELIST *images);
DLLIMPORT short switch_encimage_entries(struct ENCIMAGELIST *images,unsigned long idx1,unsigned long idx2);

#endif
