/******************************************************************************/
// scr_actn.h - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Header file. Defines exported routines from scr_actn.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_SCRACTN_H
#define ADIKT_SCRACTN_H

#include "../libadikted/globals.h"

struct LEVEL;

enum adikt_workmode
{
  MD_SLB    = 0x000, // Slab mode
  MD_TNG    = 0x001, // Thing mode
  MD_CRTR   = 0x002, // Create Creature mode
  MD_CITM   = 0x003, // Create Item mode
  MD_HELP   = 0x004, // Help mode
  MD_CLM    = 0x005, // Column mode
  MD_SCRP   = 0x006, // Script mode
  MD_TXTR   = 0x007, // Texture selection mode
  MD_CCLM   = 0x008, // Custom Column list mode
  MD_CUBE   = 0x009, // Custom Cubes mode
  MD_SLBL   = 0x00a, // Slab list mode
  MD_RWRK   = 0x00b, // File formats rework mode
  MD_SRCH   = 0x00c, // Search mode
  MD_LMAP   = 0x00d, // Open map with preview mode
  MD_SMAP   = 0x00e, // Save map with preview mode
  MD_GRFT   = 0x00f, // Graffiti input mode
  MD_CEFC   = 0x010, // Create Effect Generator mode
  MD_CTRP   = 0x011, // Create Trap mode
  MD_EITM   = 0x012, // Edit Item mode
  MD_ECRT   = 0x013, // Edit Creature mode
  MD_EFCT   = 0x014, // Edit Effect Generator mode
  MD_ETRP   = 0x015, // Edit Trap mode
  MD_TLST
};

#define MODES_COUNT 23

#define TNGDAT_ROWS 8

enum adikt_panel_viewmode
{
  PV_MODE   = 0x000, //Work mode specific data
  PV_COMPS  = 0x001, //Compass rose
  PV_SLB    = 0x002, //Slab mode panel
  PV_TNG    = 0x003, //Thing mode panel
};

enum adikt_string_input
{
  SI_NONE   = 0x000, //String input disabled
  SI_LDMAP  = 0x001, //Input map name to load
  SI_SVMAP  = 0x002, //Input map name to save
  SI_GRAFT  = 0x003, //Input graffiti text
};

enum adikt_level_preview
{
  LPREV_NONE   = 0x000,
  LPREV_LOAD   = 0x001,
  LPREV_SAVE   = 0x002,
};

enum adikt_paintmode
{
  PNTMD_NONE      = 0x00,
  PNTMD_ENAB      = 0x01,
  PNTMD_SLAB      = 0x02,
  PNTMD_OWNR      = 0x04,
  PNTMD_RNDWALL   = 0x08,
};

enum adikt_eetype
{
  EE_NONE      = 0x00,
  EE_TLBIRTH   = 0x01,
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
    // A variable for storing user-input string
    char usrinput[READ_BUFSIZE];
    int usrinput_pos;
    short usrinput_type;
    //Automated commands - allow sending multiple commands to the program.
    //Used by command line parameters
    unsigned int *automated_commands;
    // I/O enable variables
    short screen_enabled;
    short input_enabled;
  };

struct INFOPANEL_DATA {
    //What is drawn on the right panel
    short mode;
    // The way DAT entries are shown
    // 0 for no view, 1 for byte data, 2 for column idx
    short dat_view_mode;
    // Display object position/range as float
    short display_float_pos;
};

struct MAPMODE_DATA {
    // Level size, in tiles
    struct UPOINT_2D tlsize;
    //Marking variables
    short mark;
    struct IPOINT_2D markp;
    //Painting properties
    short paintmode;
    unsigned char paintownr;
    unsigned short paintroom;
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
    // Will the range of objects be visible?
    short show_obj_range;
    // Open list when creating trap
    short traps_list_on_create;
    // Open list when creating effect
    short effectgen_list_on_create;
    // Open list when creating creature
    short creature_list_on_create;
    // Open list when creating traps, doors, spellbooks, specials, ...
    short items_list_on_create;
    //Slab keys
    char *slbkey;
    short eetype;
    // Will the preview of level be visible?
    short level_preview;
    // Preview of a level, used when opening a level
    struct LEVEL *preview;
  };

