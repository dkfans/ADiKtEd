/******************************************************************************/
// lev_things.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines routines for maintaining list of TNG entries,
//   provides higher level TNG control than obj_things

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "lev_things.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "lev_column.h"

/*
 * Functions and names used in search mode
 */
const is_thing_subtype search_tngtype_func[]={
      is_spellbook,is_dngspecbox,is_crtrlair,
      is_trapbox,is_trap,is_creature,
      is_door,is_effectgen,is_statue,
      is_furniture,is_food,is_gold,
      is_torch,is_heartflame,is_polebar,
      is_lit_thing,is_herogate,is_dnheart,
      is_doorkey,
      };

const char *search_tngtype_names[]={
      "Spell books","Dung.Specials","Creature lairs",
      "Trap boxes","Deployed traps","Creatures",
      "Deployed doors","Effect Generatr","Statues",
      "Furniture items","Food (chickens)","Gold things",
      "Torches","Heart flames","Poles and bars",
      "Lit things","Hero gates","Dungeon hearts",
      "Door keys",
};

/*
 * Verifies thing types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short things_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    char child_err_msg[LINEMSG_SIZE];
    strcpy(child_err_msg,"Unknown error");
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Sweeping through things
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        unsigned short slab=get_tile_slab(lvl, i/MAP_SUBNUM_X, j/MAP_SUBNUM_Y);
        unsigned int creatures_on_subtl=0;
        unsigned int effectgenrts_on_subtl=0;
        unsigned int traps_on_subtl=0;
        unsigned int lit_things_on_subtl=0;
        unsigned int doors_on_subtl=0;
        unsigned int booksboxes_on_subtl=0;
        int things_count=get_thing_subnums(lvl,i,j);
        int categr=-1;
        unsigned short subtp_x,subtp_y,subtp_h;
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          short result=thing_verify(thing,child_err_msg);
          if (result!=VERIF_OK)
          {
            errpt->x=i/MAP_SUBNUM_X;
            errpt->y=j/MAP_SUBNUM_Y;
            sprintf(err_msg,"%s at slab %d,%d.",child_err_msg,errpt->x,errpt->y);
            return result;
          }
          unsigned char type_idx=get_thing_type(thing);
          // Checking sensitile again (one check is in thing_verify())
          if (type_idx==THING_TYPE_ITEM)
          {
            int sen_tl;
            sen_tl=get_thing_sensitile(thing);
            unsigned short stype_idx=get_thing_subtype(thing);
            int auto_sen_tl;
            auto_sen_tl=compute_item_sensitile(lvl,thing);
            if ((sen_tl!=auto_sen_tl)&&(!is_torchcndl(thing))&&(!is_spinningtng(thing))&&
                (!is_statue(thing))&&(!is_dncrucial(thing))&&(!is_furniture(thing)))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"%s for %s at slab %d,%d.","Sensitive tile incorrectly set",
              get_item_subtype_fullname(stype_idx),errpt->x,errpt->y);
              return VERIF_WARN;
            }
            if (((stype_idx==ITEM_SUBTYPE_GLDHOARD1)||(stype_idx==ITEM_SUBTYPE_GLDHOARD2)||
                (stype_idx==ITEM_SUBTYPE_GLDHOARD3)||(stype_idx==ITEM_SUBTYPE_GLDHOARD4)||
                (stype_idx==ITEM_SUBTYPE_GLDHOARD5))&&(slab!=SLAB_TYPE_TREASURE))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Gold hoarde put outside of treasure room on slab %d,%d.",
                  errpt->x,errpt->y);
              return VERIF_WARN;
            }
            if ((get_thing_subtypes_arridx(thing)==categr)&&(get_thing_subtpos_x(thing)==subtp_x)&&
                (get_thing_subtpos_y(thing)==subtp_y)&&(get_thing_subtpos_h(thing)==subtp_h)&&
                (!is_gold(thing))&&(!is_food(thing)))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Multiple %s with exactly same position on slab %d,%d.",
                  get_thing_category_fullname(categr),errpt->x,errpt->y);
              return VERIF_WARN;
            }
            categr=get_thing_subtypes_arridx(thing);
            subtp_x=get_thing_subtpos_x(thing);
            subtp_y=get_thing_subtpos_y(thing);
            subtp_h=get_thing_subtpos_h(thing);
          }
          if (is_creature(thing))  creatures_on_subtl++;
          if (is_trap(thing))  traps_on_subtl++;
          if (is_effectgen(thing))  effectgenrts_on_subtl++;
          if (is_lit_thing(thing))  lit_things_on_subtl++;
          if (is_door(thing)) doors_on_subtl++;
          if (is_spellbook(thing)||is_dngspecbox(thing)||is_trapbox(thing)
              ||is_doorbox(thing)) booksboxes_on_subtl++;
        }
        char *err_objcount=NULL;
        if (creatures_on_subtl>5)
            err_objcount="five creature";
        if (traps_on_subtl>3)
            err_objcount="three traps";
        if (effectgenrts_on_subtl>3)
            err_objcount="three effect genertrs";
        if (lit_things_on_subtl>2)
            err_objcount="two lit things";
        if (doors_on_subtl>1)
            err_objcount="one door thing";
        if (booksboxes_on_subtl>1)
            err_objcount="one book/workshop box/special";
        if (err_objcount!=NULL)
        {
            errpt->x=i/MAP_SUBNUM_X;
            errpt->y=j/MAP_SUBNUM_Y;
            sprintf(err_msg,"More than %s at one subtile on slab %d,%d.",err_objcount,errpt->x,errpt->y);
            return VERIF_WARN;
        }
      }
    }
  return VERIF_OK;
}

char *get_search_tngtype_name(unsigned short idx)
{
     int count=sizeof(search_tngtype_names)/sizeof(char *);
     if (idx<count)
       return (char *)search_tngtype_names[idx];
     else
       return "unknown(?!)";
}

is_thing_subtype get_search_tngtype_func(unsigned short idx)
{
     int count=sizeof(search_tngtype_func)/sizeof(is_thing_subtype);
     if ((idx<count)&&(search_tngtype_func[idx]!=NULL))
       return search_tngtype_func[idx];
     else
       return NULL;
}

char *get_search_objtype_name(unsigned short idx)
{
    const int count=3;
    if (idx>=count)
      return get_search_tngtype_name(idx-count);
    switch (idx)
    {
    case 0: return "Clear results";
    case 1: return "Action points";
    case 2: return "Static lights";
    }
}

/*
 * Finds next object of type matching to srch_idx value. Returns the object and
 * sets its coordinates (tx,ty). If not found, returns NULL.
 */
