/******************************************************************************/
// scr_rwrk.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  19 Jan 2008

// Purpose:
//   Defines functions for initializing and displaying the map rework screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_rwrk.h"

#include "../libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_clm.h"
#include "var_utils.h"

/*
 * Initializes variables for the rework screen.
 */
short init_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing mdrwrk variable
    workdata->mdrwrk=(struct RWRK_DATA *)malloc(sizeof(struct RWRK_DATA));
    if (workdata->mdrwrk==NULL)
     die("init_mdrwrk: Cannot allocate memory.");
    workdata->mdrwrk->view=RVM_SLB;
    return true;
}

/*
 * Deallocates memory for the rework screen.
 */
void free_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  message_log(" free_mdrwrk: starting");
  if ((workdata==NULL)||(workdata->mdrwrk==NULL)) return;
  free(workdata->mdrwrk);
  workdata->mdrwrk=NULL;
}

/*
 * Covers actions from the rework screen.
 */
void actions_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    static char usrinput[READ_BUFSIZE];
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    message_release();
    
    if (!cursor_actions(scrmode,workdata,key))
    if (!subtl_select_actions(workdata->mapmode,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          end_mdrwrk(scrmode,workdata);
          break;
        case KEY_S:
          workdata->mdrwrk->view=RVM_SLB;
          break;
        case KEY_O:
          workdata->mdrwrk->view=RVM_OWN;
          break;
        case KEY_D:
          workdata->mdrwrk->view=RVM_DAT;
          break;
        case KEY_I:
          workdata->mdrwrk->view=RVM_WIB;
          break;
        case KEY_L:
          workdata->mdrwrk->view=RVM_WLB;
          break;
        case KEY_F:
          workdata->mdrwrk->view=RVM_FLG;
          break;
        case KEY_C:
          write_def_clm_source(workdata->lvl,"adi_clm.log");
          message_info("Column log file written.");
          break;
        case KEY_T:
          write_def_tng_source(workdata->lvl,"adi_tng.log");
          message_info("Things log file written.");
          break;
        case KEY_B:
          if (get_str("Enter starting cube index: ", usrinput))
          {
            int st_cube=0;
            sscanf(usrinput,"%d",&st_cube);
            int i,k;
            popup_show("Generating cube test map","The process can take some time. Please wait...");
            short auto_upd_memory=get_datclm_auto_update(workdata->lvl);
            set_datclm_auto_update(workdata->lvl,false);
            generate_slab_bkgnd_default(workdata->lvl,SLAB_TYPE_PATH);
            const int size_x=6;
            const int size_y=5;
            const int limit_x=(workdata->lvl->tlsize.x-1)/size_x;
            const int limit_y=(workdata->lvl->tlsize.y-1)/size_y;
            for (k=0;k<limit_y;k++)
              for (i=0;i<limit_x;i++)
              {
                unsigned short cube=(st_cube+k*limit_x+i);
                if (cube<512)
                  place_cube_test(workdata->lvl,i*size_x+1,k*size_y+1,cube);
              }
            set_datclm_auto_update(workdata->lvl,auto_upd_memory);
            if (get_datclm_auto_update(workdata->lvl))
            {
              popup_show("Updating DAT/CLM for whole map","Regenarating whole map can take some time. Please wait...");
              update_datclm_for_whole_map(workdata->lvl);
            }
            message_info("Cube test map created.");
          }
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
short start_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    scrmode->mode=MD_RWRK;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

/*
 * Action function - end the rework mode.
 */
void end_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->ipanel->mode=PV_MODE;
    scrmode->mode=MD_SLB;
    message_info("Returned to Slab mode.");
}

void place_cube_test(struct LEVEL *lvl,int tx,int ty,unsigned short cube)
{
    struct DK_CUSTOM_CLM *cclm_rec;
    struct COLUMN_REC *clm_rec;
    const int clr_x=5;
    const int clr_y=2;
    int i,k;
    //Drawing the empty ground
    for (k=0;k<MAP_SUBNUM_Y*clr_y;k++)
      for (i=0;i<MAP_SUBNUM_X*clr_x;i++)
      {
          cclm_rec=create_cust_col();
          clm_rec=cclm_rec->rec;
          fill_column_rec_simp(clm_rec, 0, 0,
              0, 0, 0, 0, 0, 0x0, 0x0, 0x0);
          cust_col_add_or_update(lvl,tx*3+i,ty*3+k,cclm_rec);
      }
    for (k=0;k<clr_y;k++)
      for (i=0;i<clr_x;i++)
        remove_noncrucial_room_things(lvl,tx+i,ty+k);
    // Single cube
    cclm_rec=create_cust_col();
    clm_rec=cclm_rec->rec;
    fill_column_rec_simp(clm_rec, 0, 0,
        0, cube, 0, 0, 0, 0x0, 0x0, 0x0);
    cust_col_add_or_update(lvl,(tx+0)*MAP_SUBNUM_X+1,(ty+1)*MAP_SUBNUM_Y+1,cclm_rec);
    //Drawing the cube column
    cclm_rec=create_cust_col();
    clm_rec=cclm_rec->rec;
    fill_column_rec_simp(clm_rec, 0, cube,
        cube, cube, cube, cube, cube, 0x0, 0x0, 0x0);
    cust_col_add_or_update(lvl,(tx+2)*MAP_SUBNUM_X,(ty+1)*MAP_SUBNUM_Y+1,cclm_rec);
    //Adding static light
    unsigned char *stlight;
    stlight = create_stlight((tx+2)*MAP_SUBNUM_X,(ty+1)*MAP_SUBNUM_Y+1);
    set_stlight_subtile_h(stlight,7);
    stlight_add(lvl,stlight);
    //Drawing the whole cube tile
    for (k=0;k<MAP_SUBNUM_Y;k++)
      for (i=0;i<MAP_SUBNUM_X;i++)
      {
          cclm_rec=create_cust_col();
          clm_rec=cclm_rec->rec;
          fill_column_rec_simp(clm_rec, 0, cube,
              cube, cube, cube, cube, cube, 0x0, 0x0, 0x0);
          cust_col_add_or_update(lvl,(tx+3)*MAP_SUBNUM_X+i,(ty+1)*MAP_SUBNUM_Y+k,cclm_rec);
      }
    
    //Drawing description
    char cubestr[8];
    sprintf(cubestr,"%03x",(int)cube);
    int graf_idx;
    graf_idx=graffiti_add(lvl,tx,ty+2,0,cubestr,GRAFF_FONT_ADICLSSC,ORIENT_TWE,0x0184);
    if (graf_idx<0) return;
    //graffiti_update_columns(lvl,graf_idx);
}

/*
 * Draws screen for the rework mode.
 */
void draw_mdrwrk(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_rework_map_area(scrmode,workdata->mapmode,workdata->lvl,workdata->mdrwrk);
    if (workdata->ipanel->mode!=PV_MODE)
      draw_forced_panel(scrmode,workdata,workdata->ipanel->mode);
    else
      draw_mdrwrk_panel(scrmode,workdata);
    draw_map_cursor(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
}

void draw_mdrwrk_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_mdclm_panel(scrmode,workdata);
}

/*
 * Draws the map area for rework mode.
 * Also clears the right panel.
 */
void draw_rework_map_area(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,struct RWRK_DATA *mdrwrk)
{
    int i, k;
    for (k=0; k<scrmode->rows; k++)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(k,0);
      int ty=mapmode->map.y+k;
      if (ty >= lvl->tlsize.y)
      {
          for (i=0; i<scrmode->cols; i++)
            screen_printchr(' ');
      }
      else
      {
          for (i=0; i<scrmode->cols; i++)
          {
            int tx=mapmode->map.x+i;
            if (tx < lvl->tlsize.x)
            {
              char out_ch;
              int sx, sy;
              sx = tx*3+mapmode->subtl.x;
              sy = ty*3+mapmode->subtl.y;
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
