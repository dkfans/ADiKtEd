/******************************************************************************/
// scr_slab.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines functions for initializing and displaying the slab screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_slab.h"

#include <time.h>
#include "../libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_help.h"
#include "scr_thing.h"
#include "var_utils.h"

int display_graffiti(struct LEVEL *lvl,int scr_row, int scr_col,int graff_idx);

/*
 * Initializes variables for the mdslab screen.
 */
short init_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata==NULL) return false;
    //Creating mdslab variable
    workdata->mdslab=(struct MDSLAB_DATA *)malloc(sizeof(struct MDSLAB_DATA));
    if (workdata->mdslab==NULL)
     die("init_mdslab: Cannot allocate memory.");
    //Initialize keys for displaying and putting slabs
    init_mdslab_keys(scrmode,workdata);
    time_t curr_time=time(NULL);
    struct tm *curr_tm=localtime(&curr_time);
    if ((curr_tm->tm_mday==25)&&(curr_tm->tm_mon==4))
      workdata->mapmode->eetype=EE_TLBIRTH;
    return true;
}

/*
 * Deallocates memory for the mdslab screen.
 */
void free_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  message_log(" free_mdslab: starting");
  if ((workdata==NULL)||(workdata->mdslab==NULL)) return;
  free_mdslab_keys(scrmode,workdata);
  free(workdata->mdslab);
  workdata->mdslab=NULL;
}

/*
 * Covers actions from the slab screen.
 */
void actions_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int d;
    char *msg;
    message_release();
    if (!cursor_actions(scrmode,workdata,key))
    {
        int tx=workdata->mapmode->map.x+workdata->mapmode->screen.x;
        int ty=workdata->mapmode->map.y+workdata->mapmode->screen.y;
        switch (key)
        {
        case KEY_ENTER:
          mdstart[MD_SLBL](scrmode,workdata);
          break;
        case KEY_U: // Update all things/dat/clm/w?b
          popup_show("Updating DAT/CLM for whole map","Regenarating whole map can take some time. Please wait...");
          update_slab_owners(workdata->lvl);
          update_datclm_for_whole_map(workdata->lvl);
          message_info("All DAT/CLM/W?B entries updated.");
          break;
        case KEY_V: // Verify whole map
          level_verify_with_highlight(workdata);
          break;
        case KEY_TAB:
          end_mdslab(scrmode,workdata);
          mdstart[MD_TNG](scrmode,workdata);
          message_info("Thing mode activated");
          break;
        case KEY_C:
          end_mdslab(scrmode,workdata);
          mdstart[MD_CLM](scrmode,workdata);
          message_info("Column mode activated");
          break;
        case KEY_R:
          end_mdslab(scrmode,workdata);
          mdstart[MD_RWRK](scrmode,workdata);
          message_info("Rework mode activated");
          break;
        case KEY_CTRL_X:
          msg=malloc(LINEMSG_SIZE*sizeof(char));
          level_generate_random_extension(workdata->lvl,msg);
          message_info("Random extension: %s",msg);
          free(msg);
          break;
        case KEY_CTRL_SPACE:
          if (is_marking_enab(workdata->mapmode))
          {
            set_marking_disab(workdata->mapmode);
          } else
          {
            set_marking_start(workdata->mapmode,tx,ty);
          }
          message_info("Mark mode %s",is_marking_enab(workdata->mapmode)?"on":"off");
          break;
        case KEY_CTRL_G:
          mdstart[MD_TLST](scrmode,workdata);
          break;
        case KEY_DEL: // Delete graffiti if there is any here
          d = graffiti_idx(workdata->lvl,tx,ty);
          if (d>=0)
          {
            message_info("Graffiti deleted");
            graffiti_clear_from_columns(workdata->lvl,d);
            graffiti_del(workdata->lvl,d);
          } else
            message_error("Nothing to delete");
          break;
        case KEY_A: // Change graffiti orientation
            slb_next_graffiti_orient(workdata->lvl,graffiti_idx(workdata->lvl,tx,ty));
          break;
        case KEY_D: // Add/view graffiti down
            if ((is_marking_enab(workdata->mapmode)) || is_painting_enab(workdata->mapmode))
            {
              message_error("Can't draw graffiti whilst painting or marking");
              return;
            }
            mdstart[MD_GRFT](scrmode,workdata);
          break;
        case KEY_NUM0:
            slb_change_ownership(scrmode,workdata,PLAYER0);
            break;
        case KEY_NUM1:
            slb_change_ownership(scrmode,workdata,PLAYER1);
            break;
        case KEY_NUM2:
            slb_change_ownership(scrmode,workdata,PLAYER2);
            break;
        case KEY_NUM3:
            slb_change_ownership(scrmode,workdata,PLAYER3);
            break;
        case KEY_NUM4:
            slb_change_ownership(scrmode,workdata,PLAYER_GOOD);
            break;
        case KEY_NUM5:
            slb_change_ownership(scrmode,workdata,PLAYER_UNSET);
            break;
        case KEY_SQRBRCKTL:
          d = graffiti_idx(workdata->lvl,tx,ty);
          if (d>=0)
          {
            slb_change_graffiti_height(workdata->lvl,d,-1);
          } else
            message_error("Nothing to decrease");
          break;
        case KEY_SQRBRCKTR:
          d = graffiti_idx(workdata->lvl,tx,ty);
          if (d>=0)
          {
            slb_change_graffiti_height(workdata->lvl,d,1);
          } else
            message_error("Nothing to increase");
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
        case KEY_ESCAPE:
            message_info(get_random_tip(workdata->help));
            break;
        case KEY_F:
            slb_place_room(workdata,get_random_wall_slab());
            if (is_painting_enab(workdata->mapmode))
              set_painting_rndwall(workdata->mapmode);
            break;
        default:
            {
              unsigned char *placekeys=workdata->mdslab->placekeys;
              if ((key < workdata->mdslab->placenkeys) && (placekeys[key]!=255))
              {
                slb_place_room(workdata,placekeys[key]);
                if (is_painting_enab(workdata->mapmode))
                  set_painting_slab(workdata->mapmode,placekeys[key]);
              } else
              {
              message_info("Unrecognized slb key code: %d",key);
              speaker_beep();
              }
            }
        }
    }
    slbposcheck(scrmode,workdata);
}

