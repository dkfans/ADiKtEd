/******************************************************************************/
/** @file xtabdat8.c
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Converts picture data from bullfrog's DAT/TAB files into BMPs.
 *     Works with 8bpp files, needs a .dat and a .tab file.
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

#if (defined(MAIN_XTABDAT8)||defined(MAIN_XTABJTY))
 #include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "bulcommn.h"
#include "xtabdat8.h"


#ifdef MAIN_XTABDAT8

int main (int argc, char **argv)
{
    printf("\nBullfrog Engine DAT/TAB Pair Extractor for 8bpp graphic");
    printf("\n-------------------------------\n");
    if (argc<3)
    {
        printf("Not enought parameters.\n");
        printf("Usage:\n");
        printf("  %s <datafile> <palfile>\n","xtabdat8");
        printf("Output: pic????.bmp\n");
    	return 1;
    }

    printf("Loading palette ...");
    /*Reading palette file */
    char *palette=malloc(768);
    {
        char fname[255];
        sprintf (fname, "%s.pal", argv[2]);
        int retcode=read_palette_rgb(palette,fname,256);
        switch (retcode)
        {
        case ERR_NONE:
             break;
        case 1:
             printf("\nError - Cannot open PAL file: %s\n",fname);
             return XTABDAT8_CANT_OPEN;
        case 2:
             printf("\nError - Cannot read %d colors from PAL file: %s\n",256,fname);
             return XTABDAT8_CANT_READ;
        default:
             printf("\nError - Loading palette file %s returned fail code %d\n",fname,retcode);
             return XTABDAT8_INTERNAL;
        }
    }
    printf("Done.\n");

    /*Reading DAT,TAB and extracting images */
    IMAGELIST images;
    {
        char tabfname[255];
        sprintf (tabfname, "%s.tab", argv[1]);
        char datfname[255];
        sprintf (datfname, "%s.dat", argv[1]);
        int retcode=create_images_dattab_idx(&images,datfname,tabfname,1);
        if (retcode!=0) return retcode;
    }

    /*Looping through images and extracting to files */
    printf("Extracting images into bitmaps...");
    unsigned long picnum;
    for (picnum=0;picnum<images.count;picnum++)
    {
        /*printf ("\rExtracting: picture number %*d", 4, picnum); */
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

int create_images_dattab_idx(struct IMAGELIST *images,const char *datfname,const char *tabfname,const int verbose)
{
    if (verbose) msgprintf("Reading TAB file ...");
    /*Opening TAB file */
    struct TABFILE tabf;
    {
        int retcode=read_tabfile_data(&tabf,tabfname);
        switch (retcode)
        {
        case ERR_NONE:
             break;
        case XTABDAT8_CANT_OPEN:
             if (verbose) msgprintf("\nError - Cannot open TAB file: %s\n",tabfname);
             return XTABDAT8_CANT_OPEN;
        default:
             if (verbose) msgprintf("\nError - Loading TAB file %s returned fail code %d\n",tabfname,retcode);
             return retcode;
        }
    }
    if (verbose) msgprintf("Done.\n");

    if (verbose) msgprintf("Reading DAT file ...");
    /*Opening DAT file */
    struct DATFILE datf;
    {
        int retcode=read_datfile_data(&datf,datfname);
        switch (retcode)
        {
        case ERR_NONE:
             break;
        case XTABDAT8_CANT_OPEN:
             if (verbose) msgprintf("\nError - Cannot open DAT file: %s\n",datfname);
             return XTABDAT8_CANT_OPEN;
        default:
             if (verbose) msgprintf("\nError - Loading DAT file %s returned fail code %d\n",datfname,retcode);
             return retcode;
        }
    }
    if (verbose) msgprintf("Done.\n");

    if (verbose)
    {
        msgprintf("\n");
        msgprintf ("The TAB file informs of %lu pictures.\n", tabf.count);
        if (tabf.filelength!=(tabf.count+1)*6)
        {
            msgprintf("Warning - the TAB file contains incomplete entry at end.\n");
            msgprintf(" The truncated entry will be skipped.\n");
        }
        if (datf.count==-1)
        {
            msgprintf ("The DAT file informs of 4bpp content.\n");
            msgprintf ("Warning - this is 8bpp extractor!\n");
        } else
        {
            msgprintf ("The DAT file informs of %ld pictures with 8bpp.\n", datf.count);
        }
}
    
    if (verbose) msgprintf("Decoding images ...");
    unsigned long readcount=2;
    read_dattab_images(images,&readcount,&tabf,&datf,verbose);
    if (verbose) msgprintf("Done.\n");

    if (verbose)
    {
        msgprintf("Processed %lu of %lu bytes of DAT file.\n",readcount,datf.filelength);
        long unused=datf.filelength-readcount;
        if (unused>=0)
            msgprintf("Bytes skipped: %ld\n",unused);
          else  
            msgprintf("Bytes overlapping: %ld\n",-unused);
    }
    free_tabfile_data(&tabf);
    free_datfile_data(&datf);
    return 0;
}

int read_tabfile_data(struct TABFILE *tabf,const char *srcfname)
{
        FILE *tabfp;
        tabfp = fopen (srcfname, "rb");
        if (!tabfp)
            return XTABDAT8_CANT_OPEN;
        tabf->filelength=file_length_opened(tabfp);
        if (tabf->filelength<TABFILE_HEADER_SIZE)
            return XTABDAT8_CANT_READ;
        tabf->count=((tabf->filelength-TABFILE_HEADER_SIZE)/TABFILE_ENTRY_SIZE);
        tabf->items=malloc(tabf->count*sizeof(struct TABFILE_ITEM));
        if (!tabf->items) { fclose(tabfp);return XTABDAT8_CANT_READ; }
        unsigned char tabitm[TABFILE_ENTRY_SIZE];
        /*Skipping first entry (should be empty) */
        fread (tabitm, TABFILE_HEADER_SIZE, 1, tabfp);
        int entrynum;
        for (entrynum=0;entrynum<tabf->count;entrynum++)
        {
            struct TABFILE_ITEM *curitm=&(tabf->items[entrynum]);
            unsigned long readed=fread (tabitm, 1, TABFILE_ENTRY_SIZE, tabfp);
            curitm->offset=read_int32_le_buf(tabitm);
            curitm->width=tabitm[4];
            curitm->height=tabitm[5];
            if (readed < TABFILE_ENTRY_SIZE) return XTABDAT8_CANT_READ;
        }
        fclose(tabfp);
        return ERR_NONE;
}

short write_tabfile_data(const struct TABFILE *tabf,const char *dstfname)
{
    FILE *tabfp;
    tabfp = fopen (dstfname, "wb");
    if (tabfp==NULL)
        return XTABDAT8_CANT_OPEN;
    int entrynum;
    for (entrynum=0;entrynum<TABFILE_HEADER_SIZE;entrynum++)
        fputc(0, tabfp);
    for (entrynum=0;entrynum<tabf->count;entrynum++)
    {
            struct TABFILE_ITEM *curitm=&(tabf->items[entrynum]);
            write_int32_le_file(tabfp,curitm->offset);
            fputc(curitm->width,tabfp);
            fputc(curitm->height,tabfp);
    }
    fclose(tabfp);
    return ERR_NONE;
}

short free_tabfile_data(struct TABFILE *tabf)
{
    free(tabf->items);
    tabf->items=NULL;
    tabf->filelength=0;
    tabf->count=0;
    return ERR_NONE;
}

short alloc_datfile_data(struct DATFILE *datf,unsigned long filelength)
{
    datf->filelength=filelength;
    datf->data=malloc(filelength);
    if (datf->data==NULL)
    {
        datf->filelength=0;
        return XTABDAT8_MALLOC_ERR;
    }
    return ERR_NONE;
}

/*
 * Reads Bullfrog's DAT file from DAT/TAB pair. Note that "count" is
 * not always valid.
 */
short read_datfile_data(struct DATFILE *datf,const char *srcfname)
{
    FILE *datfp;
    datfp = fopen (srcfname, "rb");
    if (datfp==NULL) return XTABDAT8_CANT_OPEN;
    datf->count=read_int16_le_file(datfp)-1;
    datf->filelength=file_length_opened(datfp);
    datf->data=malloc(datf->filelength);
    if (datf->data==NULL) { fclose(datfp);return XTABDAT8_MALLOC_ERR; }
    fseek(datfp, 0, SEEK_SET);
    unsigned long readed=fread(datf->data, 1, datf->filelength, datfp);
    fclose(datfp);
    if (readed < datf->filelength) return XTABDAT8_CANT_READ;
    return ERR_NONE;
}

short write_datfile_data(const struct DATFILE *datf,const char *dstfname)
{
    FILE *datfp;
    datfp = fopen(dstfname, "wb");
    if (datfp==NULL)
        return XTABDAT8_CANT_OPEN;
    fwrite (datf->data, datf->filelength, 1, datfp);
    fclose(datfp);
    return ERR_NONE;
}

short free_datfile_data(struct DATFILE *datf)
{
    free(datf->data);
    datf->data=NULL;
    datf->filelength=0;
    datf->count=0;
    return ERR_NONE;
}

int read_dattab_images(struct IMAGELIST *images,unsigned long *readcount,struct TABFILE *tabf,struct DATFILE *datf,const int verbose)
{
    images->count=tabf->count;
    images->items=malloc(sizeof(struct IMAGEITEM)*(images->count));
    if (!images->items)
    {
        if (verbose) msgprintf(" Error - cannot allocate %lu bytes of memory.\n",(unsigned long)(sizeof(struct IMAGEITEM)*images->count));
        return 1;
    }
    /*Looping through images */
    unsigned long picnum;
    unsigned long errnum=0;
    unsigned long skipnum=0;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct TABFILE_ITEM *tabitem=&(tabf->items[picnum]);
        struct IMAGEITEM *item=&(images->items[picnum]);
        item->width=0;
        item->height=0;
        item->data=NULL;
        item->alpha=NULL;
       	if (verbose) msgprintf("\rPreparing picture%6lu from %06lx, %ux%u...",picnum,tabitem->offset,tabitem->width,tabitem->height);
        if (tabitem->offset >= datf->filelength)
            {
            if (verbose) msgprintf(" Skipped - Picture offset out of DAT filesize.\n");
            skipnum++;
            continue;
            }
        if ((tabitem->width*tabitem->height) < 1)
            {
            if (verbose) msgprintf(" Skipped - Picture dimensions are invalid.\n");
            skipnum++;
            continue;
            }
        unsigned long readedsize;
        int retcode;
        retcode=read_dat_image_idx(item,&readedsize,datf,tabitem->offset,tabitem->width,tabitem->height);
        *readcount+=readedsize;
        if ((retcode&XTABDAT8_COLOUR_LEAK))
        {  
            if (verbose) msgprintf (" Error - colour leak out of picture size.\n");
            errnum++;
        }
        else if ((retcode&XTABDAT8_ENDOFBUFFER))
        {  
            if (verbose) msgprintf (" Error - end of DAT buffer, picture truncated.\n");
            errnum++;
        }
    }
    if (verbose) msgprintf("\rImages decoded, %lu skipped, %lu errors.\n",skipnum,errnum);
    return errnum;
}

