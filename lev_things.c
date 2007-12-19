/*
 * lev_things.c
 *
 * Defines routines for maintaining list of TNG entries,
 * provides higher level TNG control than obj_things
 *
 */

#include "lev_things.h"

#include "globals.h"
#include "obj_things.h"
#include "lev_data.h"
#include "obj_slabs.h"

/*
 * Verifies thing types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short things_verify(struct LEVEL *lvl, char *err_msg)
{
    char child_err_msg[LINEMSG_SIZE];
    strcpy(child_err_msg,"Unknown error");
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Sweeping through things
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          short result=thing_verify(thing,child_err_msg);
          if (result!=VERIF_OK)
          {
            sprintf(err_msg,"%s at slab %d,%d.",child_err_msg,i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
            return result;
          }
        }
      }
    }
  return VERIF_OK;
}

/*
 * Sets lock state for the given door; creates/deletes spinning key thing.
 */
short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock)
{
    if (thing==NULL) return false;
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    unsigned char *thing_key=NULL;
    unsigned int tng_num=get_thing_subnums(lvl,sx,sy);
    while (tng_num>0)
    {
        tng_num--;
        thing_key=get_thing(lvl,sx,sy,tng_num);
        if ((get_thing_type(thing_key)==THING_TYPE_ITEM) && 
            (get_thing_subtype(thing_key)==ITEM_SUBTYPE_SPINNKEY))
          break;
        thing_key=NULL;
    }
    if (nlock==DOOR_PASS_UNLOCKED)
    {
        if (thing_key!=NULL)
          thing_del(lvl,sx,sy,tng_num);
    } else
    {
        if (thing_key==NULL)
        {
          thing_key = create_item(sx,sy,ITEM_SUBTYPE_SPINNKEY);
          //TODO: set the spinning key properties (height, other?)
          thing_add(lvl,thing_key);
        }
    }
    set_thing_level(thing,nlock);
    return true;
}

/*
 * Returns lock state for the given door.
 */
unsigned char get_door_lock(struct LEVEL *lvl, unsigned char *thing)
{
  return get_thing_level(thing);
}

/*
 * Returns the orientation that doors should have.
 */
unsigned char compute_door_orientation(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short tx=get_thing_tilepos_x(thing)/MAP_SUBNUM_X;
    unsigned short ty=get_thing_tilepos_y(thing)/MAP_SUBNUM_Y;
    // Check the previous orientation - if it matches, then it is preferred
    if (get_door_orientation(thing)==DOOR_ORIENT_NSPASS)
    {
      unsigned char slab_e=get_tile_slab(lvl, tx+1, ty);
      unsigned char slab_w=get_tile_slab(lvl, tx-1, ty);
      if (slab_is_tall(slab_e)&&slab_is_tall(slab_w))
        return DOOR_ORIENT_NSPASS;
      else
        return DOOR_ORIENT_EWPASS;
    } else
    {
      unsigned char slab_n=get_tile_slab(lvl, tx, ty-1);
      unsigned char slab_s=get_tile_slab(lvl, tx, ty+1);
      if (slab_is_tall(slab_n)&&slab_is_tall(slab_s))
        return DOOR_ORIENT_EWPASS;
      else
        return DOOR_ORIENT_NSPASS;
    }
}

/*
 * Creates a new thing of type door. Sets orientation and owner for given position.
 */
unsigned char *create_door(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_DOOR);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    set_thing_subtpos(thing,0x80,0x80);
    set_thing_tilepos_h(thing,5);
    set_door_orientation(thing,compute_door_orientation(lvl,thing));
    //Set default lock state
    set_thing_level(thing,DOOR_PASS_UNLOCKED);
    return thing;
}

unsigned char *create_torch(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    set_thing_tilepos_h(thing,2);
    set_thing_subtpos_h(thing,0x0e0);
    //TODO: set the subtile position near wall (find the wall first)
    // (one dimension should be 0x0c0)
    unsigned char sub_x=0x080;
    unsigned char sub_y=0x080;
    if ((sx%MAP_SUBNUM_X)==0)
      sub_x=0x040;
    else
    if ((sx%MAP_SUBNUM_X)==2)
      sub_x=0x0c0;
    else
    if ((sy%MAP_SUBNUM_Y)==0)
      sub_y=0x040;
    else
    if ((sy%MAP_SUBNUM_Y)==2)
      sub_y=0x0c0;
    set_thing_subtpos(thing,sub_x,sub_y);
    return thing;
}

/*
 * Updates TNG, APT and LGT entries for the whole map - all tiles
 * and subtiles are reset. Additionally, USE values in columns
 * are recomputed to avoid mistakes.
 */
void update_obj_for_whole_map(struct LEVEL *lvl)
{
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_clmaffective_obj_for_slab(lvl, i, k);
}

/*
 * Updates TNG, APT and LGT entries for given map tile coordinates
 * and also updates enties in all neightbour squares
 */
void update_obj_for_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_clmaffective_obj_for_slab(lvl, i, k);
      }
}

/*
 * Updates TNG, APT and LGT entries for given map tile coordinates range
 */
void update_obj_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last)
{
    int i,k;
    for (k=ty_first;k<=ty_last;k++)
      for (i=tx_first;i<=tx_last;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_clmaffective_obj_for_slab(lvl, i, k);
      }
}

void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    remove_misplaced_objs_on_slab(lvl,tx,ty);
    unsigned char slab=get_tile_slab(lvl,tx,ty);
    if (slab_is_room(slab))
    {
      update_room_things_on_slab(lvl,tx,ty);
    } else
    if (slab_is_door(slab))
    {
      update_door_things_on_slab(lvl,tx,ty);
    } else
    if (slab_needs_adjacent_torch(slab))
    {
      update_torch_things_near_slab(lvl,tx,ty);
    }
}

/*
 * Removes objects that usually belong do different slab type than the one we're on
 */
void remove_misplaced_objs_on_slab(struct LEVEL *lvl, int tx, int ty)
{
    unsigned char slab=get_tile_slab(lvl,tx,ty);
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            unsigned char expect_slb=get_usual_thing_slab(thing);
            if ((expect_slb!=slab)&&
               ((expect_slb!=SLAB_TYPE_CLAIMED)||(slab_is_tall(slab))))
                thing_del(lvl,sx, sy, i);
          }
     }
}

void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
//TODO: make the update function
//note: it can't use CLM entries, only SLB and OWN.
}

void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
//TODO: make the update function
//note: it can't use CLM entries, only SLB and OWN.
}

void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty)
{
//TODO: make the update function
//note: it can't use CLM entries, only SLB and OWN.
}



/*
 * Removes all things that usually exist only in specific rooms,
 * and aren't crucial for the game (like gates and dungeon hearts)
 */
void remove_noncrucial_room_things(int tx, int ty)
{
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_thing(thing) && (!is_crucial_thing(thing)))
                thing_del(lvl,sx, sy, i);
          }
     }
}

