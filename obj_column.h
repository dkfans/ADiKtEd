/*
 * obj_column.h
 *
 * Header file for obj_column.c.
 *
 */

#ifndef ADIKT_OBJCOLMN_H
#define ADIKT_OBJCOLMN_H

#define CUBE_EMPTY1         0x0000
  //Standard, digable earth
#define CUBE_EARTHSTD1      0x0001
#define CUBE_EARTHSTD2      0x0002
#define CUBE_EARTHSTD3      0x0003
  //Tamped earth; it looks like digable from top,
  // but is different in profile
#define CUBE_EARTHTAMPED1   0x0004
#define CUBE_EARTHTAMPED2   0x0005
#define CUBE_EARTHTAMPED3   0x0006

//TODO: add defines here

#define CUBE_LAVA1          0x0028
#define CUBE_LAVA2          0x0029

//TODO: add defines here

  // Rock with a little of something else on bottom
#define CUBE_ROCKNEARDARK   0x0198
#define CUBE_ROCKNEARLAVA   0x0199
  // Rock floors, color visible on sides
#define CUBE_ROCKFLOORBLUE1 0x019a
#define CUBE_ROCKFLOORBLUE2 0x019b
#define CUBE_ROCKFLOORBLUE3 0x019c
#define CUBE_ROCKFLOORGREY1 0x019d
#define CUBE_ROCKFLOORGREY2 0x019e
#define CUBE_ROCKFLOORGREY3 0x019f
#define CUBE_ROCKFLOORYELL1 0x01a0
#define CUBE_ROCKFLOORYELL2 0x01a1
#define CUBE_ROCKFLOORGREY4 0x01a2
#define CUBE_ROCKFLOORGREY5 0x01a3
#define CUBE_ROCKFLOORWHITE 0x01a4
#define CUBE_ROCFLOORKRED   0x01a5
  // Rocks with glassy "eye" at top
#define CUBE_EYEDROCKRED    0x017e
#define CUBE_EYEDROCKBLUE   0x01a6
#define CUBE_EYEDROCKGREEN  0x01a7
#define CUBE_EYEDROCKYELLOW 0x01a8
#define CUBE_EYEDROCKBLINK  0x01a9
#define CUBE_EYEDROCKWHITE  0x01aa
  //Various color rocks
#define CUBE_ROCKGREEN      0x01ab
#define CUBE_ROCKBLUE       0x01ac
#define CUBE_ROCKYELLOW     0x01ad
#define CUBE_ROCKBLACK      0x01ae
#define CUBE_ROCKWHITE      0x01af
#define CUBE_ROCKRED        0x01b0
  //Unpenetrable wall
#define CUBE_ROCK           0x01b1
  //Wall of stone
#define CUBE_STONEWALL1     0x01b2
#define CUBE_STONEWALL2     0x01b3
  //Wood, for briges, but maybe other things
#define CUBE_WOOD1          0x01b4
#define CUBE_WOOD2          0x01b5
#define CUBE_WOOD3          0x01b6
#define CUBE_WOOD4          0x01b7
#define CUBE_WOOD5          0x01b8
  //The purple gems
#define CUBE_GEMS1          0x01b9
#define CUBE_GEMS2          0x01ba
#define CUBE_GEMS3          0x01bb
#define CUBE_GEMS4          0x01bc

#define CUBE_MAX_INDEX         511

#define CUST_CLM_GEN_MAX_INDEX  36

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

struct DK_CUSTOM_CLM {
    int sx;
    int sy;
    unsigned short wib_val;
    struct COLUMN_REC *rec;
  };

typedef void (*cr_clm_func)(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

struct COLUMN_REC *create_column_rec();
void free_column_rec(struct COLUMN_REC *clm_rec);
short clm_rec_copy(struct COLUMN_REC *dest_rec,const struct COLUMN_REC *src_rec);
void fill_column_rec_sim(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7);
void fill_column_rec_simp(struct COLUMN_REC *clm_rec, unsigned int use, int base,
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
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2);
short compare_column_entries(const unsigned char *clmentry1, const unsigned char *clmentry2);

unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw);
short column_wib_animate(unsigned int clm);

