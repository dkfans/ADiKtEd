/*
 * lev_files.c
 *
 * Defines functions for loading and writing levels from/to disk.
 *
 */

#include "lev_files.h"

#include "globals.h"
#include "internal.h"
#include "obj_utils.h"
#include "bulcommn.h"

short load_subtile(LEVEL *lvl,unsigned char **dest,
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

unsigned char **load_subtile_malloc(LEVEL *lvl,char *fname, int length, 
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


int load_thing(LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int num, i, j;
    unsigned char *ret;
    
    mem = read_file (fname);
    if (mem.len < 2)
      return 0;
    
    num = mem.content[0]+(mem.content[1]<<8);

    // Check everything's cushty
    if (mem.len-2 != num*21)
      return 0;
    for (i=0; i < num; i++)
    {
      ret=(unsigned char *)malloc(SIZEOF_DK_TNG_REC);
      if (!ret)
          return 0;
      memcpy(ret, mem.content+SIZEOF_DK_TNG_REC*i+2, SIZEOF_DK_TNG_REC);
      //Counting hero gates
      if (ret[6] == THING_TYPE_ITEM)
      {
        if (ret[7] == ITEM_SUBTYPE_HEROGATE)
          lvl->stats.hero_gates_count++;
      }
      thing_add(lvl,ret);
    }
    free (mem.content);
    return 1;
}

short load_clm(LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int i, j;
    if (lvl->clm==NULL)
      return false;
    mem = read_file(fname);
    if (mem.len != 49152+8)
      return 0;
    for (i=0; i < 2048; i++)
      memcpy(lvl->clm[i], mem.content+SIZEOF_DK_CLM_REC*i+8, SIZEOF_DK_CLM_REC);
    free (mem.content);
    return 1;
}

/* This *must* be called *after* tng_* are set up */
int load_apt (LEVEL *lvl,char *fname)
{
    struct memory_file mem;
    int i, n, nn;
    unsigned char *ret;
    
    mem = read_file (fname);
    if (mem.len < 2)
      return 0;
    
    if (action_used==NULL)
      return false;
    
    apt_tot=0;
    nn = mem.content[0]+(mem.content[1]<<8)+
      (mem.content[2]<<16)+(mem.content[3]<<24);
    // Check everything's cushty
    if (mem.len-4 != nn*8)
      return 0;
    for (i=0; i < nn; i++)
    {
      ret=(unsigned char *)malloc(SIZEOF_DK_ACT_REC);
      if (!ret)
          return 0;
      memcpy (ret, mem.content+8*i+4, 4);
      // Make sure we know it's an action point
      memset (ret+4, 0xff, 13);
      memcpy (ret+17, mem.content+8*i+8, 4);
      thing_add(lvl,ret);
      n = (ret[20]<<8)+ret[19];
      if (n > 1024)
          return 0;
      action_used[n-1]=ret;
    }
    if (nn != apt_tot)
      die("Internal error in load_apt: nn=%d apt_tot=%d", nn, apt_tot);
    free (mem.content);
    return 1;
}

short write_slb (LEVEL *lvl,char *fname)
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

short write_own(LEVEL *lvl,char *fname)
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

short write_dat(LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    for (i=0; i < 255; i++)
    {
      for (j=0; j < 255; j++)
      {
          fputc (dat_low[j][i], fp);
          fputc (dat_high[j][i], fp);
      }
      fputc (0, fp);
      fputc (0, fp);
    }
    // Fill the rest with junk :)
    for (i=0; i < 512; i++)
      fputc (0, fp);
    fclose (fp);
    return true;
}

short write_clm (LEVEL *lvl,char *fname)
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
    for (i=0; i < 2048; i++)
      fwrite (lvl->clm[i], 24, 1, fp);
    fclose (fp);
    return true;
}

/*
 * Saves WIB file.
 */
short write_wib(LEVEL *lvl,char *fname)
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
short write_apt(LEVEL *lvl,char *fname)
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
    write_long_le_file (fp,apt_tot);
    int i, j, k;
    for (i=0; i<arr_entries_y; i++)
    {
      for (j=0; j<arr_entries_x; j++)
      {
          int num_subs=lvl->tng_subnums[i][j];
          for (k=0; k<num_subs; k++)
          {
            if (is_action_point(i,j,k))
            {
                fwrite (lvl->tng_lookup[i][j][k], 4, 1, fp);
                fwrite (lvl->tng_lookup[i][j][k]+17, 4, 1, fp);
            }
          }
      }
    }
    fclose (fp);
    return true;
}

