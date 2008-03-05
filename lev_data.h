/*
 * lev_data.h header file for lev_data.c
 */

#ifndef ADIKT_LEVDATA_H
#define ADIKT_LEVDATA_H

// Map size definitions

#define MAP_SIZE_X 85
#define MAP_SIZE_Y 85
#define MAP_SIZE_H 1
#define MAP_SUBNUM_H 8
#define MAP_SUBNUM_X 3
#define MAP_SUBNUM_Y 3
#define MAP_SUBTOTAL_X (MAP_SIZE_X*MAP_SUBNUM_X)
#define MAP_SUBTOTAL_Y (MAP_SIZE_Y*MAP_SUBNUM_Y)
#define MAP_MAXINDEX_X MAP_SIZE_X-1
#define MAP_MAXINDEX_Y MAP_SIZE_Y-1
#define COLUMN_ENTRIES 2048

#define OBJECT_TYPE_NONE     0
#define OBJECT_TYPE_ACTNPT   1
#define OBJECT_TYPE_THING    2
#define OBJECT_TYPE_STLIGHT  3
#define OBJECT_TYPE_COLUMN  16
#define OBJECT_TYPE_SLAB    17
#define OBJECT_TYPE_DATLST  18

//Disk files entries

#define SIZEOF_DK_TNG_REC 21
#define SIZEOF_DK_CLM_REC 24
#define SIZEOF_DK_APT_REC 8
#define SIZEOF_DK_LGT_REC 20

#define SIZEOF_DK_CLM_HEADER 8
#define SIZEOF_DK_APT_HEADER 4
#define SIZEOF_DK_LGT_HEADER 4
#define SIZEOF_DK_TNG_HEADER 2

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

//Orientation - for graffiti
#define ORIENT_NS         0x00
#define ORIENT_WE         0x01
#define ORIENT_SN         0x02
#define ORIENT_EW         0x03
#define ORIENT_TNS        0x04
#define ORIENT_TWE        0x05
#define ORIENT_TSN        0x06
#define ORIENT_TEW        0x07

extern const int idir_subtl_x[];
extern const int idir_subtl_y[];
 
//Font - for graffiti
#define GRAFF_FONT_NONE        0x00
#define GRAFF_FONT_ADICLSSC    0x01
#define GRAFF_FONT_ADISIZE8    0x02

struct DK_GRAFFITI {
    int tx;
    int ty;
    char *text;
    unsigned short font;
    unsigned short orient;
    int height;
    int fin_tx;
    int fin_ty;
    unsigned short cube;
  };

struct DK_SCRIPT {
    struct DK_SCRIPT_COMMAND **list;
    char **txt;  // The whole script stored as txt
    int lines_count;
};

struct LEVSTATS {
    //Things strats
    int creatures_count;
    int roomeffects_count;
    int traps_count;
    int doors_count;
    int items_count;
    //Items stats
    int hero_gates_count;
    int dn_hearts_count;
    int spellbooks_count;
    int dng_specboxes_count;
    int crtr_lairs_count;
    int statues_count;
    int torches_count;
    int gold_things_count;
    int furniture_count;
    //Various stats
    int room_things_count;
    //Stats on objects adding/removal
    int things_removed;
    int things_added;
  };

struct LEVEL {
    //map file name (for loading)
    char *fname;
    //map file name (for saving)
    char *savfname;
    //Slab file - tile type definitions, size MAP_SIZE_Y x MAP_SIZE_X
    unsigned short **slb;
    //Owners file - subtile owner index, size arr_entries_y+1 x arr_entries_x+1
    unsigned char **own;
    //Vibration file - subtile animation indices, size arr_entries_y+1 x arr_entries_x+1
    unsigned char **wib;
    //WLB file - some additional info about water and lava tiles,
    // size MAP_SIZE_Y x MAP_SIZE_X, not always present
    unsigned char **wlb;
    //Flag file - size arr_entries_y+1 x arr_entries_x+1
    unsigned short **flg;
    //Column file - constant-size array of entries used for displaying tiles,
    // size COLUMN_ENTRIES x SIZEOF_DK_CLM_REC
    unsigned char **clm;
    //How many DAT entries points at every column
    unsigned int *clm_utilize;
    //Column file header
    unsigned char *clm_hdr;
    //Texture information file - one byte file, identifies texture pack index
    unsigned char inf;
    //Script text - a text file containing level parameters as editable script;
    // number of lines and file size totally variable
    struct DK_SCRIPT script;

    //our objects - apt, tng and lgt

    unsigned char ****apt_lookup; // Index to action points, by subtile
    unsigned short **apt_subnums; // Number of action points in a subtile
    unsigned int apt_total_count; // Total number of action points

