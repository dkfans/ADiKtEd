/*
 * scr_thing.h header file for scr_thing.c
 */

#ifndef ADIKT_SCRTHING_H
#define ADIKT_SCRTHING_H

struct LEVEL;
struct SCRMODE_DATA;
struct MAPMODE_DATA;
struct WORKMODE_DATA;
struct HELP_DATA;

// Variables

struct MDTNG_DATA {
    int vistng[3][3]; // Which number are we looking at on each subtile
    short obj_ranges_changed;
  };

//Functions - init and free
short init_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - start and stop
short start_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void end_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - actions and screen
void actions_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
void draw_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void draw_mdtng_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

// Single actions
void action_edit_object(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_delete_object(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_inc_object_level(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_dec_object_level(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

//Functions - lower level
void change_visited_tile(struct WORKMODE_DATA *workdata);
int display_thing(struct HELP_DATA *help,unsigned char *thing, int scr_col, int scr_row,
        int max_row, short display_float_pos);
int display_action_point(unsigned char *actnpt, int x, int scr_row, int max_row,
        short display_float_pos);
int display_static_light(unsigned char *stlight, int x, int y, int max_row, short display_float_pos);
int display_obj_stats(struct SCRMODE_DATA *scrmode,const struct WORKMODE_DATA *workdata, int scr_row, int scr_col);
int display_tng_subtiles(struct SCRMODE_DATA *scrmode,const struct WORKMODE_DATA *workdata,
    const struct LEVEL *lvl, int scr_row, int scr_col,short compressed,int ty,int tx);

//Functions - internal
char get_thing_char(const struct LEVEL *lvl, int x, int y);
int get_tng_display_color(short obj_type,unsigned char obj_owner,short marked);
int get_visited_obj_idx(struct WORKMODE_DATA *workdata);
void set_visited_obj_idx(struct WORKMODE_DATA *workdata,int obj_idx);
void set_visited_obj_lastof(struct WORKMODE_DATA *workdata,short obj_type);
unsigned char *tng_makeitem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx);
unsigned char *tng_makeroomeffect(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx);
unsigned char *tng_maketrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx);
unsigned char *tng_makecreature(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx);
void tng_change_height(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, unsigned int sx, unsigned int sy,unsigned int z,int delta_height);
void tng_change_range(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, unsigned int sx, unsigned int sy,unsigned int z,int delta_range);

#endif // ADIKT_SCRTHING_H
