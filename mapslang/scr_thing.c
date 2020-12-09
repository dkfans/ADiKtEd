/******************************************************************************/
// scr_thing.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines functions for initializing and displaying the thing screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_thing.h"

#include <math.h>
#include "../libadikted/adikted.h"
#include "scr_help.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_thing.h"
#include "var_utils.h"

/*
 * Initializes variables for the tng screen.
 */
short init_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing mdtng variable
    workdata->mdtng=(struct MDTNG_DATA *)malloc(sizeof(struct MDTNG_DATA));
    if (workdata->mdtng==NULL)
     die("init_mdtng: Cannot allocate memory.");
    workdata->mdtng->obj_ranges_changed=true;
    return true;
}

/*
 * Deallocates memory for the tng screen.
 */
void free_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  message_log(" free_mdtng: starting");
  if ((workdata==NULL)||(workdata->mdtng==NULL)) return;
  free(workdata->mdtng);
  workdata->mdtng=NULL;
}

void change_visited_tile(struct WORKMODE_DATA *workdata)
{
    struct MDTNG_DATA *mdtng=workdata->mdtng;
    int i,j;
    for (i=0; i<MAP_SUBNUM_X; i++)
      for (j=0; j<MAP_SUBNUM_Y; j++)
        mdtng->vistng[i][j]=0;
}

/*
 * Returns visited object index on subtile
 */
int get_visited_obj_idx(struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D *subtl=&(workdata->mapmode->subtl);
    return workdata->mdtng->vistng[(subtl->x)%3][(subtl->y)%3];
}

/*
 * Set a visited object on subtile to the exact index
 */
void set_visited_obj_idx(struct WORKMODE_DATA *workdata,int obj_idx)
{
    struct IPOINT_2D *subtl=&(workdata->mapmode->subtl);
    workdata->mdtng->vistng[(subtl->x)%3][(subtl->y)%3]=obj_idx;
}

/*
 * Set a visited object on subtile to the last of given category
 */
void set_visited_obj_lastof(struct WORKMODE_DATA *workdata,short obj_type)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    struct IPOINT_2D *subtl=&(workdata->mapmode->subtl);
    int obj_idx=get_object_subtl_last(workdata->lvl,subpos.x,subpos.y,obj_type);
    workdata->mdtng->vistng[(subtl->x)%3][(subtl->y)%3]=obj_idx;
}

/*
 * Covers actions from the tng screen.
 */