int free_dattab_images(struct IMAGELIST *images)
{
    unsigned long picnum;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct IMAGEITEM *item=&(images->items[picnum]);
        free(item->data);
        free(item->alpha);
    }
    free(images->items);
    images->items=NULL;
    images->count=0;
    return true;
}

int read_dat_image_idx(struct IMAGEITEM *image,unsigned long *readedsize,
    const struct DATFILE *datf,const unsigned long off,
    const unsigned int width,const unsigned int height)
{
    /*Filling image structure */
    {
        image->width=width;
        image->height=height;
        unsigned long imgsize=width*height;
        image->data=malloc(imgsize);
        image->alpha=malloc(imgsize);
        if ((image->data==NULL)||(image->alpha==NULL))
            return XTABDAT8_NOMEMORY;
        unsigned long i;
        for (i=0;i<imgsize;i++)
        {
            /* Select color index when transparent */
            image->data[i]=0;
            image->alpha[i]=255;
        }
    }
    /*Code of error, if any occured */
    int errorcode=0;
    /*Counter of readed bytes */
    long endoff=off;
    /*Position in buffer on height */
    unsigned int r=0;
    /*position in buffer on width */
	unsigned int c=0;
    /*Time to decode picture */
    while (r < height)
    {
        char g;
        if (endoff < datf->filelength)
            g = (char) (datf->data[endoff]);
        else
            {g = 0;errorcode|=XTABDAT8_ENDOFBUFFER;}
	    endoff++;
	    if (g < 0)
	    {
           	c-=g;
        } else
        if (!g)
       	{
            c=0;
           	r++;
        } else /*being here means that g>0 */
        {
            int i;
        	for (i=0; i < g; i++)
    		{
    		    if ((r >= height))
                {
                    /*Colour leak on height - time to finish the work */
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                    break;
                } else
    		    if ((c > width))
                {
                    /*Colour leak on width - going to next line */
                    r++;c=0;
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                } else
    		    if ((c >= width))
                {
                    /*Do nothing - the error is small */
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                } else
    		    {
                    /*No leak error */
                    if (endoff < datf->filelength)
                    {
                        image->data[(width*r)+c]=datf->data[endoff];
                        image->alpha[(width*r)+c]=0;
                    }
                    else
                    {
                        errorcode|=XTABDAT8_ENDOFBUFFER;
                    }
                    endoff++;
                    c++;
                }
		    } /*for (i=... */
    	}
    }
	*readedsize=endoff-off;
	return errorcode;
}

