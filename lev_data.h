/*
 * lev_data.h header file for lev_data.c
 */

#ifndef ADIKT_LEVDATA_H
#define ADIKT_LEVDATA_H

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
    LEVSTATS stats;
  } LEVEL;

// creates object for storing map
extern short level_init();

extern LEVEL *lvl;

#endif // ADIKT_LEVDATA_H
