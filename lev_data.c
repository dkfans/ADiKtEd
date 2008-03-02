/*
 * lev_data.c
 *
 * Defines functions for maintaining the level memory structure.
 * This includes creating elements, deleting them and clearing whole structure.
 *
 */

#include "lev_data.h"

#include <math.h>
#include "globals.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "obj_column.h"
#include "lev_script.h"

const int idir_subtl_x[]={
    0, 1, 2,
    0, 1, 2,
    0, 1, 2,};

const int idir_subtl_y[]={
    0, 0, 0,
    1, 1, 1,
    2, 2, 2,};

struct LEVEL *lvl=NULL;

/*
 * creates object for storing one level; allocates memory and inits
 * the values to zero; drops any previous pointers without deallocating;
 */
short level_init(struct LEVEL **lvl_ptr)
{
    lvl=(struct LEVEL *)malloc(sizeof(struct LEVEL));
    *lvl_ptr=lvl;
    if (lvl==NULL)
        die("level_init: Out of memory");
    // map file name
    lvl->fname=(char *)malloc(DISKPATH_SIZE);
    memset(lvl->fname,0,DISKPATH_SIZE*sizeof(char));
    lvl->savfname=(char *)malloc(DISKPATH_SIZE);
    memset(lvl->savfname,0,DISKPATH_SIZE*sizeof(char));
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;

  { //allocating CLM structures
    lvl->clm = (unsigned char **)malloc(COLUMN_ENTRIES*sizeof(char *));
    if (lvl->clm==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      lvl->clm[i]=(unsigned char *)malloc(SIZEOF_DK_CLM_REC);
      if (lvl->clm[i]==NULL)
        die("level_init: Out of memory");
    }
    lvl->clm_hdr=(unsigned char *)malloc(SIZEOF_DK_CLM_HEADER);
    lvl->clm_utilize=(unsigned int *)malloc(COLUMN_ENTRIES*sizeof(unsigned int *));
  }
  { //allocating SLB structures
    int i;
    lvl->slb = (unsigned short **)malloc(MAP_SIZE_Y*sizeof(unsigned short *));
    if (lvl->slb==NULL)
        die("level_init: Out of memory");
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      lvl->slb[i] = (unsigned short *)malloc(MAP_SIZE_X*sizeof(unsigned short));
      if (lvl->slb[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating OWN structures
    int i;
    lvl->own = (unsigned char **)malloc(dat_entries_y*sizeof(char *));
    if (lvl->own==NULL)
        die("level_init: Out of memory");
    for (i=0; i < dat_entries_y; i++)
    {
      lvl->own[i] = (unsigned char *)malloc(dat_entries_x*sizeof(char));
      if (lvl->own[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating DAT structures
    lvl->dat= (unsigned short **)malloc(dat_entries_y*sizeof(short *));
    if (lvl->dat==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < dat_entries_y; i++)
    {
      lvl->dat[i]= (unsigned short *)malloc(dat_entries_x*sizeof(unsigned short));
      if (lvl->dat[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating WIB structures
    lvl->wib= (unsigned char **)malloc(dat_entries_y*sizeof(char *));
    if (lvl->wib==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < dat_entries_y; i++)
    {
      lvl->wib[i]= (unsigned char *)malloc(dat_entries_x*sizeof(char));
      if (lvl->wib[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating FLG structures
    lvl->flg= (unsigned short **)malloc(dat_entries_y*sizeof(short *));
    if (lvl->flg==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < dat_entries_y; i++)
    {
      lvl->flg[i]= (unsigned short *)malloc(dat_entries_x*sizeof(unsigned short));
      if (lvl->flg[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  {
    int i;
    lvl->tng_apt_lgt_nums = (unsigned short **)malloc(MAP_SIZE_Y*sizeof(unsigned short *));
    if (lvl->tng_apt_lgt_nums==NULL)
      die("level_init: Out of memory");
    for (i=0; i<MAP_SIZE_Y; i++)
    {
      lvl->tng_apt_lgt_nums[i]= (unsigned short *)malloc(MAP_SIZE_X*sizeof(unsigned short));
      if (lvl->tng_apt_lgt_nums[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //Allocating "things" structure
    lvl->tng_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->tng_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->tng_lookup==NULL) || (lvl->tng_subnums==NULL))
      die("level_init: Out of memory");
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->tng_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->tng_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->tng_lookup[i]==NULL) || (lvl->tng_subnums[i]==NULL))
        die("level_init: Out of memory");
    }
  }
  { //Allocating "action points" structure
    lvl->apt_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->apt_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->apt_lookup==NULL) || (lvl->apt_subnums==NULL))
      die("level_init: Out of memory");
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->apt_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->apt_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->apt_lookup[i]==NULL) || (lvl->apt_subnums[i]==NULL))
        die("level_init: Out of memory");
    }
  }
  { //Allocating "static lights" structure
    lvl->lgt_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->lgt_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->lgt_lookup==NULL) || (lvl->lgt_subnums==NULL))
      die("level_init: Out of memory");
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->lgt_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->lgt_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->lgt_lookup[i]==NULL) || (lvl->lgt_subnums[i]==NULL))
        die("level_init: Out of memory");
    }
  }
  { //Allocating WLB structure
    lvl->wlb = (unsigned char **)malloc(MAP_SIZE_Y*sizeof(unsigned char *));
    if (lvl->wlb==NULL)
      die("level_init: Out of memory");
    int i;
    for (i=0; i<MAP_SIZE_Y; i++)
    {
      lvl->wlb[i]=(unsigned char *)malloc(MAP_SIZE_X*sizeof(unsigned char));
      if (lvl->wlb[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  { //allocating cust.columns structures
    lvl->cust_clm_lookup= (struct DK_CUSTOM_CLM ***)malloc(dat_entries_y*sizeof(struct DK_CUSTOM_CLM **));
    if (lvl->cust_clm_lookup==NULL)
        die("level_init: Out of memory");
    int i;
    for (i=0; i < dat_entries_y; i++)
    {
      lvl->cust_clm_lookup[i]= (struct DK_CUSTOM_CLM **)malloc(dat_entries_x*sizeof(struct DK_CUSTOM_CLM *));
      if (lvl->cust_clm_lookup[i]==NULL)
        die("level_init: Out of memory");
    }
  }
  return level_clear(lvl);
}

/*
 * clears the "things" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_tng(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing single variables
    lvl->tng_total_count=0;
    //Clearing pointer arrays
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->tng_lookup[i][j]=NULL;
          lvl->tng_subnums[i][j]=0;
      }
    for (i=0; i<MAP_SIZE_Y; i++)
      for (j=0; j<MAP_SIZE_X; j++)
          lvl->tng_apt_lgt_nums[i][j]=0;

    //Clearing related stats variables
    lvl->stats.hero_gates_count=0;
  return true;
}

/*
 * clears the "actions" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_apt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing single variables
    lvl->apt_total_count=0;
    //Clearing pointer arrays
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->apt_lookup[i][j]=NULL;
          lvl->apt_subnums[i][j]=0;
      }
  return true;
}

/*
 * clears the "lights" structure for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_lgt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing single variables
    lvl->lgt_total_count=0;
    //Clearing pointer arrays
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->lgt_lookup[i][j]=NULL;
          lvl->lgt_subnums[i][j]=0;
      }
  return true;
}

/*
 * clears the "column" structure for storing level; will not drop any pointers;
 * requies level_init() to be run first;
 */
short level_clear_datclm(struct LEVEL *lvl)
{
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;

    // zero-filling CLM memory
    int i,k;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      lvl->clm_utilize[i]=0;
      if (lvl->clm[i]!=NULL)
        memset(lvl->clm[i],0,SIZEOF_DK_CLM_REC);
    }
    //Clearing CLM header
    memset(lvl->clm_hdr,0,SIZEOF_DK_CLM_HEADER);
    write_long_le_buf(lvl->clm_hdr+0,COLUMN_ENTRIES);
    // Setting all DAT entries to one, first column
    // (it is unused in all maps)
    for (k=0; k<dat_entries_y; k++)
      for (i=0; i<dat_entries_x; i++)
      {
          set_dat_subtile(lvl, i, k, 0);
          lvl->clm_utilize[0]++;
      }

    //Filling CLM entries with unused, zero-filled ones
    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    //Firs one is special - should have nonzero use at start
    clmentry = (unsigned char *)(lvl->clm[0]);
    fill_column_rec_sim(clm_rec,lvl->clm_utilize[0], 0,  0, 0, 0, 0, 0, 0, 0, 0);
    set_clm_entry(clmentry, clm_rec);
    fill_column_rec_sim(clm_rec,0, 0,  0, 0, 0, 0, 0, 0, 0, 0);
    for (i=1; i < COLUMN_ENTRIES; i++)
    {
      clmentry = (unsigned char *)(lvl->clm[i]);
      set_clm_entry(clmentry, clm_rec);
    }
    free_column_rec(clm_rec);
    return true;
}

/*
 * Clears (via zeroing) stats for given level.
 */
short level_clear_stats(struct LEVEL *lvl)
{
    lvl->stats.creatures_count=0;
    lvl->stats.roomeffects_count=0;
    lvl->stats.traps_count=0;
    lvl->stats.doors_count=0;
    lvl->stats.items_count=0;
    //Items stats
    lvl->stats.hero_gates_count=0;
    lvl->stats.dn_hearts_count=0;
    lvl->stats.spellbooks_count=0;
    lvl->stats.dng_specboxes_count=0;
    lvl->stats.crtr_lairs_count=0;
    lvl->stats.statues_count=0;
    lvl->stats.torches_count=0;
    lvl->stats.gold_things_count=0;
    lvl->stats.furniture_count=0;
    //Various stats
    lvl->stats.room_things_count=0;
    //Stats on objects adding/removal
    lvl->stats.things_removed=0;
    lvl->stats.things_added=0;
    return true;
}

/*
 * clears the structures for storing level which do not have separate
 * clearing function; drops any old pointers, only file name remains;
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear_other(struct LEVEL *lvl)
{
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;

    int i;
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->slb[i]!=NULL)
        memset(lvl->slb[i],0,MAP_SIZE_X*sizeof(unsigned short));
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->own[i]!=NULL)
        memset(lvl->own[i],0,dat_entries_x*sizeof(char));
    }

    for (i=0; i < dat_entries_y; i++)
    {
      if (lvl->dat[i]!=NULL)
        memset(lvl->dat[i],0,dat_entries_x*sizeof(unsigned short));
    }

    for (i=0; i < dat_entries_y; i++)
    {
      if (lvl->wib[i]!=NULL)
        memset(lvl->wib[i],COLUMN_WIB_SKEW,dat_entries_x*sizeof(char));
    }

    for (i=0; i < dat_entries_y; i++)
    {
      if (lvl->flg[i]!=NULL)
        memset(lvl->flg[i],0,dat_entries_x*sizeof(unsigned short));
    }

    for (i=0; i < MAP_SIZE_Y; i++)
    {
      if (lvl->wlb[i]!=NULL)
        memset(lvl->wlb[i],0,MAP_SIZE_X*sizeof(char));
    }
    
    // INF file is easy
    lvl->inf=0x00;

    // TXT script file
    lvl->script.list=NULL;
    lvl->script.txt=NULL;
    lvl->script.lines_count=0;

    // The Adikted-custom elements
    if (lvl->cust_clm_lookup!=NULL)
    {
      for (i=0; i < dat_entries_y; i++)
      {
        if (lvl->cust_clm_lookup[i]!=NULL)
          memset(lvl->cust_clm_lookup[i],0,dat_entries_x*sizeof(struct DK_CUSTOM_CLM  *));
      }
    }
    lvl->cust_clm_count=0;
    lvl->graffiti=NULL;
    lvl->graffiti_count=0;
    return true;
}

/*
 * clears the whole object for storing level; drops any old pointers
 * without deallocating them; requies level_init() to be run first;
 */
short level_clear(struct LEVEL *lvl)
{
  short result=true;
  result&=level_clear_lgt(lvl);
  result&=level_clear_apt(lvl);
  result&=level_clear_tng(lvl);
  result&=level_clear_datclm(lvl);
  result&=level_clear_stats(lvl);
  result&=level_clear_other(lvl);
  return result;
}

/*
 * frees structures for storing level; frees only the memory
 * allocated by level_init(); to free the content of loaded level,
 * you must call level_free() first;
 */
short level_deinit()
{
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;

    //Freeing SLB structure
    if (lvl->slb!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
          free(lvl->slb[i]);
      free(lvl->slb);
    }

    //Freeing OWN structure
    if (lvl->own!=NULL)
    {
      int i;
      for (i=0; i<dat_entries_y; i++)
          free(lvl->own[i]);
      free(lvl->own);
    }

    //Freeing DAT structure
    if (lvl->dat!=NULL)
    {
      int i;
      for (i=0; i<dat_entries_y; i++)
          free(lvl->dat[i]);
      free (lvl->dat);
    }

    //Freeing WIB structure
    if (lvl->wib!=NULL)
    {
      int i;
      for (i=0; i<dat_entries_y; i++)
          free(lvl->wib[i]);
      free (lvl->wib);
    }

    //Freeing FLG structure
    if (lvl->flg!=NULL)
    {
      int i;
      for (i=0; i<dat_entries_y; i++)
          free(lvl->flg[i]);
      free (lvl->flg);
    }

    //Freeing "things" structure
    if (lvl->tng_apt_lgt_nums!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
        free(lvl->tng_apt_lgt_nums[i]);
      free(lvl->tng_apt_lgt_nums);
    }
    if (lvl->tng_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_lookup[i]);
      free(lvl->tng_lookup);
    }
    if (lvl->tng_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_subnums[i]);
      free(lvl->tng_subnums);
    }

    //Freeing action points structure
    if (lvl->apt_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_lookup[i]);
      free(lvl->apt_lookup);
    }
    if (lvl->apt_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_subnums[i]);
      free(lvl->apt_subnums);
    }

    //Freeing static lights structure
    if (lvl->lgt_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->lgt_lookup[i]);
      free(lvl->lgt_lookup);
    }
    if (lvl->lgt_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->lgt_subnums[i]);
      free(lvl->lgt_subnums);
    }

    //Freeing column structure
    if (lvl->clm!=NULL)
    {
      int i;
      for (i=0; i<COLUMN_ENTRIES; i++)
          free (lvl->clm[i]);
      free(lvl->clm);
      free(lvl->clm_hdr);
      free(lvl->clm_utilize);
    }

    //Freeing WLB structure
    if (lvl->wlb!=NULL)
    {
      int i;
      for (i=0; i<MAP_SIZE_Y; i++)
          free(lvl->wlb[i]);
      free(lvl->wlb);
    }

    //Freeing cust.columns structure
    if (lvl->cust_clm_lookup!=NULL)
    {
      int i;
      for (i=0; i<dat_entries_y; i++)
          free(lvl->cust_clm_lookup[i]);
      free (lvl->cust_clm_lookup);
    }
    
    //TODO: free graffiti

    free(lvl->fname);
    free(lvl->savfname);

    //Final freeing - main lvl object
    free(lvl);
    lvl=NULL;
    return true;
}

