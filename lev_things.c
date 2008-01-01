/*
 * lev_things.c
 *
 * Defines routines for maintaining list of TNG entries,
 * provides higher level TNG control than obj_things
 *
 */

#include "lev_things.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"

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
            sprintf(err_msg,"%s at slab %d,%d.",child_err_msg,i/MAP_SUBNUM_X,j/MAP_SUBNUM_Y);
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
          thing_key = create_doorkey(lvl,sx,sy,ITEM_SUBTYPE_SPINNKEY);
          thing_add(lvl,thing_key);
        }
    }
    set_thing_level(thing,nlock);
    return true;
}

/*
 * Returns lock state for the given door.
 */
unsigned char get_door_lock(unsigned char *thing)
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
 * Returns an acceptable value of sensitive tile for torch
 */
unsigned short compute_torch_sensitile(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    int ntx=tx+(int)(sx%MAP_SUBNUM_X)-1;
    int nty=ty+(int)(sy%MAP_SUBNUM_Y)-1;
    if (ntx<0) ntx=0;
    if (ntx>=MAP_SIZE_X) ntx=MAP_MAXINDEX_X;
    if (nty<0) nty=0;
    if (nty>=MAP_SIZE_X) nty=MAP_MAXINDEX_X;
    if ((ntx==tx)&&(nty==ty))
        return ty*MAP_SIZE_X+tx;
    unsigned char slab;
    // Trying fo find torch wall around
    // First - try to put it in X axis
    slab=get_tile_slab(lvl,ntx,ty);
    if (slab_needs_adjacent_torch(slab))
        return ty*MAP_SIZE_X+ntx;
    //Now in Y axis
    slab=get_tile_slab(lvl,tx,nty);
    if (slab_needs_adjacent_torch(slab))
        return nty*MAP_SIZE_X+tx;
    //Now in XY corner
    slab=get_tile_slab(lvl,ntx,nty);
    if (slab_needs_adjacent_torch(slab))
        return nty*MAP_SIZE_X+ntx;
    // No torch wall - searching for any tall slab
    // First - try to put it in X axis
    slab=get_tile_slab(lvl,ntx,ty);
    if (slab_is_tall(slab))
        return ty*MAP_SIZE_X+ntx;
    //Now in Y axis
    slab=get_tile_slab(lvl,tx,nty);
    if (slab_is_tall(slab))
        return nty*MAP_SIZE_X+tx;
    //Now in XY corner
    slab=get_tile_slab(lvl,ntx,nty);
    if (slab_is_tall(slab))
        return nty*MAP_SIZE_X+ntx;
    return THING_SENSITILE_NONE;
}

/*
 * Returns an acceptable value of sensitive tile for room effect
 */
unsigned short compute_roomeffect_sensitile(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    return ty*MAP_SIZE_X+tx;
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
    unsigned short sensitile=compute_torch_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

unsigned char *create_doorkey(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,tx,ty));
    set_thing_tilepos_h(thing,4);
    set_thing_subtpos_h(thing,0x000);
    set_thing_subtpos(thing,0x080,0x080);
    //Note: in most DK maps, sensitile is set to 0, but I believe it is an error
    unsigned short sensitile=ty*MAP_SIZE_X+tx;
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type room effect
 */
unsigned char *create_roomeffect(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_ROOMEFFECT);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    unsigned short sensitile=compute_roomeffect_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type creature
 */
unsigned char *create_creature(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_CREATURE);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    return thing;
}

/*
 * Creates a new thing of type trap
 */
unsigned char *create_trap(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_TRAP);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    return thing;
}

/*
 * Creates a new thing of type item. Advanced version - uses one of specialized
 * functions to create the item.
 */
