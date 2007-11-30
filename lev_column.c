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

/*
 * Sets a CLM entry of index num, using simplified parameters.
 */
void set_clm(struct LEVEL *lvl, int num, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clmentry = (unsigned char *)lvl->clm[num];
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
    clmentry = (unsigned char *)lvl->clm[num];
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
  int num;
  unsigned char *clmentry;
  // Search for column identical to the one we want
  struct COLUMN_REC *clm_rec2;
  clm_rec2=create_column_rec();
  for (num=0;num<COLUMN_ENTRIES;num++)
  {
      clmentry = (unsigned char *)lvl->clm[num];
      if (clm_entry_is_used(clmentry))
      {
        get_clm_entry(clm_rec2, clmentry);
        if (compare_column_recs(clm_rec,clm_rec2))
          break;
      }
  }
  free_column_rec(clm_rec2);
  // If no identical column, then create one
  if (num>=COLUMN_ENTRIES)
  {
    num=column_get_free_index(lvl);
    if ((num>=0)&&(num<COLUMN_ENTRIES))
    {
       clmentry = (unsigned char *)lvl->clm[num];
       set_clm_entry(clmentry, clm_rec);
    }
  }
  // Sometimes we may not find the free entry...
  if ((num<0)||(num>=COLUMN_ENTRIES))
     return -1;
  // But if we have it - the work is nearly done
  clmentry = (unsigned char *)lvl->clm[num];
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
  for (num=0;num<COLUMN_ENTRIES;num++)
  {
      clmentry = (unsigned char *)lvl->clm[num];
      if (!clm_entry_is_used(clmentry))
          break;
  }
  if ((num>=0)&&(num<COLUMN_ENTRIES))
    return num;
  return -1;
}

/*
 * Updates DAT, CLM and w?b entries for given map position
 */
void update_datclm_for_slab(struct LEVEL *lvl, int x, int y)
{
  struct COLUMN_REC *clm_recs[9];
  //Retrieving parameters from LEVEL structure - the slab and its surrounding
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,x,y);
  int i,k;
  //Updating previously used columns
  for (k=0;k<MAP_SUBNUM_Y;k++)
    for (i=0;i<MAP_SUBNUM_X;i++)
    {
      int clmidx;
      unsigned char *clmentry;
      clmidx=get_dat_subtile(lvl, x*MAP_SUBNUM_X+i, y*MAP_SUBNUM_Y+k);
      if ((clmidx>=0)&&(clmidx<COLUMN_ENTRIES))
      {
        clmentry = (unsigned char *)lvl->clm[clmidx];
        clm_entry_use_dec(clmentry);
      }
    }

  // Creating CoLuMn for each subtile
  for (i=0;i<9;i++)
    clm_recs[i]=create_column_rec();
  create_columns_for_slab(clm_recs,surr_slb,surr_own);
  // Saving columns, retrieving DAT indices
  // and updating WIB entries
  int dat_entries[9];
  for (k=0;k<MAP_SUBNUM_Y;k++)
    for (i=0;i<MAP_SUBNUM_X;i++)
    {
      dat_entries[i]=column_find_or_create(lvl,clm_recs[i]);
      short wib_entry=column_wib_entry(clm_recs[k*MAP_SUBNUM_X+i]);
      set_subtl_wib(lvl, wib_entry, x*MAP_SUBNUM_X+i, y*MAP_SUBNUM_Y+k);
    }

  // Saving DAT indices
  set_dat(lvl,x,y, dat_entries[IDIR_NW],   dat_entries[IDIR_NORTH], dat_entries[IDIR_NE],
                   dat_entries[IDIR_WEST], dat_entries[IDIR_CENTR], dat_entries[IDIR_EAST],
                   dat_entries[IDIR_SW],   dat_entries[IDIR_SOUTH], dat_entries[IDIR_SE]);
  // Flushing dynamic data
  for (i=0;i<9;i++)
    free_column_rec(clm_recs[i]);
  free(surr_slb);
  free(surr_own);
}

/*
 * Returns slab surrounding information, needed for CLM generation.
 */
void get_slab_surround(unsigned char *surr_slb,unsigned char *surr_own,int x, int y)
{
    //Note: the surround[] array indexing must be set in a way
    // that gives right directions if IDIR_* constants are used.
    //Preparing variables
    int i,k;
    //Sweeping and extracting slab type and owner
    for (i=-1;i<=1;i++)
      for (k=-1;k<=1;k++)
      {
          if ((x+i<0)||(y+k<0)||(x+i>=MAP_MAXINDEX_X)||(y+k>=MAP_MAXINDEX_Y))
          {
              surr_slb[i+1+(k+1)*3]=SLAB_TYPE_ROCK;
              surr_own[i+1+(k+1)*3]=PLAYER_UNSET;
          }
          else
          {
              surr_slb[i+1+(k+1)*3]=lvl->slb[x+i][y+k];
              surr_own[i+1+(k+1)*3]=lvl->own[x+i][y+k];
              if (surr_own[i+1+(k+1)*3]>PLAYER_UNSET)
                  surr_own[i+1+(k+1)*3]=PLAYER_UNSET;
          }
      }
}

/*
 * Verifies column values. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short columns_verify(struct LEVEL *lvl, char *err_msg)
{
  short result;
  int i;
  for (i=0; i<COLUMN_ENTRIES; i++)
  {
      result=clm_verify_entry(lvl->clm[i],err_msg);
      if (result!=VERIF_OK)
        return result;
  }
  return VERIF_OK;
}

/*
 * Sweeps through all CLM entries and recomputes their "use" counters
 */
void update_clm_use_counters(struct LEVEL *lvl)
{
  int i;
  //Set all "use" values to 0
  struct COLUMN_REC *clm_rec;
  clm_rec=create_column_rec();
  unsigned char *clmentry;
  for (i=0; i<COLUMN_ENTRIES; i++)
  {
    clmentry = (unsigned char *)lvl->clm[i];
    get_clm_entry(clm_rec, clmentry);
    clm_rec->use=0;
    set_clm_entry(clmentry, clm_rec);
  }
  free_column_rec(clm_rec);
  //Preparing array bounds
  int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
  int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
  //Now count "use" of all columns
  int cx,cy,clmidx;
  for (cy=0; cy < arr_entries_y; cy++)
    for (cx=0; cx < arr_entries_x; cx++)
    {
      clmidx=get_dat_subtile(lvl, cx, cy);
      if ((clmidx>=0)&&(clmidx<COLUMN_ENTRIES))
      {
        clmentry = (unsigned char *)lvl->clm[clmidx];
        clm_entry_use_inc(clmentry);
      }
    }
}



void set_room_wall (int num, unsigned int use, int base, int c1, int c2,
                     int c3, int c4, int stat)
{
    int drape[5]={0xa0, 0x19a, 0x19d, 0x1a0, 0x1a3};
    int i;
    
    for (i=0; i < 5; i++)
    {
      set_clm(lvl, num+i*3, use, base, c1, c2, c3, c4, drape[i], 0, 0, 0);
      set_clm(lvl, num+i*3+1, use, base, c1+(stat<1), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), drape[i]+1, 0, 0, 0); 
      set_clm(lvl, num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2),
             c3+2*(stat<3), c4+2*(stat<4), drape[i]+2, 0, 0, 0);
    }
}

void set_room_wall2 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4, int c5, int stat)
{
    int i;
    // Wastes clm space, but that's not a problem
    for (i=0; i < 5; i++)
    {
      set_clm(lvl, num+i*3, use, base, c1, c2, c3, c4, c5, 0, 0, 0);
      set_clm(lvl, num+i*3+1, use, base, c1+(stat<0), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), c5+(stat<5), 0, 0, 0); 
      set_clm(lvl, num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2), 
             c3+2*(stat<3), c4+2*(stat<4), c5+2*(stat<5), 0, 0, 0);
    }
}

