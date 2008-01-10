/*
 * scr_rwrk.c
 *
 * Defines functions for initializing and displaying the map rework screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_rwrk.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "obj_slabs.h"
#include "obj_column.h"
#include "lev_data.h"

RWRK_DATA *mdrwrk;

/*
 * Initializes variables for the rework screen.
 */
short init_mdrwrk(void)
{
    //Creating and clearing mdrwrk variable
    mdrwrk=(RWRK_DATA *)malloc(sizeof(RWRK_DATA));
    if (mdrwrk==NULL)
     die("init_mdrwrk: Cannot allocate memory.");
    mdrwrk->view=RVM_SLB;
    return true;
}

/*
 * Deallocates memory for the rework screen.
 */
void free_mdrwrk(void)
{
  free(mdrwrk);
}

/*
 * Covers actions from the rework screen.
 */
void actions_mdrwrk(int key)
{
    int sx, sy;
    sx = (mapmode->mapx+mapmode->screenx)*3+mapmode->subtl_x;
    sy = (mapmode->mapy+mapmode->screeny)*3+mapmode->subtl_y;
    message_release();
    
    if (!cursor_actions(key))
    if (!subtl_select_actions(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          end_mdrwrk();
          break;
        case 's':
          mdrwrk->view=RVM_SLB;
          break;
        case 'o':
          mdrwrk->view=RVM_OWN;
          break;
        case 'd':
          mdrwrk->view=RVM_DAT;
          break;
        case 'i':
          mdrwrk->view=RVM_WIB;
          break;
        case 'l':
          mdrwrk->view=RVM_WLB;
          break;
        case 'f':
          mdrwrk->view=RVM_FLG;
          break;
        default:
          message_info("Unrecognized rewrk key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start the rework mode.
 */
short start_mdrwrk(struct LEVEL *lvl)
{
    scrmode->mode=MD_RWRK;
    return true;
}

/*
 * Action function - end the rework mode.
 */
void end_mdrwrk()
{
    mapmode->panel_mode=PV_MODE;
    scrmode->mode=MD_SLB;
    message_info("Returned to Slab mode.");
}

/*
 * Draws screen for the rework mode.
 */
void draw_mdrwrk()
{
    draw_rework_map_area(lvl,mdrwrk);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(lvl,mapmode->panel_mode);
    else
      draw_mdrwrk_panel();
    draw_map_cursor(lvl,true,true,false);
}

void draw_mdrwrk_panel()
{
    draw_mdclm_panel();
}

/*
 * Draws the map area for rework mode.
 * Also clears the right panel.
 */
void draw_rework_map_area(struct LEVEL *lvl,RWRK_DATA *mdrwrk)
{
    int i, k;
    for (k=0; k<scrmode->rows; k++)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(k,0);
      int ty=mapmode->mapy+k;
      if (ty >= MAP_SIZE_Y)
      {
          for (i=0; i<scrmode->cols; i++)
            screen_printchr(' ');
      }
      else
      {
          for (i=0; i<scrmode->cols; i++)
          {
            int tx=mapmode->mapx+i;
            if (tx < MAP_SIZE_X)
            {
              char out_ch;
              int sx, sy;
              sx = tx*3+mapmode->subtl_x;
              sy = ty*3+mapmode->subtl_y;
              out_ch=get_rework_map_tile_char(lvl,sx,sy,mdrwrk->view);
              screen_printchr(out_ch);
            } else
            {
              screen_printchr(' ');
            }
          }
      }
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
    }
}

/*
 * Returns a character that should de drawn on map in rework mode.
 */
int get_rework_map_tile_char(struct LEVEL *lvl,int sx,int sy,unsigned short view)
{
    char out_ch;
    unsigned short field;
    switch (view)
    {
      case RVM_SLB:
      {
        field=get_tile_slab(lvl,sx/3,sy/3);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='A'+field;
      };break;
      case RVM_OWN:
      {
        field=get_tile_owner(lvl,sx/3,sy/3);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='0'+field;
      };break;
      case RVM_DAT:
      {
        field=get_dat_subtile(lvl,sx,sy);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='A'+field;
      };break;
      case RVM_WIB:
      {
        field=get_subtl_wib(lvl,sx,sy);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='0'+field;
      };break;
      case RVM_WLB:
      {
        field=get_tile_wlb(lvl,sx/3,sy/3);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='0'+field;
      };break;
      case RVM_FLG:
      {
        field=get_subtl_flg(lvl,sx,sy);
        field&=255;
        if (field==127)
          out_ch='^';
        else
        if (field>=32)
          out_ch=field;
        else
          out_ch='0'+field;
      };break;
      default:
        out_ch='?';break;
    }
    return out_ch;
}
