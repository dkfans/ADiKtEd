/*
 * lev_things.h
 */

#ifndef ADIKT_LEVTHINGS_H
#define ADIKT_LEVTHINGS_H

struct LEVEL;

typedef short (*is_thing_subtype)(unsigned char *thing);
typedef unsigned char (*thing_subtype_switch)(unsigned char stype_idx);

typedef void (*cr_tng_func)(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);

short things_verify(struct LEVEL *lvl, char *err_msg);

unsigned char *create_door(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx);
unsigned char *create_roomeffect(unsigned int sx, unsigned int sy, unsigned char stype_idx);
unsigned char *create_creature(unsigned int sx, unsigned int sy, unsigned char stype_idx);
unsigned char *create_trap(unsigned int sx, unsigned int sy, unsigned char stype_idx);
unsigned char *create_item_adv(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx);

//Lower level functions, used by create_item_adv
unsigned char *create_torch(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx);
unsigned char *create_doorkey(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx);

short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock);
unsigned char get_door_lock(unsigned char *thing);
unsigned char compute_door_orientation(struct LEVEL *lvl, unsigned char *thing);
unsigned short compute_torch_sensitile(struct LEVEL *lvl, unsigned char *thing);
unsigned short compute_roomeffect_sensitile(struct LEVEL *lvl, unsigned char *thing);
short owned_things_count(int *count,struct LEVEL *lvl,
    unsigned char type_idx,unsigned char stype_idx);
unsigned char *find_lit_thing_on_square_radius1(struct LEVEL *lvl, int tx, int ty);
unsigned char *find_thing_on_tile(struct LEVEL *lvl,is_thing_subtype check_func, int tx, int ty);

// Updating objects functions
void update_obj_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
void update_obj_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
void update_obj_for_whole_map(struct LEVEL *lvl);
void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty);
void remove_misplaced_objs_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty);

//Functions for updating objects subtile position and height
void update_obj_subpos_and_height_for_whole_map(struct LEVEL *lvl);
void update_obj_subpos_and_height_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
void update_obj_subpos_and_height_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
short update_things_subpos_and_height_for_slab(struct LEVEL *lvl, int tx, int ty);
short update_thing_subpos_and_height(unsigned short *clm_height,unsigned char *thing);

//Updating room things - lower level function
void create_things_slb_room(cr_tng_func cr_floor,cr_tng_func cr_edge,
        cr_tng_func cr_corner,cr_tng_func cr_inside,cr_tng_func cr_nearinsd,
        struct LEVEL *lvl, int tx, int ty);
void create_things_slb_room_simple(cr_tng_func cr_any,
        struct LEVEL *lvl, int tx, int ty);
void delete_room_things(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void delete_room_things_subtl(struct LEVEL *lvl, int sx, int sy);
unsigned char *update_thing_slb_room_one_central_item(struct LEVEL *lvl, int tx, int ty,
        unsigned char itm_stype_idx,short allow_create);
unsigned char *update_thing_slb_room_one_item_subtl(struct LEVEL *lvl, int sx, int sy,
        unsigned char itm_stype_idx, short torch_mode);
void update_things_slb_portal_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_treasure_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_training_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_scavenger_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_graveyard_floor(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_graveyard_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_workshop_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_torture_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_torture_edge(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_training_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_scavenger_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_hatchery_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_temple_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_workshop_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_dungheart_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_dungheart_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_guardpost_floor(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);
void update_things_slb_prison(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);

void remove_noncrucial_room_things(int tx, int ty);


#endif // ADIKT_LEVTHINGS_H
