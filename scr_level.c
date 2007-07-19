/*
 * scr_level.c
 *
 * Defines functions for initializing and displaying all the level screens.
 *
 */

#include "scr_level.h"

#include "globals.h"
#include "var_utils.h"
#include "graffiti.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_thing.h"
#include "scr_help.h"
#include "action.h"
#include "obj_utils.h"
#include "internal.h"

// Amount of width the key takes up
int keycols=40;

/*
 * Draw the screen
 */
void draw_levscr(void)
{
    if (mark)
      mark_check();
    set_cursor_visibility(0);
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();
    rows = all_rows-2;
    cols = all_cols-keycols;
    // If we don't have much room at all, just forget it!
    if (cols < 0) return;

    set_cursor_pos(rows, 0);
    screen_setcolor(2);
    // Hope there's enough room for it :)
    if (all_cols>70)
      screen_printf_toeol("Dungeon Keeper Map Editor    %5.5s mode  %s", 
                 modenames[mode], mode_status (mode));

    set_cursor_pos(all_rows-1, 0);
    screen_setcolor(0);
    if (is_graffiti (screenx+mapx, screeny+mapy)!=-1)
    {
      char graf_txt[LINEMSG_SIZE];
      strncpy(graf_txt,get_graffiti(is_graffiti(screenx+mapx, screeny+mapy)),LINEMSG_SIZE-16);
      graf_txt[LINEMSG_SIZE-16]=0;
      message_info("Graffiti: %s",graf_txt);
    }
    screen_printf_toeol(message_get());

    switch (mode)
    {
    case MD_SLB:
      draw_slb();
      break;
    case MD_TNG:
      draw_tng();
      break;
    case MD_CRTR:
      draw_creature();
      break;
    case MD_HELP:
      draw_help();
      break;
    case MD_CLM:
      draw_clm();
      break;
    default:
      break;
    }
    set_cursor_pos(all_rows-1, all_cols-1);
    screen_refresh();
}

static void mark_check(void)
{
    if (!mark)
      return;
    if (markx <= mapx+screenx)
    {
      markl = markx;
      markr = mapx+screenx;
    }
    else
    {
      markr = markx;
      markl = mapx+screenx;
    }
    if (marky <= mapy+screeny)
    {
      markt = marky;
      markb = mapy+screeny;
    }
    else
    {
      markb = marky;
      markt = mapy+screeny;
    }
}

static char *mode_status (int mode)
{
    static char buffer[256];
    static const char *longmodenames[]={"slab", "thing", "add creature"};
    switch (mode)
    {
    case MD_CLM:
    case MD_TNG:
      sprintf (buffer, "   Position: %2d,%2d", mapx+screenx,
             mapy+screeny);
      break;
    case MD_SLB:
      sprintf (buffer, "   Position: %2d,%2d", mapx+screenx,
             mapy+screeny);
      if (mark)
          strcat (buffer, " (Marking)");
      else if (paintmode)
          strcat (buffer, " (Painting)");
      break;
    case MD_HELP:
      sprintf (buffer, "   (Help for %s mode)", longmodenames[helpformode]);
      break;
    default:
      strcpy (buffer, "(unknown mode)");
      break;
    }
    return buffer;
}

static void draw_slb(void)
{
    int i, j, k, g;
    int col;
    char *t;
    static char *hilite[4]={"wood", "braced", "iron", "magic"};
    
    for (i=0; i < rows; i++)
    {
      screen_setcolor(0);
      set_cursor_pos(i, 0);
      if (i+mapy > MAP_MAXINDEX_Y)
      {
          for (j=0; j < cols; j++)
            screen_printchr(' ');
      }
      else
      {
          for (j=0; j < cols; j++)
          {
            if (j+mapx <= MAP_MAXINDEX_X)
            {
                g = is_graffiti (j+mapx, i+mapy);
                // Are we marking?
                if (g==-1)
                {
                  col = lvl->own[j+mapx][i+mapy]+10;
                  if (lvl->slb[j+mapx][i+mapy]==0 ||
                      lvl->slb[j+mapx][i+mapy]==2 ||
                      lvl->slb[j+mapx][i+mapy]==3)
                      col=16;
                }
                else
                  col=7;
                if (mark && j+mapx >= markl && j+mapx <= markr
                  && i+mapy >= markt && i+mapy <= markb)
                  col+=10;
                screen_setcolor(col);
                if (g==-1)
                  screen_printchr(slbkey[lvl->slb[j+mapx][i+mapy]]);
                else
                  screen_printchr('@');
            }
            else
                screen_printchr(' ');
          }
      }
      screen_setcolor(2);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(0);
      screen_printchr(' ');
      if (i < slbkeyhelprows)
      {
          screen_printf_toeol("%s",slbkeyhelp[i]);
          screen_setcolor(5);
          t = slbkeyhelp[i]+1;
          while ((t = strchr (t, ':')))
          {
            set_cursor_pos(i, cols+2+(t-slbkeyhelp[i]));
            screen_printchr(*(t-1));
            t++;
          }
          /* Grotty hack time */
          for (k=0; k < 4; k++)
          {
            if ((t=strstr(slbkeyhelp[i], hilite[k])))
            {
                set_cursor_pos(i, cols+3+(t-slbkeyhelp[i]));
                screen_printchr(hilite[k][0]);
            }
          }
      }
      else
          screen_printf_toeol("");
    }
    display_tngdat();
    if (is_graffiti(screenx+mapx, screeny+mapy)==-1)
      show_cursor(slbkey[lvl->slb[screenx+mapx][screeny+mapy]]);
    else
      show_cursor('@');
}

