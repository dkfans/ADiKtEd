/******************************************************************************/
// output_scr.h - Platform independence assurance module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 12 Dec 2007

// Purpose:
//   Header file. Defines exported routines from output_scr.c

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#ifndef ADIKT_OUTPUTSCR_H
#define ADIKT_OUTPUTSCR_H

enum borderStyle {bsSingle,bsDouble,bsSolid};

#define PRINT_COLOR_LGREY_ON_BLACK    0
#define PRINT_COLOR_BLACK_ON_LGREY    1
#define PRINT_COLOR_YELLOW_ON_BLUE    2
#define PRINT_COLOR_LRED_ON_BLACK     3
#define PRINT_COLOR_YELLOW_ON_BLACK   4
#define PRINT_COLOR_WHITE_ON_BLACK    5
#define PRINT_COLOR_LMAGENT_ON_BLACK  7
#define PRINT_COLOR_YELLOW_ON_RED     8
#define PRINT_COLOR_LBLUE_ON_BLACK    31
#define PRINT_COLOR_LGREEN_ON_BLACK   32
#define PRINT_COLOR_LCYAN_ON_BLACK    34

#define PRINT_COLOR_GREY_ON_BLACK     16
#define PRINT_COLOR_MAGENT_ON_WHITE   17
#define PRINT_COLOR_BLUE_ON_LGREY     21
#define PRINT_COLOR_YELLOW_ON_LGREY   23
#define PRINT_COLOR_MAGENT_ON_LGREY   47

#define PRINT_COLOR_WHITE_ON_RED      10
#define PRINT_COLOR_WHITE_ON_BLUE     11
#define PRINT_COLOR_WHITE_ON_GREEN    12
#define PRINT_COLOR_WHITE_ON_BROWN    13
#define PRINT_COLOR_WHITE_ON_CYAN     14
#define PRINT_COLOR_RED_ON_WHITE      9
#define PRINT_COLOR_BLUE_ON_WHITE     48
#define PRINT_COLOR_GREEN_ON_WHITE    22
#define PRINT_COLOR_BROWN_ON_WHITE    49
#define PRINT_COLOR_CYAN_ON_WHITE     24

#define PRINT_COLOR_LMAGENT_ON_RED    50
#define PRINT_COLOR_LMAGENT_ON_BLUE   51
#define PRINT_COLOR_LMAGENT_ON_GREEN  52
#define PRINT_COLOR_LMAGENT_ON_BROWN  53
#define PRINT_COLOR_LMAGENT_ON_CYAN   54
#define PRINT_COLOR_GREY_ON_RED       55
#define PRINT_COLOR_GREY_ON_BLUE      56
#define PRINT_COLOR_GREY_ON_GREEN     57
#define PRINT_COLOR_GREY_ON_BROWN     58
#define PRINT_COLOR_GREY_ON_CYAN      59

#define PRINT_COLOR_LMAGENT_ON_LRED   60
#define PRINT_COLOR_LMAGENT_ON_LBLUE  61
#define PRINT_COLOR_LMAGENT_ON_LGREEN 62
#define PRINT_COLOR_LMAGENT_ON_YELLOW 63
#define PRINT_COLOR_LMAGENT_ON_LCYAN  64
#define PRINT_COLOR_GREY_ON_LRED      65
#define PRINT_COLOR_GREY_ON_LBLUE     66
#define PRINT_COLOR_GREY_ON_LGREEN    67
#define PRINT_COLOR_GREY_ON_YELLOW    68
#define PRINT_COLOR_GREY_ON_LCYAN     69

#define PRINT_COLOR_WHITE_ON_LRED     70
#define PRINT_COLOR_WHITE_ON_LBLUE    71
#define PRINT_COLOR_WHITE_ON_LGREEN   72
#define PRINT_COLOR_WHITE_ON_YELLOW   73
#define PRINT_COLOR_WHITE_ON_LCYAN    74

#define PRINT_COLOR_LRED_ON_YELLOW    80
#define PRINT_COLOR_LGREY_ON_GREY     81
#define PRINT_COLOR_WHITE_ON_GREY     82
#define PRINT_COLOR_LMAGENT_ON_GREY   83

#define PRINT_COLOR_TILESET           100

//The structure with data required for drawing function
struct DRAW_DATA {
    struct SCRMODE_DATA *scrmode;
    struct WORKMODE_DATA *workdata;
};

extern struct DRAW_DATA drawdata;

void set_cursor_visibility(short val);
int get_screen_rows(void);
int get_screen_cols(void);
void set_cursor_pos(int row,int col);
void screen_setcolor(int idx);
void screen_printchr(char dst);
void screen_printf(char *format, ...);
void screen_printf_toeol(char *format, ...);
void screen_draw_hline(int posy,int posx,int length,short border_style);
void screen_draw_vline(int posy,int posx,int length,short border_style);
void screen_draw_window(int posy,int posx,int sizey,int sizex,int border_size,short border_style);
void screen_clear(void);
void screen_refresh(void);
void screen_init(void);
void screen_reinit_and_update(void);
void screen_done(void);


#endif // ADIKT_OUTPUTSCR_H