int read_dattab_encimages(struct ENCIMAGELIST *images,unsigned long *readcount,
    const struct TABFILE *tabf,const struct DATFILE *datf,const int verbose)
{
    images->count=tabf->count;
    unsigned long all_items_size=sizeof(struct ENCIMAGEITEM)*(images->count);
    images->items=malloc(all_items_size);
    if (images->items==NULL)
    {
        if (verbose) msgprintf(" Error\ncannot allocate %lu bytes of memory.\n",all_items_size);
        return XTABDAT8_NOMEMORY;
    }
    if (verbose) msgprintf("Dividing DAT/TAB into list of images...");
    /*Looping through images */
    unsigned long picnum;
    unsigned long errnum=0;
    unsigned long skipnum=0;
    for (picnum=0;picnum<images->count;picnum++)
    {
        struct TABFILE_ITEM *tabitem=&(tabf->items[picnum]);
        struct ENCIMAGEITEM *item=&(images->items[picnum]);
        item->width=tabitem->width;
        item->height=tabitem->height;
        item->data=NULL;
        item->datsize=0;
        memset(&(item->attrib),0,sizeof(struct IMGTAB_ATTRIB));
       	if (verbose) msgprintf("\rPreparing picture%6lu from %06lx, %ux%u...",picnum,tabitem->offset,tabitem->width,tabitem->height);
        if (tabitem->offset > datf->filelength)
        {
            if (verbose) msgprintf(" Skipped - offset out of DAT filesize.\n");
            skipnum++;
            continue;
        }
        /* Determining end of the image */
        unsigned long endoffs=datf->filelength;
        if (picnum+1<images->count)
        {
            unsigned long nendoffs=tabf->items[picnum+1].offset;
            if ((nendoffs<=endoffs)&&(nendoffs>=tabitem->offset))
            {
                endoffs=nendoffs;
            } else
            {
                if (verbose)
                  msgprintf(" Truncated\nReason - Entry size exceeds DAT filesize.\n");
            }
        }
        item->datsize=endoffs-tabitem->offset;
        if (item->datsize>0)
        {
            item->data=malloc(item->datsize+1);
            if (item->data==NULL)
            {
              if (verbose) msgprintf(" Error\ncannot allocate %lu bytes of memory.\n",(unsigned long)(item->datsize));
              return XTABDAT8_NOMEMORY;
            }
            memcpy(item->data,datf->data+tabitem->offset,item->datsize);
        }
        (*readcount)+=(item->datsize);
    }
    if (verbose) msgprintf("\rImages prepared; %lu skipped, %lu errors.             \n",skipnum,errnum);
    return errnum;
}

