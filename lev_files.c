/*
 * lev_files.c
 *
 * Defines functions for loading and writing levels from/to disk.
 *
 */

#include "lev_files.h"

#include "globals.h"
#include "internal.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "bulcommn.h"

short load_subtile(struct LEVEL *lvl,unsigned char **dest,
                        char *fname, int length, int x, int y,
                        int linesize, int nlines, int lineoffset,
                        int mbytes, int byteoffset)
{
    struct memory_file mem;
    int i, j;
    long addr=0;

    mem = read_file(fname);
    if (mem.len==-1) // Couldn't open file
      return false;
    if ((mem.len<mbytes*nlines*x*y) || (mem.len!=length))
      return false;
    for (i=0; i < y; i++)
    {
      addr += linesize*lineoffset;
      for (j=0; j < x; j++)
          dest[j][i]=mem.content[addr+j*mbytes+byteoffset];
      addr += (nlines-lineoffset)*linesize;
    }
    free(mem.content);
    return true;
}

unsigned char **load_subtile_malloc(struct LEVEL *lvl,char *fname, int length, 
                        int x, int y,
                        int linesize, int nlines, int lineoffset,
                        int mbytes, int byteoffset)
{
    // Allocating mem
    unsigned char **dest;
    dest = (unsigned char **)malloc(x*sizeof(char *));
    if (dest==NULL)
      die ("load_subtile: Out of memory.\n");
    int i;
    for (i=0; i < x; i++)
    {
      dest[i]=(unsigned char *)malloc(y*sizeof(char));
      if (dest[i]==NULL)
          die ("load_subtile: Out of memory");
    }
    // Loading
    short result=load_subtile(lvl,dest,fname,length,x,y,
        linesize,nlines,lineoffset,mbytes,byteoffset);
    if (result)
      return dest;
    // Freeing on error
    for (i=0; i < x; i++)
      free(dest[i]);
    free(dest);
    return NULL;
}


int load_tng(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int tng_num;
    int i, j;
    unsigned char *thing;
    
    mem = read_file (fname);
    if (mem.len < 2)
      return false;
    
    tng_num = read_short_le_buf(mem.content);

    // Check everything's cushty
    if (mem.len != tng_num*SIZEOF_DK_TNG_REC+2)
      return false;

    for (i=0; i < tng_num; i++)
    {
      unsigned char *thing = create_thing_empty();
      int offs=SIZEOF_DK_TNG_REC*i+2;
      memcpy(thing, mem.content+offs, SIZEOF_DK_TNG_REC);
      //Counting hero gates
      if (get_thing_type(thing) == THING_TYPE_ITEM)
      {
        if (get_thing_subtype(thing) == ITEM_SUBTYPE_HEROGATE)
          lvl->stats.hero_gates_count++;
      }
      thing_add(lvl,thing);
    }
    free (mem.content);
    return true;
}

short load_clm(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int i, j;
    if (lvl->clm==NULL)
      return false;
    mem = read_file(fname);
    if (mem.len != 49152+8)
      return 0;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      int offs=SIZEOF_DK_CLM_REC*i+8;
      memcpy(lvl->clm[i], mem.content+offs, SIZEOF_DK_CLM_REC);
    }
    free (mem.content);
    return 1;
}

/* This *must* be called *after* tng_* are set up */
int load_apt (struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int i, n, nn;
    unsigned char *ret;
    
    mem = read_file (fname);
    if (mem.len < 2)
      return false;
    
    if (lvl->apt_lookup==NULL)
      return false;
    
    lvl->apt_total_count=0;
    nn = mem.content[0]+(mem.content[1]<<8)+
      (mem.content[2]<<16)+(mem.content[3]<<24);
    // Check everything's cushty
    if (mem.len != nn*8+4)
      return 0;
    for (i=0; i < nn; i++)
    {
      ret=(unsigned char *)malloc(SIZEOF_DK_APT_REC);
      if (!ret)
          return false;
      memcpy (ret, mem.content+8*i+4, 4);
      // Make sure we know it's an action point
      memset (ret+4, 0xff, 13);
      memcpy (ret+17, mem.content+8*i+8, 4);
      actnpt_add(lvl,ret);
    }
    if (nn != lvl->apt_total_count)
      die("Internal error in load_apt: nn=%d apt_tot=%d", nn, lvl->apt_total_count);
    free (mem.content);
    return true;
}

