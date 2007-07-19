/*
 * lev_files.h header file for lev_files.c
 */

#ifndef ADIKT_LEVFILES_H
#define ADIKT_LEVFILES_H

#include "lev_data.h"

extern short save_map(LEVEL *lvl,char *mapname);

extern void load_map(LEVEL *lvl,char *fname);

#endif // ADIKT_LEVFILES_H