unsigned char *find_next_object_on_map(struct LEVEL *lvl, int *tx, int *ty, unsigned short srch_idx)
{
    const int count=3;
    if (srch_idx>=count)
    {
      is_thing_subtype check_func=get_search_tngtype_func(srch_idx-count);
      return find_next_thing_on_map(lvl,tx,ty,check_func);
    }
    switch (srch_idx)
    {
    case 1: return find_next_actnpt_on_map(lvl,tx,ty);
    case 2: return find_next_stlight_on_map(lvl,tx,ty);
    case 0:
    default:
        return NULL;
    }
}

unsigned short get_search_objtype_count(void)
{
     unsigned short count=3;
     count+=sizeof(search_tngtype_func)/sizeof(is_thing_subtype);
     return count;
}

/*
 * Sets lock state for the given door; creates/deletes spinning key thing.
 */
short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock)
{
    if (thing==NULL) return false;
    unsigned short sx=get_thing_subtile_x(thing);
    unsigned short sy=get_thing_subtile_y(thing);
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
unsigned char compute_door_orientation(const struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short tx=get_thing_subtile_x(thing)/MAP_SUBNUM_X;
    unsigned short ty=get_thing_subtile_y(thing)/MAP_SUBNUM_Y;
    // Check the previous orientation - if it matches, then it is preferred
    if (get_door_orientation(thing)==DOOR_ORIENT_NSPASS)
    {
      unsigned short slab_e=get_tile_slab(lvl, tx+1, ty);
      unsigned short slab_w=get_tile_slab(lvl, tx-1, ty);
      if (slab_is_tall(slab_e)&&slab_is_tall(slab_w))
        return DOOR_ORIENT_NSPASS;
      else
        return DOOR_ORIENT_EWPASS;
    } else
    {
      unsigned short slab_n=get_tile_slab(lvl, tx, ty-1);
      unsigned short slab_s=get_tile_slab(lvl, tx, ty+1);
      if (slab_is_tall(slab_n)&&slab_is_tall(slab_s))
        return DOOR_ORIENT_EWPASS;
      else
        return DOOR_ORIENT_NSPASS;
    }
}

/*
 * Sweeps things to find an unused hero gate number
 */
unsigned short get_free_herogate_number(const struct LEVEL *lvl)
{
    return get_free_herogate_number_next(lvl,1);
}

/*
 * Sweeps things to find an unused hero gate number,
 * not smaller than given number
 */
unsigned short get_free_herogate_number_next(const struct LEVEL *lvl,const unsigned short start)
{
    unsigned int used_size=start+16;
    unsigned char *used=NULL;
    if (!create_herogate_number_used_arr(lvl,&used,&used_size))
        return start;
    int new_num=start;
    while (new_num<used_size)
    {
      if (used[new_num]==0) break;
      new_num++;
    }
    free(used);
    return new_num;
}

/*
 * Sweeps things to find an unused hero gate number,
 * not larger than given number
 */
unsigned short get_free_herogate_number_prev(const struct LEVEL *lvl,const unsigned short start)
{
    unsigned int used_size=start+2;
    unsigned char *used=NULL;
    if (!create_herogate_number_used_arr(lvl,&used,&used_size))
        return start;
    int new_num=start;
    while (new_num>1)
    {
      if (used[new_num]==0) break;
      new_num--;
    }
    free(used);
    return new_num;
}

/*
 * Creates an array which contains number of uses of each hero gate number.
 * the array is never smaller than starting value of 'size'; the function may
 * enlarge 'size' and create larger array in some cases.
 * Note: numbering starts at 1, not 0; used[0] is used for 'out of bounds' numbers
 */
short create_herogate_number_used_arr(const struct LEVEL *lvl,unsigned char **used,unsigned int *used_size)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int k;
    *used_size=max(lvl->stats.hero_gates_count+16,*used_size);
    *used=malloc((*used_size)*sizeof(unsigned char));
    if (*used==NULL) return false;
    for (k=0;k<(*used_size);k++)
      (*used)[k]=0;
    int cy, cx;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=lvl->tng_subnums[cx][cy];
          for (k=0; k<num_subs; k++)
          {
                char *thing=get_thing(lvl,cx,cy,k);
                unsigned char stype_idx=get_thing_subtype(thing);
                if (stype_idx==ITEM_SUBTYPE_HEROGATE)
                {
                  unsigned short cnum=get_thing_level(thing);
                  if (cnum<(*used_size))
                    (*used)[cnum]++;
                  else
                    (*used)[0]++;
                }
          }
      }
    }
    return true;
}

/*
 * Counts things belonging to each owner. count parameter must be
 * an array with size PLAYERS_COUNT
 */
short owned_things_count(int *count,struct LEVEL *lvl,
    unsigned char type_idx,unsigned char stype_idx)
{
    //Preparing array bounds
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int i,j,k;
    for (i=0; i < arr_entries_y; i++)
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          unsigned char type_id2=get_thing_type(thing);
          unsigned char stype_id2=get_thing_subtype(thing);
          if ((type_idx==type_id2)&&(stype_idx==stype_id2))
          {
            unsigned char own=get_thing_owner(thing);
            if (own>=PLAYERS_COUNT)
              count[PLAYER_UNSET]++;
            else
              count[own]++;
          }
        }
      }
    return true;
}

/*
 * Tries to find a thing that contains a light (lit thing)
 * on given slab and 1 square radius rectangle. If no such thing, returns NULL.
 */
unsigned char *find_lit_thing_on_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
        {
          unsigned char *thing;
          thing=find_thing_on_tile(lvl,i,k,is_lit_thing);
          if (thing!=NULL) return thing;
        }
      }
    return NULL;
}

/*
 * Tries to find a thing which matches given check function
 * on given slab. If no such thing, returns NULL.
 * Finds only first thing on a tile.
 */
unsigned char *find_thing_on_tile(struct LEVEL *lvl, int tx, int ty, is_thing_subtype check_func)
{
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int count=get_thing_subnums(lvl,sx,sy);
          for (i=0; i <count; i++)
          {
            unsigned char *thing;
            thing=get_thing(lvl,sx,sy,i);
            if (check_func(thing))
              return thing;
          }
      }
    return NULL;
}

/*
 * Tries to find a thing that contains thing which matches given check function.
 * on all slabs AFTER the given slab. If no such thing, returns NULL.
 * Returns only one matching thing on one slab.
 * passing -1 in coordinate argument means to start search with 0.
 */
