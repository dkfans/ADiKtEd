/******************************************************************************/
// output_scr.c - Platform independence assurance module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 12 Dec 2007

// Purpose:
//   Defines functions for initializing and writing output to console screen.
//   This module provides the interface for displaying text for whole program.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "output_scr.h"

#include "../libadikted/adikted.h"
#include <slang.h>

#include "scr_actn.h"

short screen_initied=false;
struct DRAW_DATA drawdata;

#if defined(unix) && !defined(GO32)
int sigwinch (int sigtype);
volatile int safe_update, update_required;
#endif

void set_cursor_visibility(short val)
{
  SLtt_set_cursor_visibility(val);
}

int get_screen_rows(void)
{
  return SLtt_Screen_Rows;
}

int get_screen_cols(void)
{
  return SLtt_Screen_Cols;
}

/*
 * Get the screen size. This is modified version of the SLANG library function
 * SLtt_get_screen_size. The modification allows it to work in MSDOS.
 */
static void get_screen_size (void)
{
#if !defined(MSDOS)
    SLtt_get_screen_size();
#else
    int r = 0, c = 0;
    union REGS regs;

    regs.h.ah = 0x0F;
    int86 (0x10, &regs, &regs);
    c = regs.h.ah;

    regs.x.ax = 0x1130, regs.h.bh = 0;
    int86 (0x10, &regs, &regs);
    r = regs.h.dl + 1;

   if ((r <= 0) || (r > SLTT_MAX_SCREEN_ROWS)) r = 24;
   if ((c <= 0) || (c > SLTT_MAX_SCREEN_COLS)) c = 80;
   SLtt_Screen_Rows = r;
   SLtt_Screen_Cols = c;
#endif
}

void set_cursor_pos(int row,int col)
{
  if (!screen_initied) return;
  SLsmg_gotorc (row,col);
}

void screen_setcolor(int idx)
{
  if (!screen_initied) return;
  SLsmg_set_color(idx);
}

void screen_printf(char *format, ...)
{
    if (!screen_initied) return;
    va_list val;
    va_start(val, format);
    SLsmg_vprintf(format, val);
    va_end(val);
}

void screen_printf_toeol(char *format, ...)
{
    if (!screen_initied) return;
    va_list val;
    va_start(val, format);
    SLsmg_vprintf(format, val);
    va_end(val);
    SLsmg_erase_eol();
}

void screen_printchr(char dst)
{
    if (!screen_initied) return;
    SLsmg_write_char(dst);
}

void screen_clear(void)
{
    if (!screen_initied) return;
    SLsmg_gotorc(0,0);
    SLsmg_erase_eos();
    //SLsmg_cls();
}

void screen_refresh(void)
{
    if (!screen_initied) return;
    SLsmg_refresh();
}

/*
 * Initialise stuff at the beginning of the program: mostly the
 * SLang library screen management.
 */
