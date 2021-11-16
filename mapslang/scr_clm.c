/******************************************************************************/
// scr_clm.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines functions for initializing and displaying the column screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_clm.h"

#include "../libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "var_utils.h"

/*
 * Initializes variables for the column screen.
 */
short init_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    return true;
}

/*
 * Deallocates memory for the column screen.
 */
void free_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
}


/*
 * Covers actions from the column screen.
 */
void actions_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    
    if (!cursor_actions(scrmode,workdata,key))
    if (!subtl_select_actions(workdata->mapmode,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          mdend[MD_CLM](scrmode,workdata);
          break;
        case KEY_U: // Update all things/dat/clm/w?b
          action_update_all_datclm(scrmode,workdata);
          break;
        case KEY_A: // update dat/clm/w?b of selected tile
          action_delele_custclm_and_update(scrmode,workdata);
          break;
        case KEY_M: // manual-set mode
          mdstart[MD_CCLM](scrmode,workdata);
          break;
        case KEY_B: // cube mode
          mdstart[MD_CUBE](scrmode,workdata);
          break;
        default:
          message_info("Unrecognized clm key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start the column mode.
 */
short start_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata==NULL) return false;
    scrmode->mode=MD_CLM;
    scrmode->usrinput_type=SI_NONE;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

/*
 * Action function - end the column mode.
 */
void end_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->ipanel->mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
    scrmode->mode=MD_SLB;
    message_info("Returned to Slab mode.");
}

int display_dat_subtiles(const struct SCRMODE_DATA *scrmode,
    const struct WORKMODE_DATA *workdata,int scr_row, int scr_col,
    const short compressed,const int ty,const int tx)
{
    struct IPOINT_2D *subtl=&(workdata->mapmode->subtl);
    int i, k;
    int color;
    // Display .dat stuff for this tile
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row, scr_col);
    scr_row+=2;
    screen_printf("%s",".dat entries");
    for (k=0; k<3; k++)
    {
        int sy=ty*3+k;
        for (i=0; i<3; i++)
        {
            int sx=tx*3+i;
            set_cursor_pos(scr_row, scr_col+i*5);
            
            if ((i==subtl->x) && (k==subtl->y) &&
             ((scrmode->mode==MD_CLM)||(scrmode->mode==MD_RWRK)) )
                color=PRINT_COLOR_BLACK_ON_LGREY;
            else
                color=PRINT_COLOR_LGREY_ON_BLACK;
            screen_setcolor(color);
            if (workdata->ipanel->dat_view_mode==2)
                screen_printf("%4u",
                        (unsigned int)get_dat_subtile(workdata->lvl,sx,sy));
            else
                screen_printf("%04X",(unsigned int)get_dat_val(workdata->lvl,sx,sy));
        }
        scr_row++;
        if (!compressed)
          scr_row++;
    }
    return scr_row;
}

/*
 * Draws screen for the column mode.
 */
void draw_mdclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_map_area(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
    if (workdata->ipanel->mode!=PV_MODE)
      draw_forced_panel(scrmode,workdata,workdata->ipanel->mode);
    else
      draw_mdclm_panel(scrmode,workdata);
    draw_map_cursor(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
}

void draw_mdclm_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D tilpos;
    get_map_tile_pos(workdata->mapmode,&tilpos);
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    struct LEVEL *lvl=workdata->lvl;
    int clm_idx=get_dat_subtile(lvl,subpos.x,subpos.y);
    char *clmentry;
    clmentry = get_subtile_column(lvl,subpos.x,subpos.y);
    int pos_y;
    pos_y=display_column(clmentry, clm_idx,0,scrmode->cols+3);
    set_cursor_pos(pos_y-3, scrmode->cols+23);
    screen_printf_toeol("WIB anim.: %03X", get_subtl_wib(lvl,subpos.x,subpos.y));
    set_cursor_pos(pos_y-2, scrmode->cols+23);
    screen_printf("Tile WLB:  %03X", get_tile_wlb(lvl,tilpos.x,tilpos.y));
    set_cursor_pos(pos_y-1, scrmode->cols+23);
    screen_printf("Subtl.FLG: %03X", get_subtl_flg(lvl,subpos.x,subpos.y));
    display_rpanel_bottom(scrmode,workdata);
}

int display_column(unsigned char *clmentry,int clm_idx, int scr_row, int scr_col)
{
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    get_clm_entry(clm_rec, clmentry);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    int scr_col2=scr_col+20;
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("Use: %04X (dec: %d)", clm_rec->use,clm_rec->use);
    set_cursor_pos(scr_row, scr_col);
    screen_printf("Lintel: %d", clm_rec->lintel);
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("CLM index: %4d", clm_idx);
    set_cursor_pos(scr_row, scr_col);
    screen_printf("Solid mask: %04X", clm_rec->solid);
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("Height: %X", clm_rec->height);
    set_cursor_pos(scr_row, scr_col2);
    screen_printf("Permanent: %d", clm_rec->permanent);
    set_cursor_pos(scr_row+1, scr_col2);
    screen_printf("Orientation: %02X", clm_rec->orientation);
    int i;
    for (i=7; i>=0; i--)
    {
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Cube %d:  %03X", i, clm_rec->c[i]);
    }
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("Base bl: %03X", clm_rec->base);
    free_column_rec(clm_rec);
    return scr_row;
}

void action_update_all_datclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
          popup_show("Updating DAT/CLM for whole map","Regenarating whole map can take some time. Please wait...");
          update_slab_owners(workdata->lvl);
          update_datclm_for_whole_map(workdata->lvl);
          message_info("DAT/CLM/W?B entries updated for whole map.");
}

void action_delele_custclm_and_update(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D tilpos;
    get_map_tile_pos(workdata->mapmode,&tilpos);
    //Deleting custom columns - to enable auto-update
    cust_cols_del_for_tile(workdata->lvl,tilpos.x,tilpos.y);
    //Updating
    update_datclm_for_slab(workdata->lvl, tilpos.x,tilpos.y);
    update_tile_wib_entries(workdata->lvl,tilpos.x,tilpos.y);
    update_tile_wlb_entry(workdata->lvl,tilpos.x,tilpos.y);
    update_tile_flg_entries(workdata->lvl,tilpos.x,tilpos.y);
    message_info("Updated DAT/CLM/W?B entries of slab %d,%d.",tilpos.x,tilpos.y);
}
