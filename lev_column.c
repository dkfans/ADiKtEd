/*
 * lev_column.c
 *
 * Functions for maintaining CLM and DAT files.
 *
 */

#include "lev_column.h"

#include "obj_column.h"
#include "lev_data.h"
#include "globals.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "graffiti.h"

// True means DAT/CLM/WIB are updated automatically
short datclm_auto_update=true;

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
      if (clm_entry_is_used(num))
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
      if (!clm_entry_is_used(num))
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
    popup_show("Updating DAT/CLM for whole map","Regenarating whole map can take some time. Please wait...");
    //Filling CLM entries with unused, zero-filled ones
    level_clear_datclm(lvl);
    add_permanent_columns(lvl);
    //setting the values from beginning
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_datclm_for_slab(lvl, i, k);
    // updating WIB (animation) entries
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_tile_wib_entries(lvl,i,k);
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
  get_slab_surround(surr_slb,surr_own,surr_tng,tx,ty);
  int i;
  // Creating CoLuMn for each subtile
  struct COLUMN_REC *clm_recs[9];
  for (i=0;i<9;i++)
    clm_recs[i]=create_column_rec();
  create_columns_for_slab(clm_recs,surr_slb,surr_own,surr_tng);
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
        unsigned char **surr_tng,int x, int y)
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
short columns_verify(struct LEVEL *lvl, char *err_msg)
{
//Warn: This is for testing/debugging
//write_def_clm_source(lvl,"aaa");
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
  int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
  int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
  //Now count "utilize" of all columns
  int cx,cy;
  for (cy=0; cy < arr_entries_y; cy++)
    for (cx=0; cx < arr_entries_x; cx++)
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
      get_subtile_column_rec(lvl, clm_rec_nw, tx*MAP_SUBNUM_X+i-1, ty*MAP_SUBNUM_Y+k-1);
      get_subtile_column_rec(lvl, clm_rec_n, tx*MAP_SUBNUM_X+i, ty*MAP_SUBNUM_Y+k-1);
      get_subtile_column_rec(lvl, clm_rec_w, tx*MAP_SUBNUM_X+i-1, ty*MAP_SUBNUM_Y+k);
      get_subtile_column_rec(lvl, clm_rec, tx*MAP_SUBNUM_X+i, ty*MAP_SUBNUM_Y+k);
      short wib_entry;
      wib_entry=column_wib_entry(clm_rec,clm_rec_n,clm_rec_w,clm_rec_nw);
      set_subtl_wib(lvl, tx*MAP_SUBNUM_X+i, ty*MAP_SUBNUM_Y+k, wib_entry);
    }
  free_column_rec(clm_rec);
  free_column_rec(clm_rec_n);
  free_column_rec(clm_rec_w);
  free_column_rec(clm_rec_nw);
}

/*
 * Returns a column entry from column on given subtile;
 * On error returns NULL
 */
unsigned char *get_subtile_column(struct LEVEL *lvl, int sx, int sy)
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
short get_subtile_column_rec(struct LEVEL *lvl, struct COLUMN_REC *clm_rec, int sx, int sy)
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
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    if ((sx<0)||(sy<0)||(sx>=arr_entries_x)||(sy>=arr_entries_y)) return;
    unsigned int val;
    val = 0x10000-d;
    lvl->dat_high[sx][sy]=(val>>8)&255;
    lvl->dat_low[sx][sy]=val&255;
}

/*
 * Returns a DAT entry for one subtile. (cx,cy) is a subtile position we want
 * to probe, function returns a column index.
 */
unsigned int get_dat_subtile(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{          
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    if ((lvl->dat_high==NULL)||(lvl->dat_low==NULL)) return 0;
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y)) return 0;
    unsigned char high=lvl->dat_high[sx][sy];
    unsigned char low=lvl->dat_low[sx][sy];
    unsigned int subtl;
    subtl=0x10000-((high<<8)+low);
    return subtl&0x0ffff;
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
 * Verifies DAT values. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short dat_verify(struct LEVEL *lvl, char *err_msg)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Sweeping through DAT entries
    int i, k;
    for (k=0; k<arr_entries_y; k++)
      for (i=0; i<arr_entries_x; i++)
      {
          int dat_idx=get_dat_subtile(lvl, i, k);
          if ((dat_idx<0)||(dat_idx>=COLUMN_ENTRIES))
          {
              sprintf(err_msg,"DAT index out of bounds at slab %d,%d.",i/MAP_SUBNUM_X, k/MAP_SUBNUM_Y);
              return VERIF_ERROR;
          }
      }
  return VERIF_OK;
}

/*
 * Returns if the column entry is used, or unused and can be overwritten.
 */
short clm_entry_is_used(unsigned int clmidx)
{
    unsigned char *clmentry;
    clmentry = (unsigned char *)(lvl->clm[clmidx]);
    if (clmentry==NULL) return false;
    unsigned int permanent=get_clm_entry_permanent(clmentry);
    return (lvl->clm_utilize[clmidx]>0)||(permanent);
}

/*
 * Returns if the slab has any custom columns on it. This includes Graffiti.
 */
short slab_has_custom_columns(struct LEVEL *lvl, int tx, int ty)
{
    if (cust_col_nidx(lvl,tx,ty,0)>=0)
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
    if (cust_col_nidx(lvl,tx,ty,0)>=0)
        mod_clms+=draw_cust_clms_on_slab(clm_recs,lvl,tx,ty);
    if (graffiti_idx(lvl,tx,ty)>=0)
        mod_clms+=draw_graffiti_on_slab(clm_recs,lvl,tx,ty);
    return mod_clms;
}

/*
 * Draws custom columns on given columns array. Returns num of changed entries.
 * draws only the cust_col, no graffiti.
 */
int draw_cust_clms_on_slab(struct COLUMN_REC *clm_recs[9],struct LEVEL *lvl, int tx, int ty)
{
    int mod_clms=0;
    //TODO
    return mod_clms;
}

/*
 * Returns index of 'num'th custom column at given tile, or -1 if not found.
 */
int cust_col_nidx(struct LEVEL *lvl, int tx, int ty,int num)
{
    int i;
    int n=0;
    struct DK_CUSTOM_CLM *ccol;
    for (i=0; i < lvl->cust_clm_count; i++)
    {
      ccol = lvl->cust_clm[i];
      if (((ccol->sx/3)==tx) && ((ccol->sy/3)==ty))
      {
          if (n==num) return i;
          n++;
      }
    }
    return -1;
}

/*
 * Returns index of next custom column at given tile,
 * the one after prev_idx; or -1 if not found.
 * To get first cust_col, prev_idx must be -1.
 */
int cust_col_idx_next(struct LEVEL *lvl, int tx, int ty,int prev_idx)
{
    if (prev_idx<-1) return -1;
    int i;
    struct DK_CUSTOM_CLM *ccol;
    for (i=prev_idx+1; i < lvl->cust_clm_count; i++)
    {
      ccol = lvl->cust_clm[i];
      if (((ccol->sx/3)==tx) && ((ccol->sy/3)==ty))
        return i;
    }
    return -1;
}
