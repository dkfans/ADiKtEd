/*
 * scr_slab.h header file for scr_slab.c
 */

#ifndef ADIKT_SCRSLAB_H
#define ADIKT_SCRSLAB_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

struct MDSLAB_DATA {
    unsigned char *placekeys;
    int placenkeys;
  };

extern struct MDSLAB_DATA *mdslab;

//Slab mode keys export
extern char *slbkey;

//Functions - init and free
short init_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level
void init_mdslab_keys(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_mdslab_keys(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - internal
void slbposcheck(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void change_ownership(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,unsigned char purchaser);
void slb_place_room(struct LEVEL *lvl,struct MAPMODE_DATA *mapmode,unsigned char room);
void slb_place_graffiti(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int tx, int ty,unsigned short orient);
void slb_next_graffiti_orient(struct LEVEL *lvl,int graf_idx);

#endif // ADIKT_SCRSLAB_H
