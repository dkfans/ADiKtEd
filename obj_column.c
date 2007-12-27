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

static void (*custom_columns_gen [CUST_CLM_GEN_MAX_INDEX+1])(struct COLUMN_REC *clm_recs[9],
        unsigned char *,unsigned char *, unsigned char **)={
     create_columns_slb_rock,create_columns_slb_gold,              //00
     create_columns_slb_fulldirt,create_columns_slb_earth,
     create_columns_slb_torchdirt,
     create_columns_slb_walldrape,create_columns_slb_walltorch,
     create_columns_slb_wallwtwins,create_columns_slb_wallwwoman,
     create_columns_slb_wallpairshr,
     create_columns_slb_path,create_columns_slb_claimed,           //0a
     create_columns_slb_lava,create_columns_slb_water,
     create_columns_slb_portal,create_columns_slb_treasure,
     create_columns_slb_library,create_columns_slb_prison,
     create_columns_slb_torture,create_columns_slb_training,
     create_columns_slb_dungheart,create_columns_slb_workshop,     //14
     create_columns_slb_scavenger,create_columns_slb_temple,
     create_columns_slb_graveyard,create_columns_slb_hatchery,
     create_columns_slb_lair,create_columns_slb_barracks,          //1a
     create_columns_slb_doorwood,create_columns_slb_doorbrace,
     create_columns_slb_dooriron,create_columns_slb_doormagic,
     create_columns_slb_bridge,create_columns_slb_gems,            //20
     create_columns_slb_guardpost,
     };

const char *custom_columns_fullnames[CUST_CLM_GEN_MAX_INDEX+1]={
     "Standard Rock","Standard Gold",              //00
     "Unaffected earth","Standard earth",
     SLB_TORCHDIRT_LTEXT,
     SLB_WALLDRAPE_LTEXT,SLB_WALLTORCH_LTEXT,
     SLB_WALLWTWINS_LTEXT,SLB_WALLWWOMAN_LTEXT,
     SLB_WALLPAIRSHR_LTEXT,
     "Standard path",SLB_CLAIMED_LTEXT,           //0A
     "Standard lava","Standard water",
     SLB_PORTAL_LTEXT,SLB_TREASURE_LTEXT,
     SLB_LIBRARY_LTEXT,SLB_PRISON_LTEXT,
     SLB_TORTURE_LTEXT,SLB_TRAINING_LTEXT,
     SLB_DUNGHEART_LTEXT,SLB_WORKSHOP_LTEXT,     //14
     SLB_SCAVENGER_LTEXT,SLB_TEMPLE_LTEXT,
     SLB_GRAVEYARD_LTEXT,SLB_HATCHERY_LTEXT,
     SLB_LAIR_LTEXT,SLB_BARRACKS_LTEXT,          //1A
     SLB_DOORWOOD_LTEXT,SLB_DOORBRACE_LTEXT,
     SLB_DOORIRON_LTEXT,SLB_DOORMAGIC_LTEXT,
     SLB_BRIDGE_LTEXT,"Standard gems",            //20
     SLB_GUARDPOST_LTEXT,
     };

//const unsigned short wib_columns_static[]={
//};

const unsigned short wib_columns_animate[]={
      0x0221, //water
      0x222, 0x223, //lava
};

const unsigned short dir_rot_000[]={
      IDIR_NW,IDIR_NORTH,IDIR_NE,IDIR_WEST,IDIR_CENTR,IDIR_EAST,
      IDIR_SW,IDIR_SOUTH,IDIR_SE, };

const unsigned short dir_rot_090[]={
      IDIR_NE,IDIR_EAST,IDIR_SE,IDIR_NORTH,IDIR_CENTR,IDIR_SOUTH,
      IDIR_NW,IDIR_WEST,IDIR_SW, };

const unsigned short dir_rot_180[]={
      IDIR_SE,IDIR_SOUTH,IDIR_SW,IDIR_EAST,IDIR_CENTR,IDIR_WEST,
      IDIR_NE,IDIR_NORTH,IDIR_NW, };