/*
 * frees "things" structure for storing level; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_tng(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Freeing object arrays
    if ((lvl->tng_subnums!=NULL) && (lvl->tng_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->tng_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                thing_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/*
 * frees "action points" structure for storing level; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_apt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Freeing object arrays
    if ((lvl->apt_subnums!=NULL) && (lvl->apt_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->apt_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                actnpt_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/*
 * frees TXT structure for storing level script; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_txt(struct LEVEL *lvl)
{
  int idx=lvl->script.lines_count;
  while (idx>0)
  {
    idx--;
    free(lvl->script.txt[idx]);
    free(lvl->script.list[idx]);
  }
  free(lvl->script.txt);
  free(lvl->script.list);
  lvl->script.lines_count=0;
  return true;
}

/*
 * frees LGT structure for storing level lights; disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared)
 */
short level_free_lgt(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Freeing object arrays
    if ((lvl->lgt_subnums!=NULL) && (lvl->lgt_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->lgt_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                stlight_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/*
 * frees structures for storing level; frees only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared - use level_clear() to set nulls to pointers)
 */
short level_free()
{
  short result=true;
  result&=level_free_lgt(lvl);
  result&=level_free_apt(lvl);
  result&=level_free_tng(lvl);
  result&=level_free_txt(lvl);
  result&=level_free_graffiti(lvl);
  return result;
}

short level_verify(struct LEVEL *lvl, char *actn_name)
{
  char err_msg[LINEMSG_SIZE];
  strcpy(err_msg,"Unknown error");
  short result=VERIF_OK;
  short nres;
  if (result!=VERIF_ERROR)
  {
    nres=level_verify_struct(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=things_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=slabs_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=actnpts_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=columns_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=dat_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=txt_verify(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  if (result!=VERIF_ERROR)
  {
    nres=level_verify_logic(lvl,err_msg);
    if (nres!=VERIF_OK) result=nres;
  }
  switch (result)
  {
    case VERIF_OK:
      message_info("Level verification passed.");
      return VERIF_OK;
    case VERIF_WARN:
      if (actn_name==NULL)
        message_error("Warning: %s",err_msg);
      else
        message_error("Warning: %s (%s performed)",err_msg,actn_name);
      return VERIF_WARN;
    default:
      if (actn_name==NULL)
      message_error("Error: %s",err_msg);
      else
      message_error("Error: %s (%s cancelled)",err_msg,actn_name);
      return VERIF_ERROR;
  }
}

/*
 * Verifies internal LEVEL structure integrity. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short level_verify_struct(struct LEVEL *lvl, char *err_msg)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Checking base pointers
    if (lvl->tng_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object tng_subnums!",LINEMSG_SIZE);
          return VERIF_ERROR;
    }
    if (lvl->apt_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object apt_subnums!",LINEMSG_SIZE);
          return VERIF_ERROR;
    }
    if (lvl->lgt_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object lgt_subnums!",LINEMSG_SIZE);
          return VERIF_ERROR;
    }
    //Sweeping through structures
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          if (thing==NULL)
          {
              sprintf(err_msg,"Null thing pointer at slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
        }

        int actpt_count=lvl->apt_subnums[i][j];
        for (k=0; k <actpt_count ; k++)
        {
          unsigned char *actnpt = lvl->apt_lookup[i][j][k];
          if (actnpt==NULL)
          {
              sprintf(err_msg,"Null action point pointer at slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
        }
        
        int stlight_count=lvl->lgt_subnums[i][j];
        for (k=0; k <stlight_count ; k++)
        {
          unsigned char *stlight = lvl->lgt_lookup[i][j][k];
          if (stlight==NULL)
          {
              sprintf(err_msg,"Null static light pointer at slab %d,%d.",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
        }
      }
    }
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      if (lvl->clm[i]==NULL)
      {
        sprintf(err_msg,"Null CoLuMn entry at index %d.",i);
        return VERIF_ERROR;
      }
    }
    if ((lvl->clm_hdr==NULL)||(lvl->clm_utilize==NULL))
    {
      sprintf(err_msg,"Null CoLuMn help arrays.");
      return VERIF_ERROR;
    }
    if (lvl->inf>7)
    {
          strncpy(err_msg,"Unexpected value of INF entry.",LINEMSG_SIZE);
          return VERIF_WARN;
    }
  return VERIF_OK;
}

/*
 * Verifies action points parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short actnpts_verify(struct LEVEL *lvl, char *err_msg)
{
  return VERIF_OK;
}

/*
 * Verifies various logic aspects of a map. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short level_verify_logic(struct LEVEL *lvl, char *err_msg)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          unsigned char type_idx=get_thing_type(thing);
//          if ((type_idx==THING_TYPE_ITEM))
          {
            int pos_h=(unsigned short)get_thing_subtile_h(thing);
            int subt_x=(unsigned short)get_thing_subtpos_x(thing);
            int subt_y=(unsigned short)get_thing_subtpos_y(thing);
            unsigned short slab=get_tile_slab(lvl,i/MAP_SUBNUM_X,j/MAP_SUBNUM_Y);
            int col_h=get_subtile_column_height(lvl,i,j);
            //Checking if we're close to sibling column - if we are, its heigh may be
            // importand too. (disabled after verifying if official editor)
/*            if ((subt_x<=64)||(subt_x>=192)||
                (subt_y<=64)||(subt_y>=192))
            {
              int modx=0;
              int mody=0;
              if (subt_x<=64) modx=1;
              if (subt_x>=192) modx=-1;
              if (subt_y<=64) mody=1;
              if (subt_y>=192) mody=-1;
              int mod_h=get_subtile_column_height(lvl,i+modx,j+mody);
              if (mod_h<col_h)
                col_h=mod_h;
            }*/
            if ((pos_h<col_h)&&(!slab_is_door(slab))||(pos_h<min(col_h,1)))
            {
              sprintf(err_msg,"Thing trapped in solid column on slab %d,%d (h=%d<%d).",i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y,pos_h,col_h);
              return VERIF_WARN;
            }
          }
        }
        
      }
    }

    //Array for storing players heart count
    int hearts[PLAYERS_COUNT];
    for (i=0; i < PLAYERS_COUNT; i++)
      hearts[i]=0;
    owned_things_count(hearts,lvl,THING_TYPE_ITEM,ITEM_SUBTYPE_DNHEART);

    if (hearts[PLAYER_UNSET]>0)
    {
        sprintf(err_msg,"Found %d unowned dungeon heart things.",hearts[PLAYER_UNSET]);
        return VERIF_WARN;
    }
    for (i=0; i < PLAYERS_COUNT; i++)
    {
      if (hearts[i]>1)
      {
        sprintf(err_msg,"Player %d owns %d dungeon heart things.",i, hearts[i]);
        return VERIF_WARN;
      }
    }
    if (hearts[0]==0)
    {
        sprintf(err_msg,"Human player doesn't have a dungeon heart thing.");
        return VERIF_WARN;
    }
  return VERIF_OK;
}

/*
 * Fills SLB/OWN structure with "default" background,
 * unowned "def_slab" surrounded by rock.
 * DAT/CLM values are not updated here.
 */
void generate_slab_bkgnd_default(struct LEVEL *lvl,unsigned short def_slab)
{
    // Filling the map with def_slab
    int i,j;
    for (i=1; i < MAP_MAXINDEX_Y; i++)
      for (j=1; j < MAP_MAXINDEX_X; j++)
      {
          set_tile_slab(lvl,i,j,def_slab);
          set_tile_owner(lvl,i,j,PLAYER_UNSET); //=5, simply ;)
      }
    // Impenetrable rock
    for (i=0; i < MAP_SIZE_X; i++)
    {
      set_tile_slab(lvl,i,0,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,i,MAP_MAXINDEX_Y,SLAB_TYPE_ROCK);
      set_tile_owner(lvl,i,0,PLAYER_UNSET);
      set_tile_owner(lvl,i,MAP_MAXINDEX_Y,PLAYER_UNSET);
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      set_tile_slab(lvl,0,i,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,MAP_MAXINDEX_X,i,SLAB_TYPE_ROCK);
      set_tile_owner(lvl,0,i,PLAYER_UNSET);
      set_tile_owner(lvl,MAP_MAXINDEX_Y,i,PLAYER_UNSET);
    }
    //Resetting the last column
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    int sx, sy;
    sx=dat_entries_x-1;
    for (sy=0; sy<dat_entries_y; sy++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
    sy=dat_entries_y-1;
    for (sx=0; sx<dat_entries_x; sx++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
}

/*
 * Fills SLB/OWN structure with "random" background,
 * map made of earth with random rock at borders.
 * DAT/CLM values are not updated here.
 */
void generate_slab_bkgnd_random(struct LEVEL *lvl)
{
// TODO: check if there are not closed regions of earth, link them with
// earth corridors
//HOW:
//1. Create array MAP_SIZE_X x MAP_SIZE_Y of unsigned short, maybe call it "slab_group"?
//2. Find an earth slab, start at map center, check at larger radius (rectangle) until found
//3. Mark the found slab as group 1
//4. From the marked slab, extend the radius and mark slabs which have
//   a neightbour alreadey marked; repeat until map edge reached
// Note: use only 4 base directions for checking neightbour slabs
//5. Sweep through whole map and mark slabs which have
//   a neightbour alreadey marked; count the slabs marked in every sweep
//6. Finish sweeping when count=0
//7. Sweep through whole map searching for unmarked earth slab. If found,
//   mark it with last_group+1 (=2 at first sweep)
//8. Repeat 5. and 6.
//9. Repeat 7 and 8 until no unmarked earth slab found
//10. Create array of unsigned int with size=last_group+1. Fill it with MAP_SIZE_X's
//11. Sweep through all slabs, and for every which has index>1 find the shortest way
//    to slab from group with smaller index (use one of four basic directions only).
//    Write it in array.
//12. Sweep throufg all slabs again and fill the shortest ways to slabs with dirt/water/lava
//13. Free the two allocaded arrays
    int i,j,k,l;
    // Filling the map with SLAB_TYPE_EARTH
    for (i=1; i < MAP_MAXINDEX_Y; i++)
      for (j=1; j < MAP_MAXINDEX_X; j++)
          set_tile_slab(lvl,i,j,SLAB_TYPE_EARTH); // Digable dirt
    // Impenetrable rock
    for (i=0; i < MAP_SIZE_X; i++)
    {
      set_tile_slab(lvl,i,0,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,i,MAP_MAXINDEX_Y,SLAB_TYPE_ROCK);
    }
    for (i=0; i < MAP_SIZE_Y; i++)
    {
      set_tile_slab(lvl,0,i,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,MAP_MAXINDEX_X,i,SLAB_TYPE_ROCK);
    }

    //Resetting the last column
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    int sx, sy;
    sx=dat_entries_x-1;
    for (sy=0; sy<dat_entries_y; sy++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
    sy=dat_entries_y-1;
    for (sx=0; sx<dat_entries_x; sx++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);

    // Impenetrable rock at borders
    for (i=1; i < (MAP_MAXINDEX_Y>>1); i++)
      for (j=0; j < MAP_MAXINDEX_X-1; j++)
      {
        int ir=MAP_MAXINDEX_Y-i;
        int rnd_bound=RAND_MAX/((i/2)+1);
        if (rand()<rnd_bound)
        {
          set_tile_slab(lvl,j,i,SLAB_TYPE_ROCK);
        }
        if (rand()<rnd_bound)
        {
          set_tile_slab(lvl,j,MAP_MAXINDEX_X-i,SLAB_TYPE_ROCK);
        }
        if (rand()<rnd_bound)
        {
          set_tile_slab(lvl,i,j,SLAB_TYPE_ROCK);
        }
        if (rand()<rnd_bound)
        {
          set_tile_slab(lvl,MAP_MAXINDEX_X-i,j,SLAB_TYPE_ROCK);
        }
      }
    int num_smears=(rand()%20);
    if (num_smears<10)
    {
      while (num_smears>4) num_smears=num_smears>>1;
      for (l=0;l<num_smears;l++)
      {
        int val=rand();
        int smr_startx=val%MAP_SIZE_X;
        int smr_starty=(val>>8)%MAP_SIZE_Y;
        val=rand();
        int smr_endx=(val)%MAP_SIZE_X;
        int smr_endy=(val>>8)%MAP_SIZE_Y;
        val=rand();
        int startr=(val)%4+2;
        int endr=(val>>8)%3+1;
        val=rand();
        int distance=ceil(sqrt((smr_startx-smr_endx)*(smr_startx-smr_endx)+(smr_starty-smr_endy)*(smr_starty-smr_endy)));
        int bend=((val)%(distance+1))-(distance>>1);
        slab_draw_smear(lvl,smr_startx,smr_starty,startr,smr_endx,smr_endy,endr,bend,SLAB_TYPE_ROCK);
      }
    }
    //Deleting small rocks and enlarging big rocks
    const int slabgrow_bound_val=4;
    int cx,cy;
    for (i=1; i < (MAP_SIZE_X>>1); i++)
      for (j=1; j < (MAP_SIZE_Y>>1); j++)
        for (k=0; k < 4; k++)
        {
            if ((k%2)==0) cx=i; else cx=MAP_MAXINDEX_X-i;
            if ((k>>1)==0) cy=j; else cy=MAP_MAXINDEX_Y-j;
            int nsibln=slab_siblings_oftype(lvl,cx,cy,SLAB_TYPE_ROCK);
            if (nsibln>slabgrow_bound_val)
            {
              set_tile_slab(lvl,cx,cy,SLAB_TYPE_ROCK);
            } else
            {
              set_tile_slab(lvl,cx,cy,SLAB_TYPE_EARTH);
            }
        }
    for (i=1; i < MAP_MAXINDEX_X-1; i++)
      for (j=1; j < MAP_MAXINDEX_Y-1; j++)
      {
          int nsibln=slab_siblings_oftype(lvl,i,j,SLAB_TYPE_ROCK);
          if (nsibln<2)
          {
              set_tile_slab(lvl,i,j,SLAB_TYPE_EARTH);
          }
          if (nsibln>7)
          {
              set_tile_slab(lvl,i,j,SLAB_TYPE_ROCK);
          }
      }
    //Everything generated here should be unclaimed
    for (i=0; i < MAP_SIZE_X; i++)
      for (j=0; j < MAP_SIZE_Y; j++)
          set_tile_owner(lvl, i, j, PLAYER_UNSET);
}

/*
 * creates new level;  requies the memory to be allocated by level_init();
 * calls level_clear(), but not level_free() at start;
 */
void start_new_map(struct LEVEL *lvl)
{
    level_clear(lvl);
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    generate_slab_bkgnd_default(lvl,SLAB_TYPE_EARTH);

    // CLM should be empty, add permanent entries
    add_permanent_columns(lvl);
    //And update all DAT/CLM values; it also updates the WIB values.
    if (datclm_auto_update)
        update_datclm_for_whole_map(lvl);

    lvl->inf=0x00;

    update_level_stats(lvl);
}

void generate_random_map(struct LEVEL *lvl)
{
    level_clear(lvl);
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    generate_slab_bkgnd_random(lvl);

    // CLM should be empty, add permanent entries
    add_permanent_columns(lvl);
    //And update all DAT/CLM values; it also updates the WIB values.
    if (datclm_auto_update)
        update_datclm_for_whole_map(lvl);
    lvl->inf=rnd(8);
    update_level_stats(lvl);
}

void free_map(void)
{
    level_free();
    level_clear(lvl);
}

char *get_thing(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *thing=NULL;
    if (num < lvl->tng_subnums[x][y])
      thing = lvl->tng_lookup[x][y][num];
    return thing;
}

/*
 * Adds a thing to the structure. Returns its index in structure.
 * Also updates statistics.
 */
int thing_add(struct LEVEL *lvl,unsigned char *thing)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_thing_subtile_x(thing)%arr_entries_x;
    y = get_thing_subtile_y(thing)%arr_entries_y;
    lvl->tng_total_count++;
    //setting TNG entries
    lvl->tng_subnums[x][y]++;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->tng_lookup[x][y]==NULL)
      lvl->tng_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y],
                          lvl->tng_subnums[x][y]*sizeof(char *));
    if (lvl->tng_lookup[x][y]==NULL)
      die("thing_add: Out of memory");
    int new_idx=lvl->tng_subnums[x][y]-1;
    lvl->tng_lookup[x][y][new_idx]=thing;
    update_thing_stats(lvl,thing,1);
    return new_idx;
}

