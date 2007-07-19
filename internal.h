/*
 * internal.h header file for internal.c
 */

#ifndef ADIKT_INTERNAL_H
#define ADIKT_INTERNAL_H

#include "lev_data.h"

/* internal.c */
extern void update_square (int x, int y);
extern void update_tngdat (int x, int y);
extern unsigned char *create_object (int x, int y, int type);
extern unsigned char *create_thing (int tx, int ty);
extern unsigned char *create_action_point (int x, int y);
extern void create_default_clm (void);
extern void create_clmdattng(void);
extern void check_doors (void);
extern int get_dat (int cx, int cy);
extern void set_dat2 (int cx, int cy, int d);
extern void free_map (void);

struct memory_file
{
    long len;
    unsigned char *content;
};

struct memory_file read_file (char *iname);

/* 0 for high-low, 1 for low-high */
extern int datmode;

extern int default_clm;

extern int vistng[3][3]; /* Which number are we looking at on each subtile */

extern char creatinput[5];

// Note that I store action points as things in here, and separate them
// out at save time. This makes lots of things a lot easier
extern unsigned int apt_tot; // Total number of action points

     // Which action point numbers are taken, and where their data is
    // Note: we assume there will be less than 1024 action points. I think
    // this should do... the most DK uses is 13
extern unsigned char **action_used;


/* 
 * Exceptionally grotty hack - we never need the actual data
 * stored in the .dat file, only what the high and low bytes should
 * be. So long as we remember this when we generate the "standard"
 * dungeon things, we'll be fine
 */
extern unsigned char **dat_low;
extern unsigned char **dat_high;

#endif // ADIKT_INTERNAL_H