/*
 * Saves TNG file.
 */
short write_tng(LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }
    fputc ((lvl->tng_total_count-apt_tot) & 255, fp);
    fputc ((lvl->tng_total_count-apt_tot)>>8, fp);
    for (i=0; i < 255; i++)
      for (j=0; j < 255; j++)
          for (k=0; k < lvl->tng_subnums[i][j]; k++)
            if (!is_action_point(i,j,k))
                fwrite (lvl->tng_lookup[i][j][k], 21, 1, fp);
    fclose (fp);
    return true;
}

/*
 * Saves the whole map. Includes all files editable in Adikted.
 */
short save_map(LEVEL *lvl,char *mapname)
{
    if (verify_map(lvl)<1)
      return false;

    //This has been changed to create_default_clm, because user should have
    // choice is updating the "things" structure
    //create_clmdattng();
    create_default_clm();

    draw_graffiti();
    
    char *fname;
    fname = (char *)malloc(strlen(mapname)+5);
    if (fname==NULL)
      die ("save_map: Out of memory");
    short result=true;
    sprintf (fname, "%s.own", mapname);
    result&=write_own(lvl,fname);
    sprintf (fname, "%s.slb", mapname);
    result&=write_slb(lvl,fname);
    sprintf (fname, "%s.dat", mapname);
    result&=write_dat(lvl,fname);
    sprintf (fname, "%s.clm", mapname);
    result&=write_clm(lvl,fname);
    sprintf (fname, "%s.tng", mapname);
    result&=write_tng(lvl,fname);
    sprintf (fname, "%s.apt", mapname);
    result&=write_apt(lvl,fname);
    sprintf (fname, "%s.wib", mapname);
    result&=write_wib(lvl,fname);
    read_graffiti();
    free(fname);
    return result;
}

/*
 * Loads the whole map. Tries to open all files editable in Adikted.
 */
void load_map(LEVEL *lvl,char *fname)
{
  char *fnames;
  int i, j;
  for (i=0; i < 3; i++)
    for (j=0; j < 3; j++)
        vistng[i][j]=0;
    
  level_free(lvl);
  if ((fname==NULL)||(strlen(fname)<1))
  {
    start_new_map(lvl);
    return;
  }
  level_clear(lvl);
  fnames = (char *)malloc(strlen (fname)+5);
  if (fnames==NULL)
    die ("load_map: Out of memory.");
  short result=true;
  if (result)
  {
    sprintf (fnames, "%s.slb", fname);
    result&=load_subtile(lvl,lvl->slb, fnames, 14450, 85, 85, 170, 1, 0, 2, 0);
  }    
  if (result)
  {
    sprintf (fnames, "%s.own", fname);
    result&=load_subtile(lvl,lvl->own, fnames, 65536, 85, 85, 256, 3, 0, 3, 0); 
  }    
  if (result)
  {
    sprintf (fnames, "%s.dat", fname);
    result&=load_subtile(lvl,dat_low, fnames, 131072, 255, 255, 512, 1, 0, 2, 0);
    result&=load_subtile(lvl,dat_high, fnames, 131072, 255, 255, 512, 1, 0, 2, 1);
  }    
  if (result)
  {
    sprintf (fnames, "%s.tng", fname);
    result&=load_thing(lvl, fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.apt", fname);
    result&=load_apt(lvl, fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.clm", fname);
    result&=load_clm (lvl,fnames);
  }    
  if (result)
  {
    sprintf (fnames, "%s.wib", fname);
    result&=load_subtile(lvl,lvl->wib, fnames, 65536, 255, 255, 256, 1, 0, 1, 0);
  }    

  if (!result)
  {
        message_error("Couldn't load \"%s\"", fnames);
        free(fnames);
        free_map();
        start_new_map(lvl);
        return;
  }
    read_graffiti();
    free(fnames);
}

