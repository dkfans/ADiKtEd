/*
 * scr_cube.h header file for scr_cube.c
 */

#ifndef ADIKT_SCRCUBE_H
#define ADIKT_SCRCUBE_H

// Variables

struct LEVEL;

typedef struct {
    struct DK_CUSTOM_CLM *cclm_rec;
    int cube_idx;
  } MDCUBE_DATA;

extern MDCUBE_DATA *mdcube;

//Functions - init and free
short init_mdcube(void);
void free_mdcube(void);

//Functions - start and stop
short start_mdcube(struct LEVEL *lvl);
void end_mdcube(void);

//Functions - actions and screen
void actions_mdcube(int key);
void draw_mdcube(void);

//Functions - lower level

//Functions - internal

#endif // ADIKT_SCRCUBE_H