unsigned char *create_item_adv(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    if (is_torch_stype(stype_idx))
    {
        thing=create_torch(lvl,sx,sy,stype_idx);
    } else
    if (stype_idx==ITEM_SUBTYPE_HEROGATE)
    {
        thing=create_item(sx,sy,stype_idx);
        set_thing_owner(thing,PLAYER_GOOD);
        //Hero gate must be numbered
        //TODO: stats should be changed in thing_add
        lvl->stats.hero_gates_count++;
        //TODO: make better gate numbering (like for action points)
        set_thing_level(thing,(char)(-lvl->stats.hero_gates_count));
    } else
    if (stype_idx==ITEM_SUBTYPE_DNHEART)
    {
        thing=create_item(sx,sy,stype_idx);
        set_thing_tilepos_h(thing,3); // Raise it up a bit
    } else
    if (stype_idx==ITEM_SUBTYPE_SPINNKEY)
    {
        thing=create_doorkey(lvl,sx,sy,stype_idx);
    } else
    {
        thing=create_item(sx,sy,stype_idx);
        if (is_room_thing(thing))
        {
          unsigned short sensitile=ty*MAP_SIZE_X+tx;
          set_thing_sensitile(thing,sensitile);
        }
    }
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

/*
 * Updates things associated with room - for every room calls the spefific
 * update function.
 */
void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned char slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
    case SLAB_TYPE_PORTAL:
/*    create_things_slb_room(delete_room_things,delete_room_things,
        delete_room_things,create_things_slb_portal_inside,delete_room_things,
        clm_recs,surr_slb,surr_own,surr_tng);*/
      break;
    case SLAB_TYPE_TREASURE:
      break;
    case SLAB_TYPE_PRISONCASE:
      break;
    case SLAB_TYPE_TORTURE:
      break;
    case SLAB_TYPE_TRAINING:
      break;
    case SLAB_TYPE_DUNGHEART:
      break;
    case SLAB_TYPE_WORKSHOP:
      break;
    case SLAB_TYPE_SCAVENGER:
      break;
    case SLAB_TYPE_TEMPLE:
      break;
    case SLAB_TYPE_GRAVEYARD:
      break;
    case SLAB_TYPE_HATCHERY:
      break;
    case SLAB_TYPE_BRIDGE:
      break;
    case SLAB_TYPE_GUARDPOST:
      break;
    case SLAB_TYPE_LIBRARY:
    case SLAB_TYPE_LAIR:
    case SLAB_TYPE_BARRACKS:
    default:
      break;
    }

//TODO: make the update function
//note: it can't use CLM entries, only SLB and OWN.
}

/*
 * Updates things on a slab containing door. If there is no door thing,
 * the function creates it, otherwise updates its parameters.
 */
void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned char door_subtype;
  unsigned char slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      door_subtype=DOOR_SUBTYPE_WOOD;
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      door_subtype=DOOR_SUBTYPE_BRACED;
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      door_subtype=DOOR_SUBTYPE_IRON;
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      door_subtype=DOOR_SUBTYPE_MAGIC;
      break;
    default:
      door_subtype=DOOR_SUBTYPE_WOOD;
      break;
    }
    //Search for door things on all subtiles, delete if there is more than one
    unsigned char *thing=NULL;
    unsigned char *sec_thing;
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
          for (i=get_thing_subnums(lvl,sx,sy)-1; i >=0; i--)
          {
            sec_thing=get_thing(lvl,sx,sy,i);
            if (is_door(sec_thing))
            {
              // delete if we already have proper thing
              if (thing!=NULL)
                thing_del(lvl,sx, sy, i);
              else
              {
                //If that's firs one - keep it, but remove from level structure
                // (we will add it later to make sure it is in central subtile)
                thing=sec_thing;
                thing_drop(lvl,sx, sy, i);
              }
            }
          }
    // If we didn't find door thing, create the new one
    if (thing==NULL)
    {
      thing=create_door(lvl, tx*3+1, ty*3+1, door_subtype);
    } else
    // and if we found one, modify its parameters
    {
      set_thing_subtype(thing,door_subtype);
      set_thing_tilepos(thing,tx*3+1,ty*3+1);
      set_door_lock(lvl, thing, get_door_lock(thing));
      set_door_orientation(thing,compute_door_orientation(lvl,thing));
    }
    thing_add(lvl,thing);
}