    unsigned char ****tng_lookup; // Index to things, by subtile
    unsigned short **tng_subnums; // Number of things in a subtile
    unsigned int tng_total_count; // Number of things in total

    //Light file - contains static light definitions
    unsigned char ****lgt_lookup; // Index to static light, by subtile
    unsigned short **lgt_subnums; // Number of static lights in a subtile
    unsigned int lgt_total_count; // Total number of static lights

    unsigned short **tng_apt_lgt_nums;    // Number of all objects in a tile

    // DAT file contains indices of columns for each subtile
    // Its content stores a graphic for whole map
    // Size arr_entries_y+1 x arr_entries_x+1 (there is single rock column at end)
    unsigned short **dat;

    // Elements that are not part of DK levels, but are importand for Adikted
    // Level statistics
    struct LEVSTATS stats;
    // Custom columns definition
    // There can be only one custom column on each subtile.
    // The lookup array size is arr_entries_y+1 x arr_entries_x+1
    struct DK_CUSTOM_CLM ***cust_clm_lookup; // Index to cust.columns, by subtile
    unsigned int cust_clm_count;
    struct DK_GRAFFITI **graffiti;
    unsigned int graffiti_count;
  };

// creates object for storing map
short level_init(struct LEVEL **lvl_ptr);
// frees object for storing map
short level_deinit();

short level_clear(struct LEVEL *lvl);
short level_clear_tng(struct LEVEL *lvl);
short level_clear_apt(struct LEVEL *lvl);
short level_clear_lgt(struct LEVEL *lvl);
short level_clear_datclm(struct LEVEL *lvl);
short level_clear_other(struct LEVEL *lvl);

short level_free();
short level_free_tng(struct LEVEL *lvl);

short level_verify(struct LEVEL *lvl, char *actn_name);
short level_verify_struct(struct LEVEL *lvl, char *err_msg);
short actnpts_verify(struct LEVEL *lvl, char *err_msg);
short level_verify_logic(struct LEVEL *lvl, char *err_msg);
void start_new_map(struct LEVEL *lvl);
void generate_random_map(struct LEVEL *lvl);
void generate_slab_bkgnd_default(struct LEVEL *lvl,unsigned short def_slab);
void generate_slab_bkgnd_random(struct LEVEL *lvl);
void free_map(void);

char *get_thing(const struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
int thing_add(struct LEVEL *lvl,unsigned char *thing);
void thing_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
void thing_drop(struct LEVEL *lvl,unsigned int x, unsigned int y, unsigned int num);
unsigned int get_thing_subnums(const struct LEVEL *lvl,unsigned int x,unsigned int y);

char *get_actnpt(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
void actnpt_add(struct LEVEL *lvl,unsigned char *actnpt);
void actnpt_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
unsigned int get_actnpt_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);

char *get_stlight(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
void stlight_add(struct LEVEL *lvl,unsigned char *stlight);
void stlight_del(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int num);
unsigned int get_stlight_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);

short get_object_type(struct LEVEL *lvl, unsigned int x, unsigned int y, unsigned int z);
unsigned char *get_object(struct LEVEL *lvl,unsigned int x,unsigned int y,unsigned int z);
void object_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z);
unsigned int get_object_subnums(struct LEVEL *lvl,unsigned int x,unsigned int y);
unsigned int get_object_tilnums(struct LEVEL *lvl,unsigned int x,unsigned int y);
int get_object_subtl_last(struct LEVEL *lvl,unsigned int x,unsigned int y,short obj_type);
void update_object_owners(struct LEVEL *lvl);

short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
void set_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy, short nval);

short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty);
void set_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty, short nval);

unsigned char get_tile_owner(const struct LEVEL *lvl, unsigned int tx, unsigned int ty);
void set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned char nval);
unsigned char get_subtl_owner(const struct LEVEL *lvl, unsigned int sx, unsigned int sy);
void set_subtl_owner(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char nval);

unsigned short get_tile_slab(const struct LEVEL *lvl, unsigned int tx, unsigned int ty);
void set_tile_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval);

unsigned int get_dat_val(const struct LEVEL *lvl, const unsigned int sx, const unsigned int sy);
void set_dat_val(struct LEVEL *lvl, int sx, int sy, unsigned int d);

unsigned short get_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
void set_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned short nval);

void update_level_stats(struct LEVEL *lvl);
void update_things_stats(struct LEVEL *lvl);
void update_thing_stats(struct LEVEL *lvl,const unsigned char *thing,short change);

extern struct LEVEL *lvl;

#endif // ADIKT_LEVDATA_H
