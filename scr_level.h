/*
 * scr_level.h header file for scr_level.c
 */

#ifndef ADIKT_SCRLEVEL_H
#define ADIKT_SCRLEVEL_H

void init_levscr(void);
void draw_levscr(void);

static void draw_slb(void);
static void draw_creature(void);
static void draw_clm(void);

static void mark_check(void);
static char *mode_status(int mode);
extern void show_cursor(char cur);

// Amount of width the key takes up
extern int keycols;

#endif // ADIKT_SCRLEVEL_H