void actions_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    unsigned char *thing;
    message_release();
    
    if (!cursor_actions(scrmode,workdata,key))
    if (!subtl_select_actions(workdata->mapmode,key))
    {
      switch (key)
      {
        case KEY_TAB:
          end_mdtng(scrmode,workdata);
          mdstart[MD_SLB](scrmode,workdata);
          message_info("Slab mode activated");
          break;
        case KEY_C: //add creature
          if (workdata->mapmode->creature_list_on_create)
            mdstart[MD_CRTR](scrmode,workdata);
          else
            tng_makecreature(scrmode,workdata,subpos.x,subpos.y,CREATR_SUBTP_WIZRD);
          break;
        case KEY_I: //add item
          mdstart[MD_CITM](scrmode,workdata);
          break;
        case KEY_B: // create spell
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_SPELLHOE);
          break;
        case KEY_H: // Add Dungeon heart
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_DNHEART);
          break;
        case KEY_SHIFT_H: // Add hero gate
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_HEROGATE);
          break;
        case KEY_D: // Dungeon special
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_SPREVMAP);
          break;
        case KEY_G: // Gold
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_GOLDCHEST);
          break;
        case KEY_SHIFT_G: // Gold
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_GOLD);
          break;
        case KEY_F: // Food
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_CHICKNSTB);
          break;
        case KEY_R: // create torch
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_TORCH);
          break;
        case KEY_P: // Poles and prison bars
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_FLAGPOST);
          break;
        case KEY_SHIFT_T: // Create trap box
          tng_makeitem(scrmode,workdata,subpos.x,subpos.y,ITEM_SUBTYPE_TBBOULDER);
          break;
        case KEY_E: // Add effect gererator
          if (workdata->mapmode->effectgen_list_on_create)
            mdstart[MD_CEFC](scrmode,workdata);
          else
            tng_makeeffectgen(scrmode,workdata,subpos.x,subpos.y,EFCTGEN_SUBTP_DRIPWTR);
          break;
        case KEY_A: // Add action point
          thing = create_actnpt(workdata->lvl, subpos.x,subpos.y);
          actnpt_add(workdata->lvl,thing);
          message_info("Added action point %d",(unsigned int)get_actnpt_number(thing));
          set_visited_obj_lastof(workdata,OBJECT_TYPE_ACTNPT);
          set_brighten_for_actnpt(workdata->mapmode,thing);
          inc_info_usr_creatobj_count(workdata->lvl);
          break;
        case KEY_SHIFT_L: // Add static light
          thing = create_stlight(subpos.x,subpos.y);
          stlight_add(workdata->lvl,thing);
          message_info("Added static light");
          set_visited_obj_lastof(workdata,OBJECT_TYPE_STLIGHT);
          set_brighten_for_stlight(workdata->mapmode,thing);
          inc_info_usr_creatobj_count(workdata->lvl);
          break;
        case KEY_K: // Copy thing to clipboard
          {
            int visiting_z=get_visited_obj_idx(workdata);
            short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
            thing=get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              copy_to_clipboard_thing(scrmode,thing);
              message_info("Thing is now in clipboard");
              break;
            case OBJECT_TYPE_ACTNPT:
              copy_to_clipboard_actnpt(scrmode,thing);
              message_info("Action point is now in clipboard");
              break;
            case OBJECT_TYPE_STLIGHT:
              copy_to_clipboard_stlight(scrmode,thing);
              message_info("Static light is now in clipboard");
              break;
            default:
              message_error("Can't copy to clipboard: nothing selected");
              break;
            }
          };break;
        case KEY_Y: // Create thing from clipboard
          if (scrmode->clip_count>0)
          {
            struct CLIPBOARD *clip_itm;
            clip_itm=get_clipboard_object(scrmode,0);
            if (clip_itm==NULL)
            {
              message_error("Elements in clipboard are not objects, can't paste.");
              break;
            }
            switch (clip_itm->dtype)
            {
            case OBJECT_TYPE_THING:
                thing = create_thing_copy(workdata->lvl,subpos.x,subpos.y,clip_itm->data);
                thing_add(workdata->lvl,thing);
                message_info("Thing pasted from clipboard at subtile %d,%d",subpos.x,subpos.y);
                if (is_effectgen(thing))
                    set_brighten_for_thing(workdata->mapmode,thing);
                set_visited_obj_lastof(workdata,OBJECT_TYPE_THING);
                inc_info_usr_creatobj_count(workdata->lvl);
                break;
            case OBJECT_TYPE_ACTNPT:
                thing = create_actnpt_copy(subpos.x,subpos.y,clip_itm->data);
                actnpt_add(workdata->lvl,thing);
                set_brighten_for_actnpt(workdata->mapmode,thing);
                message_info("Action point pasted from clipboard at subtile %d,%d",subpos.x,subpos.y);
                set_visited_obj_lastof(workdata,OBJECT_TYPE_ACTNPT);
                inc_info_usr_creatobj_count(workdata->lvl);
                break;
            case OBJECT_TYPE_STLIGHT:
                thing = create_stlight_copy(subpos.x,subpos.y,clip_itm->data);
                stlight_add(workdata->lvl,thing);
                message_info("Static light pasted from clipboard at subtile %d,%d",subpos.x,subpos.y);
                set_brighten_for_stlight(workdata->mapmode,thing);
                set_visited_obj_lastof(workdata,OBJECT_TYPE_STLIGHT);
                inc_info_usr_creatobj_count(workdata->lvl);
                break;
            default:
                message_error("Internal error: can't paste this object type.");
                break;
            }
          } else
          {
            message_error("Can't create object: clipboard is empty");
          };break;
        case KEY_U: // Update all things
          { //Storing stats on objects adding/removal
          struct LEVSTATS *stats=get_lvl_stats(workdata->lvl);
          int prev_tng_rmv=stats->things_removed;
          int prev_tng_add=stats->things_added;
          update_slab_owners(workdata->lvl);
          update_obj_for_whole_map(workdata->lvl);
          update_obj_subpos_and_height_for_whole_map(workdata->lvl);
          message_info("Auto-maintained TNG entries updated, %u added, %u removed.",
              stats->things_added-prev_tng_add,stats->things_removed-prev_tng_rmv);
          workdata->mdtng->obj_ranges_changed=true;
          change_visited_tile(workdata);
          };break;
        case KEY_CTRL_D: // Delete all things which can be auto-created.
          {
          struct LEVSTATS *stats=get_lvl_stats(workdata->lvl);
          int prev_tng_rmv=stats->things_removed;
          int prev_lgt=get_lgt_total_count(workdata->lvl);
          remove_automade_obj_for_whole_map(workdata->lvl);
          message_info("All %u auto-maintained or noncrucial objects removed.",
              stats->things_removed-prev_tng_rmv+prev_lgt-get_lgt_total_count(workdata->lvl));
          workdata->mdtng->obj_ranges_changed=true;
          change_visited_tile(workdata);
          };break;
        case KEY_V: // Verify whole map
          level_verify_with_highlight(workdata);
          break;
        case KEY_T: // Create trap
          if (workdata->mapmode->traps_list_on_create)
            mdstart[MD_CTRP](scrmode,workdata);
          else
            tng_maketrap(scrmode,workdata,subpos.x,subpos.y,TRAP_SUBTYPE_BOULDER);
          break;
        case KEY_L: // Lock / unlock a door
          {
            int visiting_z=get_visited_obj_idx(workdata);
            short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing=get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
              if (get_thing_type(thing)==THING_TYPE_DOOR)
              {
                  if (get_door_lock(thing)!=DOOR_PASS_UNLOCKED)
                  {
                    set_door_lock(workdata->lvl,thing,DOOR_PASS_UNLOCKED);
                    message_info("Door unlocked");
                  } else
                  {
                    set_door_lock(workdata->lvl,thing,DOOR_PASS_LOCKED);
                    message_info("Door locked");
                  }
              } else
              {
                  message_error("Selected thing cannot be locked.");
              }
              break;
            default:
                  message_error("Only things can be locked.");
              break;
            }
          };break;
        case KEY_O: // Change ownership of creature/trap/special/spell
          {
            int visiting_z=get_visited_obj_idx(workdata);
            short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
              set_thing_owner(thing,get_owner_next(get_thing_owner(thing)));
              message_info("Object owner switched");
              break;
            default:
              message_error("Can't change owner: no \"thing\" selected.");
              break;
            }
          };break;
        case KEY_SHIFT_S:
        case KEY_SHIFT_X: // Change type of creature
          {
            int visiting_z=get_visited_obj_idx(workdata);
            short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
              if (switch_thing_subtype(thing,(key==KEY_SHIFT_S)))
              {
                message_info("Thing type switched to next.");
              } else
                message_error("This thing has no level/type, or its limit is reached.");
              break;
            case OBJECT_TYPE_ACTNPT:
              thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
              tng_change_actnpt_level(workdata,thing,(key==KEY_SHIFT_S));
              break;
            default:
              message_error("Can't change type: no \"thing\" selected.");
              break;
            }
          };break;
        case KEY_S: // Change level of creature/trap/special/spell/lair/efct gen
          action_inc_object_level(scrmode,workdata);
          break;
        case KEY_X: // Change level of creature/trap/special/spell/lair/efct gen
          action_dec_object_level(scrmode,workdata);
          break;
        case KEY_SLASH: // Cycle through highlighted things
          {
          unsigned int obj_num=get_object_subnums(workdata->lvl,subpos.x,subpos.y);
          if (obj_num > 0)
            set_visited_obj_idx(workdata,(get_visited_obj_idx(workdata)+1)%(obj_num));
          };break;
        case KEY_SQRBRCKTL:
          {
            int visiting_z=get_visited_obj_idx(workdata);
            tng_change_height(scrmode,workdata,subpos.x,subpos.y,visiting_z,-64);
          };break;
        case KEY_SQRBRCKTR:
          {
            int visiting_z=get_visited_obj_idx(workdata);
            tng_change_height(scrmode,workdata,subpos.x,subpos.y,visiting_z,+64);
          };break;

        case KEY_NMINUS:
        case KEY_UNDERLN:
          {
            int visiting_z=get_visited_obj_idx(workdata);
            tng_change_range(scrmode,workdata,subpos.x,subpos.y,visiting_z,-64);
          };break;
        case KEY_NPLUS:
        case KEY_EQUAL:
          {
            int visiting_z=get_visited_obj_idx(workdata);
            tng_change_range(scrmode,workdata,subpos.x,subpos.y,visiting_z,+64);
          };break;

        case KEY_DEL: // delete
          action_delete_object(scrmode,workdata);
          break;
        case KEY_ENTER:
          action_edit_object(scrmode,workdata);
          break;
        case KEY_ESCAPE:
          message_info(get_random_tip(workdata->help));
          break;
        default:
          message_info("Unrecognized tng key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start the thing mode.
 */
short start_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->mapmode->mark=false;
    change_visited_tile(workdata);
    workdata->mdtng->obj_ranges_changed=true;
    scrmode->usrinput_type=SI_NONE;
    scrmode->mode=MD_TNG;
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

/*
 * Action function - end the thing mode.
 */
void end_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    workdata->ipanel->mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
}