const unsigned short dir_rot_270[]={
      IDIR_SW,IDIR_WEST,IDIR_NW,IDIR_SOUTH,IDIR_CENTR,IDIR_NORTH,
      IDIR_SE,IDIR_EAST,IDIR_NE, };

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
void fill_column_rec_autosh(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int base, 
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
{
    fill_column_rec(clm_rec, use, permanent, lintel, 0, 0, base, 0,
               c0, c1, c2, c3, c4, c5, c6, c7);
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Fills a given COLUMN_REC entry, requires all parameters stored in CLM entry.
 */
void fill_column_rec(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int height,
        const unsigned int solid, const int base, const int orientation,
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
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

void get_clm_entry(struct COLUMN_REC *clm_rec, const unsigned char *clmentry)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    clm_rec->use=clmentry[0]+(clmentry[1]<<8);
    clm_rec->permanent=clmentry[2]&1;
    clm_rec->lintel=(clmentry[2]>>1)&7;
    clm_rec->height=(clmentry[2]>>4)&15;
    clm_rec->solid=clmentry[3]+(clmentry[4]<<8);
    clm_rec->base=clmentry[5]+(clmentry[6]<<8);
    clm_rec->orientation=clmentry[7];
    clm_rec->c[0]=clmentry[8]+(clmentry[9]<<8);
    clm_rec->c[1]=clmentry[10]+(clmentry[11]<<8);
    clm_rec->c[2]=clmentry[12]+(clmentry[13]<<8);
    clm_rec->c[3]=clmentry[14]+(clmentry[15]<<8);
    clm_rec->c[4]=clmentry[16]+(clmentry[17]<<8);
    clm_rec->c[5]=clmentry[18]+(clmentry[19]<<8);
    clm_rec->c[6]=clmentry[20]+(clmentry[21]<<8);
    clm_rec->c[7]=clmentry[22]+(clmentry[23]<<8);
}

/*
 * Determines if the two CLM entries looks identically - COLUMN_REC version
 */
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2)
{
  if (clm_rec1->height != clm_rec2->height)
    return false;
  if (clm_rec1->solid != clm_rec2->solid)
    return false;
  if (clm_rec1->lintel != clm_rec2->lintel)
    return false;
  unsigned short height=clm_rec1->height;
  unsigned int solid=clm_rec1->solid;
  if (clm_rec1->base!=clm_rec2->base)
    return false;
  if ((solid&1))
    if (clm_rec1->c[0]!=clm_rec2->c[0])
      return false;
  if ((solid&2))
    if (clm_rec1->c[1]!=clm_rec2->c[1])
      return false;
  if ((solid&4))
    if (clm_rec1->c[2]!=clm_rec2->c[2])
      return false;
  if ((solid&8))
    if (clm_rec1->c[3]!=clm_rec2->c[3])
      return false;
  if ((solid&16))
    if (clm_rec1->c[4]!=clm_rec2->c[4])
      return false;
  if ((solid&32))
    if (clm_rec1->c[5]!=clm_rec2->c[5])
      return false;
  if ((solid&64))
    if (clm_rec1->c[6]!=clm_rec2->c[6])
      return false;
  if ((solid&128))
    if (clm_rec1->c[7]!=clm_rec2->c[7])
      return false;
  return true;
}

/*
 * Determines if the two CLM entries looks identically - basic version
 */
short compare_column_entries(const unsigned char *clmentry1, const unsigned char *clmentry2)
{
   //comparing 'lintel' and 'height', but not 'permanent'
    if ((clmentry1[2]&0x0fe)!=(clmentry2[2]&0x0fe))
        return false;
    // Comparing 'solid' and 'base'
    if (memcmp(clmentry1+3,clmentry2+3,4)!=0)
        return false;
    // Comparing cube values
    if (memcmp(clmentry1+8,clmentry2+8,15)!=0)
        return false;
    return true;
}

unsigned short compute_clm_rec_height(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    //First test - 3 free bottom columns makes height at the bottom.
    if ((clm_rec->c[0]==0) && (clm_rec->c[1]==0) && (clm_rec->c[2]==0))
      return 0;
    if ((clm_rec->c[1]==0) && (clm_rec->c[2]==0) && (clm_rec->c[3]==0))
      return 1;
    if ((clm_rec->c[2]==0) && (clm_rec->c[3]==0) && (clm_rec->c[4]==0))
      return 2;
    if ((clm_rec->c[3]==0) && (clm_rec->c[4]==0) && (clm_rec->c[5]==0))
      return 3;
    //Door when they are opening/open
    if ((clm_rec->c[0]==0x094) && (clm_rec->c[1]==0x0))
      return 1;
    //Strange iron door from map 67 (that's probably wrong entry - but the warn message for it annoys me)
    if ((clm_rec->base==0x0b1) && (clm_rec->c[0]==0x0) && (clm_rec->c[1]==0x17c))
      return 0;
    if ((clm_rec->c[7]!=0))
      return 8;
    if ((clm_rec->c[6]!=0))
      return 7;
    if ((clm_rec->c[5]!=0))
      return 6;
    if ((clm_rec->c[4]!=0))
      return 5;
    if ((clm_rec->c[3]!=0))
      return 4;
    if ((clm_rec->c[2]!=0))
      return 3;
    if ((clm_rec->c[1]!=0))
      return 2;
    if ((clm_rec->c[0]!=0))
      return 1;
    return 0;
}

unsigned short compute_clm_rec_solid(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    unsigned short solid=0;
    if ((clm_rec->c[7]!=0))
      solid|=128;
    if ((clm_rec->c[6]!=0))
      solid|=64;
    if ((clm_rec->c[5]!=0))
      solid|=32;
    if ((clm_rec->c[4]!=0))
      solid|=16;
    if ((clm_rec->c[3]!=0))
      solid|=8;
    if ((clm_rec->c[2]!=0))
      solid|=4;
    if ((clm_rec->c[1]!=0))
      solid|=2;
    if ((clm_rec->c[0]!=0))
      solid|=1;
    return solid;
}

unsigned int get_clm_entry_use(const unsigned char *clmentry)
{
    return (unsigned int)clmentry[0]+(clmentry[1]<<8);
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
 * Returns the value of "permanent".
 */
short get_clm_entry_permanent(const unsigned char *clmentry)
{
    if (clmentry==NULL) return 0;
    short permanent=clmentry[2]&1;
    return permanent;
}

void set_clm_entry_permanent(unsigned char *clmentry,short nperm)
{
    if (clmentry==NULL) return;
    unsigned char val;
    val=clmentry[2] & 0xfe; //clear permanent bit
    clmentry[2]=(val)|(nperm!=0);
}

unsigned short get_clm_entry_height(const unsigned char *clmentry)
{
    return (unsigned short)(clmentry[2]>>4)&15;
}

unsigned short get_clm_entry_solid(const unsigned char *clmentry)
{
    return (unsigned short)clmentry[3]+(clmentry[4]<<8);
}

short clm_rec_copy(struct COLUMN_REC *dest_rec,const struct COLUMN_REC *src_rec)
{
    memcpy(dest_rec,src_rec,sizeof(struct COLUMN_REC));
    return true;
}

/*
 * Verifies values in the CLM entry.
 */
short clm_verify_entry(const unsigned char *clmentry, char *err_msg)
{
  struct COLUMN_REC *clm_rec;
  clm_rec=create_column_rec();
  get_clm_entry(clm_rec, clmentry);
  int i;
  for (i=0;i<8;i++)
    if (clm_rec->c[i]>CUBE_MAX_INDEX)
    {
        sprintf(err_msg,"Cube entry %d too large",i);
        return VERIF_WARN;
    }
  unsigned int prop_solid=compute_clm_rec_solid(clm_rec);
  if (clm_rec->solid != prop_solid)
  {
        sprintf(err_msg,"Solid property should be %u",prop_solid);
        return VERIF_WARN;
  }
  unsigned int prop_height=compute_clm_rec_height(clm_rec);
  if (clm_rec->height != prop_height)
  {
        sprintf(err_msg,"Height property should be %u",prop_height);
        return VERIF_WARN;
  }
  free_column_rec(clm_rec);
  return VERIF_OK;
}

/*
 * Returns if the column should be animated (contains water or lava, or similar)
 */
short column_wib_animate(unsigned int clm)
{
    int array_count=sizeof(wib_columns_animate)/sizeof(unsigned short);
    int idx=arr_ushort_pos(wib_columns_animate,clm,array_count);
    if (idx>=0) return true;
    return false;
}

/*
 * Returns a WIB file entry for given column; requires three additional columns:
 *          one from the north, one from the west and from NW corner.
 */
unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw)
{
    int i;
    // Preparing info if siblings are animated
    short anim_north=false;
    if (column_wib_animate(clm_rec_n->base))
      anim_north=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_n->solid&(1<<i))&&(column_wib_animate(clm_rec_n->c[i])))
        anim_north=true;
    }
    short anim_west=false;
    if (column_wib_animate(clm_rec_w->base))
      anim_west=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_w->solid&(1<<i))&&(column_wib_animate(clm_rec_w->c[i])))
        anim_west=true;
    }
    short anim_nw=false;
    if (column_wib_animate(clm_rec_nw->base))
      anim_nw=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_nw->solid&(1<<i))&&(column_wib_animate(clm_rec_nw->c[i])))
        anim_nw=true;
    }
    //And if this one alone should animate
    short anim_this=false;
    if (column_wib_animate(clm_rec->base))
      anim_this=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec->solid&(1<<i))&&(column_wib_animate(clm_rec->c[i])))
        anim_this=true;
    }
    //Getting to decision
    if (anim_this&&anim_west&&anim_north&&anim_nw)
      return COLUMN_WIB_ANIMATE;
    return COLUMN_WIB_SKEW;
}

