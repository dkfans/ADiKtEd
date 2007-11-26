/*
 * obj_slabs.c
 *
 * Functions for maintaining slabs. Also includes some routines for owner
 * identification.
 *
 */

#include "obj_slabs.h"
#include "lev_data.h"
#include "globals.h"
#include "obj_things.h"

const char *owners[]={"Keeper 0 (Human)", "Keeper 1", "Keeper 2",
    "Keeper 3", "Heroes", "Unclaimed"};

unsigned char get_player_next(unsigned char plyr_idx)
{
  plyr_idx++;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return 0;
}

unsigned char get_player_prev(unsigned char plyr_idx)
{
  plyr_idx--;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return PLAYERS_COUNT-1;
}

/*
 * Returns square owner name string
 */
char *get_owner_type_fullname(unsigned char own_idx)
{
     int types_count=sizeof(owners)/sizeof(char *);
     if (own_idx<types_count)
       return (char *)owners[own_idx];
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
    int i_beg=-MAP_SUBNUM_X/2;
    int i_end=MAP_SUBNUM_X-MAP_SUBNUM_X/2;
    int j_beg=-MAP_SUBNUM_Y/2;
    int j_end=MAP_SUBNUM_Y-MAP_SUBNUM_Y/2;
    int i, j;
    //Sweeping and comparing slab type and owner
    for (i=x+i_beg; i<x+i_end; i++)
      for (j=y+j_beg; j<y+j_end; j++)
        if ((lvl->slb[x][y]!=lvl->slb[i][j])||(lvl->own[x][y]!=lvl->own[i][j]))
          return false;
    return true;
}

/*
 * Says how many slabs of given type are in 3x3 area surrounding given slab,
 * and including it; returns 0..9;
 */
int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned char slab_type)
{
    int amount=0;
    //Preparing variables
    int i_beg=-MAP_SUBNUM_X/2;
    int i_end=MAP_SUBNUM_X-MAP_SUBNUM_X/2;
    int j_beg=-MAP_SUBNUM_Y/2;
    int j_end=MAP_SUBNUM_Y-MAP_SUBNUM_Y/2;
    int i, j;
    //Sweeping and comparing slab type and owner
    for (i=x+i_beg; i<x+i_end; i++)
      for (j=y+j_beg; j<y+j_end; j++)
      {
      if ((i>=0) && (j>=0) && (i<MAP_SIZE_X) && (j<MAP_SIZE_Y))
        if (lvl->slb[i][j]==slab_type)
          amount++;
      }
    return amount;
}

/*
 * Use our own strange slb thing to get locked/unlocked doors right
 */
void check_doors(void)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;

    unsigned char *thing;
    int cx, cy, k;
    for (cy=0; cy < arr_entries_y; cy++)
    {
      for (cx=0; cx < arr_entries_x; cx++)
      {
          int things_count=get_thing_subnums(lvl,cx,cy);
          for (k=0; k<things_count; k++)
          {
            thing = get_thing(lvl,cx,cy,k);
            if (is_door(thing))
            {
                if ((lvl->slb[cx/MAP_SUBNUM_X][cy/MAP_SUBNUM_Y]&0xfe) == get_thing_subtype(thing)*2+40) // Update the slab
                  lvl->slb[cx/MAP_SUBNUM_X][cy/MAP_SUBNUM_Y] = get_thing_subtype(thing)*2+40+thing[14];
            }
          }
      }
    }
}

/*
 * Returns a WBI file entry for given slab
 */
int slab_default_wbi_entry(unsigned char slab_type)
{
  //NOTE: the default levels sometimes have different values here.
  // maybe a subtile starts to be animated after it is revealed?
    switch (slab_type)
    {
    case SLAB_TYPE_LAVA:
    case SLAB_TYPE_WATER:
        return SLAB_WIB_ANIMATE;
    default:
        return SLAB_WIB_SKEW;
    }
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

