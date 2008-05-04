/******************************************************************************/
// xtabjty.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 13 Mar 2007

// Purpose:
//   Converts picture data from bullfrog's JTY/TAB files into BMPs.
//   Needs a .jty and a .tab file, and color palette

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bulcommn.h"
#include "memfile.h"
#include "xtabjty.h"
#include "xtabdat8.h"


#ifdef MAIN_XTABJTY

int main (int argc, char **argv)
{
    printf("\nBullfrog Engine JTY/TAB Pair Extractor");
    printf("\n-------------------------------\n");
    if (argc<3)
    {
        printf("Not enought parameters.\n");
        printf("Usage:\n");
        printf("  %s <datafile> <palfile>\n","xtabjty");
        printf("Output: pic????.bmp\n");
    	return 1;
    }

    printf("Loading palette ...");
    //Reading palette file
    char *palette=malloc(768);
    {
        char fname[255];
        sprintf (fname, "%s.pal", argv[2]);
        int retcode=read_palette_rgb(palette,fname,256);
        if (retcode!=0)
          switch (retcode)
          {
          case 1:
             printf("\nError - Cannot open PAL file: %s\n",fname);
             return 11;
          case 2:
             printf("\nError - Cannot read %d colors from PAL file: %s\n",256,fname);
             return 12;
          default:
             printf("\nError - Loading palette file %s returned fail code %d\n",fname,retcode);
             return 19;
          }
    }
    printf("Done.\n");

    //Reading JTY,TAB and extracting images
    struct IMAGELIST images;
    {
        char tabfname[255];
        sprintf (tabfname, "%s.tab", argv[1]);
        char jtyfname[255];
        sprintf (jtyfname, "%s.jty", argv[1]);
        int retcode=create_images_jtytab_idx(&images,jtyfname,tabfname,1);
        if (retcode!=0) return retcode;
    }

    //Looping through images and extracting to files
    printf("Extracting images into bitmaps...");
    unsigned long picnum;
    for (picnum=0;picnum<images.count;picnum++)
    {
        //printf ("\rExtracting: picture number %*d", 4, picnum);
        char fname[20];
       	sprintf (fname, "pic%0*d.bmp", 4, picnum);
        IMAGEITEM *item=&(images.items[picnum]);
        if ((item->width*item->height)>0)
            write_bmp_idx(fname, item->width, item->height, palette, item->data, 0, 1, 2, 4);
    }
    printf("completed.\n");

    free_dattab_images(&images);

    free(palette);
    return 0;
}

#endif

/*
 * Creates encoded list of images from JTY/TAB files
 */
short create_images_jtytab_enc(struct ENCIMAGELIST *images,const char *jtyfname,
    const char *tabfname,const int verbose)
{
    if (verbose) printf("Reading TAB file ...");
    //Opening TAB file
    struct JTYTABFILE jtabf;
    {
        int retcode=read_jtytabfile_data(&jtabf,tabfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case 1:
            if (verbose) printf(" Error\nCannot open TAB file \"%s\"\n",tabfname);
            return JTYTB_CANNOT_OPEN;
        default:
            if (verbose) printf(" Error\nLoading TAB file \"%s\" returned fail code %d\n",tabfname,retcode);
            return retcode;
        }
    }
    if (verbose) printf(" Done.\n");

    if (verbose) printf("Reading JTY file ...");
    //Opening DAT file
    struct DATFILE jtyf;
    {
        int retcode=read_datfile_data(&jtyf,jtyfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case JTYTB_CANNOT_OPEN:
             if (verbose) printf(" Error\nCannot open JTY file \"%s\"\n",jtyfname);
             return JTYTB_CANNOT_OPEN;
        default:
             if (verbose) printf(" Error\nLoading JTY file \"%s\" returned fail code %d\n",jtyfname,retcode);
             return retcode;
        }
    }
    if (verbose) printf(" Done.\n");

    if (verbose)
    {
        printf("\n");
        printf ("The TAB file informs of %lu pictures.\n", jtabf.count);
        if (jtabf.filelength!=jtabf.count*JTYTAB_ENTRY_SIZE)
        {
            printf("Warning - the TAB file contains incomplete entry at end.\n");
            printf(" The truncated entry will be skipped.\n");
        }
        if (jtyf.count==-1)
        {
            printf ("The JTY file informs of 4bpp content.\n");
            printf ("Warning - this is 8bpp extractor!\n");
        }
    }
    unsigned long readcount=0;
    read_jtytab_encimages(images,&readcount,&jtabf,&jtyf,verbose);

    if (verbose)
    {
        printf("Processed %lu of %lu bytes of JTY file.\n",readcount,jtyf.filelength);
        long unused=jtyf.filelength-readcount;
        if (unused>0)
        {
            printf("Bytes skipped: %ld\n",unused);
        } else  
        if (unused<0)
        {
            printf("Bytes overlapping: %ld\n",-unused);
        }
    }

    free_jtytabfile_data(&jtabf);
    free_datfile_data(&jtyf);
    return ERR_NONE;
}

