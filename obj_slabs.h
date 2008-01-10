/*
 * obj_slabs.h
 */

#ifndef ADIKT_OBJSLABS_H
#define ADIKT_OBJSLABS_H

#define SLAB_TYPE_ROCK         0x00
#define SLAB_TYPE_GOLD         0x01
#define SLAB_TYPE_EARTH        0x02
#define SLAB_TYPE_TORCHDIRT    0x03
#define SLAB_TYPE_WALLDRAPE    0x04
#define SLAB_TYPE_WALLTORCH    0x05
#define SLAB_TYPE_WALLWTWINS   0x06
#define SLAB_TYPE_WALLWWOMAN   0x07
#define SLAB_TYPE_WALLPAIRSHR  0x08
#define SLAB_TYPE_PATH         0x0a
#define SLAB_TYPE_CLAIMED      0x0b
#define SLAB_TYPE_LAVA         0x0c
#define SLAB_TYPE_WATER        0x0d
#define SLAB_TYPE_PORTAL       0x0e
#define SLAB_TYPE_TREASURE     0x10
#define SLAB_TYPE_LIBRARY      0x12
#define SLAB_TYPE_PRISONCASE   0x14
#define SLAB_TYPE_TORTURE      0x16
#define SLAB_TYPE_TRAINING     0x18
#define SLAB_TYPE_DUNGHEART    0x1a
#define SLAB_TYPE_WORKSHOP     0x1c
#define SLAB_TYPE_SCAVENGER    0x1e
#define SLAB_TYPE_TEMPLE       0x20
#define SLAB_TYPE_GRAVEYARD    0x22
#define SLAB_TYPE_HATCHERY     0x24
#define SLAB_TYPE_LAIR         0x26
#define SLAB_TYPE_BARRACKS     0x28
#define SLAB_TYPE_DOORWOOD1    0x2a
#define SLAB_TYPE_DOORWOOD2    0x2b
#define SLAB_TYPE_DOORBRACE1   0x2c
#define SLAB_TYPE_DOORBRACE2   0x2d
#define SLAB_TYPE_DOORIRON1    0x2e
#define SLAB_TYPE_DOORIRON2    0x2f
#define SLAB_TYPE_DOORMAGIC1   0x30
#define SLAB_TYPE_DOORMAGIC2   0x31
#define SLAB_TYPE_BRIDGE       0x33
#define SLAB_TYPE_GEMS         0x34
#define SLAB_TYPE_GUARDPOST    0x35

#define PLAYERS_COUNT          6
#define PLAYER0                0x00
#define PLAYER1                0x01
#define PLAYER2                0x02
#define PLAYER3                0x03
#define PLAYER_GOOD            0x04
#define PLAYER_UNSET           0x05
#define PLAYER_ALL             0x06  //used in scripts only

#define SLB_UNKN_LTEXT "Unknown slab"
#define SLB_ROCK_LTEXT "Rock"
#define SLB_GOLD_LTEXT "Gold"
#define SLB_EARTH_LTEXT "Earth"
#define SLB_TORCHDIRT_LTEXT "Torch plate earth"
#define SLB_WALLDRAPE_LTEXT "Wall with drape"
#define SLB_WALLTORCH_LTEXT "Wall with torch"
#define SLB_WALLWTWINS_LTEXT "Wall with twins"
#define SLB_WALLWWOMAN_LTEXT "Wall with woman"
#define SLB_WALLPAIRSHR_LTEXT "Wall w/pair shag."
#define SLB_PATH_LTEXT "Path"
#define SLB_CLAIMED_LTEXT "Claimed ground"
#define SLB_LAVA_LTEXT "Lava"
#define SLB_WATER_LTEXT "Water"
#define SLB_PORTAL_LTEXT "Entrance"
#define SLB_TREASURE_LTEXT "Treasure room"
#define SLB_LIBRARY_LTEXT "Library"
#define SLB_PRISON_LTEXT "Prison"
#define SLB_TORTURE_LTEXT "Torture chamber"
#define SLB_TRAINING_LTEXT "Training room"
#define SLB_DUNGHEART_LTEXT "Dungeon heart"
#define SLB_WORKSHOP_LTEXT "Workshop"
#define SLB_SCAVENGER_LTEXT "Scavenger room"
#define SLB_TEMPLE_LTEXT "Temple"
#define SLB_GRAVEYARD_LTEXT "Graveyard"
#define SLB_HATCHERY_LTEXT "Hatchery"
#define SLB_LAIR_LTEXT "Lair"
#define SLB_BARRACKS_LTEXT "Barracks"
#define SLB_DOORWOOD_LTEXT "Wooden door"
#define SLB_DOORBRACE_LTEXT "Braced door"
#define SLB_DOORIRON_LTEXT "Iron door"
#define SLB_DOORMAGIC_LTEXT "Magic door"
#define SLB_BRIDGE_LTEXT "Bridge"
#define SLB_GEMS_LTEXT "Gems"
#define SLB_GUARDPOST_LTEXT "Guard post"

struct LEVEL;

short slab_is_central(struct LEVEL *lvl,int x,int y);
short slabs_verify(struct LEVEL *lvl, char *err_msg);

int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned short slab_type);
void slab_draw_smear(struct LEVEL *lvl,int startx,int starty,int startr,
                     int endx,int endy,int endr,int bend,unsigned short slab_type);
void slab_draw_circle(struct LEVEL *lvl,int x,int y,int rad,unsigned short slab_type);

short slab_is_short(unsigned short slab_type);
short slab_is_door(unsigned short slab_type);
short slab_is_room(unsigned short slab_type);
short slab_is_wall(unsigned short slab_type);
short slab_is_wealth(unsigned short slab_type);
short slab_is_space(unsigned short slab_type);
short slab_is_tall_unclmabl(unsigned short slab_type);
short slab_is_tall(unsigned short slab_type);
short slab_is_short_clmabl(unsigned short slab_type);
short slab_is_short_unclmabl(unsigned short slab_type);
short slab_is_clmabl(unsigned short slab_type);
short slab_is_claimedgnd(unsigned short slab_type);
short slab_needs_adjacent_torch(unsigned short slab_type);
short slab_verify_entry(unsigned short slab_type, char *err_msg);
char *get_slab_fullname(unsigned short slb_type);

short subtl_is_near_tall_slab(struct LEVEL *lvl,unsigned int sx,unsigned int sy);

char *get_owner_type_fullname(unsigned char own_idx);
unsigned char get_owner_next(unsigned char plyr_idx);
unsigned char get_owner_prev(unsigned char plyr_idx);
void update_slab_owners(struct LEVEL *lvl);

#endif // ADIKT_OBJSLABS_H