void set_room_wall3 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4, int c5, int stat)
{
    int i;
    // Wastes clm space, but that's not a problem
    for (i=0; i < 5; i++)
    {
      set_clm(lvl, num+i*3, use, base, c1, c2, c3, c4, c5, 0, 0, 0); 
      set_clm(lvl, num+i*3+1, use, base, c1+(stat<1), c2+(stat<2),
             c3+(stat<3), c4+(stat<4), c5, 0, 0, 0); 
      set_clm(lvl, num+i*3+2, use, base, c1+2*(stat<1), c2+2*(stat<2),
             c3+2*(stat<3), c4+2*(stat<4), c5, 0, 0, 0);
    }
}

void set_room_wall4 (int num, unsigned int use, int base, int c1, int c2,
                      int c3, int c4)
{
    int drape[5]={0xa0, 0x19a, 0x19d, 0x1a0, 0x1a3};
    int i;
    
    for (i=0; i < 5; i++)
    {
      set_clm(lvl, num+i*3, 0x1a, 0x1b, 0x19, 0x53, 0x50, 0x50, 
             drape[i], 0, 0, 0);
      set_clm(lvl, num+i*3+1, use, base, c1, c2, c3, c4, 
             drape[i]+1, 0, 0, 0); 
      set_clm(lvl, num+i*3+2, 0x1a, 0x1b, 0x19, 0x51, 0x4f, 0x4f,
             drape[i]+2, 0, 0, 0);
    }
}

/*
 * Sets a DAT value for one whole tile. (x,y) is a subtile position to fill,
 * other parameters are column indices that we want put on each subtile.
 */
void set_dat (struct LEVEL *lvl, int x, int y, int tl, int tm, int tr,
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

void set_dat_unif (struct LEVEL *lvl, int x, int y, int d)
{
    set_dat (lvl, x, y, d, d, d, d, d, d, d, d, d);
}

/*
 * Sets a DAT value for one subtile. (cx,cy) is a subtile position to fill,
 * d is a column index that we want put on this subtile.
 */
void set_dat_subtile(struct LEVEL *lvl, int cx, int cy, int d)
{
    unsigned int x;
    x = 0x10000-d;
    lvl->dat_high[cx][cy]=(x>>8)&255;
    lvl->dat_low[cx][cy]=x&255;
}

/*
 * Returns a DAT entry for one subtile. (cx,cy) is a subtile position we want
 * to probe, function returns a column index.
 */
int get_dat_subtile(struct LEVEL *lvl, int cx, int cy)
{          
    return 0x10000-((lvl->dat_high[cx][cy]<<8)+lvl->dat_low[cx][cy]);
}

void set_corner_unif (int x, int y, int normal, int border)
{
    set_corner (x, y, normal, border, border, border, border,
            border, border, border, border);
}

void set_corner (int x, int y, int normal, int b1, int b2, int b3, int b4, int b5, int b6,
                  int b7, int b8)
{
    int slbsame[3][3];
    int border[3][3];
    int i, j;
    int bx, by;
    
    border[0][0]=b1;
    border[1][0]=b2;
    border[2][0]=b3;
    border[0][1]=b4;
    border[1][1]=normal;
    border[2][1]=b5;
    border[0][2]=b6;
    border[1][2]=b7;
    border[2][2]=b8;

    if (normal)
      set_dat_unif (lvl, x, y, normal);
    bx = x*3;
    by = y*3;
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          slbsame[i][j]=0;
          if (x+i-1>=0 && x+i-1 < MAP_SIZE_X && y+j-1>=0 && y+j-1 < MAP_SIZE_Y)
          {
            if (lvl->slb[x][y]==lvl->slb[x+i-1][y+j-1] && 
                lvl->own[x][y]==lvl->own[x+i-1][y+j-1])
                slbsame[i][j]=1;
          }
      }
    }
    for (i=0; i < 3; i+=2)
      for (j=0; j < 3; j+=2)
          if (!slbsame[i][j])
            set_dat_subtile(lvl, bx+i, by+j, border[i][j]);
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          if (!slbsame[i][j])
          {
            if (j==1)
            {
                set_dat_subtile(lvl, bx+i, by, border[i][1]);
                set_dat_subtile(lvl, bx+i, by+1, border[i][1]);
                set_dat_subtile(lvl, bx+i, by+2, border[i][1]);
            }
            if (i==1)
            {
                set_dat_subtile(lvl, bx, by+j, border[1][j]);
                set_dat_subtile(lvl, bx+1, by+j, border[1][j]);
                set_dat_subtile(lvl, bx+2, by+j, border[1][j]);
            }                
          }
      }
    }
    for (i=0; i < 3; i+=2)
      for (j=0; j < 3; j+=2)
          if (!slbsame[i][1] && !slbsame[1][j])
            set_dat_subtile(lvl, bx+i, by+j, border[i][j]);
}

void set_pillar_dat_entry(int x, int y, int pillar)
{
    int slbsame[3][3];
    int i, j;
    int bx, by;
    
    bx = x*3;
    by = y*3;
    for (i=0; i < 3; i++)
    {
      for (j=0; j < 3; j++)
      {
          slbsame[i][j]=0;
          if (x+i-1>=0 && x+i-1 < MAP_SIZE_X && y+j-1>=0 && y+j-1 < MAP_SIZE_Y)
          {
            if (lvl->slb[x][y]==lvl->slb[x+i-1][y+j-1] && 
                lvl->own[x][y]==lvl->own[x+i-1][y+j-1])
                slbsame[i][j]=1;
          }
      }
    }
    for (i=0; i < 3; i+=2)
    {
      for (j=0; j < 3; j+=2)
      {
          if (slbsame[i][1] && slbsame[1][j] &&
            !slbsame[2-i][1] && !slbsame[1][2-j])
          {
            if (pillar)
                set_dat_subtile(lvl, bx+i, by+j, pillar);
          }
      }
    }
}


/*
 * Create the standard clm file. Entries structured as follows:
 * OLD - delete pending
 */