unsigned char *find_next_thing_on_map(struct LEVEL *lvl, int *tx, int *ty, is_thing_subtype check_func)
{
  if (check_func==NULL) return NULL;
  if ((*ty)<0) {(*tx)=-1;(*ty)=0;};
  if ((*tx)<0) (*tx)=-1;
  do {
      //Switching coords to next map tile
      (*tx)++;
      while (*tx>=MAP_SIZE_X)
      {
        (*tx)-=MAP_SIZE_X;
        (*ty)++;
      }
      //Searching in that tile
      unsigned char *thing;
      thing=find_thing_on_tile(lvl,*tx,*ty,check_func);
      if (thing!=NULL) return thing;
  } while ((*ty)<MAP_SIZE_Y);
  return NULL;
}

unsigned char *find_next_actnpt_on_map(struct LEVEL *lvl, int *tx, int *ty)
{
  if ((*ty)<0) {(*tx)=-1;(*ty)=0;};
  if ((*tx)<0) (*tx)=-1;
  do {
      //Switching coords to next map tile
      (*tx)++;
      while ((*tx)>=MAP_SIZE_X)
      {
        (*tx)-=MAP_SIZE_X;
        (*ty)++;
      }
      //Searching in that tile
      unsigned char *actnpt;
      int sx,sy;
      for (sx=(*tx)*3;sx<(*tx)*3+3;sx++)
        for (sy=(*ty)*3;sy<(*ty)*3+3;sy++)
        {
          actnpt=get_actnpt(lvl,sx,sy,0);
          if (actnpt!=NULL) return actnpt;
        }
  } while ((*ty)<MAP_SIZE_Y);
  return NULL;
}

unsigned char *find_next_stlight_on_map(struct LEVEL *lvl, int *tx, int *ty)
{
  if ((*ty)<0) {(*tx)=-1;(*ty)=0;};
  if ((*tx)<0) (*tx)=-1;
  do {
      //Switching coords to next map tile
      (*tx)++;
      while ((*tx)>=MAP_SIZE_X)
      {
        (*tx)-=MAP_SIZE_X;
        (*ty)++;
      }
      //Searching in that tile
      unsigned char *stlight;
      int sx,sy;
      for (sx=(*tx)*3;sx<(*tx)*3+3;sx++)
        for (sy=(*ty)*3;sy<(*ty)*3+3;sy++)
        {
          stlight=get_stlight(lvl,sx,sy,0);
          if (stlight!=NULL) return stlight;
        }
  } while ((*ty)<MAP_SIZE_Y);
  return NULL;
}

/*
 * Returns an acceptable value of sensitive tile for torch
 */
unsigned short compute_torch_sensitile(const struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_subtile_x(thing);
    unsigned short sy=get_thing_subtile_y(thing);
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
    unsigned short slab;
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
 * Returns an acceptable value of sensitive tile for effect generator
 */
unsigned short compute_effectgen_sensitile(const struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_subtile_x(thing);
    unsigned short sy=get_thing_subtile_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    return ty*MAP_SIZE_X+tx;
}

/*
 * Returns an acceptable value of sensitive tile for item thing
 */
unsigned short compute_item_sensitile(const struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_subtile_x(thing);
    unsigned short sy=get_thing_subtile_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    unsigned short stype_idx;
    stype_idx=get_thing_subtype(thing);
    unsigned short slab_type;
    slab_type=get_tile_slab(lvl,tx,ty);
    int categr=get_thing_subtypes_arridx(thing);
    switch (categr)
    {
    case THING_CATEGR_EFFCTGEN:
         return compute_effectgen_sensitile(lvl, thing);
    case THING_CATEGR_STATUE:
    case THING_CATEGR_HEARTFLAME:
    case THING_CATEGR_ROOMEQUIP:
    case THING_CATEGR_POLEBAR:
    case THING_CATEGR_FURNITURE:
      if (slab_is_room(slab_type))
        return ty*MAP_SIZE_X+tx;
      else
        return THING_SENSITILE_NONE;
    case THING_CATEGR_TORCHCNDL:
      if (is_torch(thing))
        return compute_torch_sensitile(lvl,thing);
      else
      if (slab_is_room(slab_type))
        return ty*MAP_SIZE_X+tx;
      else
        return THING_SENSITILE_NONE;
    case THING_CATEGR_ITEMEFFCT://not sure about this one
      if ((stype_idx==ITEM_SUBTYPE_TEMPLESPN)&&(slab_is_room(slab_type)))
        return ty*MAP_SIZE_X+tx;
      else
        return THING_SENSITILE_NONE;
    case THING_CATEGR_DNCRUCIAL:
      if (stype_idx==ITEM_SUBTYPE_HEROGATE)
        return THING_SENSITILE_NONE;
      else
        return ty*MAP_SIZE_X+tx;
    case THING_CATEGR_SPINNTNG:
    case THING_CATEGR_CREATLAIR://not sure about this one
    case THING_CATEGR_FOOD://not sure about this one
        return ty*MAP_SIZE_X+tx;
    case THING_CATEGR_GOLD:
// from level 19 - all gold things have sensitile=none
//      if ((stype_idx==ITEM_SUBTYPE_GOLDCHEST)||(stype_idx==ITEM_SUBTYPE_GOLD)||
//          (stype_idx==ITEM_SUBTYPE_GOLDL))
        return THING_SENSITILE_NONE;
    case THING_CATEGR_NULL:
    case THING_CATEGR_SPECIALBOX:
    case THING_CATEGR_SPELLBOOK:
    case THING_CATEGR_WRKSHOPBOX:
    case THING_CATEGR_UNKNOWN:
    case THING_CATEGR_PWHAND:
    default:
        return THING_SENSITILE_NONE;
    // These have sensitile used for other data
    case THING_CATEGR_TRAP:
    case THING_CATEGR_DOOR:
    case THING_CATEGR_CREATR:
         return 0;
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
    //This will be updated in update_tile_things_subpos_and_height
    set_thing_subtpos(thing,128,128);
    set_thing_subtile_h(thing,5);
    set_door_orientation(thing,compute_door_orientation(lvl,thing));
    set_thing_sensitile(thing,get_free_indexedthing_number(lvl));
    //Set default lock state
    set_thing_level(thing,DOOR_PASS_UNLOCKED);
    return thing;
}

unsigned char *create_torch(const struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    //This will be updated in update_tile_things_subpos_and_height
    unsigned char sub_x=0x080;
    unsigned char sub_y=0x080;
    unsigned char subtl_h=2;
    unsigned char subpos_h=224;
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
    if (((sx%MAP_SUBNUM_X)==1)&&((sy%MAP_SUBNUM_Y)==1))
    {
      subtl_h=3;
      subpos_h=0;
    }
    set_thing_subtpos(thing,sub_x,sub_y);
    set_thing_subtile_h(thing,subtl_h);
    set_thing_subtpos_h(thing,subpos_h);
    //Sensitive tile
    unsigned short sensitile=compute_torch_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

unsigned char *create_doorkey(const struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,tx,ty));
    //This will be updated in update_tile_things_subpos_and_height
    set_thing_subtile_h(thing,4);
    set_thing_subtpos_h(thing,0x000);
    set_thing_subtpos(thing,0x080,0x080);
    //Note: in most DK maps, sensitile of key is set to 0, but I believe it is an error
    unsigned short sensitile=compute_item_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type effect generator
 */
unsigned char *create_effectgen(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_EFFECTGEN);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    set_thing_range_subtile(thing,5);
    unsigned short sensitile=compute_effectgen_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type creature
 */
unsigned char *create_creature(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_CREATURE);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    set_thing_level(thing,0);
    set_thing_sensitile(thing,get_free_indexedthing_number(lvl));
    return thing;
}

/*
 * Creates a new thing of type trap
 */
unsigned char *create_trap(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_TRAP);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    set_thing_sensitile(thing,get_free_indexedthing_number(lvl));
    return thing;
}