short load_inf(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    mem = read_file(fname);
    //If wrong filesize - pannic
    if (mem.len != 1)
      return false;
    lvl->inf=mem.content[0];
    free (mem.content);
    return true;
}

short load_txt(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    mem = read_file(fname);
    //If filesize too small - pannic
    if (mem.len < 4)
      return false;
    unsigned char *content=mem.content;
    unsigned char *ptr=mem.content;
    unsigned char *ptr_end=mem.content+mem.len;
    int lines_count=0;
    while (ptr>=content)
    {
      ptr=memchr(ptr, 0x0a, (char *)ptr_end-(char *)ptr );
      lines_count++;
      if (ptr!=NULL) ptr++;
    }
    lvl->txt=(unsigned char **)malloc(lines_count*sizeof(unsigned char **));
    ptr=mem.content;
    int currline=0;
    while (currline<lines_count)
    {
      if (ptr>=ptr_end) ptr=ptr_end-1;
      unsigned char *nptr=memchr(ptr, 0x0a, ptr_end-ptr );
      while ((ptr<nptr)&&((unsigned char)ptr[0]<0x20)&&((unsigned char)ptr[0]!=0x09)) ptr++;
      if (nptr==NULL)
        nptr=ptr_end;
      int linelen=(char *)nptr-(char *)ptr;

      while ((linelen>0)&&((unsigned char)ptr[linelen-1]<=0x20)) linelen--;
      lvl->txt[currline]=(unsigned char *)malloc((linelen+1)*sizeof(unsigned char *));
      memcpy(lvl->txt[currline],ptr,linelen);
      lvl->txt[currline][linelen]='\0';
      ptr=nptr+1;
      currline++;
    }
    lvl->txt_lines_count=lines_count;
    free (mem.content);
    return true;
}

short load_lgt(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    mem = read_file(fname);
    //If wrong filesize - pannic
    if (mem.len < 4)
      return false;
    int itmcount;
    itmcount=read_long_le_buf(mem.content);
    lvl->lgt=malloc(itmcount*sizeof(DK_LGT_REC *));
    int i;
    for (i=0;i<itmcount;i++)
    {
        int mempos=4+i*SIZEOF_DK_LGT_REC;
        lvl->lgt[i]=(DK_LGT_REC *)malloc(SIZEOF_DK_LGT_REC);
        if (mem.len>=mempos+SIZEOF_DK_LGT_REC)
          memcpy(lvl->lgt[i],mem.content+mempos,SIZEOF_DK_LGT_REC);
        else
          return false;
    }
    lvl->lgt_enties_count=itmcount;
    free (mem.content);
    return true;
}

short load_wlb(struct LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    mem = read_file(fname);
    //If wrong filesize - pannic
    if (mem.len != MAP_SIZE_X*MAP_SIZE_Y)
      return false;
    int i,j;
    for (i=0;i<MAP_SIZE_Y;i++)
      for (j=0;j<MAP_SIZE_X;j++)
      {
        int mempos=i*MAP_SIZE_X+j;
        lvl->wlb[i][j]=mem.content[mempos];
      }
    free (mem.content);
    return true;
}

short write_slb (struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      for (j=0; j < MAP_SIZE_X; j++)
      {
          fputc (lvl->slb[j][i], fp);
          fputc (0, fp);
      }
    }
    fclose (fp);
    return true;
}

short write_own(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j, k, l;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      // Write each line 3 times
      for (k=0; k < 3; k++)
      {
          for (j=0; j < MAP_SIZE_X; j++)
          {
            // Write each character 3 times
            for (l=0; l < 3; l++)
                fputc (lvl->own[j][i], fp);
          }
          fputc (0, fp);
      }
    }
    // Fill the rest with junk :)
    for (i=0; i < 256; i++)
      fputc (0, fp);
    fclose (fp);
    return true;
}

