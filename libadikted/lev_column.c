/******************************************************************************/
// lev_column.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  21 Jan 2008

// Purpose:
//   Functions for maintaining CLM and DAT files.

// Comment:
//   The DAT/CLM generation algorithm was completely rewritten from the
//   Jon Skeet's version.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "lev_column.h"

#include "obj_column_def.h"
#include "obj_column.h"
#include "lev_data.h"
#include "globals.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "graffiti.h"

char const INF_STANDARD_LTEXT[]="Standard";
char const INF_ANCIENT_LTEXT[]="Ancient";
char const INF_WINTER_LTEXT[]="Winter";
char const INF_SNAKE_LTEXT[]="Snake key";
char const INF_FACE_LTEXT[]="Stone face";
char const INF_BRESTS_LTEXT[]="Big breasts";
char const INF_RGANCNT_LTEXT[]="Rough Ancient";
char const INF_SKULL_LTEXT[]="Skull relief";

const char * const inf_texture_fullnames[]={
      INF_STANDARD_LTEXT,INF_ANCIENT_LTEXT,INF_WINTER_LTEXT,
      INF_SNAKE_LTEXT,INF_FACE_LTEXT,INF_BRESTS_LTEXT,
      INF_RGANCNT_LTEXT,INF_SKULL_LTEXT,
      };

/*
 * Sets a CLM entry of index num, using simplified parameters.
 */
void set_clm(struct LEVEL *lvl, int num, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clmentry = (unsigned char *)(lvl->clm[num]);
    clm_rec=create_column_rec();
    fill_column_rec_sim(clm_rec,use, base, c0, c1, c2, c3, c4, c5, c6, c7);
    set_clm_entry(clmentry, clm_rec);
    free_column_rec(clm_rec);
}

/*
 * Sets a CLM entry of index num, allows specifying all parameters.
 */
void set_clm_ent_idx(struct LEVEL *lvl,int num, unsigned int use, int permanent,
        int lintel, int height, unsigned int solid, int base, int orientation,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clmentry = (unsigned char *)(lvl->clm[num]);
    clm_rec=create_column_rec();
    fill_column_rec(clm_rec,use, permanent, lintel, height, solid,
             base, orientation, c0, c1, c2, c3, c4, c5, c6, c7);
    set_clm_entry(clmentry, clm_rec);
    free_column_rec(clm_rec);
}

/*
 * Searches CLM structure for given column; if not found, creates it.
 */
int column_find_or_create(struct LEVEL *lvl,struct COLUMN_REC *clm_rec)
{
  if (clm_rec==NULL) return 0;
  int num=-1;
  unsigned char *clmentry;
  // Search for column identical to the one we want
  struct COLUMN_REC *clm_rec2;
  clm_rec2=create_column_rec();
  for (num=0;num<COLUMN_ENTRIES;num++)
  {
      if (clm_entry_is_used(lvl,num))
      {
        clmentry = (unsigned char *)(lvl->clm[num]);
        get_clm_entry(clm_rec2, clmentry);
        if (compare_column_recs(clm_rec,clm_rec2))
          break;
      }
  }
  free_column_rec(clm_rec2);
  // If no identical column, then create one
  if ((num<0)||(num>=COLUMN_ENTRIES))
  {
      num=column_get_free_index(lvl);
      if ((num>=0)&&(num<COLUMN_ENTRIES))
      {
         clmentry = (unsigned char *)(lvl->clm[num]);
         set_clm_entry(clmentry, clm_rec);
      }
  }
  // Sometimes we may not find the free entry...
  // If so, return 0 - index of the empty entry
  if ((num<0)||(num>=COLUMN_ENTRIES))
     return 0;
  // But if we have it - the work is nearly done
  clmentry = (unsigned char *)(lvl->clm[num]);
  // If the new entry has permanent set, make sure to keep it
  if (clm_rec->permanent) set_clm_entry_permanent(clmentry,1);
  // Now we may return the CLM index
  return num;
}

/*
 * Tries to find unused entry in CLM structure and returns its index.
 */
int column_get_free_index(struct LEVEL *lvl)
{
  int num;
  unsigned char *clmentry;
  // Search for free column entry
  // Skip the first one - it is always zero-filled entry
  for (num=1;num<COLUMN_ENTRIES;num++)
  {
      if (!clm_entry_is_used(lvl,num))
          return num;
  }
  return -1;
}

/*
 * Updates DAT, CLM and w?b entries for the whole map - all tiles
 * and subtiles are reset. Additionally, USE values in columns
 * are recomputed to avoid mistakes.
 */