/*
 * Removes given thing from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the thing.
 */
void thing_del(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    if ((x>=arr_entries_x)||(y>=arr_entries_y)) return;
    unsigned char *thing;
    thing = lvl->tng_lookup[x][y][num];
    thing_drop(lvl,x,y,num);
    free (thing);
}

/*
 * Removes given thing from the LEVEL structure, updates counter variables.
 * Does not frees memory allocated for the thing - just drops the pointers.
 * Updates thing statistics.
 */
void thing_drop(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    if ((x>=arr_entries_x)||(y>=arr_entries_y)) return;
    int i;
    if (num >= lvl->tng_subnums[x][y])
      return;
    lvl->tng_total_count--;
    unsigned char *thing;
    thing = lvl->tng_lookup[x][y][num];
    update_thing_stats(lvl,thing,-1);
    for (i=num; i < lvl->tng_subnums[x][y]-1; i++)
      lvl->tng_lookup[x][y][i]=lvl->tng_lookup[x][y][i+1];
    lvl->tng_subnums[x][y]--;
    lvl->tng_apt_lgt_nums[x/3][y/3]--;
    lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y], 
                        lvl->tng_subnums[x][y]*sizeof(char *));
}

unsigned int get_thing_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->tng_subnums[x][y];
}

char *get_actnpt(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *actnpt=NULL;
    if (num < lvl->apt_subnums[x][y])
      actnpt = lvl->apt_lookup[x][y][num];
    return actnpt;
}