int write_encimages_dattab(const struct ENCIMAGELIST *images,unsigned long *writecount,
    struct TABFILE *tabf,struct DATFILE *datf)
{
    short retcode;
    /* Preparing TAB structure */
    tabf->filelength=(images->count*TABFILE_ENTRY_SIZE);
    retcode=alloc_jtytabfile_data(tabf,images->count);
    if (retcode!=ERR_NONE) return retcode;
    /* Preparing DAT structure */
    unsigned long datlength=0;
    int entrynum;
    for (entrynum=0;entrynum<images->count;entrynum++)
        datlength+=(images->items[entrynum].datsize);
    retcode=alloc_datfile_data(datf,datlength);
    if (retcode!=ERR_NONE) return retcode;
    /* Filling the structure */
    unsigned long datoffs=0;
    for (entrynum=0;entrynum<images->count;entrynum++)
    {
        struct TABFILE_ITEM *ctab=&(tabf->items[entrynum]);
        struct ENCIMAGEITEM *img=&(images->items[entrynum]);
        memcpy(datf->data+datoffs,img->data,img->datsize);
        ctab->offset=datoffs;
        ctab->width=img->width;
        ctab->height=img->height;
        datoffs+=img->datsize;
    }
    (*writecount)=datoffs;
    return ERR_NONE;
}