void update_datclm_for_whole_map(struct LEVEL *lvl)
{
    //Filling CLM entries with unused, zero-filled ones
    level_clear_datclm(lvl);
    add_permanent_columns(lvl);
    //setting the values from beginning
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_datclm_for_slab(lvl, i, k);
    //Setting the 'last column' entries
    update_dat_last_column(lvl,SLAB_TYPE_ROCK);
    // updating WIB (animation) entries
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_tile_wib_entries(lvl,i,k);
    // updating WLB and FLG entries
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
      {
          update_tile_wlb_entry(lvl,i,k);
          update_tile_flg_entries(lvl,i,k);
      }
    update_clm_utilize_counters(lvl);
}

/*
 * Updates DAT, CLM and w?b entries for given map tile coordinates
 * and also updates enties in all neightbour squares
 */
void update_datclm_for_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_datclm_for_slab(lvl, i, k);
      }
    // updating WIB (animation) entries - wider update is requred
    for (k=ty-2;k<=ty+2;k++)
      for (i=tx-2;i<=tx+2;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_tile_wib_entries(lvl,i,k);
      }
    // updating WLB and FLG entries
    for (k=ty-2;k<=ty+2;k++)
      for (i=tx-2;i<=tx+2;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
        {
          update_tile_wlb_entry(lvl,i,k);
          update_tile_flg_entries(lvl,i,k);
        }
      }
}

/*
 * Updates DAT, CLM and w?b entries for given map tile coordinates range
 */
void update_datclm_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last)
{
    int i,k;
    for (k=ty_first;k<=ty_last;k++)
      for (i=tx_first;i<=tx_last;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_datclm_for_slab(lvl, i, k);
      }
    // updating WIB (animation) entries - wider update is requred
    for (k=ty_first-1;k<=ty_last+1;k++)
      for (i=tx_first-1;i<=tx_last+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_tile_wib_entries(lvl,i,k);
      }
    // updating WLB and FLG entries
    for (k=ty_first-1;k<=ty_last+1;k++)
      for (i=tx_first-1;i<=tx_last+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
        {
          update_tile_wlb_entry(lvl,i,k);
          update_tile_flg_entries(lvl,i,k);
        }
      }
}

/*
 * Updates DAT and CLM entries for given map position (whole tile)
 * You will probably also need to update W?B entries.
 */
void update_datclm_for_slab(struct LEVEL *lvl, int tx, int ty)
{
  //Retrieving parameters from LEVEL structure - the slab and its surrounding
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char **surr_tng=(unsigned char **)malloc(9*sizeof(unsigned char *));
  get_slab_surround(surr_slb,surr_own,surr_tng,lvl,tx,ty);
  int i;
  // Creating CoLuMn for each subtile
  struct COLUMN_REC *clm_recs[9];
  for (i=0;i<9;i++)
    clm_recs[i]=create_column_rec();
  create_columns_for_slab(clm_recs,&(lvl->optns),surr_slb,surr_own,surr_tng);
  //Custom columns, and graffiti
  if (slab_has_custom_columns(lvl, tx, ty))
    update_custom_columns_for_slab(clm_recs,lvl,tx,ty);
  //Use the columns to set DAT/CLM entries in LEVEL
  set_new_datclm_values(lvl, tx, ty, clm_recs);
  // Flushing dynamic data
  for (i=0;i<9;i++)
    free_column_rec(clm_recs[i]);
  free(surr_slb);
  free(surr_own);
  free(surr_tng);
}

/*
 * Returns slab surrounding information, needed for CLM generation.
 */