/*
 * Action function - start the mdslab mode.
 */
short start_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    scrmode->mode=MD_SLB;
    scrmode->usrinput_type=SI_NONE;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

/*
 * Action function - end the mdslab mode.
 */
void end_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    set_marking_disab(workdata->mapmode);
    set_painting_disab(workdata->mapmode);
    workdata->ipanel->mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
}

/*
 * Action subfunction - check position in slb mode.
 */
void slbposcheck(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_painting_slab(workdata->mapmode))
        slb_place_room(workdata,get_painting_slab(workdata->mapmode));
    if (is_painting_ownr(workdata->mapmode))
        change_ownership(workdata,get_painting_ownr(workdata->mapmode));
}

/*
 * Action function - place room in slb mode.
 * Places complete room, including things and CLM update
 * (these updates can be disabled).
 */
void slb_place_room(struct WORKMODE_DATA *workdata,unsigned char room)
{
    int markr,markb;
    int markl,markt;
    if (is_marking_enab(workdata->mapmode))
    {
      markr=workdata->mapmode->markr.r;
      markb=workdata->mapmode->markr.b;
      markl=workdata->mapmode->markr.l;
      markt=workdata->mapmode->markr.t;
    } else
    {
      markr=workdata->mapmode->screen.x+workdata->mapmode->map.x;
      markb=workdata->mapmode->screen.y+workdata->mapmode->map.y;
      markl=markr;
      markt=markb;
    }
    user_set_slab_rect(workdata->lvl, markl, markr, markt, markb, room);
    set_marking_disab(workdata->mapmode);
}

/*
 * Action sub-function - change the owner of slab.
 */
