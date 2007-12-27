/*
 * obj_slabs.c
 *
 * Functions for maintaining slabs. Also includes some routines for owner
 * identification and WBI files.
 *
 */

#include "obj_slabs.h"

#include "lev_data.h"
#include "globals.h"
#include "obj_things.h"

const char *owner_names[]={"Keeper 0 (Human)", "Keeper 1", "Keeper 2",
    "Keeper 3", "Heroes", "Unclaimed"};

const unsigned char owners_list[]={
    PLAYER0, PLAYER1, PLAYER2, PLAYER3,
    PLAYER_GOOD, PLAYER_UNSET
    };

const unsigned short slabs_space[]={
    SLAB_TYPE_PATH, SLAB_TYPE_CLAIMED, SLAB_TYPE_LAVA,
    SLAB_TYPE_WATER
};

const unsigned short slabs_short_unclmabl[]={
    SLAB_TYPE_PATH, SLAB_TYPE_LAVA, SLAB_TYPE_WATER
};

const unsigned short slabs_wealth[]={
    SLAB_TYPE_GOLD, SLAB_TYPE_GEMS,
};

const unsigned short slabs_tall_unclmabl[]={
    SLAB_TYPE_ROCK, SLAB_TYPE_GOLD, SLAB_TYPE_EARTH,
    SLAB_TYPE_TORCHDIRT, SLAB_TYPE_GEMS
};

const unsigned short slabs_walls[]={
    SLAB_TYPE_WALLDRAPE, SLAB_TYPE_WALLTORCH, SLAB_TYPE_WALLWTWINS,
    SLAB_TYPE_WALLWWOMAN, SLAB_TYPE_WALLPAIRSHR
};

const unsigned short slabs_rooms[]={
    SLAB_TYPE_PORTAL, SLAB_TYPE_TREASURE, SLAB_TYPE_LIBRARY,
    SLAB_TYPE_PRISONCASE, SLAB_TYPE_TORTURE, SLAB_TYPE_TRAINING,
    SLAB_TYPE_DUNGHEART, SLAB_TYPE_WORKSHOP, SLAB_TYPE_SCAVENGER,
    SLAB_TYPE_TEMPLE, SLAB_TYPE_GRAVEYARD, SLAB_TYPE_HATCHERY,
    SLAB_TYPE_LAIR, SLAB_TYPE_BARRACKS, SLAB_TYPE_BRIDGE,
    SLAB_TYPE_GUARDPOST
};

const unsigned short slabs_doors[]={
    SLAB_TYPE_DOORWOOD1, SLAB_TYPE_DOORWOOD2,
    SLAB_TYPE_DOORBRACE1, SLAB_TYPE_DOORBRACE2,
    SLAB_TYPE_DOORIRON1, SLAB_TYPE_DOORIRON2,
    SLAB_TYPE_DOORMAGIC1, SLAB_TYPE_DOORMAGIC2
};

const char *all_slabs_fullnames[]={

     SLB_ROCK_LTEXT,SLB_GOLD_LTEXT,              //00
     SLB_EARTH_LTEXT,SLB_TORCHDIRT_LTEXT,
     SLB_WALLDRAPE_LTEXT,SLB_WALLTORCH_LTEXT,
     SLB_WALLWTWINS_LTEXT,SLB_WALLWWOMAN_LTEXT,
     SLB_WALLPAIRSHR_LTEXT,SLB_UNKN_LTEXT,        //08
     SLB_PATH_LTEXT,SLB_CLAIMED_LTEXT,           //0A
     SLB_LAVA_LTEXT,SLB_WATER_LTEXT,
     SLB_PORTAL_LTEXT,SLB_UNKN_LTEXT,
     SLB_TREASURE_LTEXT,SLB_UNKN_LTEXT,
     SLB_LIBRARY_LTEXT,SLB_UNKN_LTEXT,
     SLB_PRISON_LTEXT,SLB_UNKN_LTEXT,
     SLB_TORTURE_LTEXT,SLB_UNKN_LTEXT,
     SLB_TRAINING_LTEXT,SLB_UNKN_LTEXT,
     SLB_DUNGHEART_LTEXT,SLB_UNKN_LTEXT,
     SLB_WORKSHOP_LTEXT,SLB_UNKN_LTEXT,
     SLB_SCAVENGER_LTEXT,SLB_UNKN_LTEXT,
     SLB_TEMPLE_LTEXT,SLB_UNKN_LTEXT,
     SLB_GRAVEYARD_LTEXT,SLB_UNKN_LTEXT,
     SLB_HATCHERY_LTEXT,SLB_UNKN_LTEXT,
     SLB_LAIR_LTEXT,SLB_UNKN_LTEXT,
     SLB_BARRACKS_LTEXT,SLB_UNKN_LTEXT,        //28
     SLB_DOORWOOD_LTEXT,SLB_DOORWOOD_LTEXT,
     SLB_DOORBRACE_LTEXT,SLB_DOORBRACE_LTEXT,
     SLB_DOORIRON_LTEXT,SLB_DOORIRON_LTEXT,
     SLB_DOORMAGIC_LTEXT,SLB_DOORMAGIC_LTEXT, //2f
     SLB_UNKN_LTEXT,SLB_BRIDGE_LTEXT,
     SLB_GEMS_LTEXT,SLB_GUARDPOST_LTEXT,
     };