/*
 * Sweeps things to find an unused trap/door/creature number
 */
unsigned short get_free_indexedthing_number(const struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int k;
    int used_size=lvl->stats.doors_count+lvl->stats.traps_count+lvl->stats.creatures_count+16;
    unsigned char *used=malloc(used_size*sizeof(unsigned char));
    for (k=0;k<used_size;k++)
      used[k]=0;
    int cy, cx;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=lvl->tng_subnums[cx][cy];
          for (k=0; k<num_subs; k++)
          {
              char *thing=get_thing(lvl,cx,cy,k);
              if (is_trap(thing)||is_door(thing)||is_creature(thing))
              {
                unsigned short sensitl=get_thing_sensitile(thing);
                if (sensitl<used_size) used[sensitl]=1;
              }
          }
      }
    }
    // Note: numbering starts at 1, not 0
    int new_num=1;
    while (used[new_num]>0) new_num++;
    free(used);
    return new_num;
}

/*
 * Creates a new thing of type item. Advanced version - uses one of specialized
 * functions to create the item.
 */
unsigned char *create_item_adv(const struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx)
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
        thing=create_item(lvl,sx,sy,stype_idx);
        set_thing_owner(thing,PLAYER_GOOD);
        //Hero gate must be numbered
        set_thing_level(thing,get_free_herogate_number(lvl));
        unsigned short sensitile=compute_item_sensitile(lvl,thing);
        set_thing_sensitile(thing,sensitile);
    } else
    if (stype_idx==ITEM_SUBTYPE_DNHEART)
    {
        thing=create_item(lvl,sx,sy,stype_idx);
        set_thing_subtile_h(thing,3); // Raise it up a bit
        unsigned short sensitile=compute_item_sensitile(lvl,thing);
        set_thing_sensitile(thing,sensitile);
    } else
    if (is_heartflame_stype(stype_idx))
    {
        thing=create_item(lvl,sx,sy,stype_idx);
        set_thing_subtile_h(thing,2); // That is where heart flames should be
        unsigned short sensitile=compute_item_sensitile(lvl,thing);
        set_thing_sensitile(thing,sensitile);
    } else
    if (stype_idx==ITEM_SUBTYPE_PRISONBAR)
    {
        thing=create_item(lvl,sx,sy,stype_idx);
        set_thing_subtpos(thing,128,128);
        unsigned short sensitile=compute_item_sensitile(lvl,thing);
        set_thing_sensitile(thing,sensitile);
    } else
    if (stype_idx==ITEM_SUBTYPE_SPINNKEY)
    {
        thing=create_doorkey(lvl,sx,sy,stype_idx);
    } else
    {
        thing=create_item(lvl,sx,sy,stype_idx);
        unsigned short sensitile=compute_item_sensitile(lvl,thing);
        set_thing_sensitile(thing,sensitile);
    }
    return thing;
}

/*
 * Removes all LGT entries and most of TNG enties for the whole map - all tiles
 * and subtiles are reset.
 */
void remove_automade_obj_for_whole_map(struct LEVEL *lvl)
{
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          remove_automade_obj_for_slab(lvl, i, k);
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

void remove_automade_obj_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    unsigned short slab=get_tile_slab(lvl,tx,ty);
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_stlight=get_stlight_subnums(lvl,sx,sy)-1;
          for (i=last_stlight; i >=0; i--)
              stlight_del(lvl,sx, sy, i);
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            short should_delete=false;
            int categr=get_thing_subtypes_arridx(thing);
            switch (categr)
            {
            // Categories to leave
            case THING_CATEGR_CREATR:
            case THING_CATEGR_SPECIALBOX:
            case THING_CATEGR_SPELLBOOK:
            case THING_CATEGR_WRKSHOPBOX:
            case THING_CATEGR_TRAP:
            case THING_CATEGR_GOLD:
                break;
            // Not-so-easy categories
            case THING_CATEGR_STATUE:
            case THING_CATEGR_FURNITURE:
                if (is_room_inventory(thing) && slab_is_room(slab))
                  should_delete=true;
                break;
            // Do not touch hero gates!
            case THING_CATEGR_DNCRUCIAL:
              {
                unsigned short stype_idx=get_thing_subtype(thing);
                if (stype_idx!=ITEM_SUBTYPE_HEROGATE)
                  should_delete=true;
              };break;
            // Categories to delete
            case THING_CATEGR_NULL:
            case THING_CATEGR_EFFCTGEN:
            case THING_CATEGR_ITEMEFFCT:
            case THING_CATEGR_CREATLAIR:
            case THING_CATEGR_DOOR:
            case THING_CATEGR_SPINNTNG:
            case THING_CATEGR_FOOD:
            case THING_CATEGR_TORCHCNDL:
            case THING_CATEGR_HEARTFLAME:
            case THING_CATEGR_POLEBAR:
            case THING_CATEGR_ROOMEQUIP:
            case THING_CATEGR_PWHAND:
            case THING_CATEGR_UNKNOWN:
            default:
                should_delete=true;
                break;
            }
            // If the thing is placed on incorrect place - delete anyway
            unsigned char expect_slb=get_usual_thing_slab(thing);
            if ((expect_slb!=slab)&&
               ((expect_slb!=SLAB_TYPE_CLAIMED)||(slab_is_tall(slab))))
               should_delete=true;
            // And the final act...
            if (should_delete)
                thing_del(lvl,sx, sy, i);
          }
     }
}

