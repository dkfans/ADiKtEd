/*
 * scr_cube.h header file for scr_cube.c
 */

#ifndef ADIKT_SCRCUBE_H
#define ADIKT_SCRCUBE_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

typedef struct {
    struct DK_CUSTOM_CLM *cclm_rec;
    int cube_idx;
  } MDCUBE_DATA;

extern MDCUBE_DATA *mdcube;

//Functions - init and free
short init_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level

//Functions - internal

#endif // ADIKT_SCRCUBE_H