void get_slab_surround(unsigned char *surr_slb,unsigned char *surr_own,
        unsigned char **surr_tng,const struct LEVEL *lvl,int x, int y)
{
    //Note: the surround[] array indexing must be set in a way
    // that gives right directions if IDIR_* constants are used.
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Preparing variables
    int i,k;
    int s_idx;
    //Cleanup of the input variables
    if (surr_tng!=NULL)
    {
      for (i=0;i<3;i++)
        for (k=0;k<3;k++)
          surr_tng[k*3+i]=NULL;
    }
    //Sweeping and extracting slab type and owner
    for (i=-1;i<=1;i++)
      for (k=-1;k<=1;k++)
      {
          s_idx=i+1+(k+1)*3;
          if ((x+i<0)||(y+k<0)||(x+i>=MAP_MAXINDEX_X)||(y+k>=MAP_MAXINDEX_Y))
          {
            if (surr_slb!=NULL)
              surr_slb[s_idx]=SLAB_TYPE_ROCK;
            if (surr_own!=NULL)
              surr_own[s_idx]=PLAYER_UNSET;
          }
          else
          {
            if (surr_slb!=NULL)
              surr_slb[s_idx]=get_tile_slab(lvl,x+i,y+k);
            if (surr_own!=NULL)
            {
              surr_own[s_idx]=get_tile_owner(lvl,x+i,y+k);
              if (surr_own[s_idx]>PLAYER_UNSET)
                  surr_own[s_idx]=PLAYER_UNSET;
            }
          }
      }
    //Sweeping again and extracting things
    if (surr_tng!=NULL)
    {
      for (i=-2;i<=2;i++)
        for (k=-2;k<=2;k++)
        {
          //We're not interested in corners of the 5x5 subtile block
          if (((i==-2)||(i==2))&&((k==-2)||(k==2))) continue;
          //Finding the index in target array (3x3)
          s_idx=0;
          if (i>1) { s_idx+=2; }
          else if (i>-1) s_idx+=i+1;
          if (k>1) s_idx+=2*3;
          else if (k>-1) s_idx+=(k+1)*3;
          //Now searching for the correct thing to place there
          int sx=x*MAP_SUBNUM_X+i+1;
          int sy=y*MAP_SUBNUM_Y+k+1;
          int tng_num;
          if ((sx<0)||(sy<0)||(sx>=arr_entries_x)||(sy>=arr_entries_y))
            continue;
          tng_num=get_thing_subnums(lvl,sx,sy);
          while (tng_num>0)
          {
            tng_num--;
            unsigned char *thing=get_thing(lvl,sx,sy,tng_num);
            if (is_clmaffective_thing(thing))
                surr_tng[s_idx]=thing;
          }
        }
    }
}

/*
 * Retrieves indices for given columns (possibly adds new columns to LEVEL structure)
 * then uses them to update the (tx,ty) tile. Properly updates USE/UTILIZE information
 * for both old and new columns.
 */
void set_new_datclm_values(struct LEVEL *lvl, int tx, int ty, struct COLUMN_REC *clm_recs[9])
{
  int i;
  for (i=0;i<9;i++)
  {
      int sx=tx*MAP_SUBNUM_X+idir_subtl_x[i];
      int sy=ty*MAP_SUBNUM_Y+idir_subtl_y[i];
      set_new_datclm_entry(lvl,sx,sy,clm_recs[i]);
  }
/*
  int i,k;
  //Updating previously used columns
  for (k=0;k<MAP_SUBNUM_Y;k++)
    for (i=0;i<MAP_SUBNUM_X;i++)
    {
      unsigned int clmidx;
      clmidx=get_dat_subtile(lvl, tx*MAP_SUBNUM_X+i, ty*MAP_SUBNUM_Y+k);
      clm_utilize_dec(lvl,clmidx);
    }

  // Saving columns, retrieving DAT indices
  //and updating 'utilize' counter
  int dat_entries[9];
  for (i=0;i<9;i++)
  {
      int new_dat;
      new_dat=column_find_or_create(lvl,clm_recs[i]);
      dat_entries[i]=new_dat;
      clm_utilize_inc(lvl,new_dat);
  }
  // Saving DAT indices
  set_dat(lvl,tx,ty, dat_entries[IDIR_NW],   dat_entries[IDIR_NORTH], dat_entries[IDIR_NE],
                     dat_entries[IDIR_WEST], dat_entries[IDIR_CENTR], dat_entries[IDIR_EAST],
                     dat_entries[IDIR_SW],   dat_entries[IDIR_SOUTH], dat_entries[IDIR_SE]);
*/
}

/*
 * Retrieves index for given column (possibly adds new column to LEVEL structure)
 * then uses it to update the (sx,sy) subtile. Properly updates USE/UTILIZE information
 * for both old and new columns.
 */
void set_new_datclm_entry(struct LEVEL *lvl, int sx, int sy, struct COLUMN_REC *clm_rec)
{
  unsigned int clmidx;
  clmidx=get_dat_subtile(lvl, sx, sy);
  clm_utilize_dec(lvl,clmidx);
  // Saving column, retrieving DAT index
  //and updating 'utilize' counter
  int dat_entry;
  dat_entry=column_find_or_create(lvl,clm_rec);
  clm_utilize_inc(lvl,dat_entry);
  // Saving DAT index
  set_dat_subtile(lvl,sx,sy,dat_entry);
}

/*
 * Decreases UTILIZE and USE values for column on given index
 */
void clm_utilize_dec(struct LEVEL *lvl, int clmidx)
{
  if ((clmidx<0)||(clmidx>=COLUMN_ENTRIES))
    return;
  lvl->clm_utilize[clmidx]--;
  unsigned char *clmentry;
  clmentry=lvl->clm[clmidx];
  if (clmentry!=NULL)
    clm_entry_use_dec(clmentry);
  // If the entry is unused, let's clear it completely, just for sure.
  if ((lvl->clm_utilize[clmidx]<1)&&(get_clm_entry_permanent(clmentry)==0))
  {
    lvl->clm_utilize[clmidx]=0;
    clear_clm_entry(clmentry);
  }
}

