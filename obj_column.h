/*
 * obj_column.h
 *
 * Header file for obj_column.c.
 *
 */

#ifndef ADIKT_OBJCOLMN_H
#define ADIKT_OBJCOLMN_H

#define CLM_BOX_EMPTY1         0x0000
  //Standard, digable earth
#define CLM_BOX_EARTHSTD1      0x0001
#define CLM_BOX_EARTHSTD2      0x0002
#define CLM_BOX_EARTHSTD3      0x0003
  //Tamped earth; it looks like digable from top,
  // but is different in profile
#define CLM_BOX_EARTHTAMPED1   0x0004
#define CLM_BOX_EARTHTAMPED2   0x0005
#define CLM_BOX_EARTHTAMPED3   0x0006

//TODO: add defines here

#define CLM_BOX_LAVA1          0x0028
#define CLM_BOX_LAVA2          0x0029

//TODO: add defines here

  // Rock with a little of something else on bottom
#define CLM_BOX_ROCKNEARDARK   0x0198
#define CLM_BOX_ROCKNEARLAVA   0x0199
  // Rock floors, color visible on sides
#define CLM_BOX_ROCKFLOORBLUE1 0x019a
#define CLM_BOX_ROCKFLOORBLUE2 0x019b
#define CLM_BOX_ROCKFLOORBLUE3 0x019c
#define CLM_BOX_ROCKFLOORGREY1 0x019d
#define CLM_BOX_ROCKFLOORGREY2 0x019e
#define CLM_BOX_ROCKFLOORGREY3 0x019f
#define CLM_BOX_ROCKFLOORYELL1 0x01a0
#define CLM_BOX_ROCKFLOORYELL2 0x01a1
#define CLM_BOX_ROCKFLOORGREY4 0x01a2
#define CLM_BOX_ROCKFLOORGREY5 0x01a3
#define CLM_BOX_ROCKFLOORWHITE 0x01a4
#define CLM_BOX_ROCFLOORKRED   0x01a5
  // Rocks with glassy "eye" at top
#define CLM_BOX_EYEDROCKRED    0x017e
#define CLM_BOX_EYEDROCKBLUE   0x01a6
#define CLM_BOX_EYEDROCKGREEN  0x01a7
#define CLM_BOX_EYEDROCKYELLOW 0x01a8
#define CLM_BOX_EYEDROCKBLINK  0x01a9
#define CLM_BOX_EYEDROCKWHITE  0x01aa
  //Various color rocks
#define CLM_BOX_ROCKGREEN      0x01ab
#define CLM_BOX_ROCKBLUE       0x01ac
#define CLM_BOX_ROCKYELLOW     0x01ad
#define CLM_BOX_ROCKBLACK      0x01ae
#define CLM_BOX_ROCKWHITE      0x01af
#define CLM_BOX_ROCKRED        0x01b0
  //Unpenetrable wall
#define CLM_BOX_ROCK           0x01b1
  //Wall of stone
#define CLM_BOX_STONEWALL1     0x01b2
#define CLM_BOX_STONEWALL2     0x01b3
  //Wood, for briges, but maybe other things
#define CLM_BOX_WOOD1          0x01b4
#define CLM_BOX_WOOD2          0x01b5
#define CLM_BOX_WOOD3          0x01b6
#define CLM_BOX_WOOD4          0x01b7
#define CLM_BOX_WOOD5          0x01b8
  //The purple gems
#define CLM_BOX_GEMS1          0x01b9
#define CLM_BOX_GEMS2          0x01ba
#define CLM_BOX_GEMS3          0x01bb
#define CLM_BOX_GEMS4          0x01bc

#define CLM_BOX_MAX_INDEX      511

//WIB entry values
#define COLUMN_WIB_STATIC      0x00
#define COLUMN_WIB_SKEW        0x01
#define COLUMN_WIB_ANIMATE     0x02

// Direction indices of the 9-element arrays, like slab columns
//  or surround variable
#define IDIR_CENTR 4
#define IDIR_NW    0
#define IDIR_NORTH 1
#define IDIR_NE    2
#define IDIR_EAST  5
#define IDIR_SE    8
#define IDIR_SOUTH 7
#define IDIR_SW    6
#define IDIR_WEST  3
// Now explaining the constants as elements of an array:
//       IDIR_NW     IDIR_NORTH    IDIR_NE
//       IDIR_WEST   IDIR_CENTR    IDIR_EAST
//       IDIR_SW     IDIR_SOUTH    IDIR_SE

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

typedef void (*cr_clm_func)(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

struct COLUMN_REC *create_column_rec();
void free_column_rec(struct COLUMN_REC *clm_rec);
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

short surrnd_not_enemy(unsigned char *surr_own, short direction);

#endif // ADIKT_OBJCOLMN_H
