#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#if defined(unix) && !defined(GO32)
# include <unistd.h>
# include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#elseif defined(MSDOS)
#include <dos.h>
#include <process.h>
#endif

#include <slang.h>

/* Global functions */

#include "obj_names.h"

#define MAP_SIZE_X 85
#define MAP_SIZE_Y 85
#define MAP_MAXINDEX_X MAP_SIZE_X-1
#define MAP_MAXINDEX_Y MAP_SIZE_Y-1

/* internal.c */
extern int is_spell (unsigned char *thing);
extern int is_room_thing (unsigned char *thing);
extern int is_action_point (int x, int y, int z);
extern int is_action_thing (unsigned char *thing);
extern void update_square (int x, int y);
extern void update_tngdat (int x, int y);
extern void delete_thing (int x, int y, int num);
extern void add_thing (unsigned char *thing);
extern unsigned char *create_object (int x, int y, int type);
extern unsigned char *create_thing (int tx, int ty);
extern unsigned char *create_action_point (int x, int y);
extern void create_default_clm (void);
extern void create_clmdattng(void);
extern void check_doors (void);
extern int get_dat (int cx, int cy);
extern void set_dat2 (int cx, int cy, int d);
extern void free_map (void);

/* graffiti.c */
extern int add_graffiti (int x, int y, char *string, int d);
extern void draw_graffiti (void);
extern void read_graffiti (void);
extern void delete_graffiti (int n);
extern char *get_graffiti (int n);
extern int is_graffiti (int x, int y);
extern void free_graffiti (void);

/* main.c */
extern void die (char *x);
extern void done(void);

/* file.c */
extern void load_map (char *fname);
extern void save_map (char *mapname);
extern void start_new_map (void);

/* action.c */
#define DISKPATH_SIZE   64
#define LINEMSG_SIZE    80
#define TNGCLIPBRD_SIZE 17
#define READ_BUFSIZE    256

enum adikt_workmode
{
  MD_SLB    = 0x000,
  MD_TNG    = 0x001,
  MD_CRTR   = 0x002,
  MD_HELP   = 0x003,
  MD_CLM    = 0x004,
};

extern void proc_key (void);

/* screen.c */
extern void draw_scr (void);
extern void init_slang(void);
extern int get_str (char *prompt, char *buf);
extern void die (char *x);
extern void done(void);
extern int sigwinch (int sigtype);
extern void update (void);
extern volatile int safe_update, update_required;

/* Definitions */

#if defined(unix) && !defined (GO32)
#define SEPARATOR "/"
#else
#define SEPARATOR "\\"
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#define EVER ;;

/* Global variables */

extern int finished;
extern int mode;
extern int mark;
extern int helpformode;
extern int helpy;

extern int rows;
extern int cols;

/* Amount of width the key takes up */
extern int keycols;

/* Location of cursor on screen (where appropriate) */
extern int screenx, screeny;
/* Location of top left corner of screen in map (where appropriate */
extern int mapx, mapy;

/* Which subtile is being considered in thing and data modes */
extern int sx, sy;

extern int markl, markr, markt, markb, markx, marky;

/* 0 for high-low, 1 for low-high */
extern int datmode;

extern int default_clm;

extern int vistng[3][3]; /* Which number are we looking at on each subtile */

extern char creatinput[5];

/* Global map variables */

extern unsigned char **slb;
extern unsigned char **own;
extern unsigned char **wib;

/* Note that I store action points as things in here, and separate them
 * out at save time. This makes lots of things a lot easier */

extern unsigned int apt_tot; /* Total number of action points */
extern unsigned int tng_tot; /* Number of things in total */
extern unsigned char **action_used;
/* Which action point numbers are taken, and where their data is */

/* 
 * Note we assume there will be less than 1024 action points. I think
 * this should do... the most DK uses is 13
 */

extern unsigned char ****tng_lookup; /* Index to things by subtile */
extern unsigned short **tng_nums; /* Number of things in a tile */
extern unsigned short **tng_subnums; /* Number of things in a subtile */

/* 
 * Exceptionally grotty hack - we never need the actual data
 * stored in the .dat file, only what the high and low bytes should
 * be. So long as we remember this when we generate the "standard"
 * dungeon things, we'll be fine
 */
extern unsigned char **dat_low;
extern unsigned char **dat_high;

extern unsigned char **clm; /* clm file */

extern char *slbkey;
extern unsigned char *slbplacekeys;
extern int slbplacenkeys;
extern int paintown;
extern unsigned char paintroom;
extern int paintmode;
extern char *filebase;
extern char message[LINEMSG_SIZE];
extern char tngclipboard[17];

/* Help stuff */

extern int clmhelprows;
extern char **clmhelp;
extern int slbkeyhelprows;
extern char **slbkeyhelp;
extern int crtkeyhelprows;
extern char **crtkeyhelp;
extern int slbhelprows;
extern char **slbhelp;
extern int tnghelprows;
extern char **tnghelp;
extern int crthelprows;
extern char **crthelp;
extern int helprows;
extern char **helptext;