/*
 * Increases UTILIZE and USE values for column on given index
 */
void clm_utilize_inc(struct LEVEL *lvl, int clmidx)
{
  if ((clmidx<0)||(clmidx>=COLUMN_ENTRIES))
    return;
  lvl->clm_utilize[clmidx]++;
  unsigned char *clmentry;
  clmentry=lvl->clm[clmidx];
  if (clmentry!=NULL)
    clm_entry_use_inc(clmentry);
}

/*
 * Verifies column values. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short columns_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //checking entries
    short result;
    int i,k;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      result=clm_verify_entry(lvl->clm[i],err_msg);
      if (result!=VERIF_OK)
      {
        sprintf(err_msg,"%s in column %d.",err_msg,i);
        int sx,sy;
        sx=-1;sy=-1;
        if (find_dat_entry(lvl,&sx,&sy,i))
        {
          errpt->x=sx/MAP_SUBNUM_X;
          errpt->y=sy/MAP_SUBNUM_Y;
        } else
        {
          errpt->x=-1;errpt->y=-1;
        }
        return result;
      }
    }
  return VERIF_OK;
}

/*
 * Sweeps through all CLM entries and recomputes their UTILIZE counters,
 * without changing the USE property in columns. Should be called after
 * loading DAT file. It doesn't really use CLM structure, so may be
 * called before loading CLM.
 */
void update_clm_utilize_counters(struct LEVEL *lvl)
{
  int clmidx;
  //Set all "utilize" values to 0
  for (clmidx=0; clmidx<COLUMN_ENTRIES; clmidx++)
  {
      lvl->clm_utilize[clmidx]=0;
  }
  //Preparing array bounds
  int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
  int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
  //Now count "utilize" of all columns
  int cx,cy;
  for (cy=0; cy < dat_entries_y; cy++)
    for (cx=0; cx < dat_entries_x; cx++)
    {
      clmidx=get_dat_subtile(lvl, cx, cy);
      if ((clmidx>=0)&&(clmidx<COLUMN_ENTRIES))
        lvl->clm_utilize[clmidx]++;
    }
}

/*
 * Updates WIB animation entries for all subtiles of given tile
 */
void update_tile_wib_entries(struct LEVEL *lvl, int tx, int ty)
{
  int i,k;
  struct COLUMN_REC *clm_rec;
  struct COLUMN_REC *clm_rec_n;
  struct COLUMN_REC *clm_rec_w;
  struct COLUMN_REC *clm_rec_nw;
  clm_rec=create_column_rec();
  clm_rec_n=create_column_rec();
  clm_rec_w=create_column_rec();
  clm_rec_nw=create_column_rec();
  for (k=0;k<MAP_SUBNUM_Y;k++)
    for (i=0;i<MAP_SUBNUM_X;i++)
    {
      int sx=tx*MAP_SUBNUM_X+i;
      int sy=ty*MAP_SUBNUM_Y+k;
            short wib_entry;
      if (get_cust_col(lvl,sx,sy)!=NULL)
      {
        wib_entry=get_cust_col_wib_entry(lvl,sx,sy);
      } else
      {
        get_subtile_column_rec(lvl, clm_rec_nw, sx-1, sy-1);
        get_subtile_column_rec(lvl, clm_rec_n,  sx,   sy-1);
        get_subtile_column_rec(lvl, clm_rec_w,  sx-1, sy);
        get_subtile_column_rec(lvl, clm_rec,    sx  , sy);
        wib_entry=column_wib_entry(clm_rec,clm_rec_n,clm_rec_w,clm_rec_nw);
      }
      set_subtl_wib(lvl, tx*MAP_SUBNUM_X+i, ty*MAP_SUBNUM_Y+k, wib_entry);
    }
  free_column_rec(clm_rec);
  free_column_rec(clm_rec_n);
  free_column_rec(clm_rec_w);
  free_column_rec(clm_rec_nw);
}

/*
 * Updates WLB entry for given tile
 */
void update_tile_wlb_entry(struct LEVEL *lvl, int tx, int ty)
{
  unsigned short wlb_val;
  unsigned short slab;
  slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
  case SLAB_TYPE_LAVA:
    wlb_val=TILE_WLB_LAVA;
    break;
  case SLAB_TYPE_WATER:
    wlb_val=TILE_WLB_WATER;
    break;
  case SLAB_TYPE_BRIDGE:
    wlb_val=get_tile_wlb(lvl,tx,ty);
    if ((wlb_val!=TILE_WLB_LAVA)&&(wlb_val!=TILE_WLB_WATER))
    {
      // Value is obviously wrong; try to recognize correct value
      int sib_water=slab_siblings_oftype(lvl,tx,ty,SLAB_TYPE_WATER);
      int sib_lava=slab_siblings_oftype(lvl,tx,ty,SLAB_TYPE_LAVA);
      if (sib_lava>sib_water)
        wlb_val=TILE_WLB_LAVA;
      else
        wlb_val=TILE_WLB_WATER;
    }
    break;
  default:
    wlb_val=TILE_WLB_SOLID;
    break;
  }
  set_tile_wlb(lvl,tx,ty,wlb_val);
}