void create_default_clm(void)
{
    if (!default_clm)
      default_clm=1;
    else
      return;
    // 0-99: Rocks etc
    default_clm=1;
    set_clm(lvl, 1, 0x8c5d, 0x1d, 0x2d, 0x2d, 0x2c, 0x2c, 0x2b, 0, 0, 0); // Rock
    set_clm(lvl, 2, 0xb, 0x1b, 0x19, 0xa, 0x31, 0x33, 0x32, 0, 0, 0); // Gold
    set_clm(lvl, 3, 0xb, 0x1b, 0x1a, 0xa, 0x32, 0x33, 0x31, 0, 0, 0);
    set_clm(lvl, 4, 0xb, 0x1b, 0x1b, 0xa, 0x33, 0x31, 0x32, 0, 0, 0);
    set_clm(lvl, 5, 0xb, 0x1b, 0x1c, 0xa, 0x31, 0x31, 0x33, 0, 0, 0);
    set_clm(lvl, 6, 0xb, 0x1b, 0x1d, 0xa, 0x33, 0x32, 0x31, 0, 0, 0);
    set_clm(lvl, 10, 0x177e, 0x1b, 0x1d, 0xa, 2, 2, 5, 0, 0, 0); // Earth
    set_clm(lvl, 11, 0x1819, 0x1b, 0x1c, 0xa, 1, 2, 5, 0, 0, 0); 
    set_clm(lvl, 12, 0x2f56, 0x1b, 0x1a, 0xa, 3, 1, 5, 0, 0, 0); 
    set_clm(lvl, 13, 0x2f02, 0x1b, 0x1b, 0xa, 3, 1, 5, 0, 0, 0); 
    set_clm(lvl, 14, 0x3b35, 0x1b, 0x19, 0xa, 2, 2, 5, 0, 0, 0); 
    set_clm(lvl, 15, 0x2fa5, 0x1b, 0x1b, 0xa, 3, 2, 5, 0, 0, 0); 
    set_clm(lvl, 16, 0x2f91, 0x1b, 0x1b, 0xa, 1, 2, 5, 0, 0, 0); 
    set_clm(lvl, 17, 0x2f30, 0x1b, 0x1b, 0xa, 2, 2, 5, 0, 0, 0);
    set_clm(lvl, 18, 0xd, 0x1b, 0x19, 0xa, 1, 0x18, 5, 0, 0, 0); // Torchplate
    set_clm(lvl, 20, 1, 0x1b, 0x19, 0xa, 0x1b9, 0x1ba, 0x1bb, 0, 0, 0); // Gems
    set_clm(lvl, 21, 1, 0x1b, 0x1a, 0xa, 0x1ba, 0x1bb, 0x1b9, 0, 0, 0);
    set_clm(lvl, 22, 1, 0x1b, 0x1b, 0xa, 0x1b9, 0x1b9, 0x1bc, 0, 0, 0);
    set_clm(lvl, 23, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1bb, 0, 0, 0);
    set_clm(lvl, 24, 1, 0x1b, 0x19, 0xa, 0x1bc, 0x1ba, 0x1bc, 0, 0, 0);
    set_clm(lvl, 25, 1, 0x1b, 0x1a, 0xa, 0x1b9, 0x1bb, 0x1ba, 0, 0, 0);
    set_clm(lvl, 26, 1, 0x1b, 0x1b, 0xa, 0x1ba, 0x1ba, 0x1bb, 0, 0, 0);
    set_clm(lvl, 27, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1ba, 0, 0, 0);
    set_clm(lvl, 28, 1, 0x1b, 0x1d, 0xa, 0x1b9, 0x1b9, 0x1b9, 0, 0, 0);

    // 100-199: Water, lava, paths, claimed areas
    set_clm(lvl, 100, 0xcea8, 0x0221, 0, 0, 0, 0, 0, 0, 0, 0); // Water 1
    set_clm(lvl, 105, 0x18ba, 0x0222, 0, 0, 0, 0, 0, 0, 0, 0); // Lava 1
    set_clm(lvl, 106, 0x18ba, 0x0222, 0, 0, 0, 0, 0, 0, 0, 0); // Lava 2
    set_clm(lvl, 110, 0x0594, 0x00cf, 0x1a, 0, 0, 0, 0, 0, 0, 0); // Path
    // Expand this later :) FIXME
    // Claimed areas
    set_clm(lvl, 130, 0x0045, 0x00ce, 0x00c0, 0, 0, 0, 0, 0, 0, 0); // Player 0 centre
    set_clm(lvl, 131, 0x0012, 0x00ce, 0x00c1, 0, 0, 0, 0, 0, 0, 0); // Player 1 centre
    set_clm(lvl, 132, 0x0010, 0x00ce, 0x00c2, 0, 0, 0, 0, 0, 0, 0); // Player 2 centre
    set_clm(lvl, 133, 0x0010, 0x00ce, 0x00c3, 0, 0, 0, 0, 0, 0, 0); // Player 3 centre
    set_clm(lvl, 134, 0x0184, 0x00ce, 0x00c7, 0, 0, 0, 0, 0, 0, 0); // Player 4 centre
    // Fix this properly later FIXME
    set_clm(lvl, 135, 1, 0xce, 0x80, 0, 0, 0, 0, 0, 0, 0); // Non-centres

    // 200-499: Walls
    set_clm(lvl, 200, 0x23, 0x1b, 0x1c, 0x98, 0x97, 0x96, 0x95, 0, 0, 0); // Corner
    set_clm(lvl, 202, 0x934, 0x1b, 0x19, 0xa, 2, 2, 0x43, 0, 0, 0); // Centres
    set_clm(lvl, 203, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x44, 0, 0, 0);
    set_clm(lvl, 204, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x45, 0, 0, 0);
    set_clm(lvl, 205, 0x18, 0x1b, 0x19, 0xa, 2, 2, 0x46, 0, 0, 0);
    set_clm(lvl, 206, 0x21c, 0x1b, 0x19, 0xa, 2, 2, 0x47, 0, 0, 0);
    set_clm(lvl, 207, 0x23, 0x1b, 0x19, 0xa, 2, 2, 4, 0, 0, 0);

    // Torch plate
    set_clm(lvl, 208, 3, 0x1b, 0x1a, 0x52, 0x48, 0x77, 0x4d, 0, 0, 0);
    
    // For non-corner bits of corner rooms :)
    set_clm(lvl, 209, 4, 0x1b, 0x1a, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);

    // Twins
    set_clm(lvl, 213, 1, 0x1b, 0x1c, 0x195, 0x66, 0x63, 0x60, 0, 0, 0);
    set_clm(lvl, 214, 4, 0x1b, 0x1a, 0x196, 0x67, 0x64, 0x61, 0, 0, 0);
    set_clm(lvl, 215, 1, 0x1b, 0x1b, 0x197, 0x68, 0x65, 0x62, 0, 0, 0);

    // Woman
    set_clm(lvl, 216, 0x1c, 0x1b, 0x19, 0x5d, 0x5a, 0x57, 0x54, 0, 0, 0);
    set_clm(lvl, 217, 0x1c, 0x1b, 0x1b, 0x5e, 0x5b, 0x58, 0x55, 0, 0, 0);
    set_clm(lvl, 218, 0x1c, 0x1b, 0x1c, 0x5f, 0x5c, 0x59, 0x56, 0, 0, 0);
    
    // Blank wall
    set_clm(lvl, 219, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    set_clm(lvl, 220, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    set_clm(lvl, 221, 7, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x4e, 0, 0, 0);
    
    // Shagging wall
    set_clm(lvl, 222, 0xf, 0x1d, 0x1d, 0x72, 0x6f, 0x6c, 0x69, 0, 0, 0);
    set_clm(lvl, 223, 0xb, 0x1a, 0x1d, 0x73, 0x70, 0x6d, 0x6a, 0, 0, 0);
    set_clm(lvl, 224, 0x1f, 0x1b, 0x1d, 0x74, 0x71, 0x6e, 0x6b, 0, 0, 0);
    
    // Treasure wall
    set_room_wall (225, 0xa, 0x1b, 0x19, 0x52, 0x9c, 0x99, 2);
    set_room_wall (800, 0x8f, 0x1b, 0x19, 0x52, 0x49, 0x49, 4);
    
    // Lair wall
    set_clm(lvl, 240, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm(lvl, 241, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm(lvl, 242, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm(lvl, 243, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm(lvl, 244, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm(lvl, 245, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm(lvl, 246, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm(lvl, 247, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm(lvl, 248, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm(lvl, 249, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm(lvl, 250, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm(lvl, 251, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    set_clm(lvl, 252, 0xa, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x10, 0, 0, 0);
    set_clm(lvl, 253, 0xfff0, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x11, 0, 0, 0);
    set_clm(lvl, 254, 0x4, 0x1b, 0x138, 0x17, 0x48, 0x49, 0x12, 0, 0, 0);
    
    set_clm(lvl, 815, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0xa0, 0, 0, 0);
    set_clm(lvl, 816, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0xa1, 0, 0, 0);
    set_clm(lvl, 817, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0xa2, 0, 0, 0);
    set_clm(lvl, 818, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x19a, 0, 0, 0);
    set_clm(lvl, 819, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x19b, 0, 0, 0);
    set_clm(lvl, 820, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x19c, 0, 0, 0);
    set_clm(lvl, 821, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x19d, 0, 0, 0);
    set_clm(lvl, 822, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x19e, 0, 0, 0);
    set_clm(lvl, 823, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x19f, 0, 0, 0);
    set_clm(lvl, 824, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x1a0, 0, 0, 0);
    set_clm(lvl, 825, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x1a1, 0, 0, 0);
    set_clm(lvl, 826, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x1a2, 0, 0, 0);
    set_clm(lvl, 827, 0xd, 0x1b, 0x138, 0x15, 0x48, 0x48, 0x1a3, 0, 0, 0);
    set_clm(lvl, 828, 0x1b, 0x1b, 0x138, 0x16, 0x14, 0x13, 0x1a2, 0, 0, 0);
    set_clm(lvl, 829, 0xd, 0x1b, 0x138, 0x17, 0x48, 0x48, 0x1a5, 0, 0, 0);

    // Hatchery wall
    set_room_wall (255, 8, 0x1b, 0x19, 0x155, 0x152, 0x14f, 1);
    set_room_wall4 (830, 6, 0x1b, 0x19, 0x15b, 0x158, 0x158);

    // Training room wall
    set_clm(lvl, 270, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0xa0, 0, 0, 0);
    set_clm(lvl, 271, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0xa1, 0, 0, 0);
    set_clm(lvl, 272, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0xa2, 0, 0, 0);
    set_clm(lvl, 273, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x19a, 0, 0, 0);
    set_clm(lvl, 274, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x19b, 0, 0, 0);
    set_clm(lvl, 275, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x19c, 0, 0, 0);
    set_clm(lvl, 276, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x19d, 0, 0, 0);
    set_clm(lvl, 277, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x19e, 0, 0, 0);
    set_clm(lvl, 278, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x19f, 0, 0, 0);
    set_clm(lvl, 279, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x1a0, 0, 0, 0);
    set_clm(lvl, 280, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x1a1, 0, 0, 0);
    set_clm(lvl, 281, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x1a2, 0, 0, 0);
    set_clm(lvl, 282, 0xa, 0x1b, 0x19, 0x53, 0xf5, 0xf2, 0x1a3, 0, 0, 0);
    set_clm(lvl, 283, 0xa, 0x1b, 0x19, 0xfb, 0xf6, 0xf3, 0x1a4, 0, 0, 0);
    set_clm(lvl, 284, 0xa, 0x1b, 0x19, 0x51, 0xf7, 0xf4, 0x1a5, 0, 0, 0);

    set_room_wall4 (845, 7, 0x1b, 0x19, 0xfb, 0xfa, 0xf9);

    // Library wall
    set_room_wall (285, 7, 0x1b, 0x19, 0x52, 0xab, 0xa8, 2);
    set_room_wall (860, 6, 0x1b, 0x15, 0x52, 0xb3, 0xb0, 2);
    // Workshop wall

    set_room_wall3 (300, 1, 0x1b, 0x1b, 0x52, 0x102, 0xff, 0x4d, 2);
    set_room_wall4 (875, 7, 0x1b, 0x19, 0x10e, 0x10d, 0x10c);
    
    // Leave room for guardpost wall

    // Prison wall

    set_room_wall3 (330, 3, 0x1b, 0x1c, 0x52, 0xe4, 0xe1, 0x4d, 2);
    set_room_wall4 (905, 0xc, 0x1b, 0x19, 0xbc, 0xbd, 0xbc);
    
    // Torture wall

    set_room_wall2 (345, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);
    set_room_wall2 (920, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);

    // Barracks wall
    set_room_wall2 (360, 7, 0x1b, 0x1b, 0x52, 0x18b, 0x188, 0x185, 2);
    set_room_wall4 (935, 0xc, 0x1b, 0x184, 0x184, 0x190, 0x184);
    
    // Temple wall

    set_room_wall2 (375, 0xa, 0x1b, 0x19, 0x138, 0x135, 0x132, 0x12f, 1);

    set_clm(lvl, 950, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0xa0, 0, 0, 0);
    set_clm(lvl, 951, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0xa1, 0, 0, 0);
    set_clm(lvl, 952, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0xa2, 0, 0, 0);
    set_clm(lvl, 953, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x19a, 0, 0, 0);
    set_clm(lvl, 954, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x19b, 0, 0, 0);
    set_clm(lvl, 955, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x19c, 0, 0, 0);
    set_clm(lvl, 956, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x19d, 0, 0, 0);
    set_clm(lvl, 957, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x19e, 0, 0, 0);
    set_clm(lvl, 958, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x19f, 0, 0, 0);
    set_clm(lvl, 959, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x1a0, 0, 0, 0);
    set_clm(lvl, 960, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x1a1, 0, 0, 0);
    set_clm(lvl, 961, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x1a2, 0, 0, 0);
    set_clm(lvl, 962, 4, 0x1b, 0x19, 0x122, 0x53, 0x50, 0x1a3, 0, 0, 0);
    set_clm(lvl, 963, 4, 0x1b, 0x19, 0x122, 0x120, 0x11e, 0x1a4, 0, 0, 0);
    set_clm(lvl, 964, 4, 0x1b, 0x19, 0x122, 0x51, 0x4f, 0x1a5, 0, 0, 0);

    // Graveyard wall

    set_clm(lvl, 390, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0xa0, 0, 0, 0);
    set_clm(lvl, 391, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0xa1, 0, 0, 0);
    set_clm(lvl, 392, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0xa2, 0, 0, 0);
    set_clm(lvl, 393, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19a, 0, 0, 0);
    set_clm(lvl, 394, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19b, 0, 0, 0);
    set_clm(lvl, 395, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19c, 0, 0, 0);
    set_clm(lvl, 396, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19d, 0, 0, 0);
    set_clm(lvl, 397, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19e, 0, 0, 0);
    set_clm(lvl, 398, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19f, 0, 0, 0);
    set_clm(lvl, 399, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a0, 0, 0, 0);
    set_clm(lvl, 400, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a1, 0, 0, 0);
    set_clm(lvl, 401, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a2, 0, 0, 0);
    set_clm(lvl, 402, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a3, 0, 0, 0);
    set_clm(lvl, 403, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a4, 0, 0, 0);
    set_clm(lvl, 404, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a5, 0, 0, 0);

    set_clm(lvl, 965, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0xa0, 0, 0, 0);
    set_clm(lvl, 966, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0xa1, 0, 0, 0);
    set_clm(lvl, 967, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0xa2, 0, 0, 0);
    set_clm(lvl, 968, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19a, 0, 0, 0);
    set_clm(lvl, 969, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19b, 0, 0, 0);
    set_clm(lvl, 970, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19c, 0, 0, 0);
    set_clm(lvl, 971, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x19d, 0, 0, 0);
    set_clm(lvl, 972, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x19e, 0, 0, 0);
    set_clm(lvl, 973, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x19f, 0, 0, 0);
    set_clm(lvl, 974, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a0, 0, 0, 0);
    set_clm(lvl, 975, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a1, 0, 0, 0);
    set_clm(lvl, 976, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a2, 0, 0, 0);
    set_clm(lvl, 977, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x50, 0x1a3, 0, 0, 0);
    set_clm(lvl, 978, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x1a4, 0, 0, 0);
    set_clm(lvl, 979, 0xf, 0x1b, 0x19, 0x1b3, 0x1b2, 0x4f, 0x1a5, 0, 0, 0);
    
    // Scavenger wall

    set_room_wall (405, 4, 0x1b, 0x19, 0x52, 0x112, 0x10f, 2);
    set_room_wall4 (980, 6, 0x1b, 0x19, 0x118, 0x117, 0x116);
    
    
    // Drapes
    set_clm(lvl, 450, 0xf, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa0, 0, 0, 0);
    set_clm(lvl, 451, 0xf, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa1, 0, 0, 0);
    set_clm(lvl, 452, 0x10, 0x1b, 0x19, 0x52, 0x49, 0x49, 0xa2, 0, 0, 0);
    
    set_clm(lvl, 453, 0x12, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19a, 0, 0, 0);
    set_clm(lvl, 454, 0xd, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19b, 0, 0, 0);
    set_clm(lvl, 455, 0x12, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19c, 0, 0, 0);
    
    set_clm(lvl, 456, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19d, 0, 0, 0);
    set_clm(lvl, 457, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19e, 0, 0, 0);
    set_clm(lvl, 458, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x19f, 0, 0, 0);
    
    set_clm(lvl, 459, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a0, 0, 0, 0);
    set_clm(lvl, 460, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a1, 0, 0, 0);
    set_clm(lvl, 461, 8, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a2, 0, 0, 0);

    set_clm(lvl, 462, 0x56, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a3, 0, 0, 0);
    set_clm(lvl, 463, 0x55, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a4, 0, 0, 0);
    set_clm(lvl, 464, 0x56, 0x1b, 0x19, 0x52, 0x49, 0x49, 0x1a5, 0, 0, 0);

    finish_clm(); // Put this in to avoid a stack fault
}

void finish_clm (void)
{
    // 500-799: Rooms
    set_clm(lvl, 500, 0xa0, 0x6a, 0x9f, 0, 0, 0, 0, 0, 0, 0); // Treasure
    set_clm(lvl, 501, 0xc, 0x6a, 0x9f, 0x3e, 
           0x39, 0x3a, 0x39, 0x41, 0, 0); // Pillar
    set_clm(lvl, 502, 0xc, 0x6a, 0x9f, 0x3e, 
           0x39, 0x3a, 0x41, 0, 0, 0); // Corner
    set_clm(lvl, 510, 0xfffc, 0x1b, 0x1b1, 0, 0, 0, 0, 0, 0, 0); // Lair
    set_clm(lvl, 511, 0xfffd, 0x1b, 0x1e, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm(lvl, 512, 0xfff5, 0x1b, 0x1f, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 513, 0xfff9, 0x1b, 0x20, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 514, 0xfffa, 0x1b, 0x21, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 515, 0xfff4, 0x1b, 0x22, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 516, 0xfffa, 0x1b, 0x23, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 517, 0xffef, 0x1b, 0x24, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 518, 0xfffb, 0x1b, 0x25, 0, 0, 0, 0, 0, 0, 0);
 
    
    set_clm(lvl, 520, 0x24a, 0x1b, 0, 0, 0, 0, 0, 0, 0, 0); // Hatchery
    set_clm(lvl, 521, 0xfd42, 0x1b, 0x15a, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm(lvl, 522, 0xffbc, 0x1b, 0x15b, 0x158, 0x158, 0x158,
           0x158, 0x159, 0, 0); // Pillar
    set_clm(lvl, 523, 0xffbc, 0x1b, 0x15b, 0x158, 0x158, 0x158,
           0x159, 0, 0, 0); // Pillar
    set_clm(lvl, 530, 0xff05, 0x13e, 0xfc, 0, 0, 0, 0, 0, 0, 0); // Training
    set_clm(lvl, 531, 0x23, 0x13e, 0xfe, 0, 0, 0,0, 0, 0, 0); // Border
    set_clm(lvl, 532, 5, 0x13e, 0xfc, 0xfb, 0xfa, 0xf9,
           0xf9, 0xf1, 0, 0); // Pillar
    set_clm(lvl, 533, 5, 0x13e, 0xfc, 0xfb, 0xfa, 0xf9,
           0xf1, 0, 0, 0); // Pillar
    set_clm(lvl, 540, 0xa2, 0xf8, 0xae, 0, 0, 0, 0, 0, 0, 0); // Library
    set_clm(lvl, 541, 4, 0x1b, 0xae, 0x14b, 0x14d, 0x14b, 0, 0, 0, 0); // left
    set_clm(lvl, 542, 5, 0x1b, 0xae, 0xb1, 0xb4, 0xb1, 0, 0, 0, 0); // middle
    set_clm(lvl, 543, 2, 0x1b, 0xaf, 0x14c, 0x14e, 0x14c, 0, 0, 0, 0); // right
    set_clm(lvl, 544, 1, 0x1b, 0x1a, 0xb9, 0xb8, 0xb7, 0xb6, 0, 0, 0); // Pillar/corner
    set_clm(lvl, 550, 3, 0x221, 0x1b4, 0, 0, 0, 0, 0, 0, 0); // Bridge
    set_clm(lvl, 551, 2, 0x221, 0x1b5, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 552, 3, 0x221, 0x1b6, 0, 0, 0, 0, 0, 0, 0);
    set_clm(lvl, 560, 0xffcb, 0x15a, 0x107, 0, 0, 0, 0, 0, 0, 0); // Workshop centre/edge
    set_clm(lvl, 561, 0xfe8c, 0x15a, 0x109, 0, 0, 0, 0, 0, 0, 0); // Non-centre most
    set_clm(lvl, 562, 0x1b, 0x15a, 0x108, 0, 0, 0, 0, 0, 0, 0); // Non-centre bottom
    set_clm(lvl, 563, 0x22, 0x1b, 0x1b, 0x10e, 0x10d, 0x10c,
           0x10b, 0, 0, 0); // Pillar
    set_clm(lvl, 570, 3, 0xb2, 0x80, 0x1b4, 0, 0, 0, 0, 0, 0); // Guardpost FIXME?
    set_clm(lvl, 571, 2, 0xb0, 0x80, 0x1b5, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 572, 3, 0xb0, 0x80, 0x1b6, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 580, 0xfff5, 0x118, 0xc8, 0, 0, 0, 0, 0, 0, 0); // Prison
    set_clm(lvl, 581, 0xfff8, 0x118, 0xc9, 0, 0, 0, 0, 0, 0, 0); // Borders
    set_clm(lvl, 582, 0xfff1, 0x118, 0xca, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 583, 0xfff3, 0x118, 0xcb, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 584, 0xfff0, 0x118, 0xcc, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 585, 0xffed, 0x118, 0xcd, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 586, 0xfff3, 0x118, 0xce, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 587, 0xffec, 0x118, 0xcf, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 588, 0xfff1, 0x118, 0xd0, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 589, 0xc, 0x118, 0x19, 0xbc, 0xbd, 0xbc, 0xbc, 0, 0, 0);
    set_clm(lvl, 590, 0x19, 0x118, 0xe7, 0, 0, 0, 0, 0, 0, 0); // Torture
    set_clm(lvl, 591, 0xfff5, 0x118, 0xc8, 0, 0, 0, 0, 0, 0, 0); // Non-centre
    set_clm(lvl, 592, 0xfff8, 0x118, 0xc9, 0, 0, 0, 0, 0, 0 ,0); // Border
    // Rest of borders are as prison
    set_clm(lvl, 600, 1, 0x1b, 0x183, 0, 0, 0, 0, 0, 0, 0); // Barracks
    set_clm(lvl, 601, 0x22, 0x1b, 0x1b, 0x184, 0x184, 0x18f,
           0x18e, 0x184, 0, 0); // Pillar
    set_clm(lvl, 602, 0x22, 0x1b, 0x1b, 0x184, 0x18f,
           0x18e, 0x184, 0, 0, 0); // Pillar
    // Temple
    set_clm(lvl, 610, 0x56, 0x177, 0x1b, 0x123, 0, 0, 0, 0, 0, 0); // Edge
    set_clm(lvl, 611, 8, 0x177, 0x1b, 0x122, 0, 0, 0, 0, 0, 0); // Edge corner centre
    set_clm(lvl, 612, 1, 0x177, 0x1b, 0x126, 0, 0, 0, 0, 0, 0); // Pool top left
    set_clm(lvl, 613, 2, 0x177, 0x1b, 0x127, 0, 0, 0, 0, 0, 0); // Pool top mid
    set_clm(lvl, 614, 1, 0x177, 0x1b, 0x128, 0, 0, 0, 0, 0, 0); // Pool top right
    set_clm(lvl, 615, 1, 0x177, 0x1b, 0x129, 0, 0, 0, 0, 0, 0); // Pool mid left
    set_clm(lvl, 616, 1, 0x177, 0x1b, 0x12a, 0, 0, 0, 0, 0, 0); // Pool mid mid
    set_clm(lvl, 617, 1, 0x177, 0x1b, 0x12b, 0, 0, 0, 0, 0, 0); // Pool mid right
    set_clm(lvl, 618, 1, 0x177, 0x1b, 0x12c, 0, 0, 0, 0, 0, 0); // Pool bot left
    set_clm(lvl, 619, 1, 0x177, 0x1b, 0x12d, 0, 0, 0, 0, 0, 0); // Pool bot mid
    set_clm(lvl, 620, 2, 0x177, 0x1b, 0x12e, 0, 0, 0, 0, 0, 0); // Pool bot right
    set_clm(lvl, 621, 6, 0x177, 0x1b, 0x123, 0x122, 0, 0, 0, 0, 0); // Pool wall
    set_clm(lvl, 622, 6, 0x177, 0x1b, 0x122, // Column
           0x122, 0x120, 0x11d, 0x120, 0x11e, 0x11d);
    set_clm(lvl, 623, 6, 0x177, 0x1b, // Corner
           0x122, 0x120, 0x11d, 0x120, 0x11e, 0x11d, 0);
    
    set_clm(lvl, 630, 0x4d, 0x1c, 0x142, 0, 0, 0, 0, 0, 0, 0); // Graveyard
    set_clm(lvl, 631, 2, 0x1c, 0x143, 0, 0, 0, 0, 0, 0, 0); // Border
    set_clm(lvl, 632, 9, 0x1c, 0x144, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 633, 0, 0x1c, 0x145, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 634, 8, 0x1c, 0x146, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 635, 1, 0x1c, 0x147, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 636, 0xffff, 0x1c, 0x148, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 637, 0, 0x1c, 0x149, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 638, 0, 0x1c, 0x14a, 0, 0, 0, 0, 0, 0, 0); 
    set_clm(lvl, 639, 0xf, 0x1b, 0x19, 0x98, 0x97, 0x96, 0x95, 0, 0, 0);
    set_clm(lvl, 640, 0xf, 0xf8, 0x11c, 0, 0, 0, 0, 0, 0, 0); // Scavenger
    set_clm(lvl, 641, 0x62, 0xf8, 0x119, 0, 0, 0, 0, 0, 0, 0); // off-centre
    set_clm(lvl, 642, 0xc, 0xf8, 0x119, 0x118, 0x117, 0x116, // Pillar
           0x115, 0, 0, 0);
    
    // Dungeon heart
    set_clm(lvl, 650, 0xdc, 0x1b, 0x1b, 0x52, 0x9f, 0, 0, 0, 0, 0); // Centre tiles
    set_clm(lvl, 651, 0xff7e, 0x201, 0xa4, 0, 0, 0, 0, 0, 0, 0); // Very edge
    set_clm(lvl, 652, 0xffb8, 0x1b, 0xa4, 0x52, 0x4d, 0,0,0,0,0); // Corner strange bits
    set_clm(lvl, 653, 0x17, 0x1b, 0x80, 0x180, 0, 0, 0, 0, 0, 0); // Outside corners
    set_clm(lvl, 654, 0xfd4f, 0x1b, 0x9f, 0x52, 0x4d, 0, 0, 0, 0, 0); // By above
    set_clm(lvl, 655, 0x22, 0x1b, 0x1c, 0x9f, 0, 0, 0, 0, 0, 0); // Outside edge
    set_clm(lvl, 656, 0x121, 0x1b, 0x19, 0x52, 0x9f, 0,0,0,0,0); // Inside edge
    set_clm(lvl, 657, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x17e); // Pillar - human player
    set_clm(lvl, 658, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a6); // Pillar - player 1
    set_clm(lvl, 659, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a7); // Pillar - player 2
    set_clm(lvl, 660, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Pillar - player 3
    set_clm(lvl, 661, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Dummy top in case of a "hero heart"
    set_clm(lvl, 662, 0x53, 0x1b, 0x80, 0x52, 0x48, 0x49, 0x4a,
           0x49, 0x17f, 0x1a8); // Dummy top in case of an unclaimed heart
    // Entrance
    set_clm(lvl, 700, 0xffd6, 0x1b, 0x13b, 0, 0, 0, 0, 0, 0, 0); // Very edge
    set_clm(lvl, 701, 0xb7, 0x1b, 0x1a, 0xa, 0, 0, 0, 0, 0, 0); // Edge of building
    set_clm(lvl, 702, 0xffd3, 0x1c, 0xa, 0, 0, 0, 0, 0, 0xe, 0); // Edge of roof
    set_clm(lvl, 703, 0x1f, 0x1d, 0x1a, 0xa, 
           0xf, 0xd, 0xd, 0xd, 0xe, 0); // Support pillar
    set_clm(lvl, 704, 0xfff6, 0, 0, 0, 0, 0, 0, 0, 0x89, 0); // Round the centre...
    set_clm(lvl, 705, 0xffee, 0, 0, 0, 0, 0, 0, 0, 0x8a, 0);
    set_clm(lvl, 706, 0xfff7, 0, 0, 0, 0, 0, 0, 0, 0x8b, 0);
    set_clm(lvl, 707, 0xffe4, 0, 0, 0, 0, 0, 0, 0, 0x8c, 0);
    set_clm(lvl, 708, 4, 0, 0, 0, 0, 0, 0, 0, 0x1a9, 0); // Centre
    set_clm(lvl, 709, 0xffe4, 0, 0, 0, 0, 0, 0, 0, 0x8e, 0);// Round again
    set_clm(lvl, 710, 0xfff8, 0, 0, 0, 0, 0, 0, 0, 0x8f, 0);
    set_clm(lvl, 711, 0xffe5, 0, 0, 0, 0, 0, 0, 0, 0x90, 0);
    set_clm(lvl, 712, 0xfff7, 0, 0, 0, 0, 0, 0, 0, 0x91, 0);

    // Doors
    set_clm(lvl, 720, 0xffda, 0xb5, 0x94, 0, 0, 0, 0, 0, 0, 0); // Door floor
    // top/left, centre, bottom/right
    // wooden
    set_clm(lvl, 721, 0x2e, 0xb3, 0x94, 0x169, 0x166, 0x163, 0x4d, 0, 0, 0);
    set_clm(lvl, 722, 0x1e, 0xb1, 0xc6, 0x16a, 0x167, 0x164, 0x4d, 0, 0, 0);
    set_clm(lvl, 723, 0x30, 0xba, 0x94, 0x16b, 0x168, 0x165, 0x4d, 0, 0, 0);
    // braced
    set_clm(lvl, 724, 0xe, 0xb3, 0x94, 0x172, 0x16f, 0x16c, 0x4d, 0, 0, 0);
    set_clm(lvl, 725, 0x8, 0xb1, 0xc6, 0x173, 0x170, 0x16d, 0x4d, 0, 0, 0);
    set_clm(lvl, 726, 0xe, 0xba, 0x94, 0x174, 0x171, 0x16e, 0x4d, 0, 0, 0);
    // iron
    set_clm(lvl, 727, 0x14, 0xb3, 0x94, 0x17b, 0x178, 0x175, 0x1a3, 0, 0, 0);
    set_clm(lvl, 728, 0x10, 0xb1, 0xc7, 0x17c, 0x179, 0x176, 0x1a4, 0, 0, 0);
    set_clm(lvl, 729, 0x15, 0xba, 0x94, 0x17d, 0x17a, 0x177, 0x1a5, 0, 0, 0);
    // magic
    set_clm(lvl, 730, 0x3, 0xb3, 0x94, 0x12e, 0x12b, 0x128, 0x19a, 0, 0, 0);
    set_clm(lvl, 731, 0x4, 0xb1, 0xc1, 0x12d, 0x12a, 0x127, 0x19b, 0, 0, 0);
    set_clm(lvl, 732, 0x3, 0xba, 0x94, 0x12c, 0x129, 0x126, 0x19c, 0, 0, 0);

    // Funky stuff for text
    {
      int i;
      if (fontheight == 8)
          for (i=0; i < 256; i++)
            set_clm_ent_idx(lvl, 1700+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 
                         ((i>>0)&1) ? 0x184 : 2, ((i>>1)&1) ? 0x184 : 2, 
                         ((i>>2)&1) ? 0x184 : 2, ((i>>3)&1) ? 0x184 : 2, 
                         ((i>>4)&1) ? 0x184 : 2, ((i>>5)&1) ? 0x184 : 2, 
                         ((i>>6)&1) ? 0x184 : 2, ((i>>7)&1) ? 0x184 : 2);
      else
          for (i=0; i < 256; i++)
            set_clm_ent_idx(lvl, 1700+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 0x1b,
                         fontheight>0 ? (((i>>0)&1) ? 0x184 : 2) : 0,
                         fontheight>1 ? (((i>>1)&1) ? 0x184 : 2) : 0,
                         fontheight>2 ? (((i>>2)&1) ? 0x184 : 2) : 0,
                         fontheight>3 ? (((i>>3)&1) ? 0x184 : 2) : 0,
                         fontheight>4 ? (((i>>4)&1) ? 0x184 : 2) : 0,
                         fontheight>5 ? (((i>>5)&1) ? 0x184 : 2) : 0,
                         fontheight>6 ? (((i>>6)&1) ? 0x184 : 2) : 0);
      for (i=0; i < 130; i++)
          set_clm_ent_idx(lvl, 1500+i, 0xffff, 1, 7, 8, 0x3f, 0x1b, 0, 0x1b,
                     fontheight>0 ? 2 : 0,
                     fontheight>1 ? 2 : 0,
                     fontheight>2 ? 2 : 0,
                     fontheight>3 ? 2 : 0,
                     fontheight>4 ? 2 : 0,
                     fontheight>5 ? 2 : 0,
                     fontheight>6 ? 2 : 0); 
    }
    // Set our signature on the clm
    memset (lvl->clm[2047], 0xff, 24);
}

void set_door_dat_entry(int x, int y, int datbase)
{
    int o;
    int cx, cy, i;
    
    set_dat_unif (lvl, x, y, 720);
    // Set orientation of door, fairly arbitrarily :)
    if ((!x || lvl->slb[x-1][y]<10)&&(x==MAP_MAXINDEX_X || lvl->slb[x+1][y]<10))
      o=0;
    else
      o=1;
    set_dat_subtile(lvl, x*3+o, y*3+1-o, datbase);
    set_dat_subtile(lvl, x*3+1, y*3+1, datbase+1);
    set_dat_subtile(lvl, x*3+2-o, y*3+1+o, datbase+2);
}

/*
 * Update DAT entry of a single slab
 * OLD - delete pending
 */
void update_dat_entry(struct LEVEL *lvl, int x, int y)
{
    if ((x<0) || (x>MAP_MAXINDEX_X) || (y<0) || (y>MAP_MAXINDEX_Y))
      return;

    int zx, zy, bx, by, s;
    
    int i, j;
    int slbsame[3][3];
    int slbrel[3][3];
    int ownrel[3][3];
    int surround;
    int flag1, flag2, flag3;
    
    s = lvl->slb[x][y];
    for (zx=0; zx < MAP_SUBNUM_X; zx++)
    {
      for (zy=0; zy < MAP_SUBNUM_Y; zy++)
      {
          by = y*MAP_SUBNUM_Y+zy;
          bx = x*MAP_SUBNUM_X+zx;
//          lvl->wib[bx][by]=slab_default_wbi_entry(s);
      }
    }
    surround=1;
    for (i=0; i<MAP_SUBNUM_X; i++)
    {
      for (j=0; j<MAP_SUBNUM_Y; j++)
      {
          if ((x+i-1>=0) && (x+i-1<MAP_SIZE_X) && (y+j-1>=0) && (y+j-1 < MAP_SIZE_Y))
          {
            slbrel[i][j]=lvl->slb[x+i-1][y+j-1];
            ownrel[i][j]=lvl->own[x+i-1][y+j-1];
          }
          else
          {
            slbrel[i][j]=-1;
            ownrel[i][j]=-1;
          }
          if ( (lvl->slb[x][y]==slbrel[i][j]) && (lvl->own[x][y]==ownrel[i][j]) )
            slbsame[i][j]=1;
          else
          {
            slbsame[i][j]=0;
            surround=0;
          }
      }
    }
    switch (s)
    {
    case SLAB_TYPE_ROCK: // Rock
      set_dat_unif (lvl, x, y, 1);
      break;
    case SLAB_TYPE_GOLD: // Gold
      set_dat (lvl, x, y, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2,
             rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2, rnd(5)+2);
      break;
    case SLAB_TYPE_EARTH: // Earth
      set_dat (lvl, x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      break;
    case SLAB_TYPE_TORCHDIRT: // Torch dirt
      set_dat (lvl, x, y, rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10, 
             rnd(8)+10, rnd(8)+10, rnd(8)+10);
      for (i=0; i < 3; i+=2)
      {
          if (slbrel[i][1]>9 && slbrel[i][1] != 52 &&
            (slbrel[i][1]<42 || slbrel[i][1] > 49))
            set_dat_subtile(lvl, x*3+i, y*3+1, 18);
          if (slbrel[1][i]>9 && slbrel[1][i] != 52 &&
            (slbrel[1][i]<42 || slbrel[1][i] > 49))
            set_dat_subtile(lvl, x*3+1, y*3+j, 18);
      }
      break;
    case SLAB_TYPE_WALLDRAPE: // Drape
      set_wall (x, y, 450+lvl->own[x][y]*3, 451+lvl->own[x][y]*3, 
              452+lvl->own[x][y]*3);
      break;
    case SLAB_TYPE_WALLTORCH: // Torch wall
      set_wall (x, y, 219, 220, 221);
      for (i=0; i < 3; i+=2)
      {
          if (slbrel[i][1]>9 && slbrel[i][1] != 52 &&
            (slbrel[i][1]<42 || slbrel[i][1] > 49) &&
            !picture_wall (x, y, i, 1))
            set_dat_subtile(lvl, x*3+i, y*3+1, 208);
          if (slbrel[1][i]>9 && slbrel[1][i] != 52 &&
            (slbrel[1][i]<42 || slbrel[1][i] > 49) &&
            !picture_wall (x, y, 1, i))
            set_dat_subtile(lvl, x*3+1, y*3+i, 208);
      }
      break;
    case SLAB_TYPE_WALLWTWINS: // Wall with twins
      set_wall (x, y, 213, 214, 215);
      break;
    case SLAB_TYPE_WALLWWOMAN: // Wall with woman on
      set_wall (x, y, 216, 217, 218);
      break;
    case SLAB_TYPE_WALLPAIRSHR: // Wall with pair shagging on
      set_wall (x, y, 222, 223, 224);
      break;
    case SLAB_TYPE_PATH: // Path
      set_dat_unif (lvl, x, y, 110);
      break;
    case SLAB_TYPE_CLAIMED: // Claimed
      set_dat (lvl, x, y, 135,135,135,135,130+lvl->own[x][y],135,135,135,135);
      break;
    case SLAB_TYPE_LAVA: // Lava
      set_dat_unif (lvl, x, y, 105);
      break;
    case SLAB_TYPE_WATER: // Water
      set_dat_unif (lvl, x, y, 100);
      break;
    case SLAB_TYPE_PORTAL: // Portal
      if (surround)
      {
          set_dat (lvl, x, y, 704, 705, 706, 707, 708, 709, 710, 711, 712);
      }
      else
      {
          set_dat_unif (lvl, x, y, 700); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx,  by,  703);
                  set_dat_subtile(lvl, bx-i,by,  701);
                  set_dat_subtile(lvl, bx,  by-j,701);
                }
            }
          }
          // Put floor on first, *then* roof
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if ((j==0) && (i==0)) // Ignore ourselves
                  continue; 
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-1,by-j, 701);
                  set_dat_subtile(lvl, bx,  by-j, 701);
                  set_dat_subtile(lvl, bx+1,by-j, 701);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-i, by-1,701);
                  set_dat_subtile(lvl, bx-i, by,  701);
                  set_dat_subtile(lvl, bx-i, by+1,701);
                }
            }                        
          }
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (!j && !i) // Ignore ourselves
                  continue; 
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-1, by,702);
                  set_dat_subtile(lvl, bx, by,  702);
                  set_dat_subtile(lvl, bx+1, by,702);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx, by-1,702);
                  set_dat_subtile(lvl, bx, by,  702);
                  set_dat_subtile(lvl, bx, by+1,702);
                }
            }                        
          }
      }
      break;
    case SLAB_TYPE_TREASURE: // Treasure
      set_dat_unif (lvl, x, y, 500);
      set_pillar_dat_entry(x, y, 501);
      break;
    case SLAB_TYPE_LIBRARY: // Library
      if (surround)
          set_dat (lvl, x, y, 540,540,540,541,542,543,540,540,540);
      else
      {
          set_dat_unif (lvl, x, y, 540);
          set_pillar_dat_entry(x, y, 544);
      }
      break;
    case SLAB_TYPE_PRISONCASE: // Prison case is really grotty
      set_corner (x, y, 580, 581, 582, 583, 584, 585, 586, 587, 588);
      break;
    case SLAB_TYPE_TORTURE: // Torture
      if (surround)
      {
          set_dat (lvl, x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
      }
      else
      {
          set_dat (lvl, x, y, 591, 591, 591, 591, 590,
                 591, 591, 591, 591);
          set_corner (x, y, 0, 581, 582, 583, 
                  584, 585, 586, 587, 588);
      }
      break;
    case SLAB_TYPE_TRAINING: // Training
      if (surround)
      {
          set_dat_unif (lvl, x, y, 530);
      }
      else
      {
          set_corner_unif (x, y, 530, 531);
          set_pillar_dat_entry(x, y, 532);
      }
      break;
    case SLAB_TYPE_DUNGHEART: // Dungeon heart - not perfect, 
               // but should be okay most of the time
      flag1=0; // Are we on an edge?
      flag2=0; // Have we got a top left / bottom right corner?
      flag3=0; // Have we got a top right / bottom left corner?
      if (surround)
          set_dat_unif (lvl, x, y, 650);
      else
      {
          set_dat_unif (lvl, x, y, 651); // Now we only need to add extras
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                if ((!j && !i) || (i && j)) // Ignore ourselves and corners
                  continue; 
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  flag1=1;
                  set_dat_subtile(lvl, bx, by, 656);
                  set_dat_subtile(lvl, bx-i, by-j, 655);
                  if (!i)
                  {
                      set_dat_subtile(lvl, bx-1, by, 656);      
                      set_dat_subtile(lvl, bx+1, by, 656);
                      set_dat_subtile(lvl, bx-1, by-j, 655);
                      set_dat_subtile(lvl, bx+1, by-j, 655);
                  }
                  if (!j)
                  {
                      set_dat_subtile(lvl, bx, by-1, 656);      
                      set_dat_subtile(lvl, bx, by+1, 656);
                      set_dat_subtile(lvl, bx-i, by-1, 655);
                      set_dat_subtile(lvl, bx-i, by+1, 655);
                  }
                }
            }
          }
          if (!flag1) // Not on any edges
          {
            for (i=-1; i < 2; i+=2)
            {
                for (j=-1; j < 2; j+=2)
                {
                  bx = x*3+i+1;
                  by = y*3+j+1;
                  if (slab_is_central(lvl,x+i,y+j))
                  {
                      if (i==j)
                        flag2=1;
                      else
                        flag3=1;
                      set_dat_subtile(lvl, bx, by, 657+lvl->own[x][y]);
                      set_dat_subtile(lvl, bx-i, by, 654);
                      set_dat_subtile(lvl, bx, by-j, 654);
                  }
                }
            }
            if (flag2 && !flag3)
            {
                set_dat_subtile(lvl, x*3+2, y*3, 652);
                set_dat_subtile(lvl, x*3, y*3+2, 652);
            }
            if (flag3 && !flag2)
            {
                set_dat_subtile(lvl, x*3, y*3, 652);
                set_dat_subtile(lvl, x*3+2, y*3+2, 652);
            }
            if (flag2 || flag3)
            {
                set_dat_subtile(lvl, x*3+1, y*3+1, 653);
            }
          }
      }
      break;
    case SLAB_TYPE_WORKSHOP: // Workshop 
      if (surround)
      {
          set_dat (lvl, x, y, 561, 561, 561, 561, 
                 560, 561, 561, 562, 561);
      }
      else
      {
          set_dat_unif (lvl, x, y, 560);
          set_pillar_dat_entry(x, y, 563);
      }
      break;
    case SLAB_TYPE_SCAVENGER: // Scavenger room
      set_dat (lvl, x, y, 641, 641, 641, 641, 640,
             641, 641, 641, 641);
      if (surround)
      {
      }
      else
          set_pillar_dat_entry(x, y, 642);
      break;
    case SLAB_TYPE_TEMPLE: // Temple
      if (surround)
          set_dat (lvl, x, y, 612, 613, 614, 615, 616, 617, 618, 619, 620);
      else
      {
          set_dat_unif (lvl, x, y, 610); // Now we only need to add extras
          for (i=-1; i < 2; i+=2)
          {
            for (j=-1; j < 2; j+=2)
            {
                bx = x*3+i+1;
                by = y*3+j+1;
                if (slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx, by, 622);
                  set_dat_subtile(lvl, bx-i, by-j, 611);
                }
            }
          }
          for (i=-1; i < 2; i++)
          {
            for (j=-1; j < 2; j++)
            {
                if (!j && !i) // Ignore ourselves
                  continue; 
                bx = x*3+i+1;
                by = y*3+j+1;
                
                if (!i && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-i, by-j, 610);
                  set_dat_subtile(lvl, bx-1, by, 621);
                  set_dat_subtile(lvl, bx, by, 621);
                  set_dat_subtile(lvl, bx+1, by, 621);
                }
                if (!j && slab_is_central(lvl,i+x,j+y))
                {
                  set_dat_subtile(lvl, bx-i, by-j, 610);
                  set_dat_subtile(lvl, bx, by-1, 621);
                  set_dat_subtile(lvl, bx, by, 621);
                  set_dat_subtile(lvl, bx, by+1, 621);
                }
            }                        
          }
          set_pillar_dat_entry(x, y, 0);
      }
      break;
    case SLAB_TYPE_GRAVEYARD: // Graveyard
      set_corner (x, y, 630, 631, 632, 633, 634, 635, 636, 637, 638);
      // Add dry ice instead of pillars
      for (i=0; i < 3; i+=2)
      {
          for(j=0; j < 3; j+=2)
          {
            if (slbsame[i][1] && slbsame[1][j] &&
                !slbsame[2-i][1] && !slbsame[1][2-j])
            {
            }
          }
      }
      break;
    case SLAB_TYPE_HATCHERY: // Hatchery
      set_corner_unif (x, y, 520, 521);
      set_pillar_dat_entry(x, y, 522);
      break;
    case SLAB_TYPE_LAIR: // Lair
      set_corner (x, y, 510, 511, 512, 513, 514,
                515, 516, 517, 518);
      break;
    case SLAB_TYPE_BARRACKS: // Barracks
      set_dat_unif (lvl, x, y, 600);
      set_pillar_dat_entry(x, y, 601);
      break;
    case SLAB_TYPE_DOORWOOD1:// Wooden door
    case SLAB_TYPE_DOORWOOD2: 
      set_door_dat_entry(x, y, 721);
      break;
    case SLAB_TYPE_DOORBRACE1:// Braced door
    case SLAB_TYPE_DOORBRACE2: 
      set_door_dat_entry(x, y, 724);
      break;
    case SLAB_TYPE_DOORIRON1:// Iron door
    case SLAB_TYPE_DOORIRON2: 
      set_door_dat_entry(x, y, 727);
      break;
    case SLAB_TYPE_DOORMAGIC1:// Magic door
    case SLAB_TYPE_DOORMAGIC2: 
      set_door_dat_entry(x, y, 730);
      break;
    case SLAB_TYPE_BRIDGE:// Bridge
      set_dat (lvl, x, y, 551, 552, 550, 552, 552, 552, 550, 552, 551);
      break;
    case SLAB_TYPE_GEMS:// Gems
      set_dat (lvl, x, y, rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20, 
             rnd(9)+20, rnd(9)+20, rnd(9)+20);
      break;
    case SLAB_TYPE_GUARDPOST: // Guardpost
      set_dat (lvl, x, y, 571, 572, 570, 572, 572, 572, 570, 572, 571);
      // Put the flagpost on for keepers
      if (lvl->own[x][y] < 4)
      {
      }
      break;
    }
    // Add torches to the room if it's an appropriate type
    if ((s>9) && !(s>=42 && s <=49) && s != 52)
    {
      for (i=0; i < 3; i+=2)
      {
          if ((slbrel[i][1]==3 || slbrel[i][1]==5)
            && !picture_wall (x+i-1, y, 2-i, 1))
          {
          }
          if ((slbrel[1][i]==3 || slbrel[1][i]==5)
            && !picture_wall (x, y+i-1, 1, 2-i))
          {
          }
      }
    }
}