/*
 * Adds a given action point to the LEVEL structure, updates counter variables
 */
void actnpt_add(struct LEVEL *lvl,unsigned char *actnpt)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_actnpt_subtile_x(actnpt)%arr_entries_x;
    y = get_actnpt_subtile_y(actnpt)%arr_entries_y;
    lvl->apt_total_count++;
    //setting APT entries
    unsigned int apt_snum=lvl->apt_subnums[x][y];
    apt_snum++;
    lvl->apt_subnums[x][y]=apt_snum;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->apt_lookup[x][y]==NULL)
      lvl->apt_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->apt_lookup[x][y]=(unsigned char **)realloc(lvl->apt_lookup[x][y],
                          apt_snum*sizeof(char *));
    if (lvl->apt_lookup[x][y]==NULL)
      die("actnpt_add: Cannot allocate memory.");
    unsigned int new_idx=apt_snum-1;
    lvl->apt_lookup[x][y][new_idx]=actnpt;
}

/*
 * Removes given action point from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the action point.
 */
void actnpt_del(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    x%=arr_entries_x;
    y%=arr_entries_y;
    unsigned int apt_snum=lvl->apt_subnums[x][y];
    if (num >= apt_snum)
      return;
    lvl->apt_total_count--;
    unsigned char *actnpt;
    actnpt = lvl->apt_lookup[x][y][num];
    free(actnpt);
    int i;
    apt_snum--;
    for (i=num; i < apt_snum; i++)
      lvl->apt_lookup[x][y][i]=lvl->apt_lookup[x][y][i+1];
    lvl->apt_subnums[x][y]=apt_snum;
    lvl->tng_apt_lgt_nums[x/MAP_SUBNUM_X][y/MAP_SUBNUM_Y]--;
    lvl->apt_lookup[x][y]=(unsigned char **)realloc(lvl->apt_lookup[x][y], 
                        apt_snum*sizeof(char *));
}