struct WORKMODE_DATA {
    struct LEVEL *lvl;
    struct MAPMODE_DATA *mapmode;
    struct TXTED_DATA *editor;
    struct TXTGEN_DATA *txtgen;
    struct MDCUBE_DATA *mdcube;
    struct RWRK_DATA *mdrwrk;
    struct MDTNG_DATA *mdtng;
    struct MDSLAB_DATA *mdslab;
    struct LIST_DATA *list;
    struct HELP_DATA *help;
    // Level options - are copied into current level
    struct LEVOPTIONS *optns;
    // Right panel configuration
    struct INFOPANEL_DATA *ipanel;
  };

extern void (*actions [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *,int);
extern void (*mddraw [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *);
extern short (*mdstart [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *);
extern void (*mdend [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *);

extern const char *modenames[];
extern const char *longmodenames[];
extern const char *string_input_msg[];

// indicates if the main program loop should end
extern short finished;

// Screen maintain functions
void init_levscr_basics(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata);
void init_levscr_modes(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void free_levscr(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata);
void draw_levscr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

// Keyboard action functions
void proc_key(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
short cursor_actions(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key);
short subtl_select_actions(struct MAPMODE_DATA *mapmode,int key);
short string_get_actions(struct SCRMODE_DATA *scrmode,int key,short *text_changed);
void curposcheck(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

// The single actions
void action_enter_texture_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_generate_bitmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_enter_search_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_enter_script_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_toggle_datclm_aupdate(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_toggle_compass_rose(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_create_new_map(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_create_random_backgnd(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_save_map_quick(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_enter_mapsave_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_load_map_quick(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_enter_mapload_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_quit_program(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);
void action_enter_help_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

// Action/drawing subfunctions
int change_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int new_mode);
void draw_forced_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, short panel_mode);
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
void clear_infopanel(struct INFOPANEL_DATA *ipanel);

void display_rpanel_bottom(const struct SCRMODE_DATA *scrmode,
    const struct WORKMODE_DATA *workdata);
int display_mode_keyhelp(struct HELP_DATA *help, int scr_row, int scr_col,
    int max_row,int mode,int itm_idx);
void draw_mdempty(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata);

void show_cursor(const struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,char cur);
void show_cursor_at(const struct SCRMODE_DATA *scrmode,int row,int col,char cur);
void show_no_cursor();
char *mode_status(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int mode);
short is_simple_mode(int mode);

// Paint function support
short is_painting_enab(struct MAPMODE_DATA *mapmode);
short is_painting_slab(struct MAPMODE_DATA *mapmode);
short is_painting_ownr(struct MAPMODE_DATA *mapmode);
unsigned short get_painting_slab(struct MAPMODE_DATA *mapmode);
unsigned char get_painting_ownr(struct MAPMODE_DATA *mapmode);
void set_painting_enab(struct MAPMODE_DATA *mapmode);
void set_painting_slab(struct MAPMODE_DATA *mapmode,const unsigned short slab);
void set_painting_ownr(struct MAPMODE_DATA *mapmode,const unsigned char owner);
void set_painting_rndwall(struct MAPMODE_DATA *mapmode);
void set_painting_disab(struct MAPMODE_DATA *mapmode);

// Mark function support
short is_marking_enab(struct MAPMODE_DATA *mapmode);
void set_marking_disab(struct MAPMODE_DATA *mapmode);
void set_marking_start(struct MAPMODE_DATA *mapmode,int x,int y);
void mark_check(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode);

int get_draw_map_tile_color(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int tx,int ty,short special,short darken_fg,short brighten_bg);
int get_screen_color_owned(unsigned char owner,short marked,short darken_fg,short brighten_bg);
void draw_map_area(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,short show_ground,short show_rooms,short show_things);
int get_draw_map_tile_char(const struct MAPMODE_DATA *mapmode,const struct LEVEL *lvl,
    int tx,int ty,short show_ground,short show_rooms,short show_things,short force_at);

// Current position on map
short get_map_subtile_pos(struct MAPMODE_DATA *mapmode,struct IPOINT_2D *subpos);
short get_map_tile_pos(struct MAPMODE_DATA *mapmode,struct IPOINT_2D *tilpos);

// Some specific actions
short level_verify_with_highlight(struct WORKMODE_DATA *workdata);

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