void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    remove_misplaced_objs_on_slab(lvl,tx,ty);
    unsigned short slab=get_tile_slab(lvl,tx,ty);
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
    unsigned short slab=get_tile_slab(lvl,tx,ty);
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
 * It uses only SLB/OWN entries to determine things parameters. CLM entries
 * can't be used because they may be not valid.
 */
void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned short slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
    case SLAB_TYPE_PORTAL:
      create_things_slb_room(delete_room_things,delete_room_things,
        delete_room_things,update_things_slb_portal_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TREASURE:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_treasure_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_things_slb_room_simple(update_things_slb_prison,lvl,tx,ty);
      break;
    case SLAB_TYPE_TORTURE:
      create_things_slb_room(update_things_slb_torture_edge,update_things_slb_torture_edge,
        update_things_slb_torture_edge,update_things_slb_torture_inside,update_things_slb_torture_inside,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TRAINING:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_training_corner,update_things_slb_training_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_dungheart_corner,update_things_slb_dungheart_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_workshop_corner,update_things_slb_workshop_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_scavenger_corner,update_things_slb_scavenger_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TEMPLE:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_temple_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_things_slb_room(update_things_slb_graveyard_floor,update_things_slb_graveyard_floor,
        update_things_slb_graveyard_corner,update_things_slb_graveyard_floor,update_things_slb_graveyard_floor,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_HATCHERY:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_hatchery_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_BRIDGE:
      create_things_slb_room_simple(delete_room_things,lvl,tx,ty);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_things_slb_room_simple(update_things_slb_guardpost_floor,lvl,tx,ty);
      break;
    case SLAB_TYPE_BARRACKS:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_barracks_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_LIBRARY:
    case SLAB_TYPE_LAIR:
    default:
      create_things_slb_room_simple(delete_room_things,lvl,tx,ty);
      break;
    }
}

/*
 * Updates things on a slab containing door. If there is no door thing,
 * the function creates it, otherwise updates its parameters.
 * It uses only SLB/OWN entries to determine door position. CLM entries
 * can't be used because they may be not valid.
 */
void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned char door_subtype;
  unsigned short slab=get_tile_slab(lvl,tx,ty);
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
      thing=create_door(lvl, tx*MAP_SUBNUM_X+1, ty*MAP_SUBNUM_Y+1, door_subtype);
    } else
    // and if we found one, modify its parameters
    {
      set_thing_subtype(thing,door_subtype);
      set_thing_subtile(thing,tx*MAP_SUBNUM_X+1,ty*MAP_SUBNUM_Y+1);
      set_door_lock(lvl, thing, get_door_lock(thing));
      set_door_orientation(thing,compute_door_orientation(lvl,thing));
    }
    thing_add(lvl,thing);
}

/*
 * Updates torch things near a slab which should have torches near.
 * If there is no torch thing, the function creates it,
 * otherwise updates its parameters.
 * It uses only SLB/OWN entries to determine torch position. CLM entries
 * can't be used because they may be not valid.
 */
void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_EAST, IDIR_SOUTH, IDIR_WEST, IDIR_NORTH};
  int i;
  short allow_torch[9];
  //Filling matrix
  for (i=0;i<9;i++)
  {
    int cur_tx,cur_ty;
    cur_tx=tx+idir_subtl_x[i]-1;
    cur_ty=ty+idir_subtl_y[i]-1;
    unsigned short slab;
    slab=get_tile_slab(lvl,cur_tx,cur_ty);
    allow_torch[i]=slab_allows_torch(slab);
  }
  // Well, let's just make torch where it can be made
  for (i=0;i<4;i++)
    if (allow_torch[dir_a[i]])
    {
      int cur_tx,cur_ty;
      cur_tx=tx+idir_subtl_x[dir_a[i]]-1;
      cur_ty=ty+idir_subtl_y[dir_a[i]]-1;
      //Check if there already is a light source on the destination slab
      if (find_lit_thing_on_square_radius1(lvl,cur_tx,cur_ty)==NULL)
      {
        int sx,sy;
        sx=cur_tx*3+idir_subtl_x[dir_b[i]];
        sy=cur_ty*3+idir_subtl_y[dir_b[i]];
        update_thing_slb_room_one_item_subtl(lvl,sx,sy,ITEM_SUBTYPE_TORCH,false);
      }
    }
}

/*
 * Updates things on any room slab by calling proper function
 * from these given as parameters.
 */
void create_things_slb_room(cr_tng_func cr_floor,cr_tng_func cr_edge,
        cr_tng_func cr_corner,cr_tng_func cr_inside,cr_tng_func cr_nearinsd,
        struct LEVEL *lvl, int tx, int ty)
{
  const unsigned short dir_a[]={IDIR_NORTH, IDIR_NORTH, IDIR_SOUTH, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_WEST, IDIR_EAST, IDIR_EAST, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  const unsigned short dir_x[]={0, 2, 2, 0};
  const unsigned short dir_y[]={0, 0, 2, 2};
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,NULL,lvl,tx,ty);
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  struct UPOINT_2D corner_pos={1,1};
  //Checking if completely surrounded
  if ((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr))
  {
      int i;
      for (i=0;i<4;i++)
      {
        if ((surr_slb[dir_c[i]]!=slab)||(surr_own[dir_c[i]]!=ownr))
        {
            corner_pos.x=dir_x[i];
            corner_pos.y=dir_y[i];
        }
      }
      // Doing the work
      if  ((corner_pos.x==1)||(corner_pos.y==1))
      {
          corner_pos.x=dir_x[0];
          corner_pos.y=dir_y[0];
          cr_inside(lvl,tx,ty,surr_slb,surr_own,corner_pos);
      } else
      {
          //The 'near inside' columns are usually same that floor,
          //but may differ for rooms with specific corners
          cr_nearinsd(lvl,tx,ty,surr_slb,surr_own,corner_pos);
      }
      return;
  }
  //If not completely, maybe we're surrounded from 3 sides (5 with corners)?
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
  {
      corner_pos.x=dir_x[3];
      corner_pos.y=dir_y[3];
  }
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
  {
      corner_pos.x=dir_x[0];
      corner_pos.y=dir_y[0];
  }
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
  {
      corner_pos.x=dir_x[1];
      corner_pos.y=dir_y[1];
  }
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
  {
      corner_pos.x=dir_x[2];
      corner_pos.y=dir_y[2];
  }
  if  ((corner_pos.x!=1)&&(corner_pos.y!=1))
  {
      cr_edge(lvl,tx,ty,surr_slb,surr_own,corner_pos);
      return;
  }
  //If still nothing, maybe we have same surround from two sides and 1 corner,
  // and another two are surely something else
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))))
  {
      corner_pos.x=dir_x[3];
      corner_pos.y=dir_y[3];
  }
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))))
  {
      corner_pos.x=dir_x[0];
      corner_pos.y=dir_y[0];
  }
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      corner_pos.x=dir_x[1];
      corner_pos.y=dir_y[1];
  }
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      corner_pos.x=dir_x[2];
      corner_pos.y=dir_y[2];
  }

  if  ((corner_pos.x!=1)&&(corner_pos.y!=1))
  {
      cr_corner(lvl,tx,ty,surr_slb,surr_own,corner_pos);
      return;
  }
  corner_pos.x=dir_x[0];
  corner_pos.y=dir_y[0];
  //If nothing found - update as floor of this room
  cr_floor(lvl,tx,ty,surr_slb,surr_own,corner_pos);
}