unsigned int get_actnpt_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->apt_subnums[x][y];
}

char *get_stlight(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    unsigned char *stlight=NULL;
    if (num < lvl->lgt_subnums[x][y])
      stlight = lvl->lgt_lookup[x][y][num];
    return stlight;
}

/*
 * Adds a given static light to the LEVEL structure, updates counter variables
 */
void stlight_add(struct LEVEL *lvl,unsigned char *stlight)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_stlight_subtile_x(stlight)%arr_entries_x;
    y = get_stlight_subtile_y(stlight)%arr_entries_y;
    lvl->lgt_total_count++;
    //setting LGT entries
    unsigned int lgt_snum=lvl->lgt_subnums[x][y];
    lgt_snum++;
    lvl->lgt_subnums[x][y]=lgt_snum;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->lgt_lookup[x][y]==NULL)
      lvl->lgt_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->lgt_lookup[x][y]=(unsigned char **)realloc(lvl->lgt_lookup[x][y],
                          lgt_snum*sizeof(char *));
    if (lvl->lgt_lookup[x][y]==NULL)
      die("stlight_add: Cannot allocate memory.");
    unsigned int new_idx=lgt_snum-1;
    lvl->lgt_lookup[x][y][new_idx]=stlight;
}

/*
 * Removes given static light from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the static light.
 */