/*
 * Draws screen for the thing mode.
 */
void draw_mdtng(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata->mdtng->obj_ranges_changed)
    {
      update_brighten(workdata->lvl,workdata->mapmode);
      workdata->mdtng->obj_ranges_changed=false;
    }
    draw_map_area(scrmode,workdata->mapmode,workdata->lvl,true,false,true);
    if (workdata->ipanel->mode!=PV_MODE)
      draw_forced_panel(scrmode,workdata,workdata->ipanel->mode);
    else
      draw_mdtng_panel(scrmode,workdata);
    draw_map_cursor(scrmode,workdata->mapmode,workdata->lvl,true,false,true);
    return;
}

void draw_mdtng_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    int k=0;
    int visiting_z=get_visited_obj_idx(workdata);
    short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
    char *obj=get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
    int scr_col1=scrmode->cols+3;
    switch (obj_type)
    {
    case OBJECT_TYPE_ACTNPT:
      display_action_point(obj, scr_col1, 1, scrmode->rows, workdata->ipanel->display_float_pos);
      break;
    case OBJECT_TYPE_STLIGHT:
      display_static_light(obj, scr_col1, 1, scrmode->rows, workdata->ipanel->display_float_pos);
      break;
    case OBJECT_TYPE_THING:
      display_thing(workdata->help, obj, &(workdata->lvl->tlsize), scr_col1, 1, scrmode->rows, workdata->ipanel->display_float_pos);
      break;
    default:
      k=display_mode_keyhelp(workdata->help,k,scr_col1,scrmode->rows-TNGDAT_ROWS,scrmode->mode,0);
      display_obj_stats(scrmode,workdata,k,scr_col1);
      break;
    }
    display_rpanel_bottom(scrmode,workdata);
}

char get_thing_char(const struct LEVEL *lvl, int x, int y)
{
    switch (get_object_tilnums(lvl,x,y))
    {
    case  0:
      return ' ';
      break;
    case  1:
      {
        int i,k;
        short obj_type;
        for (k=0;k<MAP_SUBNUM_Y;k++)
        {
          for (i=0;i<MAP_SUBNUM_X;i++)
          {
            obj_type=get_object_type(lvl,MAP_SUBNUM_X*x+i,MAP_SUBNUM_Y*y+k,0);
            if (obj_type!=OBJECT_TYPE_NONE) break;
          }
          if (obj_type!=OBJECT_TYPE_NONE) break;
        }
        switch (obj_type)
        {
        case OBJECT_TYPE_ACTNPT:
          return 'a';
        case OBJECT_TYPE_STLIGHT:
          return 'L';
        case OBJECT_TYPE_THING:
          {
            unsigned char *thing;
            thing=get_thing(lvl,x*MAP_SUBNUM_X+i,y*MAP_SUBNUM_Y+k,0);
            switch (get_thing_type(thing))
            {
            case THING_TYPE_CREATURE:
              return 'c';
            case THING_TYPE_EFFECTGEN:
              return 'e';
            case THING_TYPE_TRAP:
              return 't';
            case THING_TYPE_DOOR:
              return '1';
            case THING_TYPE_ITEM:
              {
              if (is_trapbox(thing))
                return 'T';
              if (is_dngspecbox(thing))
                return 'd';
              if (is_spellbook(thing))
                return 'b';
              if (is_herogate(thing))
                return 'H';
              if (is_gold(thing))
                return 'g';
              if (is_food(thing))
                return 'f';
              if (is_dnheart(thing))
                return 'h';
              if (is_torch(thing))
                return 'r';
              if (is_polebar(thing))
                return 'p';
              return '1';
              }
            default:
              return '1';
            }
          };break;
        default:
          return '1';
        }
      }
    case  2:
    case  3:
    case  4:
    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
      return (char)(get_object_tilnums(lvl,x,y)+'0');
      break;
    default:
      return '+';
    }
}

