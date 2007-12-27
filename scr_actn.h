/*
 * scr_actn.h header file for scr_actn.c
 */

#ifndef ADIKT_SCRACTN_H
#define ADIKT_SCRACTN_H

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
};

#define MODES_COUNT 10

enum adikt_panel_viewmode
{
  PV_MODE   = 0x000, //Work mode specific data
  PV_COMPS  = 0x001, //Compass rose
};



typedef struct {
    int dtype;
    unsigned char *data;
  } CLIPBOARD;

typedef struct {
    // Amount of width on right, reserved for key and info
    int keycols;
    // main program working mode
    int mode;
    // Size of the screen area where map/list is drawn
    // (the one without status lines at bottom and info text at right)
    int rows;
    int cols;
    CLIPBOARD *clipbrd;
    int clip_count;
  } SCRMODE_DATA;

typedef struct {
    //Marking variables
    short mark;
    int markx, marky;
    //Painting properties
    short paintmode;
    short paintown;
    unsigned char paintroom;
    // Location of cursor on screen (where appropriate)
    int screenx;
    int screeny;
    // Location of top left corner of screen in map (where appropriate)
    int mapx;
    int mapy;
    // Variables for drawing the mark rectangle
    int markl, markr, markt, markb;
    // Which subtile is being considered in thing and data modes
    int subtl_x;
    int subtl_y;
    //What is drawn on the right panel
    short panel_mode;
  } MAPMODE_DATA;

extern const char *modenames[];
extern const char *longmodenames[];

extern SCRMODE_DATA *scrmode;
extern MAPMODE_DATA *mapmode;

extern unsigned int *automated_commands;
// indicates if the main program loop should end
extern short finished;
// I/O enable variables
extern short screen_enabled;
extern short input_enabled;

// Screen maintain functions
void init_levscr_basics(void);
void init_levscr_modes(void);
void free_levscr(void);
void draw_levscr(struct LEVEL *lvl);

// Keyboard action functions
void proc_key(void);
short cursor_actions(int key);
short subtl_select_actions(int key);
void curposcheck(void);

int change_mode(int new_mode);
void draw_forced_panel(struct LEVEL *lvl, short panel_mode);

//Clipboard support - lower level
CLIPBOARD *get_clipboard_object(int idx);
unsigned char *get_clipboard_slab(int idx);
unsigned char *get_clipboard_datlst(int idx);
unsigned char *get_clipboard_column(int idx);
int copy_to_clipboard_thing(unsigned char *obj);
int copy_to_clipboard_actnpt(unsigned char *obj);
int copy_to_clipboard_stlight(unsigned char *obj);

//Clipboard support - lower level
int add_clipboard_any(char *obj,int obj_type);
int put_clipboard_any(char *obj,int obj_type);
void clear_clipboard();


//Lower level functions
void clear_scrmode(SCRMODE_DATA *scrmode);
void clear_mapmode(MAPMODE_DATA *mapmode);

void display_tngdat(void);
int display_mode_keyhelp(int scr_row, int scr_col,int mode);

void show_cursor(char cur);
char *mode_status(int mode);
short is_simple_mode(int mode);

void mark_check(void);
int get_draw_map_tile_color(struct LEVEL *lvl,int tx,int ty,short special);
void draw_map_area(struct LEVEL *lvl,short show_ground,short show_rooms,short show_things);
int get_draw_map_tile_char(struct LEVEL *lvl,int tx,int ty,
    short show_ground,short show_rooms,short show_things,short force_at);

#endif // ADIKT_SCRACTN_H