void stlight_del(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    x%=arr_entries_x;
    y%=arr_entries_y;
    unsigned int lgt_snum=lvl->lgt_subnums[x][y];
    if (num >= lgt_snum)
      return;
    lvl->lgt_total_count--;
    unsigned char *stlight;
    stlight = lvl->lgt_lookup[x][y][num];
    free(lvl->lgt_lookup[x][y][num]);
    int i;
    lgt_snum--;
    for (i=num; i < lgt_snum; i++)
      lvl->lgt_lookup[x][y][i]=lvl->lgt_lookup[x][y][i+1];
    lvl->lgt_subnums[x][y]=lgt_snum;
    lvl->tng_apt_lgt_nums[x/MAP_SUBNUM_X][y/MAP_SUBNUM_Y]--;
    lvl->lgt_lookup[x][y]=(unsigned char **)realloc(lvl->lgt_lookup[x][y], 
                        lgt_snum*sizeof(char *));
}

unsigned int get_stlight_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    x %= arr_entries_x;
    y %= arr_entries_y;
    return lvl->lgt_subnums[x][y];
}

/*
 * Checks what type the object is. Objects are action points, things or lights.
 * Returns one of OBJECT_TYPE_* value
 */
short get_object_type(struct LEVEL *lvl, unsigned int x, unsigned int y, unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    if ((x>=arr_entries_x)||(y>=arr_entries_y))
      return OBJECT_TYPE_NONE;
    int tng_num=lvl->tng_subnums[x][y];
    if (z<tng_num) return OBJECT_TYPE_THING;
    int apt_num=lvl->apt_subnums[x][y];
    if (z<(tng_num+apt_num)) return OBJECT_TYPE_ACTNPT;
    int lgt_num=lvl->lgt_subnums[x][y];
    if (z<(tng_num+apt_num+lgt_num)) return OBJECT_TYPE_STLIGHT;
    return OBJECT_TYPE_NONE;
}

