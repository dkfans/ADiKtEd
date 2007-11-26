/*
 * scr_level.h header file for scr_level.c
 */

#ifndef ADIKT_SCRLEVEL_H
#define ADIKT_SCRLEVEL_H

void init_levscr(void);
void draw_levscr(void);

static void draw_slb(void);
static void draw_clm(void);

static void mark_check(void);
static char *mode_status(int mode);
extern void show_cursor(char cur);

void draw_numbered_list(char* (*itemstr)(unsigned int),
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width,
        unsigned int skeyhelprows,char **skeyhelp);

//Variables for navigating in draw_numbered_list screen
extern int numbered_list_y; //the first visible row
extern int numbered_list_pos; // selected item index
extern int numbered_list_items; //number of items in the list
extern int numbered_list_cols; //number of columns in the item list


// Amount of width the key takes up
extern int keycols;

#endif // ADIKT_SCRLEVEL_H