/*
 * Returns custom column type name as text
 */
char *get_custom_column_fullname(unsigned short idx)
{
    int types_count=sizeof(custom_columns_fullnames)/sizeof(char *);
    if (idx<types_count)
      return (char *)custom_columns_fullnames[idx];
    else
      return "unknown(?!)";
}

/*
 * Executes custom column filling function with given index
 */
short fill_custom_column_data(unsigned short idx,struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    custom_columns_gen[idx](clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Fills up 9 CLM entries needed for given slab with specified surroundings.
 */
void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned char slab=surr_slb[IDIR_CENTR];
  switch (slab)
  {
    case SLAB_TYPE_ROCK:
      create_columns_slb_rock(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GOLD:
      create_columns_slb_gold(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_EARTH:
      create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORCHDIRT:
      create_columns_slb_torchdirt(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLDRAPE:
      create_columns_slb_walldrape(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLTORCH:
      create_columns_slb_walltorch(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWTWINS:
      create_columns_slb_wallwtwins(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWWOMAN:
      create_columns_slb_wallwwoman(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLPAIRSHR:
      create_columns_slb_wallpairshr(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PATH:
      create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_CLAIMED:
      create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAVA:
      create_columns_slb_lava(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WATER:
      create_columns_slb_water(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PORTAL:
      create_columns_slb_portal(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TREASURE:
      create_columns_slb_treasure(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LIBRARY:
      create_columns_slb_library(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_columns_slb_prison(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORTURE:
      create_columns_slb_torture(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TRAINING:
      create_columns_slb_training(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_columns_slb_dungheart(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_columns_slb_workshop(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_columns_slb_scavenger(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TEMPLE:
      create_columns_slb_temple(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_columns_slb_graveyard(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_HATCHERY:
      create_columns_slb_hatchery(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAIR:
      create_columns_slb_lair(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BARRACKS:
      create_columns_slb_barracks(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      create_columns_slb_doorwood(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      create_columns_slb_doorbrace(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      create_columns_slb_dooriron(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      create_columns_slb_doormagic(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BRIDGE:
      create_columns_slb_bridge(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GEMS:
      create_columns_slb_gems(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_columns_slb_guardpost(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    default: //on error, make path
      create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
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

/*
 * A helper function for using surr_tng array
 */
unsigned char *surr_tng_find(unsigned char **surr_tng,unsigned char type_idx)
{
  unsigned char *thing=surr_tng[IDIR_CENTR];
  if (thing!=NULL)
  {
    if (get_thing_type(thing)==type_idx)
      return thing;
  }
  int i;
  for (i=0;i<9;i++)
  {
    thing=surr_tng[i];
    if (thing!=NULL)
    {
      if (get_thing_type(thing)==type_idx)
        return thing;
    }
  }
  return NULL;
}

void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  //Central column
  fill_column_gold(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  //Note: can't use modify_liquid_surrounding() because more than one cube changes near water
  //corner columns
  int i;
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER) || (surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA) || (surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
  }
  // Remaining edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
  //Switch (remove) corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Creates earth which are all filled with dirt, always.
 * May be used for making earth and torchdirt columns.
 */
void create_columns_slb_fulldirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Center is always earth
  fill_column_earth(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);

  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  //Note: can't use modify_liquid_surrounding() - two cubes changes
  //Now corner slabs
  for (i=0;i<4;i++)
  {
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      {
         fill_column_earth_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      {
         fill_column_earth_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      {
         fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      }
  }
  // And the middle ones
  for (i=0;i<4;i++)
  {
      if (surr_slb[dir_a[i]]==SLAB_TYPE_WATER)
         fill_column_earth_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
      if (surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)
         fill_column_earth_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
         fill_column_earth(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Make the standard, 9x9 columns filled with dirt
  create_columns_slb_fulldirt(clm_recs,surr_slb,surr_own,surr_tng);
  //Finally - switch corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  //This is identical to standard dirt
  create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
  //But one of the c[3] entries is replaced with torch-one
  int i;
  short has_torches=false;
  for (i=0;i<4;i++)
  {
// old way - without things (pending delete, but first we must have auto-adding things)
//      if (slab_is_room(surr_slb[dir_a[i]])||slab_is_space(surr_slb[dir_a[i]]))
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        //Add the torch plate at same height where the torch thing is
        unsigned short height=get_thing_tilepos_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x018;
        has_torches=true;
    }
  }
  if (!has_torches)
    modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Creates wall with red brick inside. Returns where are whole brick walls
 */
void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
//TODO: near water and near lava support
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  //Filling matrix
  for (i=0;i<9;i++)
    allow_relief[i]=true;
  //We will use the NW, NE, SW, SE values of the matrix to help making brick
  // with shadow
  //Central slab identifies the player
  fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  //Corner slabs - white brick, small red brick or clear earth
  for (i=0;i<4;i++)
  {
    // If we're surrounded by our wall, and there is something short also - red brick
    if (((slab_is_wall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
       &&((slab_is_short(surr_slb[dir_b[i]]))||(!surrnd_not_enemy(surr_own,dir_b[i]))))
       ||((slab_is_wall(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
       &&((slab_is_short(surr_slb[dir_a[i]]))||(!surrnd_not_enemy(surr_own,dir_a[i])))))
    {
        fill_column_wall_redsmbrick_a(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    } else
    // if we're surrouned with unowned tall thing on _both_ sides, fill with dirt
    if ((slab_is_tall_unclmabl(surr_slb[dir_a[i]])&&slab_is_tall_unclmabl(surr_slb[dir_b[i]]))
      ||(((slab_is_tall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
      &&(slab_is_tall(surr_slb[dir_b[i]])&& surrnd_not_enemy(surr_own,dir_b[i])))))
    {
      fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
    } else
    // Finally - if there's something short, like ground - use the white brick
    {
      fill_column_wall_cobblestones(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
      allow_relief[dir_c[i]]=false;
    }
  }

  // And the remaining, middle slabs
  //These cannot be taken in simple 'for' loop, because different directions uses
  // different fill_column_wall_redsmbrick_* functions.
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
  //No frail columns switching on reinforced wall, so we're done.
}

/*
 * Creates wall with small drape at top
 */
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
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
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  //This variable will help in placing the bas-relief or drape;
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
    // Torch plate
  int i;
  for (i=0;i<4;i++)
  {
// the old way - delete when automake tng is done
//  if (slab_is_room(surr_slb[IDIR_SOUTH])||slab_is_space(surr_slb[IDIR_SOUTH]))
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        //Add the torch plate at same height where the torch thing is
        unsigned short height=get_thing_tilepos_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x077;
    }
  }

}

void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
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
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
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
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //This variable will help in placing the bas-relief or drape;
  //only four of its values will be used
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
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
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};

  //Center
  fill_column_claimedgnd_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_rec_sim(clm_recs[dir_c[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i])&&
        slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x07e +rnd(3);
    } else
    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        }
    } else
    if (slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x081;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x083;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x086;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x088;
          break;
        }
    }
  }

  //And the middle columns
  for (i=0;i<4;i++)
  {
    fill_column_rec_sim(clm_recs[dir_a[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    {
       clm_recs[dir_a[i]]->c[0]=0x07e +rnd(3);
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x087;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x084;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x082;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x085;
           break;
        }
    }
  }
  // Water and lava can beat all previous conditions
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x094, 0x093);
}

void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_lava(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_water(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //These are, in official editor, same for every surrounding, even near water
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_gem(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  //Switch corner columns near lava,water,...
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_portal_floor,create_columns_slb_portal_edge,
        create_columns_slb_portal_corner,create_columns_slb_portal_inside,create_columns_slb_portal_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_treasure_floor,create_columns_slb_treasure_edge,
       create_columns_slb_treasure_corner,create_columns_slb_treasure_inside,create_columns_slb_treasure_floor,
       clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_library_floor,create_columns_slb_library_edge,
        create_columns_slb_library_corner,create_columns_slb_library_inside,create_columns_slb_library_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_prison_floor,create_columns_slb_prison_edge,
        create_columns_slb_prison_corner,create_columns_slb_prison_inside,create_columns_slb_prison_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_torture_floor,create_columns_slb_torture_edge,
        create_columns_slb_torture_corner,create_columns_slb_torture_inside,create_columns_slb_torture_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_training_floor,create_columns_slb_training_edge,
        create_columns_slb_training_corner,create_columns_slb_training_inside,create_columns_slb_training_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    //Note: the 'floor' function is different in DK, and floor can change if user
    // force it to update in the game.
    create_columns_slb_room(create_columns_slb_dungheart_nearinsd,create_columns_slb_dungheart_edge,
        create_columns_slb_dungheart_corner,create_columns_slb_dungheart_inside,create_columns_slb_dungheart_nearinsd,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_workshop_floor,create_columns_slb_workshop_edge,
        create_columns_slb_workshop_corner,create_columns_slb_workshop_inside,create_columns_slb_workshop_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_scavenger_floor,create_columns_slb_scavenger_edge,
        create_columns_slb_scavenger_corner,create_columns_slb_scavenger_inside,create_columns_slb_scavenger_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_temple_floor,create_columns_slb_temple_edge,
        create_columns_slb_temple_corner,create_columns_slb_temple_inside,create_columns_slb_temple_edge,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_graveyard_floor,create_columns_slb_graveyard_edge,
        create_columns_slb_graveyard_corner,create_columns_slb_graveyard_inside,create_columns_slb_graveyard_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_hatchery_floor,create_columns_slb_hatchery_edge,
        create_columns_slb_hatchery_corner,create_columns_slb_hatchery_inside,create_columns_slb_hatchery_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_lair_floor,create_columns_slb_lair_edge,
        create_columns_slb_lair_corner,create_columns_slb_lair_inside,create_columns_slb_lair_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_barracks_floor,create_columns_slb_barracks_edge,
        create_columns_slb_barracks_corner,create_columns_slb_barracks_inside,create_columns_slb_barracks_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_door_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    fill_column_rec_sim(clm_recs[IDIR_NW],    0, 0x01b,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NORTH], 0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NE],    0, 0x0b5,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_WEST],  0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_CENTR], 0, 0x0b1,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_EAST],  0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SW],    0, 0x0b8,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SOUTH], 0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SE],    0, 0x01b,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorwood_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorwood_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doorwood_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x16b;
  clm_rec->c[pos+1]=0x168;
  clm_rec->c[pos+2]=0x165;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorbrace_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorbrace_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doorbrace_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x172;
  clm_rec->c[pos+1]=0x16f;
  clm_rec->c[pos+2]=0x16c;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x173;
  clm_rec->c[pos+1]=0x170;
  clm_rec->c[pos+2]=0x16d;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x174;
  clm_rec->c[pos+1]=0x171;
  clm_rec->c[pos+2]=0x16e;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_dooriron_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_dooriron_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_dooriron_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17b;
  clm_rec->c[pos+1]=0x178;
  clm_rec->c[pos+2]=0x175;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17c;
  clm_rec->c[pos+1]=0x179;
  clm_rec->c[pos+2]=0x176;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17d;
  clm_rec->c[pos+1]=0x17a;
  clm_rec->c[pos+2]=0x177;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doormagic_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doormagic_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void place_column_doormagic_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12e;
  clm_rec->c[pos+1]=0x12b;
  clm_rec->c[pos+2]=0x128;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12d;
  clm_rec->c[pos+1]=0x12a;
  clm_rec->c[pos+2]=0x127;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=1;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12c;
  clm_rec->c[pos+1]=0x129;
  clm_rec->c[pos+2]=0x126;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Function modifies a column made of earth,gold or similar material
 * for gaps when near unclaimed, short terrain. The corner
 * column are sometimes changed to the surrounding material.
 */
void modify_frail_columns(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
    const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
    const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
    //This decreases probability of column change if there are no adjacent tall slabs
    int base_prob=33;
    if (slab_is_tall_unclmabl(surr_slb[IDIR_WEST])||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH])||
       slab_is_tall_unclmabl(surr_slb[IDIR_EAST])||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
       base_prob=66;
    int i;
    //Let's take all corners at once, in a loop
    for (i=0;i<4;i++)
    {
      if (slab_is_short_unclmabl(surr_slb[dir_a[i]]) &&
          slab_is_short_unclmabl(surr_slb[dir_b[i]]))
      {
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_PATH)||(surr_slb[dir_b[i]]==SLAB_TYPE_PATH))
          {
            if (rnd(100)<base_prob)
              fill_column_path(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
          {
            if (rnd(100)<base_prob+33)
              fill_column_water(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
          {
            if (rnd(100)<base_prob+33)
              fill_column_lava(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          // My own mod - replacing part of gold/gems with dirt if it is near
          if ((surr_slb[IDIR_CENTR]!=SLAB_TYPE_EARTH)&&(surr_slb[IDIR_CENTR]!=SLAB_TYPE_TORCHDIRT)&&
             ((surr_slb[dir_a[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_a[i]]==SLAB_TYPE_TORCHDIRT))&&
             ((surr_slb[dir_b[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_b[i]]==SLAB_TYPE_TORCHDIRT)))
          {
            if (rnd(100)<base_prob+33)
              fill_column_earth(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          }
      }
    }
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Returns true if something was modified
 */
short modify_liquid_surrounding(struct COLUMN_REC *clm_recs[9],unsigned char *surr_slb,
        short liq_level,unsigned short water_cube,unsigned short lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  //initial tests
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  //corner columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_c[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_c[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  return result;
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Advanced version - the change is different for every direction.
 * (water_cube and lava_cube should point to 9-element matrix)
 * Returns true if something was modified
 */
short modify_liquid_surrounding_advncd(struct COLUMN_REC *clm_recs[9],
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const short liq_level,const unsigned short *water_cube,const unsigned short *lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //initial tests
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  //corner columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_c[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_c[i]];
       result=true;
    }
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube[dir_a[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube[dir_a[i]];
       result=true;
    }
  }
  return result;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * The direction is made in a way that allows access column at room corner in IDIR_SE.
 */
unsigned short *get_room_corner_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned char slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
  return (unsigned short *)dir_rot_000;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * It maps indices so that EAST is the "inside" of room.
 */
unsigned short *get_room_edge_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned char slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  return (unsigned short *)dir_rot_000;
}

void create_columns_slb_room(cr_clm_func cr_floor,cr_clm_func cr_edge,
        cr_clm_func cr_corner,cr_clm_func cr_inside,cr_clm_func cr_nearinsd,
        struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Checking if completely surrounded
  if ((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr))
  {
      if  ((surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
           (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
           (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
           (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr))
      {
          cr_inside(clm_recs,surr_slb,surr_own,surr_tng);
      } else
      {
          //The 'near inside' columns are usually same that floor,
          //but may differ for rooms with specific corners
          cr_nearinsd(clm_recs,surr_slb,surr_own,surr_tng);
      }
      return;
  }
  //If not completely, maybe we're surrounded from 3 sides (5 with corners)?
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)) ||
      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)) ||
      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)) ||
      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
  {
      cr_edge(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  //If still nothing, maybe we have same surround from two sides and 1 corner,
  // and another two are surely something else
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
//       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
//       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
//       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))) ||

      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
//       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      cr_corner(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  //If nothing found - just draw floor of this room
  cr_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_rec_sim(clm_recs[k*3+i], 0, 0x0f8,
           0x0ae +rnd(2), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0bb, 0x0ba);
}

void create_columns_slb_library_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  //Library egde is just its floor
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_library_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_library_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_library_bookcase_a(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_dungheart_floor(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_dungheart_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_NORTH]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SW]], surr_own[IDIR_CENTR]);
    fill_column_rec_sim(clm_recs[dir[IDIR_CENTR]], 0, 0x01b,
         0x080, 0x180, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_dungheart_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_dungheart_inside(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_dungheart_nearinsd(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
  //Raise the center a little bit
  place_column_univ_stair(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  int i;
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //corner columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)) ||
        ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)))
    {
      short i_op=(i+2)%4;
      if (((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)) ||
          ((surr_slb[dir_b[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)))
        place_column_dungheart_raise(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    }
  }
  //and the rest of columns
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
      short i_op=(i+2)%4;
      if ((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr))
        place_column_dungheart_raise(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    }
  }
}

void create_columns_slb_portal_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_portal_floor(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x13b, 0x15f);
}

void create_columns_slb_portal_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_portal_step(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_portal_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i;
  for (i=0;i<9;i++)
    fill_column_rec_sim(clm_recs[i], 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+i, 0);
  fill_column_portal_inside_cntr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_temple_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x075, 0x076);
}

void create_columns_slb_temple_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  const unsigned short dir_d[]={IDIR_SOUTH, IDIR_WEST, IDIR_NORTH, IDIR_EAST};
  const unsigned short dir_e[]={IDIR_EAST, IDIR_SOUTH, IDIR_WEST, IDIR_NORTH};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Temple edge is like its floor,
  //but it is higher by one cube from inside.
  create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        ((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr) ||
         (surr_slb[dir_e[i]]==slab)&&(surr_own[dir_e[i]]==ownr)))
      place_column_temple_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        ((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr)))
      place_column_temple_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_temple_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    place_column_temple_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_temple_corner(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_rec_sim(clm_recs[i], 0, 0x177,
          0x019 +rnd(5), 0x126 +i, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_hatchery_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_hatchery_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  //And the edge columns
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  //Liquid surrounding - lava surround is not as trivial as usually
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  for (i=0;i<9;i++)
    water_cube[i]=0x159;
  lava_cube[IDIR_NW]=0x15e;
  lava_cube[IDIR_NE]=0x15d;
  lava_cube[IDIR_SW]=0x15c;
  lava_cube[IDIR_SE]=0x15e;
  lava_cube[IDIR_NORTH]=0x15d;
  lava_cube[IDIR_EAST]=0x15d;
  lava_cube[IDIR_SOUTH]=0x15c;
  lava_cube[IDIR_WEST]=rnd(2)?0x15c:0x13c;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_hatchery_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_hatchery_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_hatchery_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_hatchery_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_lair_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_lair_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x1b1;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x01e;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x020;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x023;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x025;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x1b1;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x024;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x021;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x01f;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x022;
           break;
        }
    }
  }
  // Liquid surrounding is not supported for the lair ground,
  // so we've done
}

void create_columns_slb_lair_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_lair_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_graveyard_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_graveyard_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x142;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW://not west
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SW://not south
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        case IDIR_NE://not north
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_SE://not east
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x143;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x145;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x148;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x14a;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x142;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x149;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x146;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x144;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x147;
           break;
        }
    }
  }
  // Liquid surrounding is not supported for the graveyard ground,
  // so we've done
}

void create_columns_slb_graveyard_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_graveyard_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_barracks_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
    {
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
      place_column_barracks_raise(clm_recs[i], surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_training_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_training_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0fe, 0x0fd);
}

void create_columns_slb_training_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_training_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_training_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_treasure_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_treasure_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_treasure_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_treasure_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_workshop_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x109, 0x108);
}

void create_columns_slb_workshop_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_workshop_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_workshop_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  //On central slabs, we can use lava and water cubes too - they have same top.
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     clm_recs[k*3+i]->c[0]=0x107 +rnd(3);
   }
}

void create_columns_slb_scavenger_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_scavenger_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  fill_column_scavenger_inside_cntr(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x11b, 0x11a);
}

void create_columns_slb_scavenger_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_scavenger_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_scavenger_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Center
  fill_column_prison_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  //Corners
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //Surrounded by our area
       clm_recs[dir_c[i]]->c[0]=0x0c8;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { //our on side A only
        switch (dir_c[i])
        {
        case IDIR_NW://West only, not north
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_NE://North only, not east
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        case IDIR_SW://South only, not west
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SE://East only, not south
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { //our on side B only
        switch (dir_c[i])
        {
        case IDIR_NW://not west
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SW://not south
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        case IDIR_NE://not north
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_SE://not east
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        }
    } else
    { //Surrounded by not our area
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x0c9;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x0cb;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x0ce;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x0d0;
          break;
        }
    }
  }
  //And the middle columns
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x0c8;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x0cf;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x0cc;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x0ca;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x0cd;
           break;
        }
    }
  }
  // Liquid surrounding - quite hard here
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  water_cube[IDIR_NW]=0x0d9;
  water_cube[IDIR_NE]=0x0db;
  water_cube[IDIR_SW]=0x0de;
  water_cube[IDIR_SE]=0x0e0;
  water_cube[IDIR_NORTH]=0x0da;
  water_cube[IDIR_EAST]= 0x0dd;
  water_cube[IDIR_SOUTH]=0x0df;
  water_cube[IDIR_WEST]= 0x0dc;
  lava_cube[IDIR_NW]=0x0d1;
  lava_cube[IDIR_NE]=0x0d3;
  lava_cube[IDIR_SW]=0x0d6;
  lava_cube[IDIR_SE]=0x0d8;
  lava_cube[IDIR_NORTH]=0x0d2;
  lava_cube[IDIR_EAST]= 0x0d5;
  lava_cube[IDIR_SOUTH]=0x0d7;
  lava_cube[IDIR_WEST]= 0x0d4;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_prison_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_prison_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_torture_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=0x0e7;
}

void create_columns_slb_torture_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_inside(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=0x0e7;
}

void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    fill_column_guardpost_floor_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_bridge_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, 0x01d,
         0x02d, 0x02d, 0x02c, 0x02c, 0x02b, 0x0, 0x0, 0x0);
}

void fill_column_gold(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019+rnd(5), 0x00a, 0x031+rnd(3), 0x031+rnd(3), 0x031+rnd(3), 0, 0, 0);
}

void fill_column_gold_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x026, 0x031+rnd(3), 0x031+rnd(3), 0x031+rnd(3), 0x031+rnd(3), 0, 0, 0);
}

void fill_column_gold_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x02a, 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0, 0, 0);
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

/*
 * Column of grey stones, larger than brick.
 */
void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x098, 0x097, 0x096, 0x095, 0, 0, 0);
}

/*
 * Column of grey stones with some markings on.
 */
void place_column_wall_cobblestones_mk(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x03e;
  clm_rec->c[pos+1]=0x039;
  clm_rec->c[pos+2]=0x03a;
  clm_rec->c[pos+3]=0x041;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
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

void fill_column_wall_drapebrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    //we could use fill_column_wall_redsmbrick_a to get this better,
    // but this way we're not losing CLM entries
    fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a0; break;
    case PLAYER1:    clm_rec->c[4]=0x19a; break;
    case PLAYER2:    clm_rec->c[4]=0x19d; break;
    case PLAYER3:    clm_rec->c[4]=0x1a0; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a3; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
}

void fill_column_wall_drapebrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    //we could use fill_column_wall_redsmbrick_a to get this better,
    // but this way we're not losing CLM entries
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a1; break;
  case PLAYER1:    clm_rec->c[4]=0x19b; break;
  case PLAYER2:    clm_rec->c[4]=0x19e; break;
  case PLAYER3:    clm_rec->c[4]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a4; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
}

void fill_column_wall_drapebrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a2; break;
    case PLAYER1:    clm_rec->c[4]=0x19c; break;
    case PLAYER2:    clm_rec->c[4]=0x19f; break;
    case PLAYER3:    clm_rec->c[4]=0x1a2; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a5; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
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

/*
 * Claimed ground (not claimed wall)
 */
void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x0c6, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
     //The "top" is owner-dependent
     if (owner==PLAYER_GOOD)
         clm_rec->c[0]=0x0c7;
     else
     if (owner<PLAYER_UNSET)
         clm_rec->c[0]=0x0c0+owner;
}

void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x07e +rnd(3), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
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

void fill_column_gem(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x00a, 0x1b9 +rnd(3), 0x1b9 +rnd(3), 0x1b9 +rnd(3), 0, 0, 0);
}

void fill_column_library_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x0b9, 0x0b8, 0x0b7, 0x0b6, 0, 0, 0);
}

void place_column_library_bookcase_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=0x01b;
  clm_rec->c[1]=0x14b;
  clm_rec->c[2]=0x14d;
  clm_rec->c[3]=0x14b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=0x01b;
  clm_rec->c[1]=0x0b1;
  clm_rec->c[2]=0x0b4;
  clm_rec->c[3]=0x0b1;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  clm_rec->base=0x01b;
  clm_rec->c[1]=0x14c;
  clm_rec->c[2]=0x14e;
  clm_rec->c[3]=0x14c;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>1) pos=1;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=0x122;
  clm_rec->c[pos+2]=0x120;
  clm_rec->c[pos+3]=0x11d;
  clm_rec->c[pos+4]=0x120;
  clm_rec->c[pos+5]=0x11e;
  clm_rec->c[pos+6]=0x11d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_temple_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x177,
         0x019 +rnd(5), 0x123, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_temple_corner(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=clm_rec->height;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_dungheart_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x09f , 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_dungheart_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x01b,
        0x080 , 0x052, 0x048, 0x049, 0x04a, 0x049, 0x17f, 0x1aa);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[7]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[7]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[7]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[7]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[7]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[7]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_dungheart_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x052, 0x09f, 0x0, 0x0, 0, 0, 0);
}


void place_column_univ_stair(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=clm_rec->height;
  if (pos>6) pos=6;
  clm_rec->c[pos+1]=clm_rec->c[pos+0];
  clm_rec->c[pos+0]=0x019 +rnd(5);
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dungheart_raise(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=clm_rec->height;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x052;
  clm_rec->c[pos+1]=0x04d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_portal_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x138, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x019 +rnd(5), 0x00a, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x01d,
        0x019 +rnd(5), 0x00a, 0x00f, 0x00d, 0x00d, 0x00c, 0x00e, 0x0);
}

void fill_column_portal_step(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01c+rnd(2),
         0x00a, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00e, 0);
}

void fill_column_portal_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+4, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[6]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[6]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[6]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[6]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[6]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[6]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_hatchery_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b+rnd(5),
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_edge(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x15a, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x15b, 0x158, 0x158, 0x158, 0x158, 0x159, 0, 0);
}

