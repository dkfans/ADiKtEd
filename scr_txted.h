/*
 * scr_txted.h header file for scr_txted.c
 */

#ifndef ADIKT_SCRTXTED_H
#define ADIKT_SCRTXTED_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

typedef struct {
    int prevmode;
    int y;
    int rows;
    char **text;
    struct DK_SCRIPT_COMMAND **script_cmds;
  } TXTED_DATA;

extern TXTED_DATA *editor;

//Functions - init and free
short init_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level

//Functions - internal

#endif // ADIKT_SCRTXTED_H
