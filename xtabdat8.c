// converts picture data from bullfrog's DAT/TAB files into BMPs
// works with 8bpp files, needs a .dat and a .tab file


#include <stdio.h>
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

    //Reading DAT,TAB and extracting images
    IMAGELIST images;
    {
        char tabfname[255];
        sprintf (tabfname, "%s.tab", argv[1]);
        char datfname[255];
        sprintf (datfname, "%s.dat", argv[1]);
        int retcode=create_images_dattab_idx(&images,datfname,tabfname,1);
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

int create_images_dattab_idx(struct IMAGELIST *images,char *datfname,char *tabfname,int verbose)
{
    if (verbose) printf("Reading TAB file ...");
    //Opening TAB file
    struct TABFILE tabf;
    {
        int retcode=read_tabfile_data(&tabf,tabfname);
        if (retcode!=0)
          switch (retcode)
          {
          case 1:
             if (verbose) printf("\nError - Cannot open TAB file: %s\n",tabfname);
             return 11;
          default:
             if (verbose) printf("\nError - Loading TAB file %s returned fail code %d\n",tabfname,retcode);
             return 19;
          }
    }
    if (verbose) printf("Done.\n");

    if (verbose) printf("Reading DAT file ...");
    //Opening DAT file
    struct DATFILE datf;
    {
        int retcode=read_datfile_data(&datf,datfname);
        if (retcode!=0)
          switch (retcode)
          {
          case 1:
             if (verbose) printf("\nError - Cannot open DAT file: %s\n",datfname);
             return 21;
          default:
             if (verbose) printf("\nError - Loading DAT file %s returned fail code %d\n",datfname,retcode);
             return 29;
          }
    }
    if (verbose) printf("Done.\n");

    if (verbose)
    {
        printf("\n");
        printf ("The TAB file informs of %lu pictures.\n", tabf.count);
        if (tabf.filelength!=(tabf.count+1)*6)
        {
            printf("Warning - the TAB file contains incomplete entry at end.\n");
            printf(" The truncated entry will be skipped.\n");
        }
        if (datf.count==-1)
        {
            printf ("The DAT file informs of 4bpp content.\n");
            printf ("Warning - this is 8bpp extractor!\n");
        }
        else
            printf ("The DAT file informs of %ld pictures with 8bpp.\n", datf.count);
}
    
    if (verbose) printf("Decoding images ...");
    unsigned long readcount=2;
    read_dattab_images(images,&readcount,&tabf,&datf,verbose);
    if (verbose) printf("Done.\n");

    if (verbose)
    {
        printf("Processed %lu of %lu bytes of DAT file.\n",readcount,datf.filelength);
        long unused=datf.filelength-readcount;
        if (unused>=0)
            printf("Bytes skipped: %ld\n",unused);
          else  
            printf("Bytes overlapping: %ld\n",-unused);
    }
    free_tabfile_data(&tabf);
    free_datfile_data(&datf);
    return 0;
}

int read_tabfile_data(struct TABFILE *tabf,char *srcfname)
{
        FILE *tabfp;
        tabfp = fopen (srcfname, "rb");
        if (!tabfp)
        	return 1;
        tabf->filelength=file_length_opened(tabfp);
        tabf->count=tabf->filelength/6 - 1;
        tabf->items=malloc(tabf->count*sizeof(struct TABFILE_ITEM));
        if (!tabf->items) { fclose(tabfp);return 2; }
        unsigned char tabitm[6];
        //Skipping first entry (should be empty)
        fread (tabitm, 6, 1, tabfp);
        int entrynum;
        for (entrynum=0;entrynum<tabf->count;entrynum++)
        {
            struct TABFILE_ITEM *curitm=&(tabf->items[entrynum]);
            unsigned long readed=fread (tabitm, 1, 6, tabfp);
            curitm->offset=read_long_le_buf(tabitm);
            curitm->width=tabitm[4];
            curitm->height=tabitm[5];
            if (readed < 6) return 3;
        }
        fclose(tabfp);
        return 0;
}

int free_tabfile_data(struct TABFILE *tabf)
{
    free(tabf->items);
    tabf->items=NULL;
    tabf->filelength=0;
    tabf->count=0;
}

int read_datfile_data(struct DATFILE *datf,char *srcfname)
{
    FILE *datfp;
    datfp = fopen (srcfname, "rb");
    if (!datfp) return 1;
    datf->count=read_short_le_file(datfp)-1;
    datf->filelength=file_length_opened(datfp);
    datf->data=malloc(datf->filelength);
    if (!datf->data) { fclose(datfp);return 2; }
    fseek(datfp, 0, SEEK_SET);
    unsigned long readed=fread(datf->data, 1, datf->filelength, datfp);
    fclose(datfp);
    if (readed < datf->filelength) return 3;
    return 0;
}

int free_datfile_data(struct DATFILE *datf)
{
    free(datf->data);
    datf->data=NULL;
    datf->filelength=0;
    datf->count=0;
}

int read_dattab_images(struct IMAGELIST *images,unsigned long *readcount,struct TABFILE *tabf,struct DATFILE *datf,int verbose)
{
    images->count=tabf->count;
    images->items=malloc(sizeof(struct IMAGEITEM)*(images->count));
    if (!images->items)
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
        struct TABFILE_ITEM *tabitem=&(tabf->items[picnum]);
        struct IMAGEITEM *item=&(images->items[picnum]);
        item->width=0;
        item->height=0;
        item->data=NULL;
        item->alpha=NULL;
       	if (verbose) printf("\rPreparing picture%6lu from %06lx, %ux%u...",picnum,tabitem->offset,tabitem->width,tabitem->height);
        if (tabitem->offset >= datf->filelength)
            {
            if (verbose) printf(" Skipped - Picture offset out of DAT filesize.\n");
            skipnum++;
            continue;
            }
        if ((tabitem->width*tabitem->height) < 1)
            {
            if (verbose) printf(" Skipped - Picture dimensions are invalid.\n");
            skipnum++;
            continue;
            }
        unsigned long readedsize;
        int retcode;
        retcode=read_dat_image_idx(item,&readedsize,datf,tabitem->offset,tabitem->width,tabitem->height);
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
}

int read_dat_image_idx(struct IMAGEITEM *image,unsigned long *readedsize,struct DATFILE *datf,unsigned long off,unsigned int width,unsigned int height)
{
    //Filling image structure
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
            // Select color index when transparent
            image->data[i]=0;
            image->alpha[i]=255;
        }
    }
    //Code of error, if any occured
    int errorcode=0;
    //Counter of readed bytes
    long endoff=off;
    //Position in buffer on height
    unsigned int r=0;
    //position in buffer on width
	unsigned int c=0;
    //Time to decode picture
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
        } else //being here means that g>0
        {
            int i;
        	for (i=0; i < g; i++)
    		{
    		    if ((r >= height))
                {
                    //Colour leak on height - time to finish the work
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                    break;
                } else
    		    if ((c > width))
                {
                    //Colour leak on width - going to next line
                    r++;c=0;
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                } else
    		    if ((c >= width))
                {
                    //Do nothing - the error is small
                    errorcode|=XTABDAT8_COLOUR_LEAK;
                } else
    		    {
                    //No leak error
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
		    } //for (i=...
    	}
    }
	*readedsize=endoff-off;
	return errorcode;
}
