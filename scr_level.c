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
#include "obj_slabs.h"
#include "obj_things.h"
#include "obj_column.h"
#include "internal.h"

// Amount of width the key takes up
int keycols=40;
//Variables for navigating in draw_numbered_list screen
int numbered_list_y=0; //the first visible row
int numbered_list_pos=0; // selected item position (rel. to screen top)
int numbered_list_items=8; //number of items in the list
int numbered_list_cols=2; //number of columns in the item list

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
      draw_creatrkind();
      break;
    case MD_ITMT:
      draw_itemtype();
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
    case MD_CRTR:
      sprintf (buffer, "   (Selecting kind)");
      break;
    case MD_ITMT:
      sprintf (buffer, "   (Selecting type)");
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

static void draw_clm(void)
{
    int i, j, g;
    int col;
    char *clmentry;
    
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
    int clm_idx=get_dat_subtile(lvl, cx*3+sx, cy*3+sy);
    clmentry = lvl->clm[clm_idx%COLUMN_ENTRIES];
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    get_clm_entry(clm_rec, clmentry);
    screen_printf("Use: %04X (decimal:%5d)", clm_rec->use,clm_rec->use);
    set_cursor_pos(1, cols+23);
    screen_printf("Permanent: %d", clm_rec->permanent);
    set_cursor_pos(2, cols+3);
    screen_printf("Lintel: %d", clm_rec->lintel);
    set_cursor_pos(2, cols+23);
    screen_printf("Height: %X", clm_rec->height);
    set_cursor_pos(3, cols+3);
    screen_printf("Solid mask: %04X", clm_rec->solid);
    set_cursor_pos(3, cols+23);
    screen_printf("Orientation: %02X", clm_rec->orientation);
    set_cursor_pos(4, cols+3);
    screen_printf("Base block: %03X", clm_rec->base);
    for (i=0; i < 8; i++)
    {
      set_cursor_pos(5+i, cols+3);
      screen_printf("Cube %d: %03X (CLM index:%4d)", i, clm_rec->c[i],clm_idx);
    }
    free_column_rec(clm_rec);
    set_cursor_pos(14, cols+3);
    screen_printf("WIB animate: %03X", get_subtl_wib(lvl, cx*3+sx, cy*3+sy));
    set_cursor_pos(15, cols+3);
    screen_printf("WLB tile info: %03X", get_subtl_wib(lvl, cx, cy));

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

/*
 * Draws screen with a numbered list (as creature types or item types)
 * and key help for given screen.
 */
void draw_numbered_list(char* (*itemstr)(unsigned int),
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width,
        unsigned int skeyhelprows,char **skeyhelp)
{
    unsigned int line_length=min(cols,LINEMSG_SIZE);
    unsigned int entry_width=itm_width+5;
    numbered_list_cols=max((unsigned int)(line_length/entry_width),1);
    numbered_list_items=end_idx-start_idx+1;
    unsigned int num_rows=numbered_list_items/numbered_list_cols
                        + ((numbered_list_items%numbered_list_cols)>0);
    unsigned int i,k;
    int col;
    for (i=0; i < rows; i++)
    {
      set_cursor_pos(i, 0);
      char it_msg[LINEMSG_SIZE];
      if (i+numbered_list_y<num_rows)
        for (k=0;k<numbered_list_cols;k++)
        {
          int itm_idx=start_idx+numbered_list_y+i+k*num_rows;
          if (itm_idx-start_idx==numbered_list_pos)
              col=45;
          else
              col=0;
          if (itm_idx<=end_idx)
            sprintf(it_msg,"%3d: %-*.*s",
                    itm_idx,itm_width,itm_width, itemstr(itm_idx));
          else
            sprintf(it_msg,"");
          screen_setcolor(col);
          screen_printf("%s",it_msg);
        }
      screen_setcolor(0);
      screen_printf_toeol("");
      set_cursor_pos(i, cols);
      screen_setcolor(2);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(0);
      char *key_help="";
      if (i < skeyhelprows)
          key_help=skeyhelp[i];
      screen_printf_toeol(" %s",key_help);
    }
}