int display_object_posparam(const short display_float_pos,const char *param_name,
        int subtl,unsigned char subtpos)
{
    if (display_float_pos)
    {
      int subtl_frac=floor(((float)subtpos)/2.56 + 0.5);
      if (subtl_frac>99) subtl_frac=99;
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("%s: ",param_name);
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d.%02d",subtl,subtl_frac);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf(" subtiles");
    } else
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("%s: ",param_name);
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d",subtl);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf(" subtiles and ");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d",(int)subtpos);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    }
    return 1;
}

int display_object_posxy(const short display_float_pos,int scr_col, int scr_row,
        int subtl_x,int subtl_y,unsigned char subtpos_x,unsigned char subtpos_y)
{
    if (display_float_pos)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col);
      int subtl_x_frac=floor(((float)subtpos_x)/2.56 + 0.5);
      if (subtl_x_frac>99) subtl_x_frac=99;
      int subtl_y_frac=floor(((float)subtpos_y)/2.56 + 0.5);
      if (subtl_y_frac>99) subtl_y_frac=99;
      screen_printf("Subtile: ");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d.%02d , %3d.%02d",
          subtl_x,subtl_x_frac,subtl_y,subtl_y_frac);

      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("  (Tile: ");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%2d,%2d",
          subtl_x/MAP_SUBNUM_X, subtl_y/MAP_SUBNUM_Y);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf(")");
    } else
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Subtile: ");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d,%3d", subtl_x, subtl_y);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf(" (Tile: ");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%2d,%2d", subtl_x/MAP_SUBNUM_X, subtl_y/MAP_SUBNUM_Y);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf(")");
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Position within subtile:");
      screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
      screen_printf("%3d,%3d",
          subtpos_x, subtpos_y);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    };
    return scr_row;
}

int display_thing(struct HELP_DATA *help,const unsigned char *thing, const struct UPOINT_2D *tlsize,
        int scr_col, int scr_row, const int max_row, const short display_float_pos)
{
    int m, i;

    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("%s","Thing data block:");
    const int data_per_line=7;
    int data_lines=(SIZEOF_DK_TNG_REC/data_per_line)+((SIZEOF_DK_TNG_REC%data_per_line)>0);
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(scr_row++, scr_col);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_TNG_REC)
            screen_printf("%02X ", (unsigned int)thing[m*data_per_line+i]);
      }
      screen_printf_toeol("");
    }
    scr_row++;
    int subtl_x=get_thing_subtile_x(thing);
    int subtl_y=get_thing_subtile_y(thing);
    scr_row=display_object_posxy(display_float_pos,scr_col,scr_row,
        subtl_x,subtl_y,get_thing_subtpos_x(thing), get_thing_subtpos_y(thing));
    set_cursor_pos(scr_row++, scr_col);
    unsigned char type_idx=get_thing_type(thing);
    screen_printf("Thing type: ");
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf(get_thing_type_fullname(type_idx));
    set_cursor_pos(scr_row++, scr_col);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    display_object_posparam(display_float_pos,"Altitude",
        get_thing_subtile_h(thing),get_thing_subtpos_h(thing));
    // Only effect generators and doors have sensitive tile.
    // but Traps, doors and creatures may have a number written in same way
    // on this position - to check
    if ((type_idx==THING_TYPE_EFFECTGEN)||(type_idx==THING_TYPE_ITEM))
    {
        set_cursor_pos(scr_row++, scr_col);
        int sen_tl=get_thing_sensitile(thing);
        screen_printf("Sensitive tile:");
        if (sen_tl==THING_SENSITILE_NONE)
        {
            screen_printf(" none");
        } else
        {
            int sen_tlx,sen_tly;
            sen_tlx=sen_tl%tlsize->x;
            sen_tly=sen_tl/tlsize->x;
            screen_printf(" %3d, %3d", sen_tlx, sen_tly);
        }
    }

    switch (type_idx)
    {
    // Creature
    case THING_TYPE_CREATURE:
    {
      unsigned short stype_idx=get_thing_subtype(thing);
      unsigned short owner=get_thing_owner(thing);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_creature_subtype_fullname(stype_idx));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Level: ");
      screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
      screen_printf("%d", get_thing_level(thing)+1);
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner: ");
      screen_setcolor(get_screen_color_owned(owner,false,false,false));
      screen_printf(get_owner_type_fullname(owner));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      scr_row++;
      if (init_item_desc(help,MD_CRTR,stype_idx-1) && (help->drows<=(max_row-scr_row)))
      {
        for (i=0; i<help->drows; i++)
            draw_help_line(scr_row++,scr_col,help->desc[i],DHFLAG_USE_COLORS);
      }
    };break;
    // Trap
    case THING_TYPE_TRAP:
    {
      unsigned short stype_idx=get_thing_subtype(thing);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_trap_subtype_fullname(stype_idx));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner: ");
      unsigned short owner=get_thing_owner(thing);
      screen_setcolor(get_screen_color_owned(owner,false,false,false));
      screen_printf(get_owner_type_fullname(owner));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      scr_row++;
      if (init_item_desc(help,MD_CTRP,stype_idx-1) && (help->drows<=(max_row-scr_row)))
      {
        for (i=0; i<help->drows; i++)
            draw_help_line(scr_row++,scr_col,help->desc[i],DHFLAG_USE_COLORS);
      }
    };break;
    // Door
    case THING_TYPE_DOOR:
    {
      unsigned short stype_idx=get_thing_subtype(thing);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_door_subtype_fullname(stype_idx));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner: ");
      unsigned short owner=get_thing_owner(thing);
      screen_setcolor(get_screen_color_owned(owner,false,false,false));
      screen_printf(get_owner_type_fullname(owner));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("State: ");
      screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
      if (get_door_lock(thing)==DOOR_PASS_UNLOCKED)
          screen_printf("Unlocked");
      else
          screen_printf("Locked");
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    };break;
    // effect generator
    case THING_TYPE_EFFECTGEN:
    {
      unsigned short stype_idx=get_thing_subtype(thing);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_effectgen_subtype_fullname(stype_idx));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      scr_row+=display_object_posparam(display_float_pos,"Range",
          get_thing_range_subtile(thing),get_thing_range_subtpos(thing));
      if (init_item_desc(help,MD_EFCT,stype_idx-1) && (help->drows<=(max_row-scr_row)))
      {
        for (i=0; i<help->drows; i++)
            draw_help_line(scr_row++,scr_col,help->desc[i],DHFLAG_USE_COLORS);
      }
    };break;
    // Object
    case THING_TYPE_ITEM:
    {
      set_cursor_pos(scr_row++, scr_col);
      unsigned char stype_idx=get_thing_subtype(thing);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_item_subtype_fullname(stype_idx));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      if (stype_idx==ITEM_SUBTYPE_HEROGATE)
      {
          unsigned int cnum=get_thing_level(thing);
          set_cursor_pos(scr_row-1, scr_col+20);
          screen_printf("number: ");
          screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
          screen_printf("%d", cnum);
          screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      }
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Owner: ");
      unsigned short owner=get_thing_owner(thing);
      screen_setcolor(get_screen_color_owned(owner,false,false,false));
      screen_printf(get_owner_type_fullname(owner));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Category: ");
      screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
      screen_printf(get_thing_category_fullname(get_item_category(stype_idx)));
      scr_row++;
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      if (init_item_desc(help,MD_CITM,stype_idx-1) && (help->drows<=(max_row-scr_row)))
      {
        for (i=0; i<help->drows; i++)
            draw_help_line(scr_row++,scr_col,help->desc[i],DHFLAG_USE_COLORS);
      }
    };break;
    default:
    {
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf("Unrecognized thing!");
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Type index: %d", (unsigned int)type_idx);
    };break;
    }
    return scr_row;
}