unsigned char get_owner_next(unsigned char plyr_idx)
{
  plyr_idx++;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return 0;
}

unsigned char get_owner_prev(unsigned char plyr_idx)
{
  plyr_idx--;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return PLAYERS_COUNT-1;
}

/*
 * Sets slabs which can't be claimed as unowned (PLAYER_UNSET)
 */
void update_slab_owners(struct LEVEL *lvl)
{
    int tx, ty;    
    for (ty=0; ty<MAP_SIZE_Y; ty++)
      for (tx=0; tx<MAP_SIZE_X; tx++)
      {
        unsigned char slb=get_tile_slab(lvl, tx, ty);
        if (!slab_is_clmabl(slb))
          set_tile_owner(lvl,tx,ty,PLAYER_UNSET);
      }
}

/*
 * Returns square owner name string
 */
char *get_owner_type_fullname(unsigned char own_idx)
{
     int types_count=sizeof(owner_names)/sizeof(char *);
     if (own_idx<types_count)
       return (char *)owner_names[own_idx];
     else
       return "unknown(?!)";
}

/*
 * Says whether a slab is surrounded by the same things
 */
short slab_is_central(struct LEVEL *lvl,int x,int y)
{
    // Can't be central if it is on map edge
    if ((x<=0) || (y<=0) || (x>=MAP_MAXINDEX_X) || (y>=MAP_MAXINDEX_Y))
      return false;
    //Preparing variables
    int i, j;
    //Sweeping and comparing slab type and owner
    for (i=x-1; i<=x+1; i++)
      for (j=y-1; j<=y+1; j++)
        if ((lvl->slb[x][y]!=lvl->slb[i][j])||(lvl->own[x][y]!=lvl->own[i][j]))
          return false;
    return true;
}

/*
 * Verifies slab types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short slabs_verify(struct LEVEL *lvl, char *err_msg)
{
  short result;
  int i,j;
  for (i=1; i < MAP_MAXINDEX_Y; i++)
    for (j=1; j < MAP_MAXINDEX_X; j++)
    {
      result=slab_verify_entry(lvl->slb[i][j],err_msg);
      if (result!=VERIF_OK)
        return result;
    }
  return VERIF_OK;
}


/*
 * Says how many slabs of given type are in 3x3 area surrounding given slab,
 * and including it; returns 0..9;
 */
int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned char slab_type)
{
    int amount=0;
    //Preparing variables
    int i, j;
    //Sweeping and comparing slab type and owner
    for (i=x-1; i<=x+1; i++)
      for (j=y-1; j<=y+1; j++)
      {
      if ((i>=0) && (j>=0) && (i<MAP_SIZE_X) && (j<MAP_SIZE_Y))
        if (lvl->slb[i][j]==slab_type)
          amount++;
      }
    return amount;
}