static void draw_creature(void)
{
    int i;
    for (i=0; i < rows; i++)
    {
      set_cursor_pos(i, 0);
      SLsmg_set_color(0);
      // Just do two columns. If this mucks up, so be it
      char cr_msg[LINEMSG_SIZE];
      sprintf(cr_msg,"");
      if (i < 16)
      {
          sprintf(cr_msg,"%2d: %-16.16s", i+1, get_creature_subtype_fullname(i+1));
          if (i+17 < 32)
            sprintf(cr_msg+strlen(cr_msg),"%2d: %-16.16s", i+17, get_creature_subtype_fullname(i+17));
      }
      screen_printf_toeol("%s",cr_msg);

      set_cursor_pos(i, cols);
      screen_setcolor(2);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(0);
      char *key_help="";
      if (i < crtkeyhelprows)
          key_help=crtkeyhelp[i];
      screen_printf_toeol(" %s",key_help);
    }
    set_cursor_pos(get_screen_rows()-1, 17+strlen(creatinput));
}

static void draw_clm(void)
{
    int i, j, g;
    int col;
    unsigned char *c;
    
    int cx, cy;

    cx = screenx+mapx;
    cy = screeny+mapy;

    for (i=0; i < rows; i++)
    {
      screen_setcolor(0);
      set_cursor_pos(i, 0);
      if (i+mapy > MAP_MAXINDEX_Y)
      {
          for (j=0; j < cols; j++)
            screen_printchr(' ');
      }
      else
      {
          for (j=0; j < cols; j++)
          {
            if (j+mapx < MAP_SIZE_X)
            {
                g = is_graffiti(j+mapx, i+mapy);
                // Are we marking?
                if (g==-1)
                {
                  col = lvl->own[j+mapx][i+mapy]+10;
                  if ((lvl->slb[j+mapx][i+mapy]==SLAB_TYPE_ROCK) ||
                      (lvl->slb[j+mapx][i+mapy]==SLAB_TYPE_EARTH) ||
                      (lvl->slb[j+mapx][i+mapy]==SLAB_TYPE_TORCHDIRT))
                      col=16;
                }
                else
                  col=7;
                if ((mark) && (j+mapx >= markl) && (j+mapx <= markr)
                  && (i+mapy >= markt) && (i+mapy <= markb))
                  col+=10;
                screen_setcolor(col);
                if (g==-1)
                  screen_printchr(slbkey[lvl->slb[j+mapx][i+mapy]]);
                else
                  screen_printchr('@');
            }
            else
                screen_printchr(' ');
          }
      }
      screen_setcolor(2);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(0);
      screen_printf_toeol(" ");
    }
    screen_setcolor(0);
    set_cursor_pos(1, cols+3);
    c = lvl->clm [0x10000-((dat_high[cx*3+sx][cy*3+sy]<<8)+
                  dat_low[cx*3+sx][cy*3+sy])];
    screen_printf("Use: %04X ", c[0]+(c[1]<<8));
    set_cursor_pos(1, cols+23);
    screen_printf("Permanent: %d", c[2]&1);
    set_cursor_pos(2, cols+3);
    screen_printf("Lintel: %d", (c[2]>>1)&7);
    set_cursor_pos(2, cols+23);
    screen_printf("Height: %X", (c[2]>>4)&15);
    set_cursor_pos(3, cols+3);
    screen_printf("Solid mask: %04X", c[3]+(c[4]<<8));
    set_cursor_pos(3, cols+23);
    screen_printf("Orientation: %02X", c[7]);
    set_cursor_pos(4, cols+3);
    screen_printf("Base block: %03X", c[5]+(c[6]<<8));
    for (i=0; i < 8; i++)
    {
      set_cursor_pos(5+i, cols+3);
      screen_printf("Cube %d: %03X", i, c[8+i*2]+(c[9+i*2]<<8));
    }
    display_tngdat();
    if (is_graffiti(screenx+mapx, screeny+mapy)==-1)
      show_cursor(slbkey[lvl->slb[screenx+mapx][screeny+mapy]]);
    else
      show_cursor('@');
}

void show_cursor(char cur)
{
    set_cursor_pos(screeny, screenx);
    screen_setcolor(9);
    screen_printchr(cur);
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