int display_action_point(unsigned char *actnpt, int x, int scr_row, int max_row,
        short display_float_pos)
{
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, x);
    screen_printf("Action point data block:");
    const int data_per_line=8;
    int data_lines=(SIZEOF_DK_APT_REC/data_per_line)+((SIZEOF_DK_APT_REC%data_per_line)>0);
    int m, i;
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(scr_row++, x);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_APT_REC)
            screen_printf("%02X ", (unsigned int)actnpt[m*data_per_line+i]);
      }
      screen_printf_toeol("");
    }
    scr_row++;
    set_cursor_pos(scr_row++, x);
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf("Action point");
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(" number ");
    screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
    screen_printf("%d", get_actnpt_number(actnpt));
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(":");
    int subtl_x=get_actnpt_subtile_x(actnpt);
    int subtl_y=get_actnpt_subtile_y(actnpt);
    scr_row=display_object_posxy(display_float_pos,x,scr_row,
        subtl_x,subtl_y,get_actnpt_subtpos_x(actnpt), get_actnpt_subtpos_y(actnpt));
    set_cursor_pos(scr_row++, x);
    display_object_posparam(display_float_pos,"Range",
        get_actnpt_range_subtile(actnpt),get_actnpt_range_subtpos(actnpt));
    return scr_row;
}

int display_static_light(unsigned char *stlight, int x, int scr_row, int max_row,
        short display_float_pos)
{
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, x);
    screen_printf("%s","Light data block:");
    const int data_per_line=7;
    int data_lines=(SIZEOF_DK_LGT_REC/data_per_line)+((SIZEOF_DK_LGT_REC%data_per_line)>0);
    int m, i;
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(scr_row++, x);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_LGT_REC)
            screen_printf("%02X ", (unsigned int)stlight[data_byte]);
      }
      screen_printf_toeol("");
    }
    scr_row++;
    set_cursor_pos(scr_row++, x);
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf("Static light");
    int subtl_x=get_stlight_subtile_x(stlight);
    int subtl_y=get_stlight_subtile_y(stlight);
    scr_row=display_object_posxy(display_float_pos,x,scr_row,
        subtl_x,subtl_y,get_stlight_subtpos_x(stlight), get_stlight_subtpos_y(stlight));
    set_cursor_pos(scr_row++, x);
    display_object_posparam(display_float_pos,"Altitude",
        get_stlight_subtile_h(stlight),get_stlight_subtpos_h(stlight));
    set_cursor_pos(scr_row++, x);
    display_object_posparam(display_float_pos,"Range",
        get_stlight_range_subtile(stlight),get_stlight_range_subtpos(stlight));
    set_cursor_pos(scr_row++, x);
    screen_printf("Intensivity: ");
    screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
    screen_printf("%3d",(unsigned int)get_stlight_intensivity(stlight));
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    return scr_row;
}