unsigned char *get_object(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    if ((x>=arr_entries_x)||(y>=arr_entries_y))
      return NULL;
    int tng_num=lvl->tng_subnums[x][y];
    if (z<tng_num)
      return get_thing(lvl,x,y,z);
    int apt_num=lvl->apt_subnums[x][y];
    if (z<(tng_num+apt_num))
      return get_actnpt(lvl,x,y,z-tng_num);
    int lgt_num=lvl->lgt_subnums[x][y];
    if (z<(tng_num+apt_num+lgt_num))
      return get_stlight(lvl,x,y,z-tng_num-apt_num);
    return NULL;
}

/*
 * Deletes object with given coordinates and index.
 */
void object_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding indices
    sx%=arr_entries_x;
    sy%=arr_entries_y;
    int tng_num=lvl->tng_subnums[sx][sy];
    if (z<tng_num)
    {
      thing_del(lvl,sx,sy,z);
      return;
    }
    int apt_num=lvl->apt_subnums[sx][sy];
    if (z<(tng_num+apt_num))
    {
      actnpt_del(lvl,sx,sy,z-tng_num);
      return;
    }
    int lgt_num=lvl->lgt_subnums[sx][sy];
    if (z<(tng_num+apt_num+lgt_num))
    {
      stlight_del(lvl,sx,sy,z-tng_num-apt_num);
      return;
    }
}

unsigned int get_object_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    if ((x>=arr_entries_x)||(y>=arr_entries_y))
      return 0;
    return lvl->tng_subnums[x][y]+lvl->apt_subnums[x][y]+lvl->lgt_subnums[x][y];
}

unsigned int get_object_tilnums(struct LEVEL *lvl,unsigned int x,unsigned int y)
{
    if (lvl->tng_apt_lgt_nums==NULL) return 0;
    return lvl->tng_apt_lgt_nums[x%MAP_SIZE_X][y%MAP_SIZE_Y];
}

/*
 * Returns index of the last object with given type. May return -1 if no object found.
 */
int get_object_subtl_last(struct LEVEL *lvl,unsigned int x,unsigned int y,short obj_type)
{
    //Preparing array bounds
    unsigned int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    unsigned int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Bounding position
    if ((x>=arr_entries_x)||(y>=arr_entries_y))
      return 0;
    int last=-1;
    last+=lvl->tng_subnums[x][y];
    if (obj_type==OBJECT_TYPE_THING)
      return last;
    last+=lvl->apt_subnums[x][y];
    if (obj_type==OBJECT_TYPE_ACTNPT)
      return last;
    last+=lvl->lgt_subnums[x][y];
    if (obj_type==OBJECT_TYPE_STLIGHT)
      return last;
    return get_object_subnums(lvl,x,y)-1;
}

short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    const unsigned int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const unsigned int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    //Bounding position
    sx %= dat_entries_x;
    sy %= dat_entries_y;
    return lvl->wib[sx][sy];
}