void screen_init(void)
{
    get_screen_size();
    SLtt_Use_Ansi_Colors = true;
#if defined(unix) && !defined(GO32)
    signal (SIGWINCH, (void *) sigwinch);
#endif
    if (SLsmg_init_smg()!=0)
    {
      fprintf(stderr, "screen_init: SLsmg_init_smg returned error code\n");
      SLang_reset_tty ();
      exit (1);
    }

    SLtt_set_color(PRINT_COLOR_LGREY_ON_BLACK , "buffer", "lightgray", "black");
    SLtt_set_color(PRINT_COLOR_BLACK_ON_LGREY , "select", "black", "lightgray");
    SLtt_set_color(PRINT_COLOR_YELLOW_ON_BLUE , "status", "yellow", "blue");
    SLtt_set_color(PRINT_COLOR_LRED_ON_BLACK  , "escape", "brightred", "black");
    SLtt_set_color(PRINT_COLOR_YELLOW_ON_BLACK, "invalid", "yellow", "black");
    SLtt_set_color(PRINT_COLOR_YELLOW_ON_RED, "y_on_r", "yellow", "red");
    SLtt_set_color(PRINT_COLOR_WHITE_ON_BLACK , "invalid", "white", "black");
    SLtt_set_color(PRINT_COLOR_LMAGENT_ON_BLACK, "graffiti", "brightmagenta", "black");
    SLtt_set_color(PRINT_COLOR_RED_ON_WHITE   , "cursor", "red", "white");
    
    // Used in slb/tng main display
    SLtt_set_color (PRINT_COLOR_WHITE_ON_RED,  "Keeper 0","white","red");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_BLUE, "Keeper 1","white","blue");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_GREEN,"Keeper 2","white","green");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_BROWN,"Keeper 3","white","brown");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_CYAN, "Keeper 4","white","cyan");
    SLtt_set_color (15,"Unclaimed","lightgray","black");
    SLtt_set_color (PRINT_COLOR_GREY_ON_BLACK  ,"Dirt/rock","gray","black");
    SLtt_set_color (PRINT_COLOR_MAGENT_ON_WHITE, "graffiti", "magenta", "white");
    SLtt_set_color (20,"Keeper 0","red","white");
    SLtt_set_color (PRINT_COLOR_BLUE_ON_LGREY  ,"Keeper 1","blue", "lightgray");
    SLtt_set_color (PRINT_COLOR_GREEN_ON_WHITE ,"Keeper 2","green", "white");
    SLtt_set_color (PRINT_COLOR_YELLOW_ON_LGREY,"Keeper 3","yellow", "lightgray");
    SLtt_set_color (PRINT_COLOR_CYAN_ON_WHITE  ,"Keeper 4","cyan","white");
    SLtt_set_color (25,"Unclaimed", "black", "lightgray");
    SLtt_set_color (26,"Dirt/rock","black", "lightgray");
    
    // Used in dat display
    SLtt_set_color (30,"Keeper 0","brightred","black");
    SLtt_set_color (PRINT_COLOR_LBLUE_ON_BLACK,"Keeper 1","brightblue", "black");
    SLtt_set_color (PRINT_COLOR_LGREEN_ON_BLACK,"Keeper 2","brightgreen", "black");
    SLtt_set_color (PRINT_COLOR_YELLOW_ON_BLACK,"Keeper 3","yellow", "black");
    SLtt_set_color (PRINT_COLOR_LCYAN_ON_BLACK,"Keeper 4","brightcyan","black");
    SLtt_set_color (35,"Unclaimed", "lightgray", "black");
    SLtt_set_color (36,"Blank","lightgray","black");
    SLtt_set_color (40,"Keeper 0","white","red");
    SLtt_set_color (41,"Keeper 1","white","blue");
    SLtt_set_color (42,"Keeper 2","white","green");
    SLtt_set_color (43,"Keeper 3","white","brown");
    SLtt_set_color (44,"Keeper 4","white","cyan");
    SLtt_set_color (45,"Unclaimed","black","lightgray");
    SLtt_set_color (46,"Blank","black","lightgray");

    SLtt_set_color (PRINT_COLOR_BLUE_ON_WHITE  ,"Keeper 1","blue", "white");
    SLtt_set_color (PRINT_COLOR_BROWN_ON_WHITE,"Keeper 3","brown", "white");
    SLtt_set_color (PRINT_COLOR_MAGENT_ON_LGREY, "spec1", "magenta", "lightgray");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_GREY, "spec2", "brightmagenta", "gray");


    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_RED,  "Keeper 0","brightmagenta","red");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_BLUE, "Keeper 1","brightmagenta","blue");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_GREEN,"Keeper 2","brightmagenta","green");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_BROWN,"Keeper 3","brightmagenta","brown");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_CYAN, "Keeper 4","brightmagenta","cyan");
    SLtt_set_color (PRINT_COLOR_GREY_ON_RED,  "Keeper 0","grey","red");
    SLtt_set_color (PRINT_COLOR_GREY_ON_BLUE, "Keeper 1","grey","blue");
    SLtt_set_color (PRINT_COLOR_GREY_ON_GREEN,"Keeper 2","grey","green");
    SLtt_set_color (PRINT_COLOR_GREY_ON_BROWN,"Keeper 3","grey","brown");
    SLtt_set_color (PRINT_COLOR_GREY_ON_CYAN, "Keeper 4","grey","cyan");

    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_LRED,  "Keeper 0","brightmagenta","brightred");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_LBLUE, "Keeper 1","brightmagenta","brightblue");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_LGREEN,"Keeper 2","brightmagenta","brightgreen");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_YELLOW,"Keeper 3","brightmagenta","yellow");
    SLtt_set_color (PRINT_COLOR_LMAGENT_ON_LCYAN, "Keeper 4","brightmagenta","brightcyan");
    SLtt_set_color (PRINT_COLOR_GREY_ON_LRED,  "Keeper 0","grey","brightred");
    SLtt_set_color (PRINT_COLOR_GREY_ON_LBLUE, "Keeper 1","grey","brightblue");
    SLtt_set_color (PRINT_COLOR_GREY_ON_LGREEN,"Keeper 2","grey","brightgreen");
    SLtt_set_color (PRINT_COLOR_GREY_ON_YELLOW,"Keeper 3","grey","yellow");
    SLtt_set_color (PRINT_COLOR_GREY_ON_LCYAN, "Keeper 4","grey","brightcyan");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_LRED,  "Keeper 0","white","brightred");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_LBLUE, "Keeper 1","white","brightblue");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_LGREEN,"Keeper 2","white","brightgreen");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_YELLOW,"Keeper 3","white","yellow");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_LCYAN, "Keeper 4","white","brightcyan");

    SLtt_set_color (PRINT_COLOR_LRED_ON_YELLOW, "Highlight","brightred","yellow");
    SLtt_set_color (PRINT_COLOR_LGREY_ON_GREY, "Brighened unowned","lightgray","gray");
    SLtt_set_color (PRINT_COLOR_WHITE_ON_GREY, "Brighened","white","gray");

    screen_initied=true;
}

