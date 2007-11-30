/*
 * internal.h header file for internal.c
 */

#ifndef ADIKT_INTERNAL_H
#define ADIKT_INTERNAL_H

#include "lev_data.h"

struct LEVEL;

extern void update_square (int x, int y);
extern void update_tngdat (int x, int y);
extern void create_clmdattng(void);
extern void free_map (void);

extern int picture_wall (int x, int y, int dx, int dy);
extern void set_wall (int x, int y, int d1, int d2, int d3);

struct memory_file
{
    long len;
    unsigned char *content;
};

struct memory_file read_file (char *iname);

extern int vistng[3][3]; /* Which number are we looking at on each subtile */

#endif // ADIKT_INTERNAL_H