/*
 * Updates FLG entries for all subtiles of given tile
 */
void update_tile_flg_entries(struct LEVEL *lvl, int tx, int ty)
{
  unsigned short slab;
  slab=get_tile_slab(lvl,tx,ty);
  unsigned char owner;
  owner=get_tile_owner(lvl,tx,ty);
  int i,k;
  for (k=0;k<MAP_SUBNUM_Y;k++)
    for (i=0;i<MAP_SUBNUM_X;i++)
    {
      int sx=tx*MAP_SUBNUM_X+i;
      int sy=ty*MAP_SUBNUM_Y+k;
      //Checking if we're on slab type corner
      short corner=true;
      int crnr_x;
      int crnr_y;
      // the corners is FLG are strange - marked from inside, not outside
      if ((k==0)&&(i==0))
      {
        crnr_x=1;crnr_y=1;
      } else
      if ((k==2)&&(i==0))
      {
        crnr_x=1;crnr_y=-1;
      } else
      if ((k==0)&&(i==2))
      {
        crnr_x=-1;crnr_y=1;
      } else
      if ((k==2)&&(i==2))
      {
        crnr_x=-1;crnr_y=-1;
      } else
      { //If no corner - set the value to compare with the same tile
        crnr_x=0;crnr_y=0;
      }
      if (((slab==get_tile_slab(lvl,tx+crnr_x,ty))&&(owner==get_tile_owner(lvl,tx+crnr_x,ty))) ||
          ((slab==get_tile_slab(lvl,tx,ty+crnr_y))&&(owner==get_tile_owner(lvl,tx,ty+crnr_y))))
        corner=false;
      unsigned short flg_val;
      flg_val=compute_flg_for_tile(slab,corner);
      set_subtl_flg(lvl,sx,sy,flg_val);
    }
}

unsigned short compute_flg_for_tile(unsigned short slab,short corner)
{
  if (slab_is_wall(slab))
    return 0x0c0;
  if (slab_is_room(slab))
  {
    if (corner)
      return 0x054;
    else
      return 0x016;
  }
  if (slab_is_door(slab))
  {
    if (corner)
      return 0x002;
    else
      return 0x140;
  }
  if (slab_is_short(slab))
    return 0x002;
  if (slab_is_wealth(slab))
    return 0x041;
  switch (slab)
  {
  case SLAB_TYPE_EARTH:
  case SLAB_TYPE_TORCHDIRT:
    return 0x060;
  default:
    return 0x0d0;
  }
}
 
/*
 * Returns a column entry from column on given subtile;
 * On error returns NULL
 */
unsigned char *get_subtile_column(const struct LEVEL *lvl, int sx, int sy)
{
  if (lvl->clm==NULL)
    return NULL;
  unsigned int clmidx;
  clmidx=get_dat_subtile(lvl, sx, sy);
  if ((clmidx<0)||(clmidx>=COLUMN_ENTRIES))
    return NULL;
  return (unsigned char *)(lvl->clm[clmidx]);
}

/*
 * Fills a column entry with values from column on given subtile;
 * On error returns false and fills clm_rec with rock
 */
short get_subtile_column_rec(const struct LEVEL *lvl, struct COLUMN_REC *clm_rec, int sx, int sy)
{
  unsigned char *clmentry;
  clmentry=get_subtile_column(lvl,sx,sy);
  if (clmentry==NULL)
  {
    // Rock is the most static column, so use it on error
    fill_column_rock(clm_rec, PLAYER_UNSET);
    return false;
  }
  get_clm_entry(clm_rec, clmentry);
  return true;
}

unsigned short get_subtile_column_height(struct LEVEL *lvl, int sx, int sy)
{
  unsigned char *clmentry;
  clmentry=get_subtile_column(lvl,sx,sy);
  if (clmentry==NULL)
    return 0;
  return get_clm_entry_height(clmentry);
}

unsigned short get_subtile_column_solid(struct LEVEL *lvl, int sx, int sy)
{
  unsigned char *clmentry;
  clmentry=get_subtile_column(lvl,sx,sy);
  if (clmentry==NULL)
    return 0;
  return get_clm_entry_solid(clmentry);
}