void screen_draw_vline(int posy,int posx,int length,short border_style)
{
  char fill;
  switch (border_style)
  {
  case bsSingle :fill=179; break;
  case bsDouble :fill=186; break;
  case bsSolid  :fill=32; break;
  default: fill='.'; break;
  }
  int i;
  for (i=0;i<length;i++)
  {
    set_cursor_pos(posy+i,posx);
    screen_printchr(fill);
  }
}

void screen_draw_hline(int posy,int posx,int length,short border_style)
{
  set_cursor_pos(posy,posx);
  char fill;
  switch (border_style)
  {
  case bsSingle :fill=196; break;
  case bsDouble :fill=205; break;
  case bsSolid  :fill=32; break;
  default: fill='.'; break;
  }
  int i;
  for (i=0;i<length;i++)
  {
//    set_cursor_pos(posy,posx+i);
    screen_printchr(fill);
  }
}

void screen_draw_window(int posy,int posx,int sizey,int sizex,int border_size,short border_style)
{
  //The window fill
  int startx,starty;
  int border_v=0;
  if (border_size>0) border_v=((3*border_size)>>1)+1;
  startx=(posx-border_v);
  starty=(posy-border_size);
  int i;
  for (i=1;i<sizey+2*border_size+2;i++)
  {
    screen_draw_hline(starty,startx,(sizex+2*border_v+1),bsSolid);
    starty++;
  }
  //draw frame corners
  char corner1,corner2,corner3,corner4;
  switch (border_style)
  {
  case bsSingle :corner1=218;corner2=191;corner3=192;corner4=217; break;
  case bsDouble :corner1=201;corner2=187;corner3=200;corner4=188; break;
  case bsSolid  :corner1=32; corner2=32; corner3=32; corner4=32; break;
  default       :corner1='.'; corner2='.'; corner3='.'; corner4='.'; break;
  };
  set_cursor_pos(posy,posx);
  screen_printchr(corner1);
  set_cursor_pos(posy,posx+sizex);
  screen_printchr(corner2);
  set_cursor_pos(posy+sizey,posx);
  screen_printchr(corner3);
  set_cursor_pos(posy+sizey,posx+sizex);
  screen_printchr(corner4);
  //and lines between corners
  screen_draw_vline(posy+1,posx,(sizey-1),border_style);
  screen_draw_hline(posy,posx+1,(sizex-1),border_style);
  screen_draw_vline(posy+1,posx+sizex,(sizey-1),border_style);
  screen_draw_hline(posy+sizey,posx+1,(sizex-1),border_style);
}

#if defined(unix) && !defined(GO32)
/*
 * Function allows automatic screen redraw on terminal size change
 * on UNIX/LINUX. Should be set as answer to the SIGWINCH signal.
 */
int sigwinch (int sigtype)
{
    if (safe_update)
      screen_reinit_and_update();
    else
      update_required = true;
    signal (SIGWINCH, (void *) sigwinch);
    return 0;
}

#endif

/*
 * Should be called when program window size changes.
 * Automatically run on UNIX systems by sigwinch().
 */
void screen_reinit_and_update(void)
{
    if (!screen_initied) return;
    SLsmg_reset_smg();
    get_screen_size();
    SLsmg_init_smg();
    draw_levscr(drawdata.scrmode,drawdata.workdata);
}

/*
 * Clean up all the stuff that init() did.
 */
void screen_done(void)
{
    if (!screen_initied) return;
    set_cursor_visibility(true);
    SLsmg_reset_smg();
    screen_initied=false;
}