/*
 * Writes encoded list of images into JTY/TAB files
 */
short write_jtytab_images_enc(const struct ENCIMAGELIST *images,const char *jtyfname,
    const char *tabfname,const int verbose)
{
    struct JTYTABFILE jtabf;
    struct DATFILE jtyf;
    unsigned long writecount=0;
    write_encimages_jtytab(images,&writecount,&jtabf,&jtyf);
    if (verbose) printf("Writing TAB file ...");
    write_jtytabfile_data(&jtabf,tabfname);
    if (verbose) printf(" Done.\n");
    if (verbose) printf("Writing DAT file ...");
    write_datfile_data(&jtyf,jtyfname);
    if (verbose) printf(" Done.\n");
    free_jtytabfile_data(&jtabf);
    free_datfile_data(&jtyf);
    return ERR_NONE;
}

short create_images_jtytab_idx(struct IMAGELIST *images,const char *jtyfname,const char *tabfname,int verbose)
{
    if (verbose) printf("Reading TAB file ...");
    //Opening TAB file
    struct JTYTABFILE jtabf;
    {
        int retcode=read_jtytabfile_data(&jtabf,tabfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case 1:
            if (verbose) printf(" Error\nCannot open TAB file \"%s\"\n",tabfname);
            return JTYTB_CANNOT_OPEN;
        default:
            if (verbose) printf(" Error\nLoading TAB file \"%s\" returned fail code %d\n",tabfname,retcode);
            return retcode;
        }
    }
    if (verbose) printf(" Done.\n");

    if (verbose) printf("Reading JTY file ...");
    //Opening DAT file
    struct DATFILE jtyf;
    {
        int retcode=read_datfile_data(&jtyf,jtyfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case JTYTB_CANNOT_OPEN:
             if (verbose) printf(" Error\nCannot open JTY file \"%s\"\n",jtyfname);
             return JTYTB_CANNOT_OPEN;
        default:
             if (verbose) printf(" Error\nLoading JTY file \"%s\" returned fail code %d\n",jtyfname,retcode);
             return retcode;
        }
    }
    if (verbose) printf(" Done.\n");

    if (verbose)
    {
        printf("\n");
        printf ("The TAB file informs of %lu pictures.\n", jtabf.count);
        if (jtabf.filelength!=(jtabf.count+1)*6)
        {
            printf("Warning - the TAB file contains incomplete entry at end.\n");
            printf(" The truncated entry will be skipped.\n");
        }
        if (jtyf.count==-1)
        {
            printf ("The JTY file informs of 4bpp content.\n");
            printf ("Warning - this is 8bpp extractor!\n");
        }
        else
            printf ("The JTY file informs of %ld pictures.\n", jtyf.count);
    }
    if (verbose) printf("Decoding images ...");
    unsigned long readcount=2;
    read_jtytab_images(images,&readcount,&jtabf,&jtyf,verbose);
    if (verbose) printf(" Done.\n");

    if (verbose)
    {
        printf("Processed %lu of %lu bytes of JTY file.\n",readcount,jtyf.filelength);
        long unused=jtyf.filelength-readcount;
        if (unused>=0)
            printf("Bytes skipped: %ld\n",unused);
          else  
            printf("Bytes overlapping: %ld\n",-unused);
    }

    free_jtytabfile_data(&jtabf);
    free_datfile_data(&jtyf);
    return ERR_NONE;
}

short read_jtytabfile_data(struct JTYTABFILE *jtabf,const char *srcfname)
{
    FILE *tabfp;
    tabfp = fopen (srcfname, "rb");
    if (!tabfp)
        return JTYTB_CANNOT_OPEN;
    jtabf->filelength=file_length_opened(tabfp);
    if (alloc_jtytabfile_data(jtabf,jtabf->filelength/JTYTAB_ENTRY_SIZE)!=ERR_NONE)
    {
        fclose(tabfp);
        return JTYTB_MALLOC_ERR;
    }
    unsigned char tabitm[JTYTAB_ENTRY_SIZE];
    int entrynum;
    for (entrynum=0;entrynum<jtabf->count;entrynum++)
    {
            struct JTYTAB_ITEM *curitm=&(jtabf->items[entrynum]);
            unsigned long readed=fread (tabitm, 1, JTYTAB_ENTRY_SIZE, tabfp);
            curitm->offset=read_long_le_buf(tabitm);
            curitm->width=tabitm[4];
            curitm->height=tabitm[5];
            memcpy(curitm->attrib.unkn,tabitm+6,10);
            if (readed < JTYTAB_ENTRY_SIZE) return JTYTB_CANNOT_READ;
    }
    fclose(tabfp);
    return ERR_NONE;
}