/*
 * Sets a DAT value for one subtile. (cx,cy) is a subtile position to fill,
 * d is a column index that we want put on this subtile.
 * Warning: the USE variable of every column is unaffected by this function.
 */
void set_dat_subtile(struct LEVEL *lvl, int sx, int sy, int d)
{
    unsigned int val;
    val = 0x10000-d;
    set_dat_val(lvl,sx,sy,val);
}

/*
 * Returns a DAT entry for one subtile. (cx,cy) is a subtile position we want
 * to probe, function returns a column index.
 */
unsigned int get_dat_subtile(const struct LEVEL *lvl, const unsigned int sx, const unsigned int sy)
{          
    unsigned int val;
    val=0x10000-get_dat_val(lvl,sx,sy);
    //The AND will make sure that value "0" from get_dat_val returns as 0.
    return val&0x0ffff;
}

/*
 * Sets a DAT value for one whole tile. (x,y) is a tile position to fill,
 * other parameters are column indices that we want put on each subtile.
 * Warning: the USE variable of every column is unaffected by this function.
 */
void set_dat(struct LEVEL *lvl, int x, int y, int tl, int tm, int tr,
            int ml, int mm, int mr, int bl, int bm, int br)
{
    set_dat_subtile(lvl, x*3,   y*3,   tl);
    set_dat_subtile(lvl, x*3+1, y*3,   tm);
    set_dat_subtile(lvl, x*3+2, y*3,   tr);
    set_dat_subtile(lvl, x*3,   y*3+1, ml);
    set_dat_subtile(lvl, x*3+1, y*3+1, mm);
    set_dat_subtile(lvl, x*3+2, y*3+1, mr);
    set_dat_subtile(lvl, x*3,   y*3+2, bl);
    set_dat_subtile(lvl, x*3+1, y*3+2, bm);
    set_dat_subtile(lvl, x*3+2, y*3+2, br);
}

/*
 * Sets same DAT value for whole tile
 * Warning: the USE variable of every column is unaffected by this function.
 */
void set_dat_unif (struct LEVEL *lvl, int x, int y, int d)
{
    set_dat (lvl, x, y, d, d, d, d, d, d, d, d, d);
}

/*
 * Starts searching at a subtile after (sx,sy). Finds the next subtile containing
 * given clm_index and returns it in (sx,sy). On error/cannot find, returns (-1,-1)/
 * if (-1,-1) is given at start, searches from (0,0).
 */
short find_dat_entry(const struct LEVEL *lvl, int *sx, int *sy, const unsigned int clm_idx)
{
  if (((*sx)<0)||((*sy)<0))
  { (*sx)=-1; (*sy)=0; }
  do {
    (*sx)++;
    if ((*sx)>=MAP_SUBTOTAL_X)
    { (*sx)=0; (*sy)++; }
    if ((*sy)>=MAP_SUBTOTAL_Y)
    {
      (*sx)=-1; (*sy)=-1;
      return false;
    }
  } while (get_dat_subtile(lvl,*sx,*sy) != clm_idx);
  return true;
}

/*
 * Verifies DAT values. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short dat_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    //Preparing array bounds
    int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    //Sweeping through DAT entries
    int i, k;
    for (k=0; k<dat_entries_y; k++)
      for (i=0; i<dat_entries_x; i++)
      {
          int dat_idx=get_dat_subtile(lvl, i, k);
          if ((dat_idx<0)||(dat_idx>=COLUMN_ENTRIES))
          {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=k/MAP_SUBNUM_Y;
              sprintf(err_msg,"DAT index out of bounds at slab %d,%d.",errpt->x,errpt->y);
              return VERIF_ERROR;
          }
      }
  return VERIF_OK;
}

/*
 * Returns if the column entry is used, or unused and can be overwritten.
 */
short clm_entry_is_used(const struct LEVEL *lvl,unsigned int clmidx)
{
    unsigned char *clmentry;
    clmentry = (unsigned char *)(lvl->clm[clmidx]);
    if (clmentry==NULL) return false;
    unsigned int permanent=get_clm_entry_permanent(clmentry);
    return (lvl->clm_utilize[clmidx]>0)||(permanent);
}

