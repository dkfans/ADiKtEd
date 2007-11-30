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

struct LEVEL;

short slab_is_central(struct LEVEL *lvl,int x,int y);
short slabs_verify(struct LEVEL *lvl, char *err_msg);

int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned char slab_type);
void slab_draw_smear(struct LEVEL *lvl,int startx,int starty,int startr,
                     int endx,int endy,int endr,int bend,unsigned char slab_type);
void slab_draw_circle(struct LEVEL *lvl,int x,int y,int rad,unsigned char slab_type);

short slab_is_short(unsigned char slab_type);
short slab_is_door(unsigned char slab_type);
short slab_is_room(unsigned char slab_type);
short slab_is_wall(unsigned char slab_type);
short slab_is_space(unsigned char slab_type);
short slab_is_tall_unclmabl(unsigned char slab_type);
short slab_is_tall(unsigned char slab_type);
short slab_is_short_clmabl(unsigned char slab_type);
short slab_verify_entry(unsigned char slab_type, char *err_msg);

void check_doors(void);

char *get_owner_type_fullname(unsigned char own_idx);
unsigned char get_player_next(unsigned char plyr_idx);
unsigned char get_player_prev(unsigned char plyr_idx);

#endif // ADIKT_OBJSLABS_H