/*
 * Updates things on any room slab; simplified version, for some easy rooms.
 */
void create_things_slb_room_simple(cr_tng_func cr_any,
        struct LEVEL *lvl, int tx, int ty)
{
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,NULL,lvl,tx,ty);
  struct UPOINT_2D corner_pos={0,0};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Very simple...
  cr_any(lvl,tx,ty,surr_slb,surr_own,corner_pos);
}

void delete_room_things(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    int sx, sy;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
        delete_room_things_subtl(lvl,sx,sy);
      }
}

void delete_room_things_subtl(struct LEVEL *lvl, int sx, int sy)
{
    int last_thing=get_thing_subnums(lvl,sx,sy)-1;
    int i;
    for (i=last_thing; i>=0; i--)
    {
      unsigned char *thing=get_thing(lvl,sx,sy,i);
      if (is_room_inventory(thing))
      {
          //checking surrounding slabs to be sure
          // if we should delete torches
          if (is_torch(thing))
            if (subtl_is_near_tall_slab(lvl,sx,sy))
              continue;
          // Now deleting, without mercy
          thing_del(lvl,sx, sy, i);
      }
    }
}

void update_things_slb_portal_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    int sx, sy, i;
    unsigned char *thing_eff=NULL;
    unsigned char *thing_eff2=NULL;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_inventory(thing))
            {
                unsigned char stype_idx=get_thing_subtype(thing);
                if (is_effectgen(thing)&&(stype_idx==EFCTGEN_SUBTP_ENTRICE)&&(thing_eff==NULL))
                  thing_eff=thing;
                else
                //Let's leave two entrance ice effects - we sometimes use two of them
                if (is_effectgen(thing)&&(stype_idx==EFCTGEN_SUBTP_ENTRICE)&&(thing_eff2==NULL))
                  thing_eff2=thing;
                else
                thing_del(lvl,sx, sy, i);
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_eff==NULL)
    {
      thing_eff=create_effectgen(lvl,sx,sy, EFCTGEN_SUBTP_ENTRICE);
      thing_add(lvl,thing_eff);
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff,get_tile_owner(lvl,tx,ty));
    }
    //Second effect is not auto-created - user must make it
    if (thing_eff2!=NULL)
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff2,get_tile_owner(lvl,tx,ty));
    }
}

unsigned char *update_thing_slb_room_one_central_item(struct LEVEL *lvl, int tx, int ty,
        unsigned char itm_stype_idx,short allow_create)
{
    int sx, sy, i;
    unsigned char *thing_dst=NULL;
    short other_thing_at_center=false;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_inventory(thing))
            {
                unsigned char type_idx=get_thing_type(thing);
                unsigned char stype_idx=get_thing_subtype(thing);
                if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
                {
                  thing_dst=thing;
                } else
                // If we found torch, and we're searching for something else - leave the torch.
                if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(!is_torch_stype(itm_stype_idx)))
                {
                  if ((sx==tx*MAP_SUBNUM_X+1)&&(sy==ty*MAP_SUBNUM_Y+1))
                    other_thing_at_center=true;
                } else
                {
                  thing_del(lvl,sx, sy, i);
                }
            } else
            {
              if ((sx==tx*MAP_SUBNUM_X+1)&&(sy==ty*MAP_SUBNUM_Y+1))
                other_thing_at_center=true;
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_dst==NULL)
    {
      if ((!other_thing_at_center)&&(allow_create))
      {
        thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
        thing_add(lvl,thing_dst);
      }
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_dst,get_tile_owner(lvl,tx,ty));
    }
    return thing_dst;
}

unsigned char *update_thing_slb_room_one_item_subtl(struct LEVEL *lvl, int sx, int sy,
        unsigned char itm_stype_idx, short allow_delete)
{
    int i;
    unsigned char *thing_dst=NULL;
    short other_thing_there=false;
    int last_thing=get_thing_subnums(lvl,sx,sy)-1;
    for (i=last_thing; i>=0; i--)
    {
        unsigned char *thing=get_thing(lvl,sx,sy,i);
        if (is_room_inventory(thing))
        {
            unsigned char type_idx=get_thing_type(thing);
            unsigned char stype_idx=get_thing_subtype(thing);
            if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
            {
                thing_dst=thing;
            } else
            // If we found torch, and we're searching for something else - leave the torch.
            if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(!is_torch_stype(itm_stype_idx)))
            {
                other_thing_there=true;
            } else
            if (allow_delete)
            {
                thing_del(lvl,sx, sy, i);
            }
        } else
        {
            other_thing_there=true;
        }
    }
    if (thing_dst==NULL)
    {
      if (!other_thing_there)
      {
        thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
        thing_add(lvl,thing_dst);
      }
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_dst,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    }
    return thing_dst;
}

void update_things_slb_training_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TRAINPOST,true);
}

void update_things_slb_scavenger_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_SCAVNGEYE,true);
}

void update_things_slb_graveyard_floor(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_GRAVSTONE,true);
}

void update_things_slb_workshop_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_WRKSHPMCH,true);
}

void update_things_slb_torture_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORTURER,true);
}

void update_things_slb_treasure_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_CANDLSTCK,true);
}

void update_things_slb_torture_edge(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORTSPIKE,true);
}

void update_things_slb_training_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_scavenger_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_hatchery_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_temple_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    unsigned char *thing;
    thing=update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TEMPLESTA,true);
    set_thing_subtile_h(thing,2); // Temple floor is higher than ground
}

void update_things_slb_workshop_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_ANVIL,true);
}