short write_dat(struct LEVEL *lvl,char *fname)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
          fputc (lvl->dat_low[j][i], fp);
          fputc (lvl->dat_high[j][i], fp);
      }
      fputc (0, fp);
      fputc (0, fp);
    }
    // Fill the rest with zeros
    for (i=0; i < 512; i++)
      fputc (0, fp);
    fclose (fp);
    return true;
}

short write_clm (struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    fputc (0, fp);
    fputc (8, fp);
    for (i=0; i < 6; i++)
      fputc (0, fp);
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      fwrite (lvl->clm[i], 24, 1, fp);
    }
    fclose (fp);
    return true;
}

/*
 * Saves WIB file.
 */
short write_wib(struct LEVEL *lvl,char *fname)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    FILE *fp;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    int i, j;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
          fputc (lvl->wib[j][i], fp);
      fputc (0, fp);
    }
    // Fill the rest with zeros
    for (i=0; i < 256; i++)
      fputc (0, fp);
    fclose (fp);
    return true;
}

/*
 * Saves APT file.
 */
short write_apt(struct LEVEL *lvl,char *fname)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    FILE *fp;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    write_long_le_file (fp,lvl->apt_total_count);
    int cy, cx, k;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=lvl->apt_subnums[cx][cy];
          for (k=0; k<num_subs; k++)
          {
                fwrite (lvl->apt_lookup[cx][cy][k], 4, 1, fp);
                fwrite (lvl->apt_lookup[cx][cy][k]+17, 4, 1, fp);
          }
      }
    }
    fclose (fp);
    return true;
}

/*
 * Saves TNG file.
 */
short write_tng(struct LEVEL *lvl,char *fname)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    FILE *fp;
    int cx, cy, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    write_short_le_file(fp, lvl->tng_total_count);
    for (cy=0; cy < arr_entries_y; cy++)
      for (cx=0; cx < arr_entries_x; cx++)
          for (k=0; k < get_thing_subnums(lvl,cx,cy); k++)
                fwrite (get_thing(lvl,cx,cy,k), SIZEOF_DK_TNG_REC, 1, fp);
    fclose (fp);
    return true;
}

/*
 * Saves INF file. One byte file - the easy one.
 */
short write_inf(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    fputc ((lvl->inf) & 255, fp);
    fclose (fp);
    return true;
}

/*
 * Saves TXT file.
 */
short write_txt(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    for (i=0;i<lvl->txt_lines_count;i++)
    {
      fputs(lvl->txt[i],fp);
      fputs("\n",fp);
    }
    fclose(fp);
    return true;
}

/*
 * Saves LGT file.
 */
short write_lgt(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    write_long_le_file(fp, lvl->lgt_enties_count);
    for (i=0;i<lvl->lgt_enties_count;i++)
    {
        fwrite(lvl->lgt[i], SIZEOF_DK_LGT_REC, 1, fp);
    }
    fclose(fp);
    return true;
}

/*
 * Saves WLB file.
 */
short write_wlb(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    int i, j;
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      for (j=0; j < MAP_SIZE_X; j++)
          fputc(lvl->wlb[i][j], fp);
    }
    fclose(fp);
    return true;
}

/*
 * Saves the whole map. Includes all files editable in Adikted.
 */