short slab_is_room(unsigned char slab_type)
{
     int array_count=sizeof(slabs_rooms)/sizeof(unsigned short);
    //All rooms are listed in slabs_rooms array
    int idx=arr_ushort_pos(slabs_rooms,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_door(unsigned char slab_type)
{
     int array_count=sizeof(slabs_doors)/sizeof(unsigned short);
    //All doors are listed in slabs_doors array
    int idx=arr_ushort_pos(slabs_doors,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_wall(unsigned char slab_type)
{
     int array_count=sizeof(slabs_walls)/sizeof(unsigned short);
    //All walls are listed in slabs_walls array
    int idx=arr_ushort_pos(slabs_walls,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_wealth(unsigned char slab_type)
{
     int array_count=sizeof(slabs_wealth)/sizeof(unsigned short);
    int idx=arr_ushort_pos(slabs_wealth,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_space(unsigned char slab_type)
{
     int array_count=sizeof(slabs_space)/sizeof(unsigned short);
    //All such things are listed in slabs_space array
    int idx=arr_ushort_pos(slabs_space,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_tall_unclmabl(unsigned char slab_type)
{
     int array_count=sizeof(slabs_tall_unclmabl)/sizeof(unsigned short);
    //All such things are listed in slabs_tall_unclmabl array
    int idx=arr_ushort_pos(slabs_tall_unclmabl,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_short_unclmabl(unsigned char slab_type)
{
     int array_count=sizeof(slabs_short_unclmabl)/sizeof(unsigned short);
    //All such things are listed in slabs_space array
    int idx=arr_ushort_pos(slabs_short_unclmabl,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_short(unsigned char slab_type)
{
    //Rooms are short
    if (slab_is_room(slab_type)) return true;
    //But there are also other short things
    if (slab_is_space(slab_type)) return true;
    return false;
}

short slab_is_tall(unsigned char slab_type)
{
    if (slab_is_tall_unclmabl(slab_type)) return true;
    if (slab_is_wall(slab_type)) return true;
    return false;
}

short slab_is_short_clmabl(unsigned char slab_type)
{
    //Rooms are short and usually claimed
    if (slab_is_room(slab_type)) return true;
    //But there are also other claimed things
    if (slab_is_door(slab_type)) return true;
    if (slab_type==SLAB_TYPE_CLAIMED) return true;
    return false;
}

short slab_is_claimedgnd(unsigned char slab_type)
{
    if (slab_is_door(slab_type)) return true;
    if (slab_type==SLAB_TYPE_CLAIMED) return true;
    return false;
}

short slab_is_clmabl(unsigned char slab_type)
{
    if (slab_is_short_unclmabl(slab_type)) return false;
    if (slab_is_tall_unclmabl(slab_type)) return false;
    return true;
}

short slab_needs_adjacent_torch(unsigned char slab_type)
{
    if (slab_type==SLAB_TYPE_TORCHDIRT) return true;
    if (slab_type==SLAB_TYPE_WALLTORCH) return true;
    return false;
}

/*
 * Draws a smear of selected slab on the map
 */
void slab_draw_smear(struct LEVEL *lvl,int startx,int starty,int startr,
                     int endx,int endy,int endr,int bend,unsigned char slab_type)
{
    int dist_x=(startx-endx);
    int dist_y=(starty-endy);
    int distance=floor(sqrt(pow(dist_x,2)+pow(dist_y,2))+0.5f);
    if (distance<1) distance=1;
    int cx,cy,r;
    int i;
    for (i=0;i<distance;i++)
    {
        int ir=distance-i-1;
        float bend_val=2.0f*sqrt(pow(distance/2,2)-pow(i-distance/2,2))*(float)bend/distance;
        int bend_x,bend_y;
        bend_x=floor((bend_val*(float)dist_y)/(float)distance+0.5f);
        bend_y=floor((bend_val*(float)dist_x)/(float)distance+0.5f);
        //Linear change of point and radius
        cx=(startx*ir+endx*i)/distance+bend_x;
        cy=(starty*ir+endy*i)/distance-bend_y;
        r= (startr*ir+endr*i)/distance;
        //Drawing circle of given slab
        slab_draw_circle(lvl,cx,cy,r,slab_type);
    }
}

void slab_draw_circle(struct LEVEL *lvl,int x,int y,int rad,unsigned char slab_type)
{
     int i,j;
     int cx,cy;
     for (i=-rad;i<rad;i++)
       for (j=-rad;j<rad;j++)
       {
         cx=x+i;
         cy=y+j;
         if ((cx>=0) && (cy>=0) && (cx<MAP_SIZE_X) && (cy<MAP_SIZE_Y))
         {
           int crad=floor(sqrt(i*i+j*j)+0.5);
           if (crad<rad)
             lvl->slb[cx][cy]=slab_type;
         }
       }
}

/*
 * Verifies values in the SLB entry.
 */
short slab_verify_entry(unsigned char slab_type, char *err_msg)
{
    if (slab_is_door(slab_type)) return VERIF_OK;
    if (slab_is_room(slab_type)) return VERIF_OK;
    if (slab_is_wall(slab_type)) return VERIF_OK;
    if (slab_is_space(slab_type)) return VERIF_OK;
    if (slab_is_tall_unclmabl(slab_type)) return VERIF_OK;
    sprintf(err_msg,"Unknown slab entry %d",(int)slab_type);
    return VERIF_WARN;
}

/*
 * Returns slab type name string
 */
char *get_slab_fullname(unsigned short slb_type)
{
     int types_count=sizeof(all_slabs_fullnames)/sizeof(char *);
     if (slb_type<types_count)
       return (char *)all_slabs_fullnames[slb_type];
     else
       return "unknown(?!)";
}