void update_things_slb_barracks_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    int sx, sy, i;
    short allow_torch=0;
    if (slab_is_wall(surr_slb[(1)*3+(corner_pos.x)]))
        allow_torch|=1;
    if (slab_is_wall(surr_slb[(corner_pos.y)*3+(1)]))
        allow_torch|=2;
    unsigned char *thing_trch=NULL;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_inventory(thing))
            {
                unsigned char type_idx=get_thing_type(thing);
                unsigned char stype_idx=get_thing_subtype(thing);
                // If we found torch, and that's the first one, and we are allowed to have it - then keep the torch.
                if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(allow_torch>0)&&(thing_trch==NULL))
                {
                  thing_trch=thing;
                  continue;
                } else
                {
                  thing_del(lvl,sx, sy, i);
                }
            }
          }
     }
    if (allow_torch>0)
    {
      if ((allow_torch&1)==1)
      {
        sx=tx*MAP_SUBNUM_X+corner_pos.x;
        sy=ty*MAP_SUBNUM_Y+1;
      } else
      {
        sx=tx*MAP_SUBNUM_X+1;
        sy=ty*MAP_SUBNUM_Y+corner_pos.y;
      }
      if (thing_trch==NULL)
      {
        thing_trch=create_item_adv(lvl,sx,sy,ITEM_SUBTYPE_TORCH);
        thing_add(lvl,thing_trch);
      } else
      {
        set_thing_owner(thing_trch,get_tile_owner(lvl,tx,ty));
        if (allow_torch<3)
          set_thing_subtile(thing_trch,sx,sy);
        //Sensitive tile
        unsigned short sensitile=compute_torch_sensitile(lvl,thing_trch);
        set_thing_sensitile(thing_trch,sensitile);
      }
    }
    sx=tx*MAP_SUBNUM_X+2-corner_pos.x;
    sy=ty*MAP_SUBNUM_Y+2-corner_pos.y;
}

void update_things_slb_dungheart_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    unsigned char flame_stype;
    switch (surr_own[IDIR_CENTR])
    {
    case PLAYER0:
        flame_stype=ITEM_SUBTYPE_HEARTFLMR;
        break;
    case PLAYER1:
        flame_stype=ITEM_SUBTYPE_HEARTFLMB;
        break;
    case PLAYER2:
        flame_stype=ITEM_SUBTYPE_HEARTFLMG;
        break;
    case PLAYER3:
        flame_stype=ITEM_SUBTYPE_HEARTFLMY;
        break;
    case PLAYER_GOOD:
    case PLAYER_UNSET:
    default:
        flame_stype=0;
        break;
    }
    if (flame_stype!=0)
      update_thing_slb_room_one_central_item(lvl,tx,ty,flame_stype,true);
    else
      delete_room_things(lvl,tx,ty,surr_slb,surr_own,corner_pos);
}

void update_things_slb_dungheart_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    unsigned char own=surr_own[IDIR_CENTR];
    if (own>=PLAYERS_COUNT) own=PLAYER_UNSET;
    if (own==PLAYER_UNSET)
    {
      delete_room_things(lvl,tx,ty,surr_slb,surr_own,corner_pos);
      return;
    }
    //Array for storing players heart count
    int hearts[PLAYERS_COUNT];
    int i;
    for (i=0; i < PLAYERS_COUNT; i++)
      hearts[i]=0;
    owned_things_count(hearts,lvl,THING_TYPE_ITEM,ITEM_SUBTYPE_DNHEART);
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_DNHEART,(hearts[own]==0));
}

void update_things_slb_graveyard_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    int sx, sy, i;
    const unsigned char itm_stype_idx=ITEM_SUBTYPE_GRAVSTONE;
    const unsigned char eff_stype_idx=EFCTGEN_SUBTP_DRYICE;
    short allow_torch=0;
    if (slab_is_wall(surr_slb[(1)*3+(corner_pos.x)]))
        allow_torch|=1;
    if (slab_is_wall(surr_slb[(corner_pos.y)*3+(1)]))
        allow_torch|=2;
    unsigned char *thing_dst=NULL;
    unsigned char *thing_eff=NULL;
    unsigned char *thing_trch=NULL;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_inventory(thing))
            {
                unsigned char type_idx=get_thing_type(thing);
                unsigned char stype_idx=get_thing_subtype(thing);
                if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
                  thing_dst=thing;
                else
                if ((type_idx==THING_TYPE_EFFECTGEN)&&(stype_idx==eff_stype_idx)&&(thing_eff==NULL))
                  thing_eff=thing;
                else
                // If we found torch, and that's the first one, and we are allowed to have it - then keep the torch.
                if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(allow_torch>0)&&(thing_trch==NULL))
                {
                  thing_trch=thing;
                  continue;
                } else
                {
                  thing_del(lvl,sx, sy, i);
                }
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_dst==NULL)
    {
      thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
      thing_add(lvl,thing_dst);
    } else
    {
      set_thing_owner(thing_dst,get_tile_owner(lvl,tx,ty));
    }
    if (allow_torch>0)
    {
      if ((allow_torch&1)==1)
      {
        sx=tx*MAP_SUBNUM_X+corner_pos.x;
        sy=ty*MAP_SUBNUM_Y+1;
      } else
      {
        sx=tx*MAP_SUBNUM_X+1;
        sy=ty*MAP_SUBNUM_Y+corner_pos.y;
      }
      if (thing_trch==NULL)
      {
        thing_trch=create_item_adv(lvl,sx,sy,ITEM_SUBTYPE_TORCH);
        thing_add(lvl,thing_trch);
      } else
      {
        set_thing_owner(thing_trch,get_tile_owner(lvl,tx,ty));
        if (allow_torch<3)
          set_thing_subtile(thing_trch,sx,sy);
        //Sensitive tile
        unsigned short sensitile=compute_torch_sensitile(lvl,thing_trch);
        set_thing_sensitile(thing_trch,sensitile);
      }
    }
    sx=tx*MAP_SUBNUM_X+2-corner_pos.x;
    sy=ty*MAP_SUBNUM_Y+2-corner_pos.y;
    if (thing_eff==NULL)
    {
      thing_eff=create_effectgen(lvl,sx,sy,eff_stype_idx);
      thing_add(lvl,thing_eff);
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff,get_tile_owner(lvl,tx,ty));
    }
}

void update_things_slb_guardpost_floor(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
    unsigned char flag_stype;
    switch (surr_own[IDIR_CENTR])
    {
    case PLAYER0:
        flag_stype=ITEM_SUBTYPE_GURDFLAGR;
        break;
    case PLAYER1:
        flag_stype=ITEM_SUBTYPE_GURDFLAGB;
        break;
    case PLAYER2:
        flag_stype=ITEM_SUBTYPE_GURDFLAGG;
        break;
    case PLAYER3:
        flag_stype=ITEM_SUBTYPE_GURDFLAGY;
        break;
    case PLAYER_GOOD:
    case PLAYER_UNSET:
    default:
        flag_stype=ITEM_SUBTYPE_FLAGPOST;
        break;
    }
    unsigned char *thing=update_thing_slb_room_one_central_item(lvl,tx,ty,flag_stype,true);
    set_thing_subtile_h(thing,2);
}