short save_map(struct LEVEL *lvl)
{
    if (verify_map(lvl)<1)
      return false;

    //This has been changed to create_default_clm, because user should have
    // choice is updating the "things" structure
    //create_clmdattng();
    create_clmdat();

    draw_graffiti();

//!!!!!!!!!!!WARN: hack for generating test maps
/*
int i,j;
  int entridx=1;
  for (i=0; i<MAP_SIZE_Y; i++)
    for (j=0; j<MAP_SIZE_X; j++)
    {
      int num=i*MAP_MAXINDEX_X+j;
      if ((i%2==1)&&(j%20!=0)&&(entridx<170))
        {
          if (j%20>9)
            set_dat_unif (j, i, entridx);
          else
            set_dat (j, i, 0, 0, 0, 0, entridx, 0, 0, 0, 0);
          int clmidx=entridx+276;
          int btmidx=(clmidx==0x0199)?0x0028:clmidx;
          set_clm_entry (entridx, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 
                         btmidx, btmidx,
                         clmidx, clmidx,
                         clmidx, clmidx, 
                         clmidx, clmidx );
        if (j%20==19)
            entridx++;
        }
        else
        {
        set_dat_unif (j, i, 0);
        }
    }
*/
    
    char *fnames;
    fnames = (char *)malloc(strlen(lvl->fname)+5);
    if (fnames==NULL)
      die ("save_map: Out of memory");
    short result=true;
    sprintf (fnames, "%s.own", lvl->fname);
    result&=write_own(lvl,fnames);
    sprintf (fnames, "%s.slb", lvl->fname);
    result&=write_slb(lvl,fnames);
    sprintf (fnames, "%s.dat", lvl->fname);
    result&=write_dat(lvl,fnames);
    sprintf (fnames, "%s.clm", lvl->fname);
    result&=write_clm(lvl,fnames);
    sprintf (fnames, "%s.tng", lvl->fname);
    result&=write_tng(lvl,fnames);
    sprintf (fnames, "%s.apt", lvl->fname);
    result&=write_apt(lvl,fnames);
    sprintf (fnames, "%s.wib", lvl->fname);
    result&=write_wib(lvl,fnames);
    sprintf (fnames, "%s.inf", lvl->fname);
    result&=write_inf(lvl,fnames);
    sprintf (fnames, "%s.txt", lvl->fname);
    result&=write_txt(lvl,fnames);
    sprintf (fnames, "%s.lgt", lvl->fname);
    result&=write_lgt(lvl,fnames);
    sprintf (fnames, "%s.wlb", lvl->fname);
    result&=write_wlb(lvl,fnames);
    read_graffiti();
    free(fnames);
    return result;
}

/*
 * Loads the whole map. Tries to open all files editable in Adikted.
 */
short load_map(struct LEVEL *lvl)
{
  char *fnames;
  int i, j;
  for (i=0; i < 3; i++)
    for (j=0; j < 3; j++)
        vistng[i][j]=0;
    
  level_free(lvl);
  if ((lvl->fname==NULL)||(strlen(lvl->fname)<1))
  {
    start_new_map(lvl);
    return false;
  }
  level_clear(lvl);
  fnames = (char *)malloc(strlen(lvl->fname)+5);
  if (fnames==NULL)
    die ("load_map: Out of memory.");
  short result=true;
  if (result)
  {
    sprintf (fnames, "%s.slb", lvl->fname);
    result&=load_subtile(lvl,lvl->slb, fnames, 14450, 85, 85, 170, 1, 0, 2, 0);
  }    
  if (result)
  {
    sprintf (fnames, "%s.own", lvl->fname);
    result&=load_subtile(lvl,lvl->own, fnames, 65536, 85, 85, 256, 3, 0, 3, 0); 
  }    
  if (result)
  {
    sprintf (fnames, "%s.dat", lvl->fname);
    result&=load_subtile(lvl,lvl->dat_low, fnames, 131072, 255, 255, 512, 1, 0, 2, 0);
    result&=load_subtile(lvl,lvl->dat_high, fnames, 131072, 255, 255, 512, 1, 0, 2, 1);
  }    
  if (result)
  {
    sprintf (fnames, "%s.tng", lvl->fname);
    result&=load_tng(lvl, fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.apt", lvl->fname);
    result&=load_apt(lvl, fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.clm", lvl->fname);
    result&=load_clm(lvl,fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.wib", lvl->fname);
    result&=load_subtile(lvl,lvl->wib, fnames, 65536, 255, 255, 256, 1, 0, 1, 0);
  }    
  if (result)
  {
    sprintf (fnames, "%s.inf", lvl->fname);
    result&=load_inf(lvl,fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.txt", lvl->fname);
    result&=load_txt(lvl,fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.lgt", lvl->fname);
    result&=load_lgt(lvl,fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.wlb", lvl->fname);
    result&=load_wlb(lvl,fnames);
  }    

  if (!result)
  {
        message_error("Couldn't load \"%s\"", fnames);
        free(fnames);
        free_map();
        start_new_map(lvl);
        return false;
  }
    read_graffiti();
    free(fnames);
    return true;
}

