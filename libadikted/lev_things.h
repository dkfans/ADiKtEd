/******************************************************************************/
// lev_things.h - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from lev_things.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_LEVTHINGS_H
#define ADIKT_LEVTHINGS_H

struct LEVEL;
struct IPOINT_2D;

#include "globals.h"

typedef void (*cr_tng_func)(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,const struct UPOINT_2D corner_pos);

DLLIMPORT short things_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);

DLLIMPORT char *get_search_tngtype_name(unsigned short idx);
DLLIMPORT is_thing_subtype get_search_tngtype_func(unsigned short idx);
DLLIMPORT char *get_search_objtype_name(unsigned short idx);
DLLIMPORT unsigned short get_search_objtype_count(void);
DLLIMPORT unsigned char *find_next_object_on_map(struct LEVEL *lvl, int *tx, int *ty, unsigned short srch_idx);
DLLIMPORT short subtl_in_effectgen_range(struct LEVEL *lvl,unsigned int sx,unsigned int sy);

DLLIMPORT long get_nearest_thing_idx(struct LEVEL *lvl,
    const unsigned int ssx,const unsigned int ssy,
    unsigned int *sx,unsigned int *sy,unsigned int *num);
DLLIMPORT long get_nearest_object_idx(struct LEVEL *lvl,
    const unsigned int ssx,const unsigned int ssy,
    unsigned int *sx,unsigned int *sy,unsigned int *z);

DLLIMPORT unsigned char *create_door(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx);
DLLIMPORT unsigned char *create_effectgen(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx);
DLLIMPORT unsigned char *create_creature(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx);
DLLIMPORT unsigned char *create_trap(const struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned char stype_idx);
DLLIMPORT unsigned char *create_item_adv(const struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx);

//Lower level functions, used by create_item_adv
DLLIMPORT unsigned char *create_torch(const struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx);
DLLIMPORT unsigned char *create_doorkey(const struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx);

DLLIMPORT short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock);
DLLIMPORT unsigned char get_door_lock(const unsigned char *thing);
DLLIMPORT unsigned char compute_door_orientation(const struct LEVEL *lvl, unsigned char *thing);
DLLIMPORT unsigned short compute_torch_sensitile(const struct LEVEL *lvl, unsigned char *thing);
DLLIMPORT unsigned short compute_effectgen_sensitile(const struct LEVEL *lvl, unsigned char *thing);
DLLIMPORT unsigned short compute_item_sensitile(const struct LEVEL *lvl, unsigned char *thing);
DLLIMPORT unsigned short get_free_indexedthing_number(const struct LEVEL *lvl);
DLLIMPORT short owned_things_count(int *count,struct LEVEL *lvl,
    unsigned char type_idx,unsigned char stype_idx);
DLLIMPORT unsigned char *find_lit_thing_on_square_radius1(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT unsigned char *find_thing_on_tile(struct LEVEL *lvl, int tx, int ty, is_thing_subtype check_func);
DLLIMPORT unsigned char *find_next_thing_on_map(struct LEVEL *lvl, int *tx, int *ty, is_thing_subtype check_func);
DLLIMPORT unsigned char *find_next_actnpt_on_map(struct LEVEL *lvl, int *tx, int *ty);
DLLIMPORT unsigned char *find_next_stlight_on_map(struct LEVEL *lvl, int *tx, int *ty);

DLLIMPORT unsigned short get_free_herogate_number(const struct LEVEL *lvl);
DLLIMPORT unsigned short get_free_herogate_number_next(const struct LEVEL *lvl,const unsigned short start);
DLLIMPORT unsigned short get_free_herogate_number_prev(const struct LEVEL *lvl,const unsigned short start);
DLLIMPORT short create_herogate_number_used_arr(const struct LEVEL *lvl,unsigned char **used,unsigned int *used_size);

// Updating objects functions
DLLIMPORT void update_obj_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
DLLIMPORT void update_obj_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_obj_for_whole_map(struct LEVEL *lvl);
DLLIMPORT void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void remove_misplaced_objs_on_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty);

//Functions for updating objects subtile position and height
DLLIMPORT void update_obj_subpos_and_height_for_whole_map(struct LEVEL *lvl);
DLLIMPORT void update_obj_subpos_and_height_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void update_obj_subpos_and_height_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
DLLIMPORT short update_things_subpos_and_height_for_slab(struct LEVEL *lvl, int tx, int ty);
DLLIMPORT short update_thing_subpos_and_height(unsigned short *clm_height,unsigned char *thing);
DLLIMPORT void remove_automade_obj_for_whole_map(struct LEVEL *lvl);
DLLIMPORT void remove_automade_obj_for_slab(struct LEVEL *lvl, int tx, int ty);

//Updating room things - lower level function
DLLIMPORT void create_things_slb_room(cr_tng_func cr_floor,cr_tng_func cr_edge,
        cr_tng_func cr_corner,cr_tng_func cr_inside,cr_tng_func cr_nearinsd,
        struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void create_things_slb_room_simple(cr_tng_func cr_any,
        struct LEVEL *lvl, int tx, int ty);
DLLIMPORT void delete_room_things(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
DLLIMPORT void delete_room_things_subtl(struct LEVEL *lvl, int sx, int sy);
unsigned char *update_thing_slb_room_one_central_item(struct LEVEL *lvl, int tx, int ty,
        unsigned char itm_stype_idx,short allow_create);
unsigned char *update_thing_slb_room_one_item_subtl(struct LEVEL *lvl, int sx, int sy,
        unsigned char itm_stype_idx, short torch_mode);
void update_things_slb_portal_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_treasure_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_training_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_scavenger_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_graveyard_floor(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_graveyard_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_workshop_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_torture_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_torture_edge(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_training_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_scavenger_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_hatchery_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_barracks_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_temple_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_workshop_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_dungheart_corner(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_dungheart_inside(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_guardpost_floor(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);
void update_things_slb_prison(struct LEVEL *lvl, const int tx, const int ty,
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const struct UPOINT_2D corner_pos);

DLLIMPORT void remove_noncrucial_room_things(struct LEVEL *lvl, int tx, int ty);


#endif // ADIKT_LEVTHINGS_H
