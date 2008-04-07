/*
 * scr_cube.h header file for scr_cube.c
 */

#ifndef ADIKT_SCRCUBE_H
#define ADIKT_SCRCUBE_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

struct MDCUBE_DATA {
    struct DK_CUSTOM_CLM *cclm_rec;
    int cube_idx;
  };

//Functions - init and free
short init_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level

//Functions - internal

#endif // ADIKT_SCRCUBE_H
