/*
 * lev_data.h header file for lev_data.c
 */

#ifndef ADIKT_LEVDATA_H
#define ADIKT_LEVDATA_H

// Map size definitions

#define MAP_SIZE_X 85
#define MAP_SIZE_Y 85
#define MAP_SUBNUM_X 3
#define MAP_SUBNUM_Y 3
#define MAP_MAXINDEX_X MAP_SIZE_X-1
#define MAP_MAXINDEX_Y MAP_SIZE_Y-1
#define COLUMN_ENTRIES 2048

#define OBJECT_TYPE_NONE   0
#define OBJECT_TYPE_ACTNPT 1
#define OBJECT_TYPE_THING  2

// Note we assume there will be less than MAX_ACTIONS of action points.
// I think this should do... the most DK uses is 13
#define MAX_ACTIONS 1024

//Disk files entries

#define SIZEOF_DK_TNG_REC 21
#define SIZEOF_DK_CLM_REC 24
#define SIZEOF_DK_APT_REC 21  //change after separating from tng
#define SIZEOF_DK_LGT_REC 20

typedef struct {
    unsigned char data[20];
  } DK_LGT_REC;

typedef struct {
    int hero_gates_count;
  } LEVSTATS;

struct LEVEL {
    //map file name
    char *fname;
    //Slab file - tile type definitions, size MAP_SIZE_Y x MAP_SIZE_X
    unsigned char **slb;
    //Owners file - tile owner index, size MAP_SIZE_Y x MAP_SIZE_X
    unsigned char **own;
    //Vibration file - subtile animation indices, size arr_entries_y x arr_entries_x
    unsigned char **wib;
    //WLB file - some additional info about water and lava tiles,
    // size MAP_SIZE_Y x MAP_SIZE_X
    unsigned char **wlb;
    //Column file - constant-size array of entries used for displaying tiles,
    // size COLUMN_ENTRIES x SIZEOF_DK_CLM_REC
    unsigned char **clm;
    //Texture information file - one byte file, identifies texture pack index
    unsigned char inf;
    //Script text - a text file containing level parameters as editable script;
    // number of lines and file size totally variable
    unsigned char **txt;
    unsigned int txt_lines_count;
    //Light file - contains static light definitions
    DK_LGT_REC **lgt;
    unsigned int lgt_enties_count;

    // Note that I store action points as things in here, and separate them
    // out at save time. This makes lots of things a lot easier

    // Which action point numbers are taken, and where their data is
    // Note: we assume there will be less than 1024 action points. I think
    // this should do... the most DK uses is 13
//    unsigned char **apt;
    unsigned char ****apt_lookup; // Index to action points, by subtile
    unsigned short **apt_subnums; // Number of action points in a subtile
    unsigned int apt_total_count; // Total number of action points

    unsigned char ****tng_lookup; // Index to things, by subtile
    unsigned short **tng_subnums; // Number of things in a subtile
    unsigned int tng_total_count; // Number of things in total

    unsigned short **tng_apt_nums;    // Number of things and actions in a tile

    // Exceptionally grotty hack - we never need the actual data
    // stored in the .dat file, only what the high and low bytes should
    // be. So long as we remember this when we generate the "standard"
    // dungeon things, we'll be fine
    unsigned char **dat_low;
    unsigned char **dat_high;

    LEVSTATS stats;
  };

// creates object for storing map
short level_init();
// frees object for storing map
short level_deinit();

short level_clear(struct LEVEL *lvl);
short level_clear_tng(struct LEVEL *lvl);

short level_free();
short level_free_tng(struct LEVEL *lvl);

short level_verify(struct LEVEL *lvl, char *actn_name);
short level_verify_struct(struct LEVEL *lvl, char *err_msg);
short actnpts_verify(struct LEVEL *lvl, char *err_msg);
short level_verify_logic(struct LEVEL *lvl, char *err_msg);
void start_new_map(struct LEVEL *lvl);
void free_map(void);

char *get_thing(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
void thing_add(struct LEVEL *lvl,unsigned char *thing);
void thing_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
unsigned int get_thing_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);

char *get_actnpt(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
void actnpt_add(struct LEVEL *lvl,unsigned char *actnpt);
void actnpt_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
unsigned int get_actnpt_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);

short get_object_type(struct LEVEL *lvl, unsigned int x, unsigned int y, unsigned int z);
char *get_object(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z);
void object_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z);
unsigned int get_object_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);
unsigned int get_object_tilnums(struct LEVEL *lvl,unsigned int x,unsigned int y);

short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
void set_subtl_wib(struct LEVEL *lvl, short nval, unsigned int sx, unsigned int sy);
short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty);
void set_tile_wlb(struct LEVEL *lvl, short nval, unsigned int tx, unsigned int ty);

extern struct LEVEL *lvl;

#endif // ADIKT_LEVDATA_H
