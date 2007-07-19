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

//Disk files entries

#define SIZEOF_DK_TNG_REC 21
#define SIZEOF_DK_CLM_REC 24
#define SIZEOF_DK_ACT_REC 21

typedef struct {
    int hero_gates_count;
  } LEVSTATS;

typedef struct {
    // map info variables
    unsigned char **slb;
    unsigned char **own;
    unsigned char **wib;
    unsigned char **clm; // clm file

    unsigned char ****tng_lookup; // Index to things by subtile
    unsigned short **tng_nums;    // Number of things in a tile
    unsigned short **tng_subnums; // Number of things in a subtile
    unsigned int tng_total_count; // Number of things in total

    LEVSTATS stats;
  } LEVEL;

// creates object for storing map
extern short level_init();
// frees object for storing map
extern short level_deinit();

extern short level_clear(LEVEL *lvl);
extern short level_clear_tng(LEVEL *lvl);

extern short level_free();
extern short level_free_tng(LEVEL *lvl);

extern int verify_map(LEVEL *lvl);
extern void start_new_map(LEVEL *lvl);
extern void free_map(void);

extern void thing_add(LEVEL *lvl,unsigned char *thing);
extern void thing_del(LEVEL *lvl,int x, int y, int num);

extern LEVEL *lvl;

#endif // ADIKT_LEVDATA_H
