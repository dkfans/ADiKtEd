/*
 * obj_column_def.h
 *
 * Header file for obj_column_def.c.
 *
 */

#ifndef ADIKT_OBJCOLDEF_H
#define ADIKT_OBJCOLDEF_H

//WIB entry values
#define COLUMN_WIB_STATIC      0x00
#define COLUMN_WIB_SKEW        0x01
#define COLUMN_WIB_ANIMATE     0x02

//WLB entry values
#define TILE_WLB_SOLID       0x00
#define TILE_WLB_LAVA        0x01
#define TILE_WLB_WATER       0x02

struct COLUMN_REC {
    unsigned int use;
    short permanent;
    unsigned short lintel;
    unsigned short height;
    // Solidmask - appears to be a bitmask for which of the cubes are non-zero,
    // with the lowest cube being represented by the least significant bit.
    unsigned int solid;
    // Baseblock - determines the very lowest level (below the lowest cube).
    unsigned int base;
    // Orientation (always 0)
    short orientation;
    // c1 is the lowest square, c8 - the top one.
    unsigned int c[8];
  };

// Functions for maintaining COLUMN_REC structure

struct COLUMN_REC *create_column_rec(void);
struct COLUMN_REC *create_column_recp(void);
void free_column_rec(struct COLUMN_REC *clm_rec);
void clear_clm_entry(unsigned char *clmentry);
void fill_column_rec_simp(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
void fill_column_rec_sim(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
void fill_column_rec_autosh(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int base, 
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7);
void fill_column_rec(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int height,
        const unsigned int solid, const int base, const int orientation,
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7);
unsigned short compute_clm_rec_height(const struct COLUMN_REC *clm_rec);
unsigned short compute_clm_rec_solid(const struct COLUMN_REC *clm_rec);
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2);
short clm_rec_copy(struct COLUMN_REC *dest_rec,const struct COLUMN_REC *src_rec);

// Converting COLUMN_REC to raw column entry (clmentry) and back

void set_clm_entry(unsigned char *clmentry, struct COLUMN_REC *clm_rec);
void get_clm_entry(struct COLUMN_REC *clm_rec, const unsigned char *clmentry);

// Functions for working directly on clmentry, without converting to clm_rec

short compare_column_entries(const unsigned char *clmentry1, const unsigned char *clmentry2);
unsigned int get_clm_entry_use(const unsigned char *clmentry);
unsigned int clm_entry_use_inc(unsigned char *clmentry);
unsigned int clm_entry_use_dec(unsigned char *clmentry);
short get_clm_entry_permanent(const unsigned char *clmentry);
void set_clm_entry_permanent(unsigned char *clmentry,short nperm);
unsigned short get_clm_entry_height(const unsigned char *clmentry);
unsigned short get_clm_entry_solid(const unsigned char *clmentry);
unsigned short get_clm_entry_topcube(const unsigned char *clmentry);


short clm_verify_entry(const unsigned char *clmentry, char *err_msg);
short cube_wib_animate(unsigned int cube);
short is_animated_cube(unsigned int cube);
unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw);
char *get_cube_fullname(unsigned short idx);

// Short columns

void fill_column_path(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_lava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_water(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earthground(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_rockground(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_rock_gndlev(struct COLUMN_REC *clm_rec, unsigned char owner);

// Placing doors

void place_column_doorwood_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doorwood_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doorwood_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doorbrace_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doorbrace_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doorbrace_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_dooriron_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_dooriron_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_dooriron_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doormagic_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doormagic_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_doormagic_c(struct COLUMN_REC *clm_rec, unsigned char owner);

// Unclaimed tall columns

void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gold(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gold_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gold_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gem(struct COLUMN_REC *clm_rec, unsigned char owner);

// Reinforced Walls

void fill_column_wall_centr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_cobblestones_mk(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_redsmbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_redsmbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_drapebrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_drapebrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_drapebrick_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_twinsbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_twinsbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_twinsbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_womanbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_womanbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_womanbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_pairshrbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_pairshrbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_pairshrbrick_c(struct COLUMN_REC *clm_rec, unsigned char owner);

void place_column_wall_drape_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_drape_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_drape_c(struct COLUMN_REC *clm_rec, unsigned char owner);

// Room equipment columns

void fill_column_library_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_temple_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_corner(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_edge(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_dungheart_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_dungheart_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_dungheart_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_univ_stair(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_dungheart_raise(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_portal_edge(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_portal_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_portal_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_portal_step(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_portal_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_hatchery_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_hatchery_edge(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_hatchery_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_lair_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_graveyard_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_barracks_raise(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_barracks_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_barracks_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_training_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_training_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_treasure_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_treasure_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_workshop_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_workshop_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_scavenger_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_scavenger_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_scavenger_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_prison_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_guardpost_floor_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_guardpost_floor_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_guardpost_floor_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_bridge_inside(struct COLUMN_REC *clm_rec, unsigned char owner);

// Room-specific walls

void place_column_wall_portal_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_portal_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_treasure_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_treasure_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_treasure_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_training_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_training_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_training_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_library_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_library_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_library_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_scavenger_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_scavenger_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_scavenger_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_graveyard_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_graveyard_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_graveyard_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_lair_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_workshop_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_workshop_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_workshop_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_prison_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_prison_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_prison_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_torture_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_torture_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_torture_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_barracks_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_barracks_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_barracks_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_temple_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_temple_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_temple_c(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_hatchery_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_hatchery_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_hatchery_c(struct COLUMN_REC *clm_rec, unsigned char owner);

#endif // ADIKT_OBJCOLDEF_H