short write_jtytabfile_data(const struct JTYTABFILE *jtabf,const char *dstfname)
{
    FILE *tabfp;
    tabfp = fopen (dstfname, "wb");
    if (tabfp==NULL)
        return JTYTB_CANNOT_OPEN;
    int entrynum;
    for (entrynum=0;entrynum<jtabf->count;entrynum++)
    {
            struct JTYTAB_ITEM *curitm=&(jtabf->items[entrynum]);
            write_long_le_file(tabfp,curitm->offset);
            fputc(curitm->width,tabfp);
            fputc(curitm->height,tabfp);
            fwrite (curitm->attrib.unkn, 10, 1, tabfp);
    }
    fclose(tabfp);
    return ERR_NONE;
}

/*
 * Allocates given amount of entries in JTYTABFILE structure.
 */
short alloc_jtytabfile_data(struct JTYTABFILE *jtabf,long count)
{
    jtabf->count=count;
    jtabf->items=malloc(jtabf->count*sizeof(struct JTYTAB_ITEM));
    if (jtabf->items==NULL)
    {
        jtabf->count=0;
        return JTYTB_MALLOC_ERR;
    }
    return ERR_NONE;
}


/*
 * Frees all entries in JTYTABFILE structure.
 */
short free_jtytabfile_data(struct JTYTABFILE *jtabf)
{
    free(jtabf->items);
    jtabf->items=NULL;
    jtabf->filelength=0;
    jtabf->count=0;
    return ERR_NONE;
}

/*
 * Reads images from JTY/TAB pair without decoding them.
 */
int read_jtytab_encimages(struct ENCIMAGELIST *images,unsigned long *readcount,
    const struct JTYTABFILE *jtabf,const struct DATFILE *jtyf,const int verbose)
{
    images->count=jtabf->count;
    unsigned long all_items_size=sizeof(struct ENCIMAGEITEM)*(images->count);
    images->items=malloc(all_items_size);
    if (images->items==NULL)
    {
        if (verbose) printf(" Error\nReason - cannot allocate %lu bytes of memory.\n",all_items_size);
        return JTYTB_MALLOC_ERR;
    }
    //Looping through images
    unsigned long picnum;
    unsigned long errnum=0;
    unsigned long skipnum=0;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct JTYTAB_ITEM *jtabitem=&(jtabf->items[picnum]);
        struct ENCIMAGEITEM *item=&(images->items[picnum]);
        item->width=jtabitem->width;
        item->height=jtabitem->height;
        item->data=NULL;
        item->datsize=0;
        memcpy(&(item->attrib),&(jtabitem->attrib),sizeof(struct JTYTAB_ATTRIB));
       	if (verbose) printf("\rPreparing picture%6lu from %06lx, %ux%u...",picnum,jtabitem->offset,jtabitem->width,jtabitem->height);
        if (jtabitem->offset > jtyf->filelength)
        {
            if (verbose) printf(" Skipped\nReason - Picture offset out of JTY filesize.\n");
            skipnum++;
            continue;
        }
        // Determining end of the image
        unsigned long endoffs=jtyf->filelength;
        if (picnum+1<images->count)
        {
            unsigned long nendoffs=jtabf->items[picnum+1].offset;
            if ((nendoffs<=endoffs)&&(nendoffs>=jtabitem->offset))
            {
                endoffs=nendoffs;
            } else
            {
                if (verbose)
                  printf(" Truncated\nReason - Entry size exceeds JTY filesize.\n");
            }
        }
        item->datsize=endoffs-jtabitem->offset;
        if (item->datsize>0)
        {
            item->data=malloc(item->datsize+1);
            if (item->data==NULL)
            {
              if (verbose) printf(" Error - cannot allocate %lu bytes of memory.\n",(unsigned long)(item->datsize));
              return JTYTB_MALLOC_ERR;
            }
            memcpy(item->data,jtyf->data+jtabitem->offset,item->datsize);
        }
        (*readcount)+=(item->datsize);
    }
    if (verbose) printf("\rImages stored; %lu skipped, %lu errors.             \n",skipnum,errnum);
    return errnum;
}

/*
 * Writes encoded images into JTY/TAB pair structure.
 */