void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty)
{

//TODO: make the update function
//note: it can't use CLM entries, only SLB and OWN.
}

/*
 * Updates things on any room slab by calling proper function
 * from these given as parameters.
 */
void create_things_slb_room(cr_tng_func cr_floor,cr_tng_func cr_edge,
        cr_tng_func cr_corner,cr_tng_func cr_inside,cr_tng_func cr_nearinsd,
        struct LEVEL *lvl, int tx, int ty)
{
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,NULL,tx,ty);
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
          cr_inside(lvl,tx,ty,surr_slb,surr_own);
      } else
      {
          //The 'near inside' columns are usually same that floor,
          //but may differ for rooms with specific corners
          cr_nearinsd(lvl,tx,ty,surr_slb,surr_own);
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
      cr_edge(lvl,tx,ty,surr_slb,surr_own);
      return;
  }
  //If still nothing, maybe we have same surround from two sides and 1 corner,
  // and another two are surely something else
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))) ||

      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      cr_corner(lvl,tx,ty,surr_slb,surr_own);
      return;
  }
  //If nothing found - update as floor of this room
  cr_floor(lvl,tx,ty,surr_slb,surr_own);
}


/*
 * Update a single slab
 * OLD - delete pending, but first it may be useful in making new functiions
 */
/*void update_tng(int x, int y)
{
    int zx, zy, bx, by, s;
    
    int i, j;
    int slbsame[3][3];
    int slbrel[3][3];
    int ownrel[3][3];
    int surround;
    int flag1, flag2, flag3;
    unsigned char *thing;
    
    if ((x<0) || (x>MAP_MAXINDEX_X) || (y<0) || (y>MAP_MAXINDEX_Y))
      return;
    remove_noncrucial_room_things(x, y);
    s = lvl->slb[x][y];
    surround=1;
    for (i=0; i<3; i++)
    {
      for (j=0; j<3; j++)
      {
          if ((x+i-1>=0) && (x+i-1<MAP_SIZE_X) && (y+j-1>=0) && (y+j-1 < MAP_SIZE_Y))
          {
            slbrel[i][j]=lvl->slb[x+i-1][y+j-1];
            ownrel[i][j]=lvl->own[x+i-1][y+j-1];
            if ( (lvl->slb[x][y]==slbrel[i][j]) && (lvl->own[x][y]==ownrel[i][j]) )
              slbsame[i][j]=1;
            else
            {
              slbsame[i][j]=0;
              surround=0;
            }
          }
          else
          {
            slbrel[i][j]=-1;
            ownrel[i][j]=-1;
            slbsame[i][j]=0;
            surround=0;
          }
      }
    }
    switch (s)
    {
    case SLAB_TYPE_PORTAL: // Portal
      if (surround)
      {
          thing = create_roomeffect(x*3+1, y*3+1, ROOMEFC_SUBTP_ENTRICE); // Dry ice
          thing_add(lvl,thing);
      }
      break;
    case SLAB_TYPE_PRISONCASE: // Prison case is really grotty
      // Sides first
      if ((!slbsame [0][1]) && (slbrel[0][1]>9))
      {
          thing_add(lvl,create_item(x*3, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3, y*3+1, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      if ((!slbsame[2][1]) && (slbrel[2][1]>9))
      {
          thing_add(lvl,create_item(x*3+2, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+2, y*3+1, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+2, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      if (!slbsame [1][0] && slbrel[1][0] > 9)
      {
          if (slbsame[0][1] || slbrel[0][1]<9)
            thing_add(lvl,create_item(x*3, y*3, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+1, y*3, ITEM_SUBTYPE_PRISONBAR));
          if (slbsame[2][1] || slbrel[2][1]<9)
            thing_add(lvl,create_item(x*3+2, y*3, ITEM_SUBTYPE_PRISONBAR));
      }
      if ((!slbsame[1][2]) && (slbrel[1][2]>9))
      {
          if ((slbsame[0][1]) || (slbrel[0][1]<9))
            thing_add(lvl,create_item(x*3, y*3+2, ITEM_SUBTYPE_PRISONBAR));
          thing_add(lvl,create_item(x*3+1, y*3+2, ITEM_SUBTYPE_PRISONBAR));
          if ((slbsame[2][1]) || (slbrel[2][1]<9))
            thing_add(lvl,create_item(x*3+2, y*3+2, ITEM_SUBTYPE_PRISONBAR));
      }
      break;
    case SLAB_TYPE_TORTURE: // Torture
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TORTURER);
      }
      else
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TORTSPIKE);
      }
      thing_add(lvl,thing);
      break;
    case SLAB_TYPE_TRAINING: // Training
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_TRAINPOST);
          thing_add(lvl,thing);
      }
      break;
    case SLAB_TYPE_DUNGHEART: // Dungeon heart - not perfect, 
               // but should be okay most of the time
      flag1=0; // Are we on an edge?
      flag2=0; // Have we got a top left / bottom right corner?
      flag3=0; // Have we got a top right / bottom left corner?
      {
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
                  }
                }
            }
            if (flag2 || flag3)
            {
                char *obj=create_item(x*3+1, y*3+1,
                  (lvl->own[x][y]>0 && lvl->own[x][y]<4 ? 0x77+lvl->own[x][y] : ITEM_SUBTYPE_HEARTFLMR));
                thing_add(lvl,obj);
            }
          }
      }
      break;
    case SLAB_TYPE_WORKSHOP: // Workshop 
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_WRKSHPMCH);
          thing_add(lvl,thing);
      }
      break;
    case SLAB_TYPE_SCAVENGER: // Scavenger room
      if (surround)
      {
          thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_SCAVNGEYE);
          thing_add(lvl,thing);
      }
      break;
    case SLAB_TYPE_GRAVEYARD: // Graveyard
      thing = create_item(x*3+1, y*3+1, ITEM_SUBTYPE_GRAVSTONE);
      thing_add(lvl,thing);
      // Add dry ice instead of pillars
      for (i=0; i < 3; i+=2)
      {
          for(j=0; j < 3; j+=2)
          {
            if (slbsame[i][1] && slbsame[1][j] &&
                !slbsame[2-i][1] && !slbsame[1][2-j])
            {
                thing = create_roomeffect(x*3+i, y*3+j, ROOMEFC_SUBTP_ENTRICE); // Dry ice
                thing_add(lvl,thing);
            }
          }
      }
      break;
    case SLAB_TYPE_GUARDPOST: // Guardpost
      // Put the flagpost on for keepers
      if (lvl->own[x][y] < 4)
      {
          thing = create_item(x*3+1, y*3+1, 0x73+lvl->own[x][y]);
          set_thing_tilepos_h(thing,2);
          thing_add(lvl,thing);
      }
      break;
    }
    // Add torches to the room if it's an appropriate type
    if (s > 9 && !(s>=42 && s <=49) && s != 52)
    {
      for (i=0; i < 3; i+=2)
      {
          if ((slbrel[i][1]==3 || slbrel[i][1]==5)
            && !picture_wall (x+i-1, y, 2-i, 1))
          {
            thing = create_item(x*3+i, y*3+1, ITEM_SUBTYPE_TORCH);
            set_thing_tilepos_h(thing,2);
            set_thing_subtpos_h(thing,0xe0);
            set_thing_subtpos(thing,i*0x40+0x40,-1);
            thing_add(lvl,thing);
          }
          if ((slbrel[1][i]==3 || slbrel[1][i]==5)
            && !picture_wall (x, y+i-1, 1, 2-i))
          {
            thing = create_item(x*3+1, y*3+i, ITEM_SUBTYPE_TORCH);
            set_thing_tilepos_h(thing,2);
            set_thing_subtpos_h(thing,0xe0);
            set_thing_subtpos(thing,0x80,i*0x40+0x40);
            thing_add(lvl,thing);
          }
      }
    }
}*/

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

