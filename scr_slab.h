/*
 * scr_slab.h header file for scr_slab.c
 */

#ifndef ADIKT_SCRSLAB_H
#define ADIKT_SCRSLAB_H

// Variables

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

struct MDSLAB_DATA {
    unsigned char *placekeys;
    int placenkeys;
  };

//Functions - init and free
short init_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short start_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void draw_mdslab_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void actions_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
void init_mdslab_keys(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdslab_keys(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - internal
void slbposcheck(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void change_ownership(struct WORKMODE_DATA *workdata,unsigned char purchaser);
void slb_change_ownership(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,unsigned char purchaser);
void slb_place_room(struct WORKMODE_DATA *workdata,unsigned char room);
void slb_next_graffiti_orient(struct LEVEL *lvl,int graf_idx);
void slb_change_graffiti_height(struct LEVEL *lvl,int graf_idx,int delta);

#endif // ADIKT_SCRSLAB_H
