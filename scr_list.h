/*
 * scr_list.h header file for scr_list.c
 */

#ifndef ADIKT_SCRLIST_H
#define ADIKT_SCRLIST_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

//Variables for navigating in list screen

typedef struct {
    int prevmode;
    int y; //the first visible row
    int pos; // selected item position (rel. to screen top)
    int items; //number of items in the list
    int cols; //number of columns in the item list
  } LIST_DATA;

extern LIST_DATA *list;

//Functions - init and free
short init_list(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_list(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_list(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int lstmode);
void end_list(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
short start_mdtextr(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
short start_mdslbl(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void draw_crtre(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_crtre(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_itemt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_itemt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdtextr(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_mdtextr(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdcclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_mdcclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_mdcube(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdslbl(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_mdslbl(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdsrch(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void actions_mdsrch(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);

//Functions - lower level
short actions_list(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_numbered_list(char *(*itemstr)(unsigned short),
        struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width);

//Functions - internal

#endif // ADIKT_SCRLIST_H
