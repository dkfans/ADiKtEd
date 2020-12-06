/******************************************************************************/
// scr_thing.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   KeeperFX Team
// Created:  06 Dec 2020

// Purpose:
//   Defines functions for editing slab tilesets.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/


#include "../libadikted/globals.h"
#include "../libadikted/lev_data.h"
#include "../libadikted/msg_log.h"
#include "input_kb.h"
#include "output_scr.h"
#include "scr_actn.h"
#include "var_utils.h"

static int active_tileset = 0;
static short choose_tileset = 0; 

short start_mdtileset(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    scrmode->mode=MD_TLST;
    scrmode->usrinput_type=SI_NONE;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

void end_mdtileset(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    set_marking_disab(workdata->mapmode);
    set_painting_disab(workdata->mapmode);
    workdata->ipanel->mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
    scrmode->mode=MD_SLB;
}

void actions_mdtileset(struct SCRMODE_DATA *scrmode, struct WORKMODE_DATA *workdata, int key)
{
    message_release();
    if (choose_tileset)
    {
        switch (key)
        {
        case KEY_UP:
            active_tileset-=4;
            if (active_tileset < 0)
                active_tileset = 15;
            return;
        case KEY_DOWN:
            active_tileset+=4;
            if (active_tileset > 15)
                active_tileset = 0;
            return;
        case KEY_LEFT:
            active_tileset-=1;
            if (active_tileset < 0)
                active_tileset = 15;
            return;
        case KEY_RIGHT:
            active_tileset+=1;
            if (active_tileset > 15)
                active_tileset = 0;
            return;
        default:
            break;
        }
    }
    int tx=workdata->mapmode->screen.x+workdata->mapmode->map.x;
    int ty=workdata->mapmode->screen.y+workdata->mapmode->map.y;
    if (cursor_actions(scrmode,workdata,key))
    {
        if (is_painting_enab(workdata->mapmode))
        {
            set_slx_tileset(workdata->lvl, tx, ty, active_tileset);
        }
        return;
    }
    switch (key)
    {
    case KEY_ESCAPE:
        mdend[scrmode->mode](scrmode,workdata);
        break;
    case KEY_TAB:
        choose_tileset = !choose_tileset;
        break;
    case KEY_ENTER:
    case KEY_SPACE:
        if (choose_tileset)
        {
            choose_tileset = 0;
        }
        set_slx_tileset(workdata->lvl, tx, ty, active_tileset);
        break;
    case KEY_Z:
        if (!is_painting_enab(workdata->mapmode))
        {
          set_painting_enab(workdata->mapmode);
        } else
        {
          set_painting_disab(workdata->mapmode);
        }
        message_info("Paint mode %s",(is_painting_enab(workdata->mapmode))?"on":"off");
        break;
    default:
        message_info("Unrecognized \"%s\" key code: %d", longmodenames[scrmode->mode], key);
        speaker_beep();
    }
}

short color_mdtileset(struct SCRMODE_DATA *scrmode, struct MAPMODE_DATA *mapmode, struct LEVEL *lvl, int tx, int ty)
{
    short tileset = get_slx_tileset(lvl, tx, ty);
    return tileset + PRINT_COLOR_TILESET;
}

void draw_right_panel(struct SCRMODE_DATA *scrmode)
{
    static const char *texture_shortname[16] = { // 7 symbols
    "( DEF  )", "STNDRD", "ANCIENT", "WINTER",
     "SNAKE", "STONEFC", "BREASTS", "ROUGH",
     "SKULL", "(?  9 )", "(? 10 )", "(? 11 )",
     "(? 12 )", "(? 13 )", "(? 14 )", "(? 15 )",
    };
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            set_cursor_pos(2 + j, scrmode->cols + 3 + 9*i);
            screen_setcolor(choose_tileset?PRINT_COLOR_WHITE_ON_BLACK:0);
            screen_printf("%c", (active_tileset == (i + j * 4))?'*':' ');
            screen_setcolor( i + j * 4  + PRINT_COLOR_TILESET);
            screen_printf("%7s", texture_shortname[j*4 + i]);
        }
    }
}

void draw_mdtileset(struct SCRMODE_DATA *scrmode, struct WORKMODE_DATA *workdata)
{
    message_log(" draw_mdtileset: drawing map");
    draw_map_area(scrmode, workdata->mapmode,workdata->lvl, true, false, false);
    message_log(" draw_mdtileset: drawing standard panel");
    draw_right_panel(scrmode);
    draw_map_cursor(scrmode, workdata->mapmode,workdata->lvl, true, false, false);
}
