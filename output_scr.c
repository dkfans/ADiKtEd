/*
 * output_scr.c
 *
 * Defines functions for initializing and writing output to console screen.
 * This module provides the interface for displaying text for whole program.
 *
 */

#include "output_scr.h"

#include "globals.h"
#include <slang.h>

short screen_initied=false;

#if defined(unix) && !defined(GO32)
int sigwinch (int sigtype);
void update (void);
volatile int safe_update, update_required;
#endif

void set_cursor_visibility(short val)
{
  SLtt_set_cursor_visibility(val);
}

int get_screen_rows()
{
  return SLtt_Screen_Rows;
}

int get_screen_cols()
{
  return SLtt_Screen_Cols;
}

/*
 * Get the screen size.
 */
static void get_screen_size (void)
{
    int r = 0, c = 0;

#ifdef TIOCGWINSZ
    struct winsize wind_struct;

    if ((ioctl(1,TIOCGWINSZ,&wind_struct) == 0)
      || (ioctl(0, TIOCGWINSZ, &wind_struct) == 0)
      || (ioctl(2, TIOCGWINSZ, &wind_struct) == 0)) {
        c = (int) wind_struct.ws_col;
        r = (int) wind_struct.ws_row;
    }
#elif defined(MSDOS)
    union REGS regs;

    regs.h.ah = 0x0F;
    int86 (0x10, &regs, &regs);
    c = regs.h.ah;

    regs.x.ax = 0x1130, regs.h.bh = 0;
    int86 (0x10, &regs, &regs);
    r = regs.h.dl + 1;
#endif

    if ((r <= 0) || (r > 200)) r = 24;
    if ((c <= 0) || (c > 250)) c = 80;
    SLtt_Screen_Rows = r;
    SLtt_Screen_Cols = c;
}

void set_cursor_pos(int row,int col)
{
  SLsmg_gotorc (row,col);
}

void screen_setcolor(int idx)
{
  SLsmg_set_color(idx);
}

void screen_printf(char *format, ...)
{
    va_list val;
    va_start(val, format);
    SLsmg_vprintf(format, val);
    va_end(val);
}

void screen_printf_toeol(char *format, ...)
{
    va_list val;
    va_start(val, format);
    SLsmg_vprintf(format, val);
    va_end(val);
    SLsmg_erase_eol();
}

void screen_printchr(char dst)
{
    SLsmg_write_char(dst);
}

void screen_refresh()
{
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

    SLtt_set_color (0, "buffer", "lightgray", "black");
    SLtt_set_color (1, "select", "black", "lightgray");
    SLtt_set_color (2, "status", "yellow", "blue");
    SLtt_set_color (3, "escape", "brightred", "black");
    SLtt_set_color (4, "invalid", "yellow", "black");
    SLtt_set_color (5, "invalid", "white", "black");
    SLtt_set_color (7, "graffiti", "brightmagenta", "black");
    SLtt_set_color (9, "cursor", "red", "white");
    
    /* Used in slb/tng main display */    
    SLtt_set_color (10,"Keeper 0","white","red");
    SLtt_set_color (11,"Keeper 1","white","cyan");
    SLtt_set_color (12,"Keeper 2","white","green");
    SLtt_set_color (13,"Keeper 3","white","brown");
    SLtt_set_color (14,"Keeper 4","white","blue");
    SLtt_set_color (15,"Unclaimed","lightgray","black");
    SLtt_set_color (16,"Dirt/rock","gray","black");
    SLtt_set_color (17, "graffiti", "magenta", "white");
    SLtt_set_color (20,"Keeper 0","red","white");
    SLtt_set_color (21,"Keeper 1","cyan","white");
    SLtt_set_color (22,"Keeper 2","green", "white");
    SLtt_set_color (23,"Keeper 3","yellow", "lightgray");
    SLtt_set_color (24,"Keeper 4","blue", "lightgray");
    SLtt_set_color (25,"Unclaimed", "black", "lightgray");
    SLtt_set_color (26,"Dirt/rock","black", "lightgray");
    
    /* Used in dat display */
    SLtt_set_color (30,"Keeper 0","brightred","black");
    SLtt_set_color (31,"Keeper 1","brightcyan","black");
    SLtt_set_color (33,"Keeper 2","brightgreen", "black");
    SLtt_set_color (33,"Keeper 3","yellow", "black");
    SLtt_set_color (34,"Keeper 4","brightblue", "black");
    SLtt_set_color (35,"Unclaimed", "lightgray", "black");
    SLtt_set_color (36,"Blank","lightgray","black");
    SLtt_set_color (40,"Keeper 0","white","red");
    SLtt_set_color (41,"Keeper 1","white","cyan");
    SLtt_set_color (42,"Keeper 2","white","green");
    SLtt_set_color (43,"Keeper 3","white","brown");
    SLtt_set_color (44,"Keeper 4","white","blue");
    SLtt_set_color (45,"Unclaimed","black","lightgray");
    SLtt_set_color (46,"Blank","black","lightgray");

    screen_initied=true;
}

#if defined(unix) && !defined(GO32)
int sigwinch (int sigtype)
{
    if (safe_update)
      update();
    else
      update_required = TRUE;
    signal (SIGWINCH, (void *) sigwinch);
    return 0;
}

void update(void) 
{
    SLsmg_reset_smg ();
    get_screen_size ();
    SLsmg_init_smg ();
    draw_scr ();
}
#endif

/*
 * Clean up all the stuff that init() did.
 */
void screen_done(void)
{
    if (!screen_initied) return;
    set_cursor_visibility(1);
    SLsmg_reset_smg();
    screen_initied=false;
}