void set_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy, short nval)
{
    //Preparing array bounds
    const unsigned int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const unsigned int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    //Bounding position
    if ((sx>=dat_entries_x)||(sy>=dat_entries_y))
        return;
    lvl->wib[sx][sy]=nval;
}

short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    //Bounding position
    tx %= MAP_SIZE_X;
    ty %= MAP_SIZE_Y;
    return lvl->wlb[tx][ty];
}

void set_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty, short nval)
{
    //Bounding position
    if ((tx>=MAP_SIZE_X)||(ty>=MAP_SIZE_Y)) return;
    lvl->wlb[tx][ty]=nval;
}

unsigned char get_subtl_owner(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    const unsigned int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const unsigned int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    //Bounding position
    sx %= dat_entries_x;
    sy %= dat_entries_y;
    return lvl->own[sx][sy];
}

void set_subtl_owner(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char nval)
{
    //Preparing array bounds
    const unsigned int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const unsigned int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    //Bounding position
    if ((sx>=dat_entries_x)||(sy>=dat_entries_y)) return;
    lvl->own[sx][sy]=nval;
}

unsigned char get_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    return get_subtl_owner(lvl,tx*MAP_SUBNUM_X+1,ty*MAP_SUBNUM_Y+1);
}

void set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned char nval)
{
    int subtl_x,subtl_y;
    for (subtl_y=0;subtl_y<MAP_SUBNUM_Y;subtl_y++)
      for (subtl_x=0;subtl_x<MAP_SUBNUM_X;subtl_x++)
        set_subtl_owner(lvl,tx*MAP_SUBNUM_X+subtl_x,ty*MAP_SUBNUM_Y+subtl_y,nval);
}

unsigned short get_tile_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    if (lvl->slb==NULL) return SLAB_TYPE_ROCK;
    //Bounding position
    if ((tx>=MAP_SIZE_X)||(ty>=MAP_SIZE_Y)) return SLAB_TYPE_ROCK;
    return lvl->slb[tx][ty];
}

/*
 * Sets a new value to SLB item, without updating DAT or CLM or anything.
 */
void set_tile_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval)
{
    //Bounding position
    if ((tx>=MAP_SIZE_X)||(ty>=MAP_SIZE_Y)) return;
    lvl->slb[tx][ty]=nval;
}

/*
 * Returns raw DAT value value for one subtile.
 * Warning: the USE variable of every column is unaffected by this function.
 */
unsigned int get_dat_val(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    if (lvl->dat==NULL) return 0;
    if ((sx>=dat_entries_x)||(sy>=dat_entries_y)) return 0;
    return lvl->dat[sx][sy];
}

/*
 * Sets a DAT value for one subtile. (cx,cy) is a subtile position to fill,
 * d is the raw value. Low level - sets the RAW value, not column index.
 */
void set_dat_val(struct LEVEL *lvl, int sx, int sy, unsigned int d)
{
    //Preparing array bounds
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    if (lvl->dat==NULL) return;
    if ((sx<0)||(sy<0)||(sx>=dat_entries_x)||(sy>=dat_entries_y)) return;
    lvl->dat[sx][sy]=d;
}

/*
 * Returns FLG value for one subtile.
 */
unsigned short get_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    //Preparing array bounds
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    if (lvl->flg==NULL) return 0;
    if ((sx>=dat_entries_x)||(sy>=dat_entries_y)) return 0;
    return lvl->flg[sx][sy];
}

/*
 * Sets a FLG value for one subtile. (cx,cy) is a subtile position to fill,
 * nval is the value.
 */
void set_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned short nval)
{
    //Preparing array bounds
    const int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    const int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    if (lvl->flg==NULL) return;
    if ((sx<0)||(sy<0)||(sx>=dat_entries_x)||(sy>=dat_entries_y)) return;
    lvl->flg[sx][sy]=nval;
}

/*
 * Updates some statistics about the level; The update includes
 * "utilize" values of columns
 */
void update_level_stats(struct LEVEL *lvl)
{
     update_clm_utilize_counters(lvl);
     update_things_stats(lvl);
}

void update_things_stats(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Clearing previous stats
    level_clear_stats(lvl);
    //Sweeping through structures
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          update_thing_stats(lvl,thing,1);
        }
      }
    }
}

void update_thing_stats(struct LEVEL *lvl,unsigned char *thing,short change)
{
          if (thing==NULL) return;
          unsigned char type_idx=get_thing_type(thing);
          switch (type_idx)
          {
          case THING_TYPE_CREATURE:
              lvl->stats.creatures_count+=change;
              break;
          case THING_TYPE_ROOMEFFECT:
              lvl->stats.roomeffects_count+=change;
              break;
          case THING_TYPE_TRAP:
              lvl->stats.traps_count+=change;
              break;
          case THING_TYPE_DOOR:
              lvl->stats.doors_count+=change;
              break;
          case THING_TYPE_ITEM:
              lvl->stats.items_count+=change;
              break;
          }
          if (is_herogate(thing))
              lvl->stats.hero_gates_count+=change;
          if (is_dnheart(thing))
              lvl->stats.dn_hearts_count+=change;
          if (is_spellbook(thing))
              lvl->stats.spellbooks_count+=change;
          if (is_dngspecbox(thing))
              lvl->stats.dng_specboxes_count+=change;
          if (is_crtrlair(thing))
              lvl->stats.crtr_lairs_count+=change;
          if (is_statue(thing))
              lvl->stats.statues_count+=change;
          if (is_torch(thing))
              lvl->stats.torches_count+=change;
          if (is_gold(thing))
              lvl->stats.gold_things_count+=change;
          if (is_room_thing(thing))
              lvl->stats.room_things_count+=change;
          if (is_furniture(thing))
              lvl->stats.furniture_count+=change;
          if (change>0)
              lvl->stats.things_added+=change;
          else
              lvl->stats.things_removed-=change;
}
