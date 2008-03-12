/*
 * scr_actn.h header file for scr_actn.c
 */

#ifndef ADIKT_SCRACTN_H
#define ADIKT_SCRACTN_H

#include "globals.h"

struct LEVEL;

enum adikt_workmode
{
  MD_SLB    = 0x000,
  MD_TNG    = 0x001,
  MD_CRTR   = 0x002,
  MD_ITMT   = 0x003,
  MD_HELP   = 0x004,
  MD_CLM    = 0x005,
  MD_SCRP   = 0x006,
  MD_TXTR   = 0x007,
  MD_CCLM   = 0x008,
  MD_CUBE   = 0x009,
  MD_SLBL   = 0x00a,
  MD_RWRK   = 0x00b,
  MD_SRCH   = 0x00c,
};

#define MODES_COUNT 13

#define TNGDAT_ROWS 8

enum adikt_panel_viewmode
{
  PV_MODE   = 0x000, //Work mode specific data
  PV_COMPS  = 0x001, //Compass rose
};

struct CLIPBOARD {
    int dtype;
    unsigned char *data;
  };

struct SCRMODE_DATA {
    // Amount of width on right, reserved for key and info
    int keycols;
    // main program working mode
    int mode;
    // Size of the screen area where map/list is drawn
    // (the one without status lines at bottom and info text at right)
    int rows;
    int cols;
    // Clipboard
    struct CLIPBOARD *clipbrd;
    int clip_count;
    //Automated commands - allow sending multiple commands to the program.
    //Used by command line parameters
    unsigned int *automated_commands;
    // I/O enable variables
    short screen_enabled;
    short input_enabled;
  };

struct MAPMODE_DATA {
    //Marking variables
    short mark;
    struct IPOINT_2D markp;
    //Painting properties
    short paintmode;
    short paintown;
    unsigned char paintroom;
    // Location of cursor on screen (where appropriate)
    struct IPOINT_2D screen;
    // Location of top left corner of screen in map (where appropriate)
    struct IPOINT_2D map;
    // Variables for drawing the mark rectangle
    struct IRECT_2D markr;
    // Highlighted squares
    int **hilight;
    // Brightened squares
    int **brighten;
    // Which subtile is being considered in thing and data modes
    struct IPOINT_2D subtl;
    //What is drawn on the right panel
    short panel_mode;
    // The way DAT entries are shown
    // 0 for no view, 1 for byte data, 2 for column idx
    short dat_view_mode;
    // Will the range of objects be visible?
    short show_obj_range;
  };

extern const char *modenames[];
extern const char *longmodenames[];

// indicates if the main program loop should end
extern short finished;

// Screen maintain functions
void init_levscr_basics(struct SCRMODE_DATA **scrmode,struct MAPMODE_DATA **mapmode);
void init_levscr_modes(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
void free_levscr(struct SCRMODE_DATA **scrmode,struct MAPMODE_DATA **mapmode);
void draw_levscr(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

// Keyboard action functions
void proc_key(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
short cursor_actions(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key);
short subtl_select_actions(struct MAPMODE_DATA *mapmode,int key);
void curposcheck(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

int change_mode(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int new_mode);
void draw_forced_panel(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl, short panel_mode);
void draw_map_cursor(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,short show_ground,short show_rooms,short show_things);

//Clipboard support - lower level
struct CLIPBOARD *get_clipboard_object(struct SCRMODE_DATA *scrmode,int idx);
unsigned char *get_clipboard_slab(struct SCRMODE_DATA *scrmode,int idx);
unsigned char *get_clipboard_datlst(struct SCRMODE_DATA *scrmode,int idx);
unsigned char *get_clipboard_column(struct SCRMODE_DATA *scrmode,int idx);
int copy_to_clipboard_thing(struct SCRMODE_DATA *scrmode,unsigned char *obj);
int copy_to_clipboard_actnpt(struct SCRMODE_DATA *scrmode,unsigned char *obj);
int copy_to_clipboard_stlight(struct SCRMODE_DATA *scrmode,unsigned char *obj);

//Clipboard support - lower level
int add_clipboard_any(struct SCRMODE_DATA *scrmode,char *obj,int obj_type);
int put_clipboard_any(struct SCRMODE_DATA *scrmode,char *obj,int obj_type);
void clear_clipboard(struct SCRMODE_DATA *scrmode);

//Lower level functions
void clear_scrmode(struct SCRMODE_DATA *scrmode);
void clear_mapmode(struct MAPMODE_DATA *mapmode);

void display_tngdat(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);
int display_mode_keyhelp(int scr_row, int scr_col,int mode);
void draw_mdempty(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl);

void show_cursor(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,char cur);
char *mode_status(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,int mode);
short is_simple_mode(int mode);

void mark_check(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);
int get_draw_map_tile_color(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int tx,int ty,short special,short darken_fg,short brighten_bg);
int get_screen_color_owned(unsigned char owner,short marked,short darken_fg,short brighten_bg);
void draw_map_area(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,short show_ground,short show_rooms,short show_things);
int get_draw_map_tile_char(struct LEVEL *lvl,int tx,int ty,
    short show_ground,short show_rooms,short show_things,short force_at);

// Some specific actions
short level_verify_with_highlight(struct LEVEL *lvl,struct MAPMODE_DATA *mapmode);

// highlight is used for search and showing error position
int get_tile_highlight(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty);
void set_tile_highlight(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty, int nval);
void clear_highlight(struct MAPMODE_DATA *mapmode);

// brighten is used to display range of some objects
short get_tile_brighten(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty);
void set_tile_brighten(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty, short nval);
void clear_brighten(struct MAPMODE_DATA *mapmode);
void update_brighten(struct LEVEL *lvl,struct MAPMODE_DATA *mapmode);
void set_brighten_for_thing(struct MAPMODE_DATA *mapmode,unsigned char *thing);
void set_brighten_for_actnpt(struct MAPMODE_DATA *mapmode,unsigned char *actnpt);
void set_brighten_for_stlight(struct MAPMODE_DATA *mapmode,unsigned char *stlight);
void set_brighten_for_range(struct MAPMODE_DATA *mapmode,
    unsigned int pos_x,unsigned int pos_y,unsigned int rng);

#endif // ADIKT_SCRACTN_H
