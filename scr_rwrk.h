/*
 * scr_rwrk.h header file for scr_rwrk.c
 */

#ifndef ADIKT_SCRRWRK_H
#define ADIKT_SCRRWRK_H

enum rewrk_viewmode
{
  RVM_SLB   = 0x000,
  RVM_OWN   = 0x001,
  RVM_DAT   = 0x002,
  RVM_WIB   = 0x003,
  RVM_WLB   = 0x004,
  RVM_FLG   = 0x005,
};

struct RWRK_DATA {
  unsigned short view;
  };

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;

//Functions - init and free
short init_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void draw_mdrwrk_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
void draw_rework_map_area(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,struct RWRK_DATA *mdrwrk);
int get_rework_map_tile_char(struct LEVEL *lvl,int sx,int sy,unsigned short view);

//Functions - internal
void place_cube_test(struct LEVEL *lvl,int tx,int ty,unsigned short cube);

#endif // ADIKT_SCRRWRK_H