void change_ownership(struct WORKMODE_DATA *workdata,unsigned char purchaser)
{
    int tx=workdata->mapmode->screen.x+workdata->mapmode->map.x;
    int ty=workdata->mapmode->screen.y+workdata->mapmode->map.y;
    // Sanity check, almost certainly unneeded
    if ((tx >= workdata->lvl->tlsize.x) || (tx >= workdata->lvl->tlsize.y) ||
        (tx < 0) || (ty < 0))
      return;
    if (!is_marking_enab(workdata->mapmode))
    {
      set_tile_owner(workdata->lvl,tx,ty,purchaser);
      if (get_obj_auto_update(workdata->lvl))
        update_obj_for_square_radius1(workdata->lvl,tx,ty);
      if (get_datclm_auto_update(workdata->lvl))
        update_datclm_for_square_radius1(workdata->lvl,tx,ty);
      if (get_obj_auto_update(workdata->lvl))
        update_obj_subpos_and_height_for_square_radius1(workdata->lvl,tx,ty);
      return;
    }
    // another sanity check - this time for marking mode
    if ((workdata->mapmode->markr.r+1 > workdata->lvl->tlsize.x) || (workdata->mapmode->markr.b+1 > workdata->lvl->tlsize.y) ||
      (workdata->mapmode->markr.l < 0) || (workdata->mapmode->markr.t < 0))
      return;
    for (tx=workdata->mapmode->markr.l; tx<=workdata->mapmode->markr.r; tx++)
      for (ty=workdata->mapmode->markr.t; ty<=workdata->mapmode->markr.b; ty++)
          set_tile_owner(workdata->lvl,tx,ty,purchaser);
      if (get_obj_auto_update(workdata->lvl))
          update_obj_for_square(workdata->lvl, workdata->mapmode->markr.l-1, workdata->mapmode->markr.r+1, workdata->mapmode->markr.t-1, workdata->mapmode->markr.b+1);
      if (get_datclm_auto_update(workdata->lvl))
          update_datclm_for_square(workdata->lvl, workdata->mapmode->markr.l-1, workdata->mapmode->markr.r+1, workdata->mapmode->markr.t-1, workdata->mapmode->markr.b+1);
      if (get_obj_auto_update(workdata->lvl))
          update_obj_subpos_and_height_for_square(workdata->lvl, workdata->mapmode->markr.l-1, workdata->mapmode->markr.r+1, workdata->mapmode->markr.t-1, workdata->mapmode->markr.b+1);
    set_marking_disab(workdata->mapmode);
}

/*
 * Action function - change the owner of a slab and display message.
 */
void slb_change_ownership(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,unsigned char purchaser)
{
    if (is_painting_enab(workdata->mapmode))
        set_painting_ownr(workdata->mapmode,purchaser);
    change_ownership(workdata,purchaser);
    message_info("Slab owner changed to %s",get_owner_type_fullname(purchaser));
}

void slb_change_graffiti_height(struct LEVEL *lvl,int graf_idx,int delta)
{
    struct DK_GRAFFITI *graf;
    graf=get_graffiti(lvl,graf_idx);
    if (graf==NULL)
    {
      message_error("Graffiti doesn't exist");
      return;
    }
    int nheight=graf->height+delta;
    char *oper;
    if (delta>0)
        oper="in";
    else
        oper="de";
    if (set_graffiti_height(graf,nheight)==nheight)
    {
      message_info("Graffiti height %screased",oper);
      graffiti_update_columns(lvl,graf_idx);
    } else
      message_error("Graffiti height limit reached");
}

void slb_next_graffiti_orient(struct LEVEL *lvl,int graf_idx)
{
    if (graf_idx<0)
    {
        message_error("Nothing to change found");
        return;
    }
    struct DK_GRAFFITI *graf;
    graf=get_graffiti(lvl, graf_idx);
    if (graf==NULL)
    {
        message_error("Bad graffiti object index");
        return;
    }
    unsigned short new_orient=get_orientation_next(graf->orient);
    graffiti_clear_from_columns(lvl,graf_idx);
    short result=set_graffiti_orientation(graf,lvl,new_orient);
    graffiti_update_columns(lvl,graf_idx);
    if (!result)
    {
        message_error("Cannot change orientation");
        return;
    }
    message_info("Graffiti orientation altered");
}

/*
 * Init function - creates arrays for keyboard shortcuts in slab mode.
 */