void fill_column_lair_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x1b1, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_graveyard_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01c,
         0x142, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_barracks_raise(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  unsigned int pos=clm_rec->height;
  if (pos>7) pos=7;
  if (pos<1) pos=1;
  clm_rec->c[pos-1]=0x183;
  clm_rec->c[pos+0]=0x194;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_barracks_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x01b,
         0x181, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_barracks_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x01b,
        0x01a, 0x184, 0x184, 0x18f, 0x18e, 0x184, 0x0, 0x0);
}

void fill_column_training_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x13e,
         0x0fc, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_training_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x13e,
        0x0fc, 0x0fb, 0x0fa, 0x0f9, 0x0f9, 0x0f1, 0x0, 0x0);
}

void fill_column_treasure_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x06a,
         0x09f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_treasure_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x06a,
        0x09f, 0x03e, 0x039, 0x03a, 0x039, 0x041, 0x0, 0x0);
}

void fill_column_workshop_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x15a,
         0x107, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_workshop_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x01b,
        0x01c, 0x10e, 0x10d, 0x10c, 0x10b, 0x0, 0x0, 0x0);
}


void fill_column_scavenger_floor(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x119, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_scavenger_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0f8,
        0x019, 0x118, 0x117, 0x116, 0x115, 0x0, 0x0, 0x0);
}

void fill_column_scavenger_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x11c, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_prison_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x118,
         0x0c8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), 0x1b4, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), 0x1b5, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), 0x1b6, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_bridge_inside(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x1f3,
         0x1b8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

