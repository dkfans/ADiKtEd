/******************************************************************************/
// textmenu.h - Console-based menu interface module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 31 May 2008

// Purpose:
//   Header file. Defines exported routines from textmenu.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef BULL_TEXTMENU_H
#define BULL_TEXTMENU_H

#include "../libadikted/globals.h"

#define MIT_NUMERIC 0
#define MIT_BOOLEAN 1
#define MIT_CHOICE  2
#define MIT_MENU    3
#define MIT_SPECIAL 4
#define MIT_STATIC  5

#define DK_LONG_MAX	2147483647L
#define DK_LONG_MIN	(-LONG_MAX-1)

struct WORKMODE_DATA;

union _value
{
    long *longptr;
    long num;
};

struct _limits
{
    long min;
    long max;
};

typedef struct _menuitem
{
    // Specified
    int type;
    int x, y;
    char *label;
    union _value value;
    struct _limits limit;
    void *param;
    
    // Set by addItem
    int num;
    int up;
    int down;
    int left;
    int right;
    
} *menuitem;

typedef struct _menu 
{
    struct _menu *back;
    int nitems;
    menuitem *items;
    char *title;
    int first;
    int last;
} *menu;

struct workmenu
{
    // Where we are
    menu cmenu;
    int citem;
    short item_open;
    // Reading values from keyboard/choice box
    int limit_min;
    int limit_max;
    int choicen;
    int choicecols;
    int choicew;
    char edit_value[LINEMSG_SIZE]; // Don't allow more than LINEMSG_SIZE characters!
    // Menu (not the opened item) parameters
    int mleft;
    int mtop;
    int mwidth;
    int mheight;
    // short process_special(struct WORKMODE_DATA *workdata,union _value val,void *param);
    short (*process_special)(struct WORKMODE_DATA *,union _value ,void *);
};


// Functions

menu new_menu(char *title);
menuitem menu_new_item(int x, int y, const char *label, int type,
        union _value value, void *param);
void menu_item_limits(menuitem dest, long min, long max);
void add_item_to_menu(menu dest, menuitem src);
struct workmenu *new_workmenu(menu src);
short workmenu_enter(struct workmenu *currmnu,menu nmenu);
short workmenu_compute_size_rect(struct workmenu *currmnu);

inline union _value menu_value_num(long num);
inline union _value menu_value_longptr(long *longptr);

short action_menukey(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key);
short action_menukey_treemove(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key);
short action_menukey_numeric(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key);
short action_menukey_choice(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key);

void draw_menuscr(struct workmenu *currmnu,int rows,int cols);
void draw_menu_cursor(struct workmenu *currmnu,int rows,int cols);

#endif // BULL_TEXTMENU_H