int display_obj_stats(struct SCRMODE_DATA *scrmode,const struct WORKMODE_DATA *workdata, int scr_row, int scr_col)
{
    char stat_buf[LINEMSG_SIZE];
    struct LEVEL *lvl=workdata->lvl;
    int m, i;
    int scr_col2=scr_col+20;
    int scr_col_sub1=scr_col+2;
    int scr_col_sub2=scr_col2+1;
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    int line_num=0;
    int mainitem_count=0;
    int subitem_count=0;
    do {
      short line_type=get_level_objstats_textln(lvl,stat_buf,line_num);
      if (line_type==STLT_NONE) break;
      if ((line_type==STLT_HEADER)&&(scrmode->rows < scr_row+TNGDAT_ROWS+2))
          break;
      switch (line_type)
      {
      case STLT_HEADER:
        set_cursor_pos(scr_row++, scr_col);
        mainitem_count=0;
        subitem_count=0;
        break;
      case STLT_MAINITEM:
        if ((mainitem_count%2)>0)
          set_cursor_pos(scr_row++, scr_col2);
        else
          set_cursor_pos(scr_row, scr_col);
        mainitem_count++;
        subitem_count=0;
        break;
      case STLT_SUBITEM:
        if ((subitem_count%2)>0)
          set_cursor_pos(scr_row++, scr_col_sub2);
        else
          set_cursor_pos(scr_row, scr_col_sub1);
        mainitem_count=0;
        subitem_count++;
        break;
      case STLT_EMPTY:
      default:
        mainitem_count=0;
        subitem_count=0;
        break;
      }
      screen_printf("%s",stat_buf);
      line_num++;
    } while (scrmode->rows > scr_row+TNGDAT_ROWS);
    scr_row++;
    return scr_row;
}

int display_tng_subtiles(const struct SCRMODE_DATA *scrmode,
    const struct WORKMODE_DATA *workdata, const struct LEVEL *lvl,
    int scr_row, int scr_col,const short compressed,const int ty,const int tx)
{
    set_cursor_pos(scr_row, scr_col);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(".tng entries");
    int i,k;
    unsigned char *thing;
    struct IPOINT_2D *subtl=&(workdata->mapmode->subtl);
    int entry_size=6;
    scr_row+=2;
    for (k=0; k<MAP_SUBNUM_Y; k++)
    {
        int sy=ty*MAP_SUBNUM_Y+k;
        for (i=0; i<MAP_SUBNUM_X; i++)
        {
            int sx=tx*MAP_SUBNUM_X+i;
            screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
            set_cursor_pos(scr_row+1, scr_col+i*entry_size);
            unsigned int obj_num=get_object_subnums(lvl,sx,sy);
            int visiting_z=workdata->mdtng->vistng[i][k];
            if (obj_num > 1)
            {
                if (scrmode->mode==MD_TNG)
                  screen_printf("(%d/%d)", visiting_z+1, obj_num);
                else
                  screen_printf("(%d)", obj_num);
            }
            set_cursor_pos(scr_row, scr_col+i*entry_size);
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            unsigned char obj_owner=PLAYER_UNSET;
            char *type_text;
            switch (obj_type)
            {
              case OBJECT_TYPE_ACTNPT:
                {
                  type_text="action";
                };break;
              case OBJECT_TYPE_STLIGHT:
                {
                  type_text="light";
                };break;
              case OBJECT_TYPE_THING:
                {
                  thing=get_object(lvl,sx,sy,visiting_z);
                  obj_owner=get_thing_owner(thing);
                  switch (get_thing_type(thing))
                  {
                  case THING_TYPE_CREATURE:
                    type_text=get_creature_subtype_shortname(get_thing_subtype(thing));
                    break;
                  case THING_TYPE_ITEM:
                    type_text=get_item_subtype_shortname(get_thing_subtype(thing));
                    break;
                  default:
                    type_text=get_thing_type_shortname(get_thing_type(thing));
                    break;
                  }
                };break;
              default:
                {
                    type_text="     ";
                };break;
            }
            int color;
            if ((i==subtl->x) && (k==subtl->y) && (scrmode->mode==MD_TNG))
                color=get_tng_display_color(obj_type,obj_owner,true);
            else
                color=get_tng_display_color(obj_type,obj_owner,false);
            screen_setcolor(color);
            screen_printf("%s",type_text);
        }
        scr_row++;
        if (!compressed)
          scr_row++;
    }
    return scr_row;
}

int get_tng_display_color(short obj_type,unsigned char obj_owner,short marked)
{
    int col;
    switch (obj_type)
    {
    // Things that can be owned
    case OBJECT_TYPE_THING:
        return get_screen_color_owned(obj_owner,marked,false,false);
    // Things with no owner
    case OBJECT_TYPE_ACTNPT:
    case OBJECT_TYPE_STLIGHT:
    default:
        if (marked)
          return PRINT_COLOR_BLACK_ON_LGREY;
        else
          return PRINT_COLOR_LGREY_ON_BLACK;
    }
}

unsigned char *tng_makeitem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing=create_item_adv(workdata->lvl,sx,sy,stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create thing");
        return NULL;
    }
    thing_add(workdata->lvl,thing);
    set_visited_obj_lastof(workdata,OBJECT_TYPE_THING);
    message_info("Item added: %s",get_item_subtype_fullname(stype_idx));
    inc_info_usr_creatobj_count(workdata->lvl);
    return thing;
}

unsigned char *tng_makeeffectgen(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_effectgen(workdata->lvl,sx,sy,stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create Effect Generator");
        return NULL;
    }
    thing_add(workdata->lvl,thing);
    message_info("Effect Generator added to map at (%d,%d)",sx,sy);
    set_visited_obj_lastof(workdata,OBJECT_TYPE_THING);
    set_brighten_for_thing(workdata->mapmode,thing);
    inc_info_usr_creatobj_count(workdata->lvl);
    return thing;
}

unsigned char *tng_maketrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_trap (workdata->lvl, sx, sy, stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create Trap");
        return NULL;
    }
    thing_add(workdata->lvl,thing);
    message_info("Trap added to map at (%d,%d)",sx,sy);
    set_visited_obj_lastof(workdata,OBJECT_TYPE_THING);
    inc_info_usr_creatobj_count(workdata->lvl);
    return thing;
}

unsigned char *tng_makecreature(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_creature(workdata->lvl,sx,sy,stype_idx);
    thing_add(workdata->lvl,thing);
    // Show the new thing
    set_visited_obj_lastof(workdata,OBJECT_TYPE_THING);
    message_info("Creature added to map at (%d,%d)",sx,sy);
    inc_info_usr_creatobj_count(workdata->lvl);
    return thing;
}