short update_dat_last_column(struct LEVEL *lvl, unsigned short slab)
{
  //Preparing array bounds
  int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
  int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
  //Retrieving parameters from LEVEL structure - the slab and its surrounding
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char **surr_tng=(unsigned char **)malloc(9*sizeof(unsigned char *));
  get_slab_surround(surr_slb,surr_own,surr_tng,lvl,MAP_SIZE_X,MAP_SIZE_Y);
  surr_slb[IDIR_CENTR]=slab;
  int i;
  // Creating CoLuMn for each subtile
  struct COLUMN_REC *clm_recs[9];
  for (i=0;i<9;i++)
    clm_recs[i]=create_column_rec();
  create_columns_for_slab(clm_recs,&(lvl->optns),surr_slb,surr_own,surr_tng);
  //Use the columns to set DAT/CLM entries in LEVEL
  int sx, sy;
  sx=dat_entries_x-1;
  for (sy=0; sy<dat_entries_y; sy++)
      set_new_datclm_entry(lvl,sx,sy,clm_recs[(sy%MAP_SUBNUM_Y)*MAP_SUBNUM_X]);
  sy=dat_entries_y-1;
  for (sx=0; sx<dat_entries_x; sx++)
      set_new_datclm_entry(lvl,sx,sy,clm_recs[sx%MAP_SUBNUM_X]);
  // Flushing dynamic data
  for (i=0;i<9;i++)
    free_column_rec(clm_recs[i]);
  free(surr_slb);
  free(surr_own);
  free(surr_tng);
}

/*
 * Returns if the slab has any custom columns on it. This includes Graffiti.
 */
short slab_has_custom_columns(struct LEVEL *lvl, int tx, int ty)
{
    if (cust_cols_num_on_tile(lvl,tx,ty)>0)
        return true;
    if (graffiti_idx(lvl,tx,ty)>=0)
        return true;
    return false;
}

/*
 * Updates custom columns for a slab. Returns number of changed columns.
 * Note: also updates graffiti.
 */
int update_custom_columns_for_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty)
{
    int mod_clms=0;
    if (cust_cols_num_on_tile(lvl,tx,ty)>0)
        mod_clms+=place_cust_clms_on_slab(clm_recs,lvl,tx,ty);
    if (graffiti_idx(lvl,tx,ty)>=0)
        mod_clms+=place_graffiti_on_slab(clm_recs,lvl,tx,ty);
    return mod_clms;
}

/*
 * Draws custom columns on given columns array. Returns num of changed entries.
 * draws only the cust_col, no graffiti.
 */
int place_cust_clms_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty)
{
    int mod_clms=0;
    int i,k;
    for (k=0;k<3;k++)
    {
      int sy=ty*3+k;
      for (i=0;i<3;i++)
      {
          int sx=tx*3+i;
          struct COLUMN_REC *cclm_rec;
          cclm_rec=get_cust_col_rec(lvl,sx,sy);
          if (cclm_rec!=NULL)
          {
            clm_rec_copy(clm_recs[k*3+i],cclm_rec);
            mod_clms++;
          }
      }
    }
    return mod_clms;
}

/*
 * Adds custom column object to level data, or updates existing custom column.
 * Also updates the real level DAT/CLM entries where the column is put.
 * Returns true on success.
 */
short cust_col_add_or_update(struct LEVEL *lvl,int sx,int sy,struct DK_CUSTOM_CLM *ccol)
{
    if ((lvl==NULL)||(lvl->cust_clm_lookup==NULL)) return false;
    if ((ccol==NULL)||(ccol->rec==NULL)) return false;
    int idx;
    //Check if we already have the column at this place
    struct DK_CUSTOM_CLM *ccol_old;
    ccol_old=get_cust_col(lvl,sx,sy);
    if (ccol_old!=NULL)
      cust_col_del(lvl,sx,sy);
    //Old is gone, set the new one
    set_cust_col(lvl,sx,sy,ccol);
    if (lvl->optns.datclm_auto_update)
    {
      update_datclm_for_slab(lvl,sx/3,sy/3);
      update_tile_wib_entries(lvl,sx/3,sy/3);
      update_tile_wlb_entry(lvl,sx/3,sy/3);
      update_tile_flg_entries(lvl,sx/3,sy/3);
    }
    return true;
}






/*
 * Returns number of cust.columns at a tile.
 */
int cust_cols_num_on_tile(struct LEVEL *lvl, int tx, int ty)
{
    //We can skip tx,ty range checking, as it is made in get_cust_col
    int sx_max=(tx+1)*MAP_SUBNUM_X;
    int sy_max=(ty+1)*MAP_SUBNUM_Y;
    int sx,sy;
    int count=0;
    for (sx=tx*MAP_SUBNUM_X;sx<sx_max;sx++)
      for (sy=ty*MAP_SUBNUM_Y;sy<sy_max;sy++)
        if (get_cust_col(lvl,sx,sy)!=NULL) count++;
    return count;
}

/*
 * Returns custom column on given subtile.
 * If no cust.column there, returns NULL.
 */
