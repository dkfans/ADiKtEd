/*
 * xtabjty.h   define exported routines from xtabjty.c
 */

#ifndef BULL_XTABJTY_H
#define BULL_XTABJTY_H

#include "xtabdat8.h"

// Data types

#define JTYTB_FILE_BADDATA  1
#define JTYTB_CANNOT_OPEN   2
#define JTYTB_MALLOC_ERR    3
#define JTYTB_INTERNAL      4
#define JTYTB_CANNOT_READ   5

#define JTYTAB_ENTRY_SIZE 16

struct JTYTAB_ITEM {
    unsigned long offset;
    unsigned int width;
    unsigned int height;
    unsigned char unkn[10];
       };

struct JTYTABFILE {
    long count;
    unsigned long filelength;
    struct JTYTAB_ITEM *items;
       };


// Routines

short alloc_jtytabfile_data(struct JTYTABFILE *jtabf,long count);
short free_jtytabfile_data(struct JTYTABFILE *jtabf);

short read_jtytabfile_data(struct JTYTABFILE *jtabf,const char *srcfname);
short write_jtytabfile_data(const struct JTYTABFILE *jtabf,const char *dstfname);

short create_images_jtytab_idx(struct IMAGELIST *images,const char *jtyfname,const char *tabfname,int verbose);
int read_jtytab_images(struct IMAGELIST *images,unsigned long *readcount,struct JTYTABFILE *jtabf,struct DATFILE *jtyf,int verbose);

short switch_jtytabfile_entries(struct JTYTABFILE *jtabf,unsigned long idx1,unsigned long idx2);

#endif