void tng_change_height(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, unsigned int sx, unsigned int sy,unsigned int z,int delta_height)
{
    unsigned char *obj=get_object(workdata->lvl,sx,sy,z);
    short obj_type=get_object_type(workdata->lvl,sx,sy,z);
    int height,subheight;
    if (obj==NULL)
    {
        message_error("Cannot find object");
        return;
    }
    int tile_limit;
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      height=get_stlight_subtile_h(obj);
      subheight=get_stlight_subtpos_h(obj)+delta_height;
      tile_limit=MAP_SUBNUM_H;
      break;
    case OBJECT_TYPE_ACTNPT:
      height=get_actnpt_range_subtile(obj);
      subheight=get_actnpt_range_subtpos(obj)+delta_height;
      tile_limit=workdata->lvl->tlsize.x/2;
      break;
    case OBJECT_TYPE_THING:
      height=get_thing_subtile_h(obj);
      subheight=get_thing_subtpos_h(obj)+delta_height;
      tile_limit=MAP_SUBNUM_H;
      break;
    default:
      message_error("Cannot recognize object");
      return;
    }
    while (subheight<0)
    {
        height--;
        subheight+=256;
    }
    while (subheight>255)
    {
        height++;
        subheight-=256;
    }
    if (height>=tile_limit)
    {
        height=tile_limit-1;
        subheight=255;
        message_error("Max parameter value reached");
    } else
    if (height<0)
    {
        height=0;
        subheight=0;
        message_error("Min parameter value reached");
    } else
    {
        message_info("Object parameter adjusted");
    }
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      set_stlight_subtile_h(obj,height);
      set_stlight_subtpos_h(obj,subheight);
      break;
    case OBJECT_TYPE_ACTNPT:
      set_actnpt_range_subtile(obj,height);
      set_actnpt_range_subtpos(obj,subheight);
      set_brighten_for_actnpt(workdata->mapmode,obj);
      break;
    case OBJECT_TYPE_THING:
      set_thing_subtile_h(obj,height);
      set_thing_subtpos_h(obj,subheight);
      break;
    }
}

void tng_change_range(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, unsigned int sx, unsigned int sy,unsigned int z,int delta_range)
{
    unsigned char *obj=get_object(workdata->lvl,sx,sy,z);
    short obj_type=get_object_type(workdata->lvl,sx,sy,z);
    int rng,subrng;
    if (obj==NULL)
    {
        message_error("Cannot find object");
        return;
    }
    int tile_limit;
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      rng=get_stlight_range_subtile(obj);
      subrng=get_stlight_range_subtpos(obj)+delta_range;
      tile_limit=workdata->lvl->tlsize.x/2;
      break;
    case OBJECT_TYPE_ACTNPT:
      rng=get_actnpt_range_subtile(obj);
      subrng=get_actnpt_range_subtpos(obj)+delta_range;
      tile_limit=workdata->lvl->tlsize.x/2;
      break;
    case OBJECT_TYPE_THING:
      {
        switch (get_thing_type(obj))
        {
        case THING_TYPE_EFFECTGEN:
          rng=get_thing_range_subtile(obj);
          subrng=get_thing_range_subtpos(obj)+delta_range;
          tile_limit=workdata->lvl->tlsize.x/2;
          break;
        case THING_TYPE_ITEM:
        case THING_TYPE_CREATURE:
        case THING_TYPE_DOOR:
        case THING_TYPE_TRAP:
        default:
          message_error("Selected thing have no range");
          return;
        }
      };break;
    default:
      message_error("Cannot recognize object");
      return;
    }
    while (subrng<0)
    {
        rng--;
        subrng+=256;
    }
    while (subrng>255)
    {
        rng++;
        subrng-=256;
    }
    if (rng>=tile_limit)
    {
        rng=tile_limit-1;
        subrng=255;
        message_error("Max parameter value reached");
    } else
    if (rng<0)
    {
        rng=0;
        subrng=0;
        message_error("Min parameter value reached");
    } else
    {
        message_info("Object parameter adjusted");
    }
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      set_stlight_range_subtile(obj,rng);
      set_stlight_range_subtpos(obj,subrng);
      if (delta_range>0)
          set_brighten_for_stlight(workdata->mapmode,obj);
      else
          workdata->mdtng->obj_ranges_changed=true;
      break;
    case OBJECT_TYPE_ACTNPT:
      set_actnpt_range_subtile(obj,rng);
      set_actnpt_range_subtpos(obj,subrng);
      if (delta_range>0)
          set_brighten_for_actnpt(workdata->mapmode,obj);
      else
          workdata->mdtng->obj_ranges_changed=true;
      break;
    case OBJECT_TYPE_THING:
      set_thing_range_subtile(obj,rng);
      set_thing_range_subtpos(obj,subrng);
      if (delta_range>0)
          set_brighten_for_thing(workdata->mapmode,obj);
      else
          workdata->mdtng->obj_ranges_changed=true;
      break;
    }
}

void action_edit_object(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    int visiting_z=get_visited_obj_idx(workdata);
    short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
    unsigned char *thing;
    thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
      {
        unsigned char type_idx=get_thing_type(thing);
        switch (type_idx)
        {
        case THING_TYPE_CREATURE:
            mdstart[MD_ECRT](scrmode,workdata);
            break;
        case THING_TYPE_TRAP:
            mdstart[MD_ETRP](scrmode,workdata);
            break;
        case THING_TYPE_ITEM:
            mdstart[MD_EITM](scrmode,workdata);
            break;
        case THING_TYPE_EFFECTGEN:
            mdstart[MD_EFCT](scrmode,workdata);
            break;
        case THING_TYPE_DOOR:
            message_error("Can't edit door: no such function.");
            break;
        }
      };break;
    default:
        message_error("Can't edit: no thing selected.");
        break;
    }
}