struct DK_CUSTOM_CLM *get_cust_col(struct LEVEL *lvl, int sx, int sy)
{
    //Preparing array bounds
    int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    int i;
    if ((lvl==NULL)||(lvl->cust_clm_lookup==NULL))
      return NULL;
    if ((sx<0)||(sx>=dat_entries_x)||(sy<0)||(sy>=dat_entries_y))
      return NULL;
    return lvl->cust_clm_lookup[sx][sy];
}

/*
 * Returns COLUMN_REC of custom column on given subtile.
 * If no cust.column there, returns NULL.
 */
struct COLUMN_REC *get_cust_col_rec(struct LEVEL *lvl, int sx, int sy)
{
    struct DK_CUSTOM_CLM *ccol=get_cust_col(lvl,sx,sy);
    if (ccol==NULL) return NULL;
    return ccol->rec;
}

/*
 * Returns WIB entry of custom column on given subtile.
 * If no cust.column there, returns COLUMN_WIB_SKEW.
 */
unsigned short get_cust_col_wib_entry(struct LEVEL *lvl, int sx, int sy)
{
    struct DK_CUSTOM_CLM *ccol=get_cust_col(lvl,sx,sy);
    if (ccol==NULL) return COLUMN_WIB_SKEW;
    return ccol->wib_val;
}

/*
 * Adds custom column object to level data, without filling the column
 * nor updating slabs. Adds the given column pointer without
 * making a copy of it. Returns true on success.
 */
short set_cust_col(struct LEVEL *lvl,int sx,int sy,struct DK_CUSTOM_CLM *ccol)
{
    //Preparing array bounds
    int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    int i;
    if ((lvl==NULL)||(lvl->cust_clm_lookup==NULL))
      return false;
    if ((sx<0)||(sx>=dat_entries_x)||(sy<0)||(sy>=dat_entries_y))
      return false;
    if ((ccol==NULL)||(ccol->rec==NULL))
      return false;
    lvl->cust_clm_lookup[sx][sy]=ccol;
    lvl->cust_clm_count++;
    return true;
}

unsigned int get_cust_clm_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->cust_clm_count;
}

/*
 * Creates a new custom column and fills its properties.
 * The new object is not added to LEVEL structure.
 */
struct DK_CUSTOM_CLM *create_cust_col(void)
{
    struct DK_CUSTOM_CLM *ccol;
    //Filling graffiti structure
    ccol = (struct DK_CUSTOM_CLM *)malloc(sizeof(struct DK_CUSTOM_CLM));
    if (ccol==NULL)
    {
        message_error("Cannot alloc memory for custom column item");
        return NULL;
    }
    ccol->wib_val=COLUMN_WIB_SKEW;
    ccol->rec=create_column_rec();
    return ccol;
}

/*
 * Removes custom column from level and frees its memory.
 * The level graphic (DAT/CLM) is not updated.
 * Returns if there was anything to remove.
 */
short cust_col_del(struct LEVEL *lvl, int sx, int sy)
{
    //Preparing array bounds
    int dat_entries_x=MAP_SIZE_X*MAP_SUBNUM_X+1;
    int dat_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y+1;
    int i;
    if ((lvl==NULL)||(lvl->cust_clm_lookup==NULL))
      return false;
    if ((sx<0)||(sx>=dat_entries_x)||(sy<0)||(sy>=dat_entries_y))
      return false;
    struct DK_CUSTOM_CLM *ccol;
    ccol=lvl->cust_clm_lookup[sx][sy];
    lvl->cust_clm_lookup[sx][sy]=NULL;
    if (ccol==NULL) return false;
    //Decrease the count by one
    lvl->cust_clm_count--;
    //Decrease amount of allocated memory, or free the block
    free_column_rec(ccol->rec);
    free(ccol);
    return true;
}

/*
 * Removes custom columns for whole tile, deallocates memory.
 * The level graphic (DAT/CLM) is not updated.
 * Returns num. of deleted cust.columns.
 */
int cust_cols_del_for_tile(struct LEVEL *lvl, int tx, int ty)
{
    //We can skip tx,ty range checking, as it is made in get_cust_col
    int sx_max=(tx+1)*MAP_SUBNUM_X;
    int sy_max=(ty+1)*MAP_SUBNUM_Y;
    int sx,sy;
    int count=0;
    for (sx=tx*MAP_SUBNUM_X;sx<sx_max;sx++)
      for (sy=ty*MAP_SUBNUM_Y;sy<sy_max;sy++)
        if (cust_col_del(lvl,sx,sy)) count++;
    return count;
}

/*
 * Returns texture type name string
 */
char *get_texture_fullname(unsigned short inf_type)
{
     int types_count=sizeof(inf_texture_fullnames)/sizeof(char *);
     if (inf_type<types_count)
       return (char *)inf_texture_fullnames[inf_type];
     else
       return "unknown(?!)";
}