void update_things_slb_prison(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos)
{
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  unsigned char pris_bars[9];
  int i;
  for (i=0;i<9;i++)
    pris_bars[i]=false;
  // Base directions
  if (((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_NORTH]))
    pris_bars[IDIR_NORTH]=true;
  if (((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_EAST]))
    pris_bars[IDIR_EAST]=true;
  if (((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_SOUTH]))
    pris_bars[IDIR_SOUTH]=true;
  if (((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_WEST]))
    pris_bars[IDIR_WEST]=true;
  //Edges
  if ((pris_bars[IDIR_NORTH]==true)||(pris_bars[IDIR_EAST]==true))
    pris_bars[IDIR_NE]=true;
  if ((pris_bars[IDIR_NORTH]==true)||(pris_bars[IDIR_WEST]==true))
    pris_bars[IDIR_NW]=true;
  if ((pris_bars[IDIR_SOUTH]==true)||(pris_bars[IDIR_WEST]==true))
    pris_bars[IDIR_SW]=true;
  if ((pris_bars[IDIR_SOUTH]==true)||(pris_bars[IDIR_EAST]==true))
    pris_bars[IDIR_SE]=true;
  //Creating the bars where needed
  for (i=0;i<9;i++)
  {
    int sx=tx*3+idir_subtl_x[i];
    int sy=ty*3+idir_subtl_y[i];
    if (pris_bars[i])
    {
      //Create/update prison bar
      update_thing_slb_room_one_item_subtl(lvl,sx,sy,ITEM_SUBTYPE_PRISONBAR,true);
    } else
    {
      //Delete unwanted things
      delete_room_things_subtl(lvl,sx,sy);
    }
  }
}

/*
 * Updates position within subtile and height of all things on the whole map - all tiles
 * and subtiles are modified.
 */
void update_obj_subpos_and_height_for_whole_map(struct LEVEL *lvl)
{
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_things_subpos_and_height_for_slab(lvl, i, k);
}

/*
 * Updates position within subtile and height of all things in given map tile coordinates
 * and also updates enties in all neightbour squares
 */
void update_obj_subpos_and_height_for_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_things_subpos_and_height_for_slab(lvl, i, k);
      }
}

/*
 * Updates position within subtile and height of all things
 * in given map tile coordinates range
 */
void update_obj_subpos_and_height_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last)
{
    int i,k;
    for (k=ty_first;k<=ty_last;k++)
      for (i=tx_first;i<=tx_last;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_things_subpos_and_height_for_slab(lvl, i, k);
      }
}

/*
 * Updates position within subtile and height of all things in a tile.
 * Uses Column entries to find valid position.
 */
short update_things_subpos_and_height_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    int sx, sy, i;
    short result=true;
    unsigned short clm_height[9];
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          //Getting height of the adjacent columns
          for (i=0;i<9;i++)
          {
            int cur_sx,cur_sy;
            cur_sx=sx+idir_subtl_x[i]-1;
            cur_sy=sy+idir_subtl_y[i]-1;
            unsigned char *clmentry;
            clmentry=get_subtile_column(lvl,cur_sx,cur_sy);
            clm_height[i]=get_clm_entry_height(clmentry);
          }
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            result&=update_thing_subpos_and_height(clm_height,thing);
          }
     }
    return result;
}

/*
 * Updates position within subtile and height of a thing.
 * Uses Column entries heigh to find valid position.
 */
short update_thing_subpos_and_height(unsigned short *clm_height,unsigned char *thing)
{
    if (thing==NULL) return false;
    unsigned char stype_idx=get_thing_subtype(thing);
    int sx, sy, i;
    sx=get_thing_subtile_x(thing);
    sy=get_thing_subtile_y(thing);
    //Setting things parameters
    if (is_door(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR];
        unsigned char subpos_h=0;
        if (subtl_h<4) { subtl_h=4; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_subtile_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_doorkey(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR]-1;
        unsigned char subpos_h=0;
        if (subtl_h<1) { subtl_h=1; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_subtile_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_heartflame(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR];
        unsigned char subpos_h=0;
        if (subtl_h<1) { subtl_h=1; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_subtile_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_torch(thing))
    {
      const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH,
                                    IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
      const unsigned short dir_a_subx[]={0x040, 0x080, 0x0c0, 0x080,
                                         0x040, 0x0c0, 0x0c0, 0x040};
      const unsigned short dir_a_suby[]={0x080, 0x040, 0x080, 0x0c0,
                                         0x040, 0x040, 0x0c0, 0x0c0};
      unsigned char sub_x=0x080;
      unsigned char sub_y=0x080;
      unsigned char subtl_h=clm_height[IDIR_CENTR]+1;
      unsigned char subpos_h=224;
      //Set the subtile position tall column
      for (i=0;i<8;i++)
        if (clm_height[dir_a[i]]>subtl_h)
        {
          sub_x=dir_a_subx[i];
          sub_y=dir_a_suby[i];
          // If found a position - stop searching
          break;
        }
      // If it is at center - rise it a little higher
      if (((sx%MAP_SUBNUM_X)==1)&&((sy%MAP_SUBNUM_Y)==1))
      {
        subtl_h=clm_height[IDIR_CENTR]+2;
        subpos_h=0;
      }
      if (subtl_h<2) { subtl_h=2; subpos_h=0; }
      if (subtl_h>7) { subtl_h=7; subpos_h=255; }
      set_thing_subtpos(thing,sub_x,sub_y);
      set_thing_subtile_h(thing,subtl_h);
      set_thing_subtpos_h(thing,subpos_h);
    }
    return true;
}

/*
 * Removes all things that usually exist only in specific rooms,
 * and aren't crucial for the game (like gates and dungeon hearts)
 */
void remove_noncrucial_room_things(struct LEVEL *lvl,int tx, int ty)
{
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_inventory(thing) && (!is_dncrucial(thing)))
                thing_del(lvl,sx, sy, i);
          }
     }
}

/*
 * Returns TRUE if subtile with given coordinates is within range of
 * any effect generator on map.
 */
short subtl_in_effectgen_range(struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    const int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    const int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int curr_sx, curr_sy, i;
    for (curr_sx=0; curr_sx < arr_entries_x; curr_sx++)
      for (curr_sy=0; curr_sy < arr_entries_y; curr_sy++)
      {
          int last_thing=get_thing_subnums(lvl,curr_sx,curr_sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,curr_sx,curr_sy,i);
            if (is_effectgen(thing))
              if (subtl_in_thing_range(thing,sx,sy))
                return true;
          }
      }
    return false;
}