void init_mdslab_keys(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    int i;
    // Arrays storing keyboard shortcuts and corresponding rooms
    // These must end with zero
    static const unsigned int room_keys[]={
      KEY_T,       KEY_SHIFT_L,   KEY_SHIFT_H,
      KEY_SHIFT_T, KEY_L,         KEY_PERCENT,
      KEY_SHIFT_W, KEY_SHIFT_G,   KEY_SHIFT_P,
      KEY_SHIFT_O, KEY_SHIFT_B,   KEY_E,
      KEY_G,       KEY_S,         KEY_SHIFT_S,
      KEY_H,       KEY_HASH,      KEY_DOT,
      KEY_NMINUS,  KEY_SPACE,     KEY_SLASH,
      KEY_DASH,    KEY_BSLASH,    KEY_AMPRSNT,
      KEY_ASTERIX, KEY_RNDBRCKTL, KEY_DOLLAR,
      KEY_EXCLAM,  KEY_APOSTRT,   KEY_TILDE,
      KEY_EQUAL,   KEY_W,         KEY_B,
      KEY_I,       KEY_M,         KEY_SHIFT_E,
       0};
    static const unsigned char room_types[]={
      SLAB_TYPE_TREASURE, SLAB_TYPE_LAIR, SLAB_TYPE_HATCHERY,
      SLAB_TYPE_TRAINING, SLAB_TYPE_LIBRARY, SLAB_TYPE_BRIDGE,
      SLAB_TYPE_WORKSHOP, SLAB_TYPE_GUARDPOST, SLAB_TYPE_PRISONCASE,
      SLAB_TYPE_TORTURE, SLAB_TYPE_BARRACKS, SLAB_TYPE_TEMPLE,
      SLAB_TYPE_GRAVEYARD, SLAB_TYPE_SCAVENGER, SLAB_TYPE_SCAVENGER,
      SLAB_TYPE_DUNGHEART, SLAB_TYPE_ROCK, SLAB_TYPE_EARTH,
      SLAB_TYPE_PATH, SLAB_TYPE_CLAIMED, SLAB_TYPE_TORCHDIRT,
      SLAB_TYPE_WALLDRAPE, SLAB_TYPE_WALLTORCH, SLAB_TYPE_WALLWTWINS,
      SLAB_TYPE_WALLWWOMAN, SLAB_TYPE_WALLPAIRSHR, SLAB_TYPE_GOLD,
      SLAB_TYPE_GEMS, SLAB_TYPE_WATER, SLAB_TYPE_WATER,
      SLAB_TYPE_LAVA, SLAB_TYPE_DOORWOOD1, SLAB_TYPE_DOORBRACE1,
      SLAB_TYPE_DOORIRON1, SLAB_TYPE_DOORMAGIC1, SLAB_TYPE_PORTAL,
      0};

    if (sizeof(room_keys)/sizeof(*room_keys) != sizeof(room_types)/sizeof(*room_types))
      die("init_keys: Number of rooms doesn't match number of keys");
    i=0;
    unsigned int nkeys=1;
    while (room_keys[i])
    {
      if (room_keys[i] > nkeys)
        nkeys=room_keys[i];
      i++;
    }
    nkeys++;
    workdata->mdslab->placekeys=(unsigned char *)malloc(nkeys*sizeof(char));
    if ((workdata->mdslab->placekeys)==NULL)
      die("init_keys: Can't alloc memory for key table");

     // I don't *think* 255 is used by slb... if it is, we can always put
     // the key in manually in slbactions
    for (i=0; i < nkeys; i++)
      workdata->mdslab->placekeys[i]=255;
    workdata->mdslab->placenkeys=nkeys;
    i=0;
    while (room_keys[i])
    {
      workdata->mdslab->placekeys[room_keys[i]]=room_types[i];
      i++;
    }

    const int MAX_NUM_KEYS=256;
    workdata->mapmode->slbkey=(char *)malloc(MAX_NUM_KEYS);
    if (!workdata->mapmode->slbkey)
      die("init_slbkey: Out of memory");
    for (i=0; i < MAX_NUM_KEYS; i++)
      workdata->mapmode->slbkey[i]='?';
    i=0;
    while (room_keys[i])
    {
      workdata->mapmode->slbkey[room_types[i]]=key_to_ascii(room_keys[i]);
      i++;
    }
    // Hack for some slabs that are not in room_types[], 
    // or if we just want another char to view
    workdata->mapmode->slbkey[SLAB_TYPE_DOORWOOD2]='w';
    workdata->mapmode->slbkey[SLAB_TYPE_DOORBRACE2]='b';
    workdata->mapmode->slbkey[SLAB_TYPE_DOORIRON2]='i';
    workdata->mapmode->slbkey[SLAB_TYPE_DOORMAGIC2]='m';
}

/*
 * Frees memory allocated by init_mdslab_keys. Should be called in free_mdslab().
 */
void free_mdslab_keys(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata==NULL) return;
    if (workdata->mdslab!=NULL)
      free(workdata->mdslab->placekeys);
    if (workdata->mapmode!=NULL)
      free(workdata->mapmode->slbkey);
}

/*
 * Draws screen for the mdslab mode.
 */
