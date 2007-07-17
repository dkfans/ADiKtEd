/*
 * action.h   defines exported routines from action.c
 */

#ifndef ADIKT_ACTION_H
#define ADIKT_ACTION_H

#define TNGCLIPBRD_SIZE 17

enum adikt_workmode
{
  MD_SLB    = 0x000,
  MD_TNG    = 0x001,
  MD_CRTR   = 0x002,
  MD_HELP   = 0x003,
  MD_CLM    = 0x004,
};

extern const char *modenames[];

extern void init_keys(void);

// indicates if the main program loop should end
extern short finished;
// main program working mode
extern int mode;

//Marking mode
extern short mark;
extern int markl, markr, markt, markb, markx, marky;

//Painting properties
extern short paintmode;
extern short paintown;
extern unsigned char paintroom;

//Clipboard
extern char tngclipboard[TNGCLIPBRD_SIZE];

// Size of the screen area where map is drawn
extern int rows;
extern int cols;

/* Location of cursor on screen (where appropriate) */
extern int screenx, screeny;
/* Location of top left corner of screen in map (where appropriate */
extern int mapx, mapy;

/* Which subtile is being considered in thing and data modes */
extern int sx, sy;

#endif // ADIKT_ACTION_H