/*
 * Creates encoded list of images from DAT/TAB files
 */
short create_images_dattab_enc(struct ENCIMAGELIST *images,const char *datfname,
    const char *tabfname,const int verbose)
{
    if (verbose) msgprintf("Reading TAB file ...");
    /*Opening TAB file */
    struct TABFILE tabf;
    {
        int retcode=read_tabfile_data(&tabf,tabfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case XTABDAT8_CANT_OPEN:
            if (verbose) msgprintf(" Error\nCannot open TAB file \"%s\"\n",tabfname);
            return XTABDAT8_CANT_OPEN;
        default:
            if (verbose) msgprintf(" Error\nLoading TAB file \"%s\" returned fail code %d\n",tabfname,retcode);
            return retcode;
        }
    }
    if (verbose) msgprintf(" Done.\n");

    if (verbose) msgprintf("Reading DAT file ...");
    /*Opening DAT file */
    struct DATFILE datf;
    {
        int retcode=read_datfile_data(&datf,datfname);
        switch (retcode)
        {
        case ERR_NONE:
            break;
        case XTABDAT8_CANT_OPEN:
             if (verbose) msgprintf(" Error\nCannot open DAT file \"%s\"\n",datfname);
             return XTABDAT8_CANT_OPEN;
        default:
             if (verbose) msgprintf(" Error\nLoading DAT file \"%s\" returned fail code %d\n",datfname,retcode);
             return retcode;
        }
    }
    if (verbose) msgprintf(" Done.\n");

    if (verbose)
    {
        msgprintf ("The TAB file informs of %lu pictures.\n", tabf.count);
        if (tabf.filelength != (tabf.count*TABFILE_ENTRY_SIZE+TABFILE_HEADER_SIZE))
        {
            msgprintf("\n");
            msgprintf("Warning - the TAB file contains incomplete entry at end.\n");
            msgprintf(" The truncated entry will be skipped.\n");
            msgprintf("\n");
        }
        if (datf.count==-1)
        {
            msgprintf("\n");
            msgprintf ("The DAT file informs of 4bpp content.\n");
            msgprintf ("Warning - this is 8bpp extractor!\n");
            msgprintf("\n");
        }
    }
    unsigned long readcount=0;
    read_dattab_encimages(images,&readcount,&tabf,&datf,verbose);

    if (verbose)
    {
        msgprintf("Processed %lu of %lu bytes of DAT file.\n",readcount,datf.filelength);
        long unused=datf.filelength-readcount;
        if (unused>0)
        {
            msgprintf("Bytes skipped: %ld\n",unused);
        } else  
        if (unused<0)
        {
            msgprintf("Bytes overlapping: %ld\n",-unused);
        }
    }

    free_tabfile_data(&tabf);
    free_datfile_data(&datf);
    return ERR_NONE;
}

/*
 * Writes encoded list of images into DAT/TAB files
 */
short write_dattab_images_enc(const struct ENCIMAGELIST *images,const char *datfname,
    const char *tabfname,const int verbose)
{
    struct TABFILE tabf;
    struct DATFILE datf;
    unsigned long writecount=0;
    write_encimages_dattab(images,&writecount,&tabf,&datf);
    if (verbose) msgprintf("Writing TAB file ...");
    write_tabfile_data(&tabf,tabfname);
    if (verbose) msgprintf(" Done.\n");
    if (verbose) msgprintf("Writing DAT file ...");
    write_datfile_data(&datf,datfname);
    if (verbose) msgprintf(" Done.\n");
    free_tabfile_data(&tabf);
    free_datfile_data(&datf);
    return ERR_NONE;
}
