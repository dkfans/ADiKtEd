/******************************************************************************/
/** @file xtabdat8.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from xtabdat8.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     05 Jul 2005
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef BULL_XTABDAT8_H
#define BULL_XTABDAT8_H

#if (defined(MAIN_XTABDAT8)||defined(MAIN_XTABJTY))
 #define msgprintf printf
#else
 #define msgprintf message_log
#endif

/* Error returns */

#define XTABDAT8_COLOUR_LEAK 2
#define XTABDAT8_ENDOFBUFFER 4
#define XTABDAT8_NOMEMORY    8
#define XTABDAT8_CANT_OPEN   9
#define XTABDAT8_CANT_READ  10
#define XTABDAT8_MALLOC_ERR 11
#define XTABDAT8_INTERNAL   12

/* Values */

#define TABFILE_ENTRY_SIZE  6
#define TABFILE_HEADER_SIZE 6

/* Data types */

struct IMGTAB_ATTRIB {
    unsigned char unkn[10];
       };

struct TABFILE_ITEM {
    unsigned long offset;
    unsigned int width;
    unsigned int height;
/*    struct IMGTAB_ATTRIB attrib; */
       };

struct TABFILE {
    long count;
    unsigned long filelength;
    struct TABFILE_ITEM *items;
       };

struct DATFILE {
    long count;
    unsigned long filelength;
    unsigned char *data;
       };

/*Compressed (rle) images */

struct ENCIMAGEITEM {
    unsigned int width;
    unsigned int height;
    struct IMGTAB_ATTRIB attrib;
    unsigned char *data;
    unsigned int datsize;
       };

struct ENCIMAGELIST {
    unsigned long count;
    struct ENCIMAGEITEM *items;
       };

/*Decompressed (bitmap) images */

struct IMAGEITEM {
    unsigned int width;
    unsigned int height;
    unsigned char *data;
    unsigned char *alpha;
       };

struct IMAGELIST {
    unsigned long count;
    struct IMAGEITEM *items;
       };

/* Routines */

DLLIMPORT int read_tabfile_data(struct TABFILE *tabf,const char *srcfname);
DLLIMPORT short write_tabfile_data(const struct TABFILE *tabf,const char *dstfname);
DLLIMPORT short free_tabfile_data(struct TABFILE *tabf);

DLLIMPORT short read_datfile_data(struct DATFILE *datf,const char *srcfname);
DLLIMPORT short write_datfile_data(const struct DATFILE *datf,const char *dstfname);
DLLIMPORT short alloc_datfile_data(struct DATFILE *datf,unsigned long filelength);
DLLIMPORT short free_datfile_data(struct DATFILE *datf);

DLLIMPORT int read_dattab_images(struct IMAGELIST *images,unsigned long *readcount,struct TABFILE *tabf,struct DATFILE *datf,const int verbose);
DLLIMPORT int free_dattab_images(struct IMAGELIST *images);
DLLIMPORT int read_dat_image_idx(struct IMAGEITEM *image,unsigned long *readedsize,
    const struct DATFILE *datf,const unsigned long off,
    const unsigned int width,const unsigned int height);

DLLIMPORT int read_dattab_encimages(struct ENCIMAGELIST *images,unsigned long *readcount,
    const struct TABFILE *tabf,const struct DATFILE *datf,const int verbose);
DLLIMPORT int write_encimages_dattab(const struct ENCIMAGELIST *images,unsigned long *writecount,
    struct TABFILE *tabf,struct DATFILE *datf);

DLLIMPORT short create_images_dattab_enc(struct ENCIMAGELIST *images,const char *datfname,
    const char *tabfname,const int verbose);
DLLIMPORT short write_dattab_images_enc(const struct ENCIMAGELIST *images,const char *datfname,
    const char *tabfname,const int verbose);

DLLIMPORT int create_images_dattab_idx(struct IMAGELIST *images,const char *datfname,
    const char *tabfname,const int verbose);

#endif
