/*
 * scr_list.h header file for scr_list.c
 */

#ifndef ADIKT_SCRLIST_H
#define ADIKT_SCRLIST_H

//Variables for navigating in list screen

typedef struct {
    int prevmode;
    int y; //the first visible row
    int pos; // selected item position (rel. to screen top)
    int items; //number of items in the list
    int cols; //number of columns in the item list
  } LIST_DATA;

extern LIST_DATA *list;

//Functions - init and free
short init_list(void);
void free_list(void);

//Functions - start and stop
short start_list(int lstmode);
void end_list(void);

//Functions - actions and screen
void draw_crtre();
void actions_crtre(int key);
void draw_itemt();
void actions_itemt(int key);
void actions_textr(int key);

//Functions - lower level
short actions_list(int key);
void draw_numbered_list(char *(*itemstr)(unsigned char),
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width);

//Functions - internal

#endif // ADIKT_SCRLIST_H
