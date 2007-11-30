/*
 * obj_column.c
 *
 * Functions for maintaining single CLM entries.
 *
 */

#include "obj_column.h"

#include "lev_data.h"
#include "globals.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "graffiti.h"

//const unsigned short wib_columns_static[]={
//};

const unsigned short wib_columns_animate[]={
      0x0221, //water
      0x222, 0x223, //lava
};

/*
 * Creates empty COLUMN_REC structure, sets default values inside
*/
struct COLUMN_REC *create_column_rec()
{
    struct COLUMN_REC *clm_rec=(struct COLUMN_REC *)malloc(sizeof(struct COLUMN_REC));
    if (clm_rec==NULL) die("Cannot allocate memory for clm_rec");
    fill_column_rec(clm_rec, 0, 1, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0);
    return clm_rec;
}

/*
 * Frees memory of given COLUMN_REC entry,.
 */
void free_column_rec(struct COLUMN_REC *clm_rec)
{
    free(clm_rec);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=1, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_simp(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use,1, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=0, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_sim(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use, 0, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using orientation=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_autosh(struct COLUMN_REC *clm_rec, unsigned int use,
                 int permanent, int lintel, int base, 
                 int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    int solid=0;
    int height=0;
    if (c0)
    {
      height=1;
      solid|=1;
    }
    if (c1)
    {
      height=2;
      solid|=2;
    }
    if (c2)
    {
      height=3;
      solid|=4;
    }
    if (c3)
    {
      height=4;
      solid|=8;
    }
    if (c4)
    {
      height=5;
      solid|=16;
    }
    if (c5)
    {
      height=6;
      solid|=32;
    }
    if (c6)
    {
      height=7;
      solid|=64;
    }
    if (c7)
    {
      height=8;
      solid|=128;
    }
    fill_column_rec(clm_rec, use, permanent, lintel, height, solid, base, 0,
               c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, requires all parameters stored in CLM entry.
 */
void fill_column_rec(struct COLUMN_REC *clm_rec, unsigned int use, int permanent, int lintel,
                 int height, unsigned int solid, int base, int orientation,
                 int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    clm_rec->use=use;
    clm_rec->permanent=permanent;
    clm_rec->lintel=lintel;
    clm_rec->height=height;
    clm_rec->solid=solid;
    clm_rec->base=base;
    clm_rec->orientation=orientation;
    clm_rec->c[0]=c0;
    clm_rec->c[1]=c1;
    clm_rec->c[2]=c2;
    clm_rec->c[3]=c3;
    clm_rec->c[4]=c4;
    clm_rec->c[5]=c5;
    clm_rec->c[6]=c6;
    clm_rec->c[7]=c7;
}

/*
 * Sets a CLM entry; c1 is the lowest square, c8 - the top one.
 */
void set_clm_entry(unsigned char *clmentry, struct COLUMN_REC *clm_rec)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    unsigned char *x=clmentry;
    x[0]=clm_rec->use&255;
    x[1]=(clm_rec->use>>8)&255;
    x[2]=(clm_rec->permanent&1)+((clm_rec->lintel&7)<<1)+((clm_rec->height&15)<<4);
    x[3]=clm_rec->solid&255;
    x[4]=(clm_rec->solid>>8)&255;
    x[5]=clm_rec->base&255;
    x[6]=(clm_rec->base>>8)&255;
    x[7]=clm_rec->orientation&255;
    x[8]=clm_rec->c[0]&255;
    x[9]=(clm_rec->c[0]>>8)&255;
    x[10]=clm_rec->c[1]&255;
    x[11]=(clm_rec->c[1]>>8)&255;
    x[12]=clm_rec->c[2]&255;
    x[13]=(clm_rec->c[2]>>8)&255;
    x[14]=clm_rec->c[3]&255;
    x[15]=(clm_rec->c[3]>>8)&255;
    x[16]=clm_rec->c[4]&255;
    x[17]=(clm_rec->c[4]>>8)&255;
    x[18]=clm_rec->c[5]&255;
    x[19]=(clm_rec->c[5]>>8)&255;
    x[20]=clm_rec->c[6]&255;
    x[21]=(clm_rec->c[6]>>8)&255;
    x[22]=clm_rec->c[7]&255;
    x[23]=(clm_rec->c[7]>>8)&255;
}

void get_clm_entry(struct COLUMN_REC *clm_rec, unsigned char *clmentry)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    unsigned char *x=clmentry;
    clm_rec->use=x[0]+(x[1]<<8);
    clm_rec->permanent=x[2]&1;
    clm_rec->lintel=(x[2]>>1)&7;
    clm_rec->height=(x[2]>>4)&15;
    clm_rec->solid=x[3]+(x[4]<<8);
    clm_rec->base=x[5]+(x[6]<<8);
    clm_rec->orientation=x[7];
    clm_rec->c[0]=x[8]+(x[9]<<8);
    clm_rec->c[1]=x[10]+(x[11]<<8);
    clm_rec->c[2]=x[12]+(x[13]<<8);
    clm_rec->c[3]=x[14]+(x[15]<<8);
    clm_rec->c[4]=x[16]+(x[17]<<8);
    clm_rec->c[5]=x[18]+(x[19]<<8);
    clm_rec->c[6]=x[20]+(x[21]<<8);
    clm_rec->c[7]=x[22]+(x[23]<<8);
}

/*
 * Determines if the two CLM entries looks identically
 */
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2)
{
  if (clm_rec1->height != clm_rec2->height) return false;
  if (clm_rec1->solid != clm_rec2->solid) return false;
  unsigned short height=clm_rec1->height;
  unsigned int solid=clm_rec1->solid;
  if (clm_rec1->base!=clm_rec2->base)
    return false;
  if ((solid&1)&&(height>=1))
    if (clm_rec1->c[0]!=clm_rec2->c[0])
      return false;
  if ((solid&2)&&(height>=2))
    if (clm_rec1->c[1]!=clm_rec2->c[1])
      return false;
  if ((solid&4)&&(height>=3))
    if (clm_rec1->c[2]!=clm_rec2->c[2])
      return false;
  if ((solid&8)&&(height>=4))
    if (clm_rec1->c[3]!=clm_rec2->c[3])
      return false;
  if ((solid&16)&&(height>=5))
    if (clm_rec1->c[4]!=clm_rec2->c[4])
      return false;
  if ((solid&32)&&(height>=6))
    if (clm_rec1->c[5]!=clm_rec2->c[5])
      return false;
  if ((solid&64)&&(height>=7))
    if (clm_rec1->c[6]!=clm_rec2->c[6])
      return false;
  if ((solid&128)&&(height>=8))
    if (clm_rec1->c[7]!=clm_rec2->c[7])
      return false;
  return true;
}

/*
 * Increases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_inc(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use<65535) use++;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Decreases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_dec(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use>0) use--;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Returns if the column entry is used, or unused and can be overwritten.
 */
short clm_entry_is_used(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    short permanent=clmentry[2]&1;
    return (use>0)||(permanent);
}

void set_clm_entry_permanent(unsigned char *clmentry,short nperm)
{
    unsigned char val;
    val=clmentry[2] & 0xfe; //clear permanent bit
    clmentry[2]=(val)|(nperm&1);
}

/*
 * Verifies values in the CLM entry.
 */
short clm_verify_entry(unsigned char *clmentry, char *err_msg)
{
  struct COLUMN_REC *clm_rec;
  clm_rec=create_column_rec();
  get_clm_entry(clm_rec, clmentry);
  int i;
  for (i=0;i<8;i++)
    if (clm_rec->c[i]>CLM_BOX_MAX_INDEX)
    {
        sprintf(err_msg,"Cube entry %d too large",i);
        return VERIF_WARN;
    }
  free_column_rec(clm_rec);
  return VERIF_OK;
}

short column_wib_animate(unsigned int clm)
{
    int array_count=sizeof(wib_columns_animate)/sizeof(unsigned short);
    int idx=arr_ushort_pos(wib_columns_animate,clm,array_count);
    if (idx>=0) return true;
    return false;
}

/*
 * Returns a proper WIB file entry for given column
 */
unsigned short column_wib_entry(struct COLUMN_REC *clm_rec)
{
    if (column_wib_animate(clm_rec->base))
      return COLUMN_WIB_ANIMATE;
    int i;
    for (i=0;i<8;i++)
    {
      if (column_wib_animate(clm_rec->c[i]))
        return COLUMN_WIB_ANIMATE;
    }
    return COLUMN_WIB_SKEW;
}

/*
 * Fills up 9 CLM entries needed for given slab with specified surroundings.
 */
void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  unsigned char slab=surr_slb[IDIR_CENTR];
  switch (slab)
  {
    case SLAB_TYPE_ROCK:
      create_columns_slb_rock(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_GOLD:
      create_columns_slb_gold(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_EARTH:
      create_columns_slb_earth(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_TORCHDIRT:
      create_columns_slb_torchdirt(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WALLDRAPE:
      create_columns_slb_walldrape(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WALLTORCH:
      create_columns_slb_walltorch(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WALLWTWINS:
      create_columns_slb_wallwtwins(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WALLWWOMAN:
      create_columns_slb_wallwwoman(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WALLPAIRSHR:
      create_columns_slb_wallpairshr(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_PATH:
      create_columns_slb_path(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_CLAIMED:
      create_columns_slb_claimed(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_LAVA:
      create_columns_slb_lava(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WATER:
      create_columns_slb_water(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_PORTAL:
      create_columns_slb_portal(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_TREASURE:
      create_columns_slb_treasure(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_LIBRARY:
      create_columns_slb_library(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_columns_slb_prison(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_TORTURE:
      create_columns_slb_torture(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_TRAINING:
      create_columns_slb_training(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_columns_slb_dungheart(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_columns_slb_workshop(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_columns_slb_scavenger(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_TEMPLE:
      create_columns_slb_temple(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_columns_slb_graveyard(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_HATCHERY:
      create_columns_slb_hatchery(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_LAIR:
      create_columns_slb_lair(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_BARRACKS:
      create_columns_slb_barracks(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      create_columns_slb_doorwood(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      create_columns_slb_doorbrace(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      create_columns_slb_dooriron(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      create_columns_slb_doormagic(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_BRIDGE:
      create_columns_slb_bridge(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_GEMS:
      create_columns_slb_gems(clm_recs,surr_slb,surr_own);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_columns_slb_guardpost(clm_recs,surr_slb,surr_own);
      break;
    default: //on error, make path
      create_columns_slb_path(clm_recs,surr_slb,surr_own);
      break;
    }
}

/*
 * A helper function for using surr_own array
 */
short surrnd_not_enemy(unsigned char *surr_own, short direction)
{
    if ((surr_own[IDIR_CENTR]==surr_own[direction%9])
      ||((surr_own[direction%9])>=PLAYER_UNSET))
        return true;
    return false;
}

void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
//TODO: partial gold near path and dirt
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_gold(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
//TODO: earth near lava, partial earth (near lava, water and path)
  //Center is always water
  fill_column_earth(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);

  //Now corner slabs
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_WEST]==SLAB_TYPE_WATER))
  {
     fill_column_earth_nearwater(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  } else
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA))
  {
     fill_column_earth_nearlava(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  } else
  {
     fill_column_earth(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  }
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_EAST]==SLAB_TYPE_WATER))
  {
     fill_column_earth_nearwater(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
  } else
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA))
  {
     fill_column_earth_nearlava(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
  } else
  {
     fill_column_earth(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
  }
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_WEST]==SLAB_TYPE_WATER))
  {
     fill_column_earth_nearwater(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  } else
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA))
  {
     fill_column_earth_nearlava(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  } else
  {
     fill_column_earth(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  }
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_EAST]==SLAB_TYPE_WATER))
  {
     fill_column_earth_nearwater(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
  } else
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA))
  {
     fill_column_earth_nearlava(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
  } else
  {
     fill_column_earth(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
  }

  // And the middle ones
  if (surr_slb[IDIR_NORTH]==SLAB_TYPE_WATER)
     fill_column_earth_nearwater(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
  else
     fill_column_earth(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_EAST]==SLAB_TYPE_WATER)
     fill_column_earth_nearwater(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
  else
     fill_column_earth(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_SOUTH]==SLAB_TYPE_WATER)
     fill_column_earth_nearwater(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
  else
     fill_column_earth(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_WEST]==SLAB_TYPE_WATER)
     fill_column_earth_nearwater(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
  else
     fill_column_earth(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
}

void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
//TODO: create torch place only where the item "torch" exists, skip near lava.
  //This is identical to standard dirt
  create_columns_slb_earth(clm_recs,surr_slb,surr_own);
  //But one of the c[3] entries is replaced with torch-one
  if (slab_is_room(surr_slb[IDIR_NORTH])||slab_is_space(surr_slb[IDIR_NORTH]))
    clm_recs[IDIR_NORTH]->c[3]=0x018;
  else
  if (slab_is_room(surr_slb[IDIR_EAST])||slab_is_space(surr_slb[IDIR_EAST]))
    clm_recs[IDIR_EAST]->c[3]=0x018;
  else
  if (slab_is_room(surr_slb[IDIR_SOUTH])||slab_is_space(surr_slb[IDIR_SOUTH]))
    clm_recs[IDIR_SOUTH]->c[3]=0x018;
  else
  if (slab_is_room(surr_slb[IDIR_WEST])||slab_is_space(surr_slb[IDIR_WEST]))
    clm_recs[IDIR_WEST]->c[3]=0x018;
}

/*
 * Creates wall with red brick inside. Returns where are whole brick walls
 */
void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own)
{
//TODO: near water and near lava support
  int i;
  //Filling matrix
  for (i=0;i<9;i++)
    allow_relief[i]=true;
  //We will use the NW, NE, SW, SE values of the matrix to help making brick
  // with shadow
  //Central slab identifies the player
  fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  //Corner slabs - white brick, small red brick or clear earth - NW corner
  // If we're surrounded by our wall, and there is something short also - red brick
  if (((slab_is_wall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
     &&((slab_is_short(surr_slb[IDIR_WEST]))||(!surrnd_not_enemy(surr_own,IDIR_WEST))))
     ||((slab_is_wall(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
     &&((slab_is_short(surr_slb[IDIR_NORTH]))||(!surrnd_not_enemy(surr_own,IDIR_NORTH)))))
  {
      fill_column_wall_redsmbrick_a(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
//      fill_column_wall_redsmbrick_c(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
  } else
  // if we're surrouned with unowned tall thing on _both_ sides, fill with dirt
  if ((slab_is_tall_unclmabl(surr_slb[IDIR_NORTH])&&slab_is_tall_unclmabl(surr_slb[IDIR_WEST]))
//    ||(slab_is_tall(surr_slb[IDIR_NW])&&
    ||(((slab_is_tall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
    &&(slab_is_tall(surr_slb[IDIR_WEST])&& surrnd_not_enemy(surr_own,IDIR_WEST)))))
  {
    fill_column_earth(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
    allow_relief[IDIR_WEST]=false;
  } else
  // Finally - if there's something short, like ground - use the white brick
  {
    fill_column_wall_cobblestones(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
    allow_relief[IDIR_WEST]=false;
    allow_relief[IDIR_NW]=false;
  }

  //Corner slabs - white brick, small red brick or clear earth - NE corner
  // If we're surrounded by our wall - red brick
  if (((slab_is_wall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
     &&((slab_is_short(surr_slb[IDIR_EAST]))||(!surrnd_not_enemy(surr_own,IDIR_EAST))))
     ||((slab_is_wall(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
     &&((slab_is_short(surr_slb[IDIR_NORTH]))||(!surrnd_not_enemy(surr_own,IDIR_NORTH)))))
  {
      fill_column_wall_redsmbrick_a(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
//      fill_column_wall_redsmbrick_c(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
  } else
  // if we're surrouned with unowned tall thing on both sides, fill with dirt
  if ((slab_is_tall_unclmabl(surr_slb[IDIR_NORTH])&&slab_is_tall_unclmabl(surr_slb[IDIR_EAST]))
//    ||(slab_is_tall(surr_slb[IDIR_NE])&&
    ||(((slab_is_tall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
    &&(slab_is_tall(surr_slb[IDIR_EAST])&& surrnd_not_enemy(surr_own,IDIR_EAST)))))
  {
    fill_column_earth(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
    allow_relief[IDIR_EAST]=false;
  } else
  // Finally - if there's something short, like ground - use the white brick
  {
    fill_column_wall_cobblestones(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
    allow_relief[IDIR_EAST]=false;
    allow_relief[IDIR_NE]=false;
  }

  //Corner slabs - white brick, small red brick or clear earth - SW corner
  // If we're surrounded by our wall - red brick
  if (((slab_is_wall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
     &&((slab_is_short(surr_slb[IDIR_WEST]))||(!surrnd_not_enemy(surr_own,IDIR_WEST))))
     ||((slab_is_wall(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
     &&((slab_is_short(surr_slb[IDIR_SOUTH]))||(!surrnd_not_enemy(surr_own,IDIR_SOUTH)))))
  {
      fill_column_wall_redsmbrick_a(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
  } else
  // if we're surrouned with unowned tall thing on both sides, fill with dirt
  if ((slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH])&&slab_is_tall_unclmabl(surr_slb[IDIR_WEST]))
//    ||(slab_is_tall(surr_slb[IDIR_SW])&&
    ||(((slab_is_tall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
    &&(slab_is_tall(surr_slb[IDIR_WEST])&& surrnd_not_enemy(surr_own,IDIR_WEST)))))
  {
    fill_column_earth(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
    allow_relief[IDIR_WEST]=false;
  } else
  // Finally - if there's something short, like ground - use the white brick
  {
    fill_column_wall_cobblestones(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
    allow_relief[IDIR_WEST]=false;
    allow_relief[IDIR_SW]=false;
  }

  //Corner slabs - white brick, small red brick or clear earth - SE corner
  // If we're surrounded by our wall - red brick
  if (((slab_is_wall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
     &&((slab_is_short(surr_slb[IDIR_EAST]))||(!surrnd_not_enemy(surr_own,IDIR_EAST))))
     ||((slab_is_wall(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
     &&((slab_is_short(surr_slb[IDIR_SOUTH]))||(!surrnd_not_enemy(surr_own,IDIR_SOUTH)))))
  {
      fill_column_wall_redsmbrick_a(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
  } else
  // if we're surrouned with unowned tall thing on both sides, fill with dirt
  if ((slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH])&&slab_is_tall_unclmabl(surr_slb[IDIR_EAST]))
//    ||(slab_is_tall(surr_slb[IDIR_SE])
    ||(((slab_is_tall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
    &&(slab_is_tall(surr_slb[IDIR_EAST])&& surrnd_not_enemy(surr_own,IDIR_EAST)))))
  {
    fill_column_earth(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
    allow_relief[IDIR_EAST]=false;
  } else
  // Finally - if there's something short, like ground - use the white brick
  {
    fill_column_wall_cobblestones(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
    allow_relief[IDIR_EAST]=false;
    allow_relief[IDIR_SE]=false;
  }

  // And the remaining, middle slabs
  if ((slab_is_wall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH]))
  {
    fill_column_earth(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
  } else
  {
    if (allow_relief[IDIR_NE])
      // just standard red brick
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    else
      // the "b" brick has dark edge from this view
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_EAST]))
  {
    fill_column_earth(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_EAST]=false;
  } else
  {
    if (allow_relief[IDIR_SE])
      // just standard red brick
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    else
      // the "b" brick has dark edge from this view
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
  {
    fill_column_earth(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
  } else
  {
    if (allow_relief[IDIR_SW])
      // just standard red brick
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    else
      // the "c" brick has dark edge from this view
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_WEST]))
  {
    fill_column_earth(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_WEST]=false;
  } else
  {
    if (allow_relief[IDIR_NW])
      // just standard red brick
      fill_column_wall_redsmbrick_b(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    else
      // the "c" brick has dark edge from this view
      fill_column_wall_redsmbrick_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  }
}

/*
 * Creates wall with small drape at top
 */
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_drapebrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_drapebrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_drapebrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_drapebrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_drapebrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

/*
 * Creates wall with torch plate
 */
void create_columns_slb_walltorch(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
//TODO: place the plate where a thing is
  //This variable will help in placing the bas-relief or drape;
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own);
    // Torch plate
  if (slab_is_room(surr_slb[IDIR_NORTH])||slab_is_space(surr_slb[IDIR_NORTH]))
    clm_recs[IDIR_NORTH]->c[3]=0x077;
  else
  if (slab_is_room(surr_slb[IDIR_EAST])||slab_is_space(surr_slb[IDIR_EAST]))
    clm_recs[IDIR_EAST]->c[3]=0x077;
  else
  if (slab_is_room(surr_slb[IDIR_SOUTH])||slab_is_space(surr_slb[IDIR_SOUTH]))
    clm_recs[IDIR_SOUTH]->c[3]=0x077;
  else
  if (slab_is_room(surr_slb[IDIR_WEST])||slab_is_space(surr_slb[IDIR_WEST]))
    clm_recs[IDIR_WEST]->c[3]=0x077;
}

void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}
void create_columns_slb_wallwwoman(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_wallpairshr(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own);
  //If there's enought place for drape - draw it
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //Central CLM entry
  fill_column_path(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  //Corners
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA))
  {
     fill_column_path_nearlava(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA))
  {
     fill_column_path_nearlava(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA))
  {
     fill_column_path_nearlava(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)||(surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA))
  {
     fill_column_path_nearlava(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
  //And middle ones
  if (surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)
  {
     fill_column_path_nearlava(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
  {
     fill_column_path_nearlava(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)
  {
     fill_column_path_nearlava(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);

  if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
  {
     fill_column_path_nearlava(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
  } else
     fill_column_path(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
}

void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  //Center
  fill_column_claimedgnd_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners - NW and NE
  if (surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)
  { //Lava in north - both corners to lava-edge
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
  {
    if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    { //Surrounded by our area
      fill_column_claimedgnd_surr(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
    } else
    { //our on north only
      fill_column_rec_sim(clm_recs[IDIR_NW], 0, 0x0ce,
         0x084, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
    if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    { //Surrounded by our area
      fill_column_claimedgnd_surr(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
    } else
    { //our on north only
      fill_column_rec_sim(clm_recs[IDIR_NE], 0, 0x0ce,
         0x085, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
  } else
  {
    if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_NW],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    { //our on west only
      fill_column_rec_sim(clm_recs[IDIR_NW], 0, 0x0ce,
         0x082, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    } else
    { //Surrounded by not our area
      fill_column_rec_sim(clm_recs[IDIR_NW], 0, 0x0ce,
         0x081, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
    if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_NE],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    { //our on east only
      fill_column_rec_sim(clm_recs[IDIR_NE], 0, 0x0ce,
         0x082, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    } else
    { //Surrounded by not our area
      fill_column_rec_sim(clm_recs[IDIR_NE], 0, 0x0ce,
         0x083, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
  }
  //Corners - SW and SE
  if (surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)
  { //Lava in south - both corners to lava-edge
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
  {
    if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    { //Surrounded by our area
      fill_column_claimedgnd_surr(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
    } else
    { //our on south only
      fill_column_rec_sim(clm_recs[IDIR_SW], 0, 0x0ce,
         0x084, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
    if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    { //Surrounded by our area
      fill_column_claimedgnd_surr(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
    } else
    { //our on south only
      fill_column_rec_sim(clm_recs[IDIR_SE], 0, 0x0ce,
         0x085, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
  } else
  {
    if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_SW],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    { //our on west only
      fill_column_rec_sim(clm_recs[IDIR_SW], 0, 0x0ce,
         0x087, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    } else
    { //Surrounded by not our area
      fill_column_rec_sim(clm_recs[IDIR_SW], 0, 0x0ce,
         0x086, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
    if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
    { //Lava near
       fill_column_claimedgnd_nearlava(clm_recs[IDIR_SE],surr_own[IDIR_CENTR]);
    } else
    if (slab_is_short_clmabl(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    { //our on east only
      fill_column_rec_sim(clm_recs[IDIR_SE], 0, 0x0ce,
         0x087, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    } else
    { //Surrounded by not our area
      fill_column_rec_sim(clm_recs[IDIR_SE], 0, 0x0ce,
         0x088, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    }
  }
  //And the middle columns
  if (surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA)
  { //Lava - nothing more matters
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
  {
    fill_column_claimedgnd_surr(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
  } else
  {
    fill_column_rec_sim(clm_recs[IDIR_SOUTH], 0, 0x0ce,
       0x087, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
  }
  if (surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA)
  { //Lava - nothing more matters
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
  {
    fill_column_claimedgnd_surr(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  } else
  {
    fill_column_rec_sim(clm_recs[IDIR_WEST], 0, 0x0ce,
       0x084, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
  }
  if (surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA)
  { //Lava - nothing more matters
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
  {
    fill_column_claimedgnd_surr(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
  } else
  {
    fill_column_rec_sim(clm_recs[IDIR_NORTH], 0, 0x0ce,
       0x082, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
  }
  if (surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA)
  { //Lava - nothing more matters
     fill_column_claimedgnd_nearlava(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
  } else
  if (slab_is_short_clmabl(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
  {
    fill_column_claimedgnd_surr(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
  } else
  {
    fill_column_rec_sim(clm_recs[IDIR_EAST], 0, 0x0ce,
       0x085, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
  }
}

void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_lava(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_water(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

//!!!!!!!!!! unfinished beyond this line

void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    set_clm(lvl, 20, 1, 0x1b, 0x19, 0xa, 0x1b9, 0x1ba, 0x1bb, 0, 0, 0); // Gems
    set_clm(lvl, 21, 1, 0x1b, 0x1a, 0xa, 0x1ba, 0x1bb, 0x1b9, 0, 0, 0);
    set_clm(lvl, 22, 1, 0x1b, 0x1b, 0xa, 0x1b9, 0x1b9, 0x1bc, 0, 0, 0);
    set_clm(lvl, 23, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1bb, 0, 0, 0);
    set_clm(lvl, 24, 1, 0x1b, 0x19, 0xa, 0x1bc, 0x1ba, 0x1bc, 0, 0, 0);
    set_clm(lvl, 25, 1, 0x1b, 0x1a, 0xa, 0x1b9, 0x1bb, 0x1ba, 0, 0, 0);
    set_clm(lvl, 26, 1, 0x1b, 0x1b, 0xa, 0x1ba, 0x1ba, 0x1bb, 0, 0, 0);
    set_clm(lvl, 27, 1, 0x1b, 0x1c, 0xa, 0x1bb, 0x1bc, 0x1ba, 0, 0, 0);
    set_clm(lvl, 28, 1, 0x1b, 0x1d, 0xa, 0x1b9, 0x1b9, 0x1b9, 0, 0, 0);
*/
}

void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}
void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}
void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}
void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}

void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}

void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    // Treasure wall
    set_room_wall (225, 0xa, 0x1b, 0x19, 0x52, 0x9c, 0x99, 2);
    set_room_wall (800, 0x8f, 0x1b, 0x19, 0x52, 0x49, 0x49, 4);
*/
}

void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*    // Library wall
    set_room_wall (285, 7, 0x1b, 0x19, 0x52, 0xab, 0xa8, 2);
    set_room_wall (860, 6, 0x1b, 0x15, 0x52, 0xb3, 0xb0, 2);
*/
}

void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    // Prison wall
    set_room_wall3 (330, 3, 0x1b, 0x1c, 0x52, 0xe4, 0xe1, 0x4d, 2);
    set_room_wall4 (905, 0xc, 0x1b, 0x19, 0xbc, 0xbd, 0xbc);
*/
}

void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*    
    // Torture wall
    set_room_wall2 (345, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);
    set_room_wall2 (920, 7, 0x1b, 0x1b, 0x52, 0xee, 0xeb, 0xe8, 2);
*/
}

void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
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
*/
}

void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}
void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    // Workshop wall
    set_room_wall3 (300, 1, 0x1b, 0x1b, 0x52, 0x102, 0xff, 0x4d, 2);
    set_room_wall4 (875, 7, 0x1b, 0x19, 0x10e, 0x10d, 0x10c);
*/
}

void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*    
    // Scavenger wall
    set_room_wall (405, 4, 0x1b, 0x19, 0x52, 0x112, 0x10f, 2);
    set_room_wall4 (980, 6, 0x1b, 0x19, 0x118, 0x117, 0x116);
*/
}

void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*   
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
*/
}

void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
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
*/
}

void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    // Hatchery wall
    set_room_wall (255, 8, 0x1b, 0x19, 0x155, 0x152, 0x14f, 1);
    set_room_wall4 (830, 6, 0x1b, 0x19, 0x15b, 0x158, 0x158);
*/
}

void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*    
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
*/
}

void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
/*
    // Barracks wall
    set_room_wall2 (360, 7, 0x1b, 0x1b, 0x52, 0x18b, 0x188, 0x185, 2);
    set_room_wall4 (935, 0xc, 0x1b, 0x184, 0x184, 0x190, 0x184);
*/
}

void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}

void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own)
{
}

void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, 0x01d,
         0x02d, 0x02d, 0x02c, 0x02c, 0x02b, 0x0, 0x0, 0x0);
}

void fill_column_gold(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019+rnd(5), 0x00a, 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019+rnd(5), 0x00a, 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x026, 0x001+rnd(3), 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x02a, 0x030, 0x030, 0x030, 0x030, 0, 0, 0);
}

void fill_column_wall_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     //fill_column_rec_sim(clm_rec, 0, 0x01b,
     //    0x019+rnd(5), 0x00a, 0x001+rnd(3), 0x001+rnd(3), 0x004, 0, 0, 0);
     // Simplified - why wasting CLM entires if only top is visible anyway?
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x00a, 0x002, 0x002, 0x004, 0, 0, 0);
     //The "top" is owner-dependent
     if (owner<PLAYER_UNSET)
         clm_rec->c[4]=0x043+owner;
}

void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x098, 0x097, 0x096, 0x095, 0, 0, 0);
}

void fill_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x053, 0x050, 0x050, 0x04e, 0, 0, 0);
}

void fill_column_wall_redsmbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x052, 0x048+rnd(3), 0x048+rnd(3), 0x04d, 0, 0, 0);
}

void fill_column_wall_redsmbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x051, 0x04f, 0x04f, 0x04c, 0, 0, 0);
}

//TODO: drape color changes with player!!! (unowned=no drape??)
void fill_column_wall_drapebrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x0a0, 0, 0, 0);
}

void fill_column_wall_drapebrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x0a1, 0, 0, 0);
}

void fill_column_wall_drapebrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x0a2, 0, 0, 0);
}

void fill_column_wall_twinsbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x195, 0x066, 0x063, 0x060, 0, 0, 0);
}

void fill_column_wall_twinsbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x196, 0x067, 0x064, 0x061, 0, 0, 0);
}

void fill_column_wall_twinsbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x197, 0x068, 0x065, 0x062, 0, 0, 0);
}

void fill_column_wall_womanbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x05d, 0x05a, 0x057, 0x054, 0, 0, 0);
}

void fill_column_wall_womanbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x05e, 0x05b, 0x058, 0x055, 0, 0, 0);
}

void fill_column_wall_womanbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x05f, 0x05c, 0x059, 0x056, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x072, 0x06f, 0x06c, 0x069, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x073, 0x070, 0x06d, 0x06a, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x074, 0x071, 0x06e, 0x06b, 0, 0, 0);
}

void fill_column_path(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0cf,
         0x019 +rnd(5), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_path_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0cf,
         0x02f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x0c6, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
     //The "top" is owner-dependent
     if (owner==PLAYER_GOOD)
         clm_rec->c[4]=0x0c7;
     else
     if (owner<PLAYER_UNSET)
         clm_rec->c[4]=0x0c0+owner;
}

void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x07e +rnd(3), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x093, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_lava(struct COLUMN_REC *clm_rec, unsigned char owner)
{ //Simplicity of this one scares me..
     fill_column_rec_sim(clm_rec, 0, 0x222 +rnd(2),
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_water(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0221,
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

