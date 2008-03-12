/*
 * scr_clm.h header file for scr_clm.c
 */

#ifndef ADIKT_SCRCLM_H
#define ADIKT_SCRCLM_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

//Functions - init and free
short init_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdclm(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void draw_mdclm_panel(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level
int display_dat_subtiles(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int scr_row, int scr_col,int ty,int tx);
int display_column(unsigned char *clmentry,int clm_idx, int scr_row, int scr_col);

//Functions - internal

#endif // ADIKT_SCRCLM_H
