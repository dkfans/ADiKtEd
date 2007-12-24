/*
 * lev_things.h
 */

#ifndef ADIKT_LEVTHINGS_H
#define ADIKT_LEVTHINGS_H

struct LEVEL;

typedef void (*cr_tng_func)(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own);

short things_verify(struct LEVEL *lvl, char *err_msg);

unsigned char *create_door(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx);
unsigned char *create_torch(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx);

short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock);
unsigned char get_door_lock(unsigned char *thing);
unsigned char compute_door_orientation(struct LEVEL *lvl, unsigned char *thing);

void update_obj_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last);
void update_obj_for_square_radius1(struct LEVEL *lvl, int tx, int ty);
void update_obj_for_whole_map(struct LEVEL *lvl);
void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty);
void remove_misplaced_objs_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty);
void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty);

void remove_noncrucial_room_things(int tx, int ty);


#endif // ADIKT_LEVTHINGS_H
