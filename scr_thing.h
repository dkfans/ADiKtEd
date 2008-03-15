/*
 * scr_thing.h header file for scr_thing.c
 */

#ifndef ADIKT_SCRTHING_H
#define ADIKT_SCRTHING_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;

// Variables

typedef struct {
    int vistng[3][3]; // Which number are we looking at on each subtile
    short obj_ranges_changed;
  } MDTNG_DATA;

extern MDTNG_DATA *mdtng;

//Functions - init and free
short init_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

//Functions - start and stop
short start_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void end_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - actions and screen
void actions_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
void draw_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
void draw_mdtng_panel(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

//Functions - lower level
void change_visited_tile(struct MAPMODE_DATA *mapmode);
int display_thing(unsigned char *thing, int x, int y);
int display_action_point(unsigned char *actnpt, int x, int y);
int display_static_light(unsigned char *stlight, int x, int y);
int display_obj_stats(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,const struct LEVEL *lvl, int scr_row, int scr_col);
int display_tng_subtiles(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,const struct LEVEL *lvl, int scr_row, int scr_col,int ty,int tx);

//Functions - internal
char get_thing_char(const struct LEVEL *lvl, int x, int y);
int get_tng_display_color(short obj_type,unsigned char obj_owner,short marked);
void tng_makeitem(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int sx,int sy,unsigned char stype_idx);
void tng_change_height(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_height);
void tng_change_range(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_range);

#endif // ADIKT_SCRTHING_H