void action_delete_object(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    int visiting_z=get_visited_obj_idx(workdata);
    short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
        object_del(workdata->lvl,subpos.x,subpos.y,visiting_z);
        message_info_force("Static light deleted.");
        workdata->mdtng->obj_ranges_changed=true;
        break;
    case OBJECT_TYPE_ACTNPT:
        object_del(workdata->lvl,subpos.x,subpos.y,visiting_z);
        message_info_force("Action point deleted.");
        workdata->mdtng->obj_ranges_changed=true;
        break;
    case OBJECT_TYPE_THING:
        object_del(workdata->lvl,subpos.x,subpos.y,visiting_z);
        message_info_force("Thing deleted.");
        workdata->mdtng->obj_ranges_changed=true;
        break;
    default:
        message_error("Nothing to delete.");
        break;
    }
    if (visiting_z>0)
        set_visited_obj_idx(workdata,visiting_z-1);
    else
        set_visited_obj_idx(workdata,get_object_subnums(workdata->lvl,subpos.x,subpos.y)-1);
}

void action_inc_object_level(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    int visiting_z=get_visited_obj_idx(workdata);
    short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
      {
        unsigned char *thing;
        thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        if (is_creature(thing))
        {
            unsigned short crtr_lev=get_thing_level(thing);
            if (crtr_lev<9)
            {
                set_thing_level(thing,crtr_lev+1);
                message_info("Creature level increased.");
            } else
                message_error("Creature level limit reached.");
        } else
        if (is_herogate(thing))
        {
            tng_change_herogate_number(workdata,thing,true);
        } else
        if (is_dnheart(thing))
        {
            message_error("Dungeon Heart has no alternative.");
        } else
        if (switch_thing_subtype(thing,true))
        {
            message_info("Item type switched to next.");
        } else
            message_error("This thing has no level/type, or its limit is reached.");
      };break;
    case OBJECT_TYPE_STLIGHT:
      {
        unsigned char *stlight;
        stlight = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        unsigned int intens=get_stlight_intensivity(stlight);
        if (intens<255)
        {
            if (intens<127)
              intens++;
            else
              intens+=4;
            if (intens>255) intens=255;
            set_stlight_intensivity(stlight,intens);
            message_info("Static light intensivity increased.");
        } else
        {
            message_error("Maximum static light intensivity reached.");
        }
      };break;
    case OBJECT_TYPE_ACTNPT:
      {
        unsigned char *actnpt;
        actnpt = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        tng_change_actnpt_level(workdata,actnpt,true);
      };break;
    default:
        message_error("Can't change level: no thing selected.");
        break;
    }
}

void action_dec_object_level(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    int visiting_z=get_visited_obj_idx(workdata);
    short obj_type=get_object_type(workdata->lvl,subpos.x,subpos.y,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
      {
        unsigned char *thing;
        thing = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        if (is_creature(thing))
        {
            unsigned short crtr_lev=get_thing_level(thing);
            if (crtr_lev>0)
            {
                set_thing_level(thing,crtr_lev-1);
                message_info("Creature level decreased.");
            } else
            message_error("Creature level limit reached.");
        } else
        if (is_herogate(thing))
        {
            tng_change_herogate_number(workdata,thing,false);
        } else
        if (is_dnheart(thing))
        {
            message_error("Dungeon Heart has no alternative.");
        } else
        if (switch_thing_subtype(thing,false))
        {
            message_info("Item type switched to previous.");
        } else
            message_error("This thing has no level/type, or its limit is reached.");
      };break;
    case OBJECT_TYPE_STLIGHT:
      {
        unsigned char *stlight;
        stlight = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        unsigned int intens=get_stlight_intensivity(stlight);
        if (intens>1)
        {
            intens--;
            set_stlight_intensivity(stlight,intens);
            message_info("Static light intensivity decreased.");
        } else
        {
            message_error("Minimum static light intensivity reached.");
        }
      };break;
    case OBJECT_TYPE_ACTNPT:
      {
        unsigned char *actnpt;
        actnpt = get_object(workdata->lvl,subpos.x,subpos.y,visiting_z);
        tng_change_actnpt_level(workdata,actnpt,false);
      };break;
    default:
        message_error("Can't change level: no thing selected.");
        break;
    }
}

void tng_change_actnpt_level(struct WORKMODE_DATA *workdata,unsigned char *actnpt,short forward)
{
    unsigned short num=get_actnpt_number(actnpt);
    unsigned short apt_num;
    if (forward)
        apt_num=get_free_actnpt_number_next(workdata->lvl,num);
    else
        apt_num=get_free_actnpt_number_prev(workdata->lvl,num);
    if (num!=apt_num)
    {
        set_actnpt_number(actnpt,apt_num);
        char *oper;
        if (apt_num>num)
          oper="increased";
        else
          oper="decreased";
        message_info("%s number %s.","Action point",oper);
    } else
    {
        message_error("%s number limit reached.","Action point");
    }
}

void tng_change_herogate_number(struct WORKMODE_DATA *workdata,unsigned char *thing,short forward)
{
    unsigned short num=get_thing_level(thing);
    unsigned short newnum;
    if (forward)
        newnum=get_free_herogate_number_next(workdata->lvl,num);
    else
        newnum=get_free_herogate_number_prev(workdata->lvl,num);
    if (num!=newnum)
    {
        set_thing_level(thing,newnum);
        char *oper;
        if (newnum>num)
          oper="increased";
        else
          oper="decreased";
        message_info("%s number %s.","Hero gate",oper);
    } else
    {
        message_error("%s number limit reached.","Hero gate");
    }
}
