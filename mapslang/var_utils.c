/******************************************************************************/
// var_utils.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  15 Nov 2007

// Purpose:
//   Various utility functions.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "var_utils.h"

#include "../libadikted/globals.h"
#include <stdarg.h>
#include "input_kb.h"
#include "../libadikted/msg_log.h"
#include "../libadikted/lev_data.h"
#include "output_scr.h"
#include "scr_actn.h"

/*
 * Popups are temporary messages, visible only until next screen redraw.
 */
void popup_show(const char *title,const char *format, ...)
{
    char *msg;
    msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    if (msg==NULL)
        die("popup_show: Cannot allocate memory");
    va_list val;
    va_start(val, format);
    vsprintf(msg, format, val);
    va_end(val);
    int rows=get_screen_rows();
    int cols=get_screen_cols();
    int textlen=max(strlen(title),strlen(msg));
    int window_rows=4;
    int window_cols=3*cols/4;
    if (textlen+2>window_cols)
        window_cols=textlen+2;
    if (window_cols>cols-2)
        window_cols=max(cols-2,0);
    int posx=(cols-window_cols)>>1;
    int posy=(rows-window_rows)>>1;
    screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
    screen_draw_window(posy,posx,window_rows,window_cols,1,bsDouble);
    int title_x=(cols-strlen(title)-2)>>1;
    if (title_x<posx) title_x=posx;
    set_cursor_pos(posy,title_x);
    screen_printf(" %s ",title);
    set_cursor_pos(posy+2,posx+1);
    screen_printf("%s",msg);
    screen_refresh();
    // Write to log file if it is prepared
    message_log_simp(msg);
    free(msg);
}

void die(const char *format, ...)
{
      done(&(drawdata.scrmode),drawdata.workdata);
      va_list val;
      va_start(val, format);
      vfprintf(stderr, format, val);
      // Write to log file if it is prepared
      message_log_vl(format, val);
      va_end(val);
      fprintf(stderr, "\n");
      message_log_simp(PROGRAM_NAME " has died");
      free_messages();
      exit(1);
}

/*
 * Clean up all the stuff that init() did.
 */
void done(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata!=NULL)
    {
        if (workdata->lvl!=NULL)
          level_deinit(&(workdata->lvl));
        if (scrmode!=NULL)
          free_levscr(scrmode,workdata);
    }
    // Write to log file if it is prepared
    message_log_simp(PROGRAM_NAME " work is done");
    free_messages();
    input_done();
}