void draw_mdslab(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" draw_mdslab: drawing map");
    draw_map_area(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
    if (workdata->ipanel->mode!=PV_MODE)
    {
      message_log(" draw_mdslab: drawing forced panel");
      draw_forced_panel(scrmode,workdata,workdata->ipanel->mode);
    } else
    {
      message_log(" draw_mdslab: drawing standard panel");
      draw_mdslab_panel(scrmode,workdata);
    }
    draw_map_cursor(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
}

void draw_mdslab_panel(const struct SCRMODE_DATA *scrmode,const struct WORKMODE_DATA *workdata)
{
    int tx,ty;
    tx=workdata->mapmode->map.x+workdata->mapmode->screen.x;
    ty=workdata->mapmode->map.y+workdata->mapmode->screen.y;
    int scr_row=0;
    int scr_col=scrmode->cols+3;
    int graff_idx=graffiti_idx(workdata->lvl,tx,ty);
    if (graff_idx<0)
    {
//        message_log(" draw_mdslab_panel: not on graffiti");
        scr_row=display_mode_keyhelp(workdata->help,scr_row,scr_col,scrmode->rows-TNGDAT_ROWS-2,scrmode->mode,0);
    } else
    {
//        message_log(" draw_mdslab_panel: drawing graffiti");
        scr_row=display_graffiti(workdata->lvl,scr_row,scr_col,graff_idx);
    }
    if (scrmode->rows >= scr_row+TNGDAT_ROWS+3)
    {
        set_cursor_pos(scrmode->rows-TNGDAT_ROWS-2, scr_col);
        unsigned short slb_type=get_tile_slab(workdata->lvl,tx,ty);
        screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
        screen_printf(".slb entry:%3d ",slb_type);
        screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
        screen_printf("%s",get_slab_fullname(slb_type));
        if (slb_type==SLAB_TYPE_BRIDGE)
        {
          screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
          screen_printf(" on ");
          screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
          unsigned short wlb_val=get_tile_wlb(workdata->lvl,tx,ty);
          if (wlb_val==TILE_WLB_LAVA)
            screen_printf("%s",get_slab_fullname(SLAB_TYPE_LAVA));
          else
            screen_printf("%s",get_slab_fullname(SLAB_TYPE_WATER));
      }
    }
//    message_log(" draw_mdslab_panel: drawing bottom");
    display_rpanel_bottom(scrmode,workdata);
}

int display_graffiti(struct LEVEL *lvl,int scr_row, int scr_col,int graff_idx)
{
    int scr_col1=scr_col+2;
    int scr_col2=scr_col+20;
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("%s %d","Graffiti",graff_idx+1);
    struct DK_GRAFFITI *graf=get_graffiti(lvl,graff_idx);
    if (graf==NULL) return scr_row;
    set_cursor_pos(scr_row, scr_col1);
    screen_printf("Position: %d,%d",graf->tile.x,graf->tile.y);
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("End: %d,%d",graf->fin_tile.x,graf->fin_tile.y);
    set_cursor_pos(scr_row, scr_col1);
    screen_printf("Orientation: %s",get_orientation_shortname(graf->orient));
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("Font: %s",get_font_longname(graf->font));
    set_cursor_pos(scr_row, scr_col1);
    screen_printf("Height pos: %d",graf->height);
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("Cube: 0x%04x",graf->cube);
    set_cursor_pos(scr_row++, scr_col1);
    screen_printf("Text: ");
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf("%s",graf->text);
    return scr_row;
}

/*
 * Action function - start the mdgrafit mode.
 */
short start_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    scrmode->mode=MD_GRFT;
    scrmode->usrinput_type=SI_GRAFT;
    inc_info_usr_mdswtch_count(workdata->lvl);
    message_info("Graffiti mode started");
    return true;
}

/*
 * Action function - end the mdgrafit mode.
 */
void end_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->ipanel->mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
    scrmode->mode=MD_SLB;
}

/*
 * Draws screen for the mdgrafit mode.
 */
void draw_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_map_area(scrmode,workdata->mapmode,workdata->lvl,true,true,false);
    if (workdata->ipanel->mode!=PV_MODE)
      draw_forced_panel(scrmode,workdata,workdata->ipanel->mode);
    else
      draw_mdslab_panel(scrmode,workdata);
}


/*
 * Covers actions from the graffiti input screen.
 */
void actions_mdgrafit(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    short text_changed;
    if (!string_get_actions(scrmode,key,&text_changed))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          mdend[MD_GRFT](scrmode,workdata);
          message_info("Graffiti creation cancelled");
          break;
        case KEY_ENTER:
          {
          int tx,ty;
          tx=workdata->mapmode->map.x+workdata->mapmode->screen.x;
          ty=workdata->mapmode->map.y+workdata->mapmode->screen.y;
          int graf_idx;
          graf_idx=graffiti_add(workdata->lvl,tx,ty,0,scrmode->usrinput,GRAFF_FONT_ADICLSSC,ORIENT_NS,0x0184);
          mdend[MD_GRFT](scrmode,workdata);
          if (graf_idx<0)
          {
            message_error("Cannot add graffiti(not enought room?)");
            break;
          }
          graffiti_update_columns(workdata->lvl,graf_idx);
          message_info("Graffiti placed");
          };break;
        default:
          message_info("Unrecognized \"%s\" key code: %d",longmodenames[MD_GRFT],key);
          speaker_beep();
      }
    }
}