int write_encimages_jtytab(const struct ENCIMAGELIST *images,unsigned long *writecount,
    struct JTYTABFILE *jtabf,struct DATFILE *jtyf)
{
    short retcode;
    // Preparing TAB structure
    jtabf->filelength=(images->count*JTYTAB_ENTRY_SIZE);
    retcode=alloc_jtytabfile_data(jtabf,images->count);
    if (retcode!=ERR_NONE) return retcode;
    // Preparing JTY structure
    unsigned long datlength=0;
    int entrynum;
    for (entrynum=0;entrynum<images->count;entrynum++)
        datlength+=(images->items[entrynum].datsize);
    retcode=alloc_datfile_data(jtyf,datlength);
    if (retcode!=ERR_NONE) return retcode;
    // Filling the structure
    unsigned long datoffs=0;
    for (entrynum=0;entrynum<images->count;entrynum++)
    {
        struct JTYTAB_ITEM *cjtab=&(jtabf->items[entrynum]);
        struct ENCIMAGEITEM *img=&(images->items[entrynum]);
        memcpy(jtyf->data+datoffs,img->data,img->datsize);
        cjtab->offset=datoffs;
        cjtab->width=img->width;
        cjtab->height=img->height;
        memcpy(&(cjtab->attrib),&(img->attrib),sizeof(struct JTYTAB_ATTRIB));
        datoffs+=img->datsize;
    }
    (*writecount)=datoffs;
    return ERR_NONE;
}

/*
 * Frees memory allocated for encoded images
 */
short free_dattab_encimages(struct ENCIMAGELIST *images)
{
    unsigned long picnum;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct ENCIMAGEITEM *item=&(images->items[picnum]);
        free(item->data);
    }
    free(images->items);
    images->items=NULL;
    images->count=0;
    return ERR_NONE;
}

/*
 * Switches two ENCIMAGELIST entries
 */
short switch_encimage_entries(struct ENCIMAGELIST *images,unsigned long idx1,unsigned long idx2)
{
    if ((idx1>=images->count)||(idx2>=images->count))
        return JTYTB_INTERNAL;
    struct ENCIMAGEITEM item;
    memcpy(&item,&(images->items[idx1]),sizeof(struct ENCIMAGEITEM));
    memcpy(&(images->items[idx1]),&(images->items[idx2]),sizeof(struct ENCIMAGEITEM));
    memcpy(&(images->items[idx2]),&item,sizeof(struct ENCIMAGEITEM));
    return ERR_NONE;
}

int read_jtytab_images(struct IMAGELIST *images,unsigned long *readcount,
    const struct JTYTABFILE *jtabf,const struct DATFILE *jtyf,const int verbose)
{
    images->count=jtabf->count;
    images->items=malloc(sizeof(struct IMAGEITEM)*(images->count));
    if (images->items==NULL)
    {
        if (verbose) printf(" Error - cannot allocate %lu bytes of memory.\n",(unsigned long)(sizeof(struct IMAGEITEM)*images->count));
        return 1;
    }
    //Looping through images
    unsigned long picnum;
    unsigned long errnum=0;
    unsigned long skipnum=0;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct JTYTAB_ITEM *jtabitem=&(jtabf->items[picnum]);
        struct IMAGEITEM *item=&(images->items[picnum]);
        item->width=0;
        item->height=0;
        item->data=NULL;
        item->alpha=NULL;
       	if (verbose) printf("\rPreparing picture%6lu from %06lx, %ux%u...",picnum,jtabitem->offset,jtabitem->width,jtabitem->height);
        if (jtabitem->offset >= jtyf->filelength)
            {
            if (verbose) printf(" Skipped - Picture offset out of JTY filesize.\n");
            skipnum++;
            continue;
            }
        if ((jtabitem->width*jtabitem->height) < 1)
            {
            if (verbose) printf(" Skipped - Picture dimensions are invalid.\n");
            skipnum++;
            continue;
            }
        unsigned long readedsize;
        int retcode;
        retcode=read_dat_image_idx(item,&readedsize,jtyf,jtabitem->offset,jtabitem->width,jtabitem->height);
        *readcount+=readedsize;
        if ((retcode&XTABDAT8_COLOUR_LEAK))
        {  
            if (verbose) printf (" Error - colour leak out of picture size.\n");
            errnum++;
        }
        else if ((retcode&XTABDAT8_ENDOFBUFFER))
        {  
            if (verbose) printf (" Error - end of DAT buffer, picture truncated.\n");
            errnum++;
        }
    }
    if (verbose) printf("\rImages decoded, %lu skipped, %lu errors.\n",skipnum,errnum);
    return errnum;
}

/*
 * Switches two JTYTABFILE entries
 */
short switch_jtytabfile_entries(struct JTYTABFILE *jtabf,unsigned long idx1,unsigned long idx2)
{
    if ((idx1>=jtabf->count)||(idx2>=jtabf->count))
        return JTYTB_INTERNAL;
    struct JTYTAB_ITEM item;
    memcpy(&item,&(jtabf->items[idx1]),sizeof(struct JTYTAB_ITEM));
    memcpy(&(jtabf->items[idx1]),&(jtabf->items[idx2]),sizeof(struct JTYTAB_ITEM));
    memcpy(&(jtabf->items[idx2]),&item,sizeof(struct JTYTAB_ITEM));
    return ERR_NONE;
}