void set_clm_entry(unsigned char *clmentry, struct COLUMN_REC *clm_rec);
void get_clm_entry(struct COLUMN_REC *clm_rec, const unsigned char *clmentry);
unsigned int clm_entry_use_inc(unsigned char *clmentry);
unsigned int clm_entry_use_dec(unsigned char *clmentry);
unsigned int get_clm_entry_use(const unsigned char *clmentry);
void set_clm_entry_permanent(unsigned char *clmentry,short nperm);
short get_clm_entry_permanent(const unsigned char *clmentry);
unsigned short compute_clm_rec_solid(const struct COLUMN_REC *clm_rec);
unsigned short compute_clm_rec_height(const struct COLUMN_REC *clm_rec);
unsigned short get_clm_entry_height(const unsigned char *clmentry);
unsigned short get_clm_entry_solid(const unsigned char *clmentry);
short clm_verify_entry(const unsigned char *clmentry, char *err_msg);

char *get_custom_column_fullname(unsigned short idx);
short fill_custom_column_data(unsigned short idx,struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
unsigned char *surr_tng_find(unsigned char **surr_tng,unsigned char type_idx);

void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_fulldirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_walltorch(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallwwoman(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallpairshr(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void modify_frail_columns(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
short modify_liquid_surrounding(struct COLUMN_REC *clm_recs[9],unsigned char *surr_slb,
        short liq_level,unsigned short water_cube,unsigned short lava_cube);
short modify_liquid_surrounding_advncd(struct COLUMN_REC *clm_recs[9],
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const short liq_level,const unsigned short *water_cube,const unsigned short *lava_cube);
unsigned short *get_room_corner_direction_indices(unsigned char *surr_slb,unsigned char *surr_own);
unsigned short *get_room_edge_direction_indices(unsigned char *surr_slb,unsigned char *surr_own);

void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void fill_columns_slb_roomrelief(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
short fill_side_columns_room_relief(struct COLUMN_REC *clm_reca,struct COLUMN_REC *clm_recb,
    struct COLUMN_REC *clm_recc,unsigned short room_slab,unsigned char owner, short corner, short edge);

void create_columns_slb_room(cr_clm_func cr_floor,cr_clm_func cr_edge,
        cr_clm_func cr_corner,cr_clm_func cr_inside,cr_clm_func cr_nearinsd,
        struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_slb_portal_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_nearinsd(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_slb_door_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

//User columns - used only in "manual columns" function, not as standard ones
void create_columns_slb_thingems_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_rock_gndlev(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

// Functions for preparing single CoLuMns
void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_lava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_water(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gem(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_gold(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gold_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_gold_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_earth_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_path(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_nearwater(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_claimedgnd_nearlava(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_rock_gndlev(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_wall_centr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_wall_cobblestones_mk(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_redsmbrick_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, unsigned char owner);
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

void fill_column_library_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_library_bookcase_c(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_dungheart_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_dungheart_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_dungheart_inside(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_dungheart_raise(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_univ_stair(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_temple_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_edge(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_temple_corner(struct COLUMN_REC *clm_rec, unsigned char owner);

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

void fill_column_barracks_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_barracks_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);
void place_column_barracks_raise(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_training_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_training_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_treasure_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_treasure_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_workshop_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_workshop_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_scavenger_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_scavenger_floor(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_scavenger_pillar(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_prison_inside(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_guardpost_floor_a(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_guardpost_floor_b(struct COLUMN_REC *clm_rec, unsigned char owner);
void fill_column_guardpost_floor_c(struct COLUMN_REC *clm_rec, unsigned char owner);

void fill_column_bridge_inside(struct COLUMN_REC *clm_rec, unsigned char owner);

short surrnd_not_enemy(unsigned char *surr_own, short direction);

#endif // ADIKT_OBJCOLMN_H
