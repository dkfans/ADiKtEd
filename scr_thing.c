/*
 * scr_thing.c
 *
 * Defines functions for initializing and displaying the thing screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_thing.h"

#include "globals.h"
#include "scr_help.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "lev_data.h"
#include "lev_things.h"
#include "obj_things.h"
#include "obj_actnpts.h"
#include "obj_slabs.h"

// Variables

MDTNG_DATA *mdtng;

/*
 * Initializes variables for the tng screen.
 */
short init_mdtng(void)
{
    //Creating and clearing mdtng variable
    mdtng=(MDTNG_DATA *)malloc(sizeof(MDTNG_DATA));
    if (mdtng==NULL)
     die("init_mdtng: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the tng screen.
 */
void free_mdtng(void)
{
  free(mdtng);
}

void change_visited_tile()
{
    int i,j;
    for (i=0; i<3; i++)
      for (j=0; j<3; j++)
        mdtng->vistng[i][j]=0;
}

/*
 * Covers actions from the tng screen.
 */
void actions_mdtng(int key)
{
    int sx, sy;
    unsigned char *thing;
    sx = (mapmode->mapx+mapmode->screenx)*3+mapmode->subtl_x;
    sy = (mapmode->mapy+mapmode->screeny)*3+mapmode->subtl_y;
    message_release();
    
    if (!cursor_actions(key))
    if (!subtl_select_actions(key))
    {
      switch (key)
      {
        case KEY_TAB:
          end_mdtng();
          start_mdslab(lvl);
          message_info("Slab mode activated");
          break;
        case 'c': //add creature
          start_list(lvl,MD_CRTR);
          break;
        case 'i': //add item
          start_list(lvl,MD_ITMT);
          break;
        case 'b' : // create spell
          tng_makeitem(sx,sy,ITEM_SUBTYPE_SPELLHOE);
          message_info("Added spell");
          break;
        case 'h' : // Add Dungeon heart
          tng_makeitem(sx,sy,ITEM_SUBTYPE_DNHEART);
          message_info("Added Dungeon heart");
          break;
        case 'H' : // Add hero gate
          tng_makeitem(sx,sy,ITEM_SUBTYPE_HEROGATE);
          message_info("Added hero gate");
          break;
        case 'd': // Dungeon special
          tng_makeitem(sx,sy,ITEM_SUBTYPE_SPREVMAP);
          message_info("Added dungeon special");
          break;
        case 'g': // Gold
          tng_makeitem(sx,sy,ITEM_SUBTYPE_GOLDCHEST);
          message_info("Added gold chest");
          break;
        case 'G': // Gold
          tng_makeitem(sx,sy,ITEM_SUBTYPE_GOLD);
          message_info("Added gold");
          break;
        case 'f': // Food
          tng_makeitem(sx,sy,ITEM_SUBTYPE_CHICKNSTB);
          message_info("Added food (chicken)");
          break;
        case 'r' : // create torch
          tng_makeitem(sx,sy,ITEM_SUBTYPE_TORCH);
          message_info("Added torch");
          break;
        case 'e': // Add room effect
          thing = create_roomeffect(sx, sy, ROOMEFC_SUBTP_DRIPWTR);
          thing_add(lvl,thing);
          message_info("Added room effect");
          mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
          break;
        case 'a' : // Add action point
          thing = create_actnpt(lvl, sx, sy);
          actnpt_add(lvl,thing);
          message_info("Added action point %d",(unsigned int)get_actnpt_number(thing));
          mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_ACTNPT);
          break;
        case 'L' : // Add static light
          thing = create_stlight(sx, sy);
          stlight_add(lvl,thing);
          message_info("Added static light");
          mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_STLIGHT);
          break;
        case 'k' : // Copy thing to clipboard
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            thing=get_object(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              copy_to_clipboard_thing(thing);
              message_info("Thing is now in clipboard");
              break;
            case OBJECT_TYPE_ACTNPT:
              copy_to_clipboard_actnpt(thing);
              message_info("Action point is now in clipboard");
              break;
            case OBJECT_TYPE_STLIGHT:
              copy_to_clipboard_stlight(thing);
              message_info("Static light is now in clipboard");
              break;
            default:
              message_error("Can't copy to clipboard: nothing selected");
              break;
            }
          };break;
        case 'y' : // Create thing from clipboard
          if (scrmode->clip_count>0)
          {
            struct CLIPBOARD *clip_itm;
            clip_itm=get_clipboard_object(0);
            if (clip_itm==NULL)
            {
              message_error("Elements in clipboard are not objects, can't paste.");
              break;
            }
            switch (clip_itm->dtype)
            {
            case OBJECT_TYPE_THING:
                thing = create_thing_copy(sx,sy,clip_itm->data);
                // Make sure the coordinates are preserved for objects
                //TODO: place this condition where creating new thing, not only here
                if (get_thing_subtpos_y(thing)==1)
                    set_thing_sensitile(thing,MAP_SIZE_X*(mapmode->mapy+mapmode->screeny)+(mapmode->mapx+mapmode->screenx));
                thing_add(lvl,thing);
                message_info("Thing pasted from clipboard at subtile %d,%d",sx,sy);
                mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
                break;
            case OBJECT_TYPE_ACTNPT:
                thing = create_actnpt_copy(sx,sy,clip_itm->data);
                actnpt_add(lvl,thing);
                message_info("Action point pasted from clipboard at subtile %d,%d",sx,sy);
                mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_ACTNPT);
                break;
            case OBJECT_TYPE_STLIGHT:
                thing = create_stlight_copy(sx,sy,clip_itm->data);
                stlight_add(lvl,thing);
                message_info("Static light pasted from clipboard at subtile %d,%d",sx,sy);
                mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_STLIGHT);
                break;
            default:
                message_error("Internal error: can't paste this object type.");
                break;
            }
          } else
          {
            message_error("Can't create object: clipboard is empty");
          };break;
        case 'u': // Update all things/dat/clm/w?b
          update_slab_owners(lvl);
          update_datclm_for_whole_map(lvl);
          message_info("DAT/CLM/W?B entries updated");
          break;
        case 'v': // Verify whole map
          level_verify(lvl,NULL);
          break;
        case 't' : // Create trap
          thing = create_trap (sx, sy, TRAP_SUBTYPE_BOULDER);
          thing_add(lvl,thing);
          message_info("Added trap");
          mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
          break;
        case 'T' : // Create trap box
          tng_makeitem(sx,sy,ITEM_SUBTYPE_TBBOULDER);
          message_info("Added trap box");
          break;
        case 'l' : // Lock / unlock a door
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing=get_object(lvl,sx,sy,visiting_z);
              if (get_thing_type(thing)==THING_TYPE_DOOR)
              {
                  if (get_door_lock(thing)!=DOOR_PASS_UNLOCKED)
                  {
                    set_door_lock(lvl,thing,DOOR_PASS_UNLOCKED);
                    message_info("Door unlocked");
                  } else
                  {
                    set_door_lock(lvl,thing,DOOR_PASS_LOCKED);
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
          //TODO: should we update the CLM ?
          };break;
        case 'o' : // Change ownership of creature/trap/special/spell
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,sx,sy,visiting_z);
              set_thing_owner(thing,get_owner_next(get_thing_owner(thing)));
              message_info("Object owner switched");
              break;
            default:
              message_error("Can't change owner: no \"thing\" selected.");
              break;
            }
          };break;
        case 'S' :
        case 'X' : // Change type of creature
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,sx,sy,visiting_z);
              if (!is_creature(thing))
              {
                 message_error("No creature selected");
                 break;
              }
              if (key=='S')
                set_thing_subtype(thing,get_creature_next(get_thing_subtype(thing)));
              else
                set_thing_subtype(thing,get_creature_prev(get_thing_subtype(thing)));
              message_info("Creature kind changed");
              break;
            default:
              message_error("Can't change type: no \"thing\" selected.");
              break;
            }
          };break;
        case 's' : // Change level of creature/trap/special/spell/lair/room efct
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,sx,sy,visiting_z);
              if (is_creature(thing))
              {
                if (get_thing_level(thing)<9)
                  set_thing_level(thing,get_thing_level(thing)+1);
              } else
              if (is_trap(thing))
              {
                  set_thing_subtype(thing,get_trap_next(get_thing_subtype(thing)));
              } else
              if (is_dngspecbox(thing))
              {
                  set_thing_subtype(thing,get_dngspecbox_next(get_thing_subtype(thing)));
              } else
              if (is_spellbook(thing))
              {
                  set_thing_subtype(thing,get_spellbook_next(get_thing_subtype(thing)));
              } else
              if (is_trapbox(thing))
              {
                  set_thing_subtype(thing,get_trapbox_next(get_thing_subtype(thing)));
              } else
              if (is_crtrlair(thing))
              {
                  set_thing_subtype(thing,get_crtrlair_next(get_thing_subtype(thing)));
              } else
              if (is_roomeffect(thing))
              {
                  set_thing_subtype(thing,get_roomeffect_next(get_thing_subtype(thing)));
              } else
              if (is_door(thing))
              {
                  set_thing_subtype(thing,get_door_next(get_thing_subtype(thing)));
              } else
              if (is_statue(thing))
              {
                  set_thing_subtype(thing,get_statue_next(get_thing_subtype(thing)));
              } else
              if (is_furniture(thing))
              {
                  set_thing_subtype(thing,get_furniture_next(get_thing_subtype(thing)));
              } else
              if (is_food(thing))
              {
                  set_thing_subtype(thing,get_food_next(get_thing_subtype(thing)));
              } else
              if (is_gold(thing))
              {
                  set_thing_subtype(thing,get_gold_next(get_thing_subtype(thing)));
              } else
              if (is_torch(thing))
              {
                  set_thing_subtype(thing,get_torch_next(get_thing_subtype(thing)));
              } else
                  message_error("This item has no level nor type.");
              break;
            default:
              message_error("Can't change level: no thing selected.");
              break;
            }
          };break;
        case 'x' : // Change level of creature/trap/special/spell/lair/room efct
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,sx,sy,visiting_z);
              if (is_creature(thing))
              {
                if (get_thing_level(thing)>0)
                  set_thing_level(thing,get_thing_level(thing)-1);
              } else
              if (is_trap(thing))
              {
                  set_thing_subtype(thing,get_trap_prev(get_thing_subtype(thing)));
              } else
              if (is_dngspecbox(thing))
              {
                  set_thing_subtype(thing,get_dngspecbox_prev(get_thing_subtype(thing)));
              } else
              if (is_spellbook(thing))
              {
                  set_thing_subtype(thing,get_spellbook_prev(get_thing_subtype(thing)));
              } else
              if (is_trapbox(thing))
              {
                  set_thing_subtype(thing,get_trapbox_prev(get_thing_subtype(thing)));
              } else
              if (is_crtrlair(thing))
              {
                  set_thing_subtype(thing,get_crtrlair_prev(get_thing_subtype(thing)));
              } else
              if (is_roomeffect(thing))
              {
                  set_thing_subtype(thing,get_roomeffect_prev(get_thing_subtype(thing)));
              } else
              if (is_door(thing))
              {
                  set_thing_subtype(thing,get_door_prev(get_thing_subtype(thing)));
              } else
              if (is_statue(thing))
              {
                  set_thing_subtype(thing,get_statue_prev(get_thing_subtype(thing)));
              } else
              if (is_furniture(thing))
              {
                  set_thing_subtype(thing,get_furniture_prev(get_thing_subtype(thing)));
              } else
              if (is_food(thing))
              {
                  set_thing_subtype(thing,get_food_prev(get_thing_subtype(thing)));
              } else
              if (is_gold(thing))
              {
                  set_thing_subtype(thing,get_gold_prev(get_thing_subtype(thing)));
              } else
              if (is_torch(thing))
              {
                  set_thing_subtype(thing,get_torch_prev(get_thing_subtype(thing)));
              } else
                  message_error("This item has no level nor type.");
              break;
            default:
              message_error("Can't change level: no thing selected.");
              break;
            }
          };break;
        case '/' : // Cycle through highlighted things
          {
          unsigned int obj_num=get_object_subnums(lvl,sx,sy);
          if (obj_num > 0)
            mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=(mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]+1)%(obj_num);
          };break;
        case '[' :
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            tng_change_height(lvl,sx,sy,visiting_z,-64);
          };break;
        case ']' :
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            tng_change_height(lvl,sx,sy, visiting_z,+64);
          };break;
        case KEY_DEL: // delete
          {
            int visiting_z=mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            switch (obj_type)
            {
            case OBJECT_TYPE_STLIGHT:
              object_del(lvl,sx,sy, visiting_z);
              message_info_force("Static light deleted.");
              break;
            case OBJECT_TYPE_ACTNPT:
              object_del(lvl,sx,sy, visiting_z);
              message_info_force("Action point deleted.");
              break;
            case OBJECT_TYPE_THING:
              object_del(lvl,sx,sy, visiting_z);
              message_info_force("Thing deleted.");
              break;
            default:
              message_error("Nothing to delete.");
              break;
            }
            if (visiting_z>0)
              mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]--;
            else
              mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subnums(lvl,sx,sy)-1;;
          };break;
        case KEY_ESCAPE:
          message_info(get_random_tip());
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
short start_mdtng(struct LEVEL *lvl)
{
    mapmode->mark=false;
    change_visited_tile();
    scrmode->mode=MD_TNG;
    return true;
}

/*
 * Action function - end the thing mode.
 */
void end_mdtng()
{
    mapmode->panel_mode=PV_MODE;
}

/*
 * Draws screen for the thing mode.
 */
void draw_mdtng()
{
    draw_map_area(lvl,true,false,true);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(lvl,mapmode->panel_mode);
    else
      draw_mdtng_panel();
    draw_map_cursor(lvl,true,false,true);
    return;
}

void draw_mdtng_panel()
{
    int sx, sy;
    sx = (mapmode->screenx+mapmode->mapx)*3+mapmode->subtl_x;
    sy = (mapmode->screeny+mapmode->mapy)*3+mapmode->subtl_y;
    int k=0;
    short obj_type=get_object_type(lvl,sx,sy,mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]);
    char *obj=get_object(lvl,sx,sy,mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]);
    int scr_col1=scrmode->cols+3;
    switch (obj_type)
    {
    case OBJECT_TYPE_ACTNPT:
      display_action_point(obj, scr_col1, 1);
      break;
    case OBJECT_TYPE_STLIGHT:
      display_static_light(obj, scr_col1, 1);
      break;
    case OBJECT_TYPE_THING:
      display_thing(obj, scr_col1, 1);
      break;
    default:
      k=display_mode_keyhelp(k,scr_col1,scrmode->mode);
      display_obj_stats(k,scr_col1);
      break;
    }
    display_tngdat();
}

char get_thing_char (int x, int y)
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
        for (k=0;k<3;k++)
        {
          for (i=0;i<3;i++)
          {
            obj_type=get_object_type(lvl,3*x+i,3*y+k,0);
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
            thing=get_thing(lvl,x*3+i,y*3+k,0);
            switch (get_thing_type(thing))
            {
            case THING_TYPE_CREATURE:
              return 'c';
            case THING_TYPE_ROOMEFFECT:
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

int display_thing(unsigned char *thing, int x, int y)
{
    int m, i;

    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(y++, x);
    screen_printf("%s","Thing data block:");
    const int data_per_line=7;
    int data_lines=(SIZEOF_DK_TNG_REC/data_per_line)+((SIZEOF_DK_TNG_REC%data_per_line)>0);
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(y++, x);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_TNG_REC)
            screen_printf("%02X ", (unsigned int)thing[m*data_per_line+i]);
      }
      screen_printf_toeol("");
    }
    y++;
    set_cursor_pos(y++, x);
    int tpos_x=get_thing_tilepos_x(thing);
    int tpos_y=get_thing_tilepos_y(thing);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", tpos_x, tpos_y,
              tpos_x/MAP_SUBNUM_X, tpos_y/MAP_SUBNUM_Y);
    set_cursor_pos(y++, x);
    screen_printf("Position within subtile: %3d, %3d",
              get_thing_subtpos_x(thing), get_thing_subtpos_y(thing));
    set_cursor_pos(y++, x);
    unsigned char type_idx=get_thing_type(thing);
    screen_printf("Thing type: %s",get_thing_type_fullname(type_idx));
    set_cursor_pos(y++, x);
    screen_printf("Altitude: %3d within subtile %d",
        get_thing_subtpos_h(thing),get_thing_tilepos_h(thing));
    if ((type_idx==THING_TYPE_ROOMEFFECT)||(type_idx==THING_TYPE_ITEM))
    {
        set_cursor_pos(y++, x);
        int sen_tl=get_thing_sensitile(thing);
        screen_printf("Sensitive tile:");
        if (sen_tl==THING_SENSITILE_NONE)
        {
            screen_printf(" none");
        } else
        {
            int sen_tlx,sen_tly;
            sen_tlx=sen_tl%MAP_SIZE_X;
            sen_tly=sen_tl/MAP_SIZE_X;
            screen_printf(" %3d, %3d", sen_tlx, sen_tly);
        }
    }

    switch (type_idx)
    {
    // Creature
    case THING_TYPE_CREATURE:
    {
      set_cursor_pos(y++, x);
      screen_printf("Kind: %s", get_creature_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Level: %d", get_thing_level(thing)+1);
      set_cursor_pos(y++, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
    };break;
    // Trap
    case THING_TYPE_TRAP:
    {
      set_cursor_pos(y++, x);
      screen_printf("Kind : %s", get_trap_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
    };break;
    // Door
    case THING_TYPE_DOOR:
    {
      set_cursor_pos(y++, x);
      screen_printf("Kind: %s", get_door_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
      set_cursor_pos(y++, x);
      char *lock_state;
      if (get_door_lock(thing)==DOOR_PASS_UNLOCKED)
          lock_state="Unlocked";
      else
          lock_state="Locked";
      screen_printf("State: %s",lock_state);
    };break;
    // Room effect
    case THING_TYPE_ROOMEFFECT:
    {
      set_cursor_pos(y++, x);
      screen_printf("Kind: %s",get_roomeffect_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Range: %3d subtiles and %3d",
          0,0);//get_thing_tile_range(thing),get_thing_subt_range(thing));  TODO
    };break;
    // Object
    case THING_TYPE_ITEM:
    {
      set_cursor_pos(y++, x);
      screen_printf("Kind: %s",get_item_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
      set_cursor_pos(y++, x);
      screen_printf("Category: %s",get_item_category_fullname(get_thing_subtype(thing)));
    };break;
    default:
    {
      set_cursor_pos(y++, x);
      screen_printf("Unrecognized thing!");
      set_cursor_pos(y++, x);
      screen_printf("Type index: %d", (unsigned int)type_idx);
    };break;
    }
    return y;
}

int display_action_point (unsigned char *actnpt, int x, int y)
{
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(y++, x);
    screen_printf("%s","Action point data block:");
    const int data_per_line=8;
    int data_lines=(SIZEOF_DK_APT_REC/data_per_line)+((SIZEOF_DK_APT_REC%data_per_line)>0);
    int m, i;
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(y++, x);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_APT_REC)
            screen_printf("%02X ", (unsigned int)actnpt[m*data_per_line+i]);
      }
      screen_printf_toeol("");
    }
    y++;
    set_cursor_pos(y++, x);
    screen_printf("Action point number %d:", get_actnpt_number(actnpt));
    set_cursor_pos(y++, x);
    int subtlpos_x=get_actnpt_tilepos_x(actnpt);
    int subtlpos_y=get_actnpt_tilepos_y(actnpt);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", subtlpos_x, subtlpos_y,
        subtlpos_x/3, subtlpos_y/3);
    set_cursor_pos(y++, x);
    screen_printf("Position within subtile: %3d, %3d",
        get_actnpt_subtpos_x(actnpt), get_actnpt_subtpos_y(actnpt));
    set_cursor_pos(y++, x);
    screen_printf("Range: %3d subtiles and %3d",
        get_actnpt_tile_range(actnpt),get_actnpt_subt_range(actnpt));
    return y;
}

int display_static_light(unsigned char *stlight, int x, int y)
{
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(y++, x);
    screen_printf("%s","Light data block:");
    const int data_per_line=7;
    int data_lines=(SIZEOF_DK_LGT_REC/data_per_line)+((SIZEOF_DK_LGT_REC%data_per_line)>0);
    int m, i;
    for (m=0; m < data_lines; m++)
    {
      set_cursor_pos(y++, x);
      for (i=0; i < data_per_line; i++)
      {
          int data_byte=m*data_per_line+i;
          if (data_byte<SIZEOF_DK_LGT_REC)
            screen_printf("%02X ", (unsigned int)stlight[data_byte]);
      }
      screen_printf_toeol("");
    }
    y++;
    set_cursor_pos(y++, x);
    screen_printf("Static light");
    set_cursor_pos(y++, x);
    int subtlpos_x=get_stlight_tilepos_x(stlight);
    int subtlpos_y=get_stlight_tilepos_y(stlight);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", subtlpos_x, subtlpos_y,
        subtlpos_x/3, subtlpos_y/3);
    set_cursor_pos(y++, x);
    screen_printf("Position within subtile: %3d, %3d",
        (unsigned int)get_stlight_subtpos_x(stlight), (unsigned int)get_stlight_subtpos_y(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Altitude: %3d within subtile %d",
        (unsigned int)get_stlight_subtpos_h(stlight),(unsigned int)get_stlight_tilepos_h(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Range: %3d subtiles and %d",
        (unsigned int)get_stlight_tile_range(stlight),(unsigned int)get_stlight_subt_range(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Intensivity: %3d",(unsigned int)get_stlight_intensivity(stlight));
    return y;
}

int display_obj_stats(int scr_row, int scr_col)
{
    int m, i;
    int scr_col1=scr_col+2;
    int scr_col2=scr_col+21;
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("%s","Map objects statistics");
    set_cursor_pos(scr_row, scr_col);
    screen_printf("Static lights:%4d",lvl->lgt_total_count);
    set_cursor_pos(scr_row++, scr_col+20);
    screen_printf("Graffiti:%5d",lvl->graffiti_count);
    set_cursor_pos(scr_row, scr_col);
    screen_printf("Action points:%4d",lvl->apt_total_count);
    set_cursor_pos(scr_row++, scr_col+20);
    screen_printf("Cust.clms:%4d",lvl->cust_clm_count);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("Things on map:%4d",lvl->tng_total_count);
    if (scrmode->rows >= scr_row+TNGDAT_ROWS+3)
    {
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("Creatures:%6d",lvl->stats.creatures_count);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("Traps:%4d",lvl->stats.traps_count);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("Room Effcts:%4d",lvl->stats.roomeffects_count);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("Doors:%4d",lvl->stats.doors_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Items:%4d",lvl->stats.items_count);
    }
    if (scrmode->rows >= scr_row+TNGDAT_ROWS+6)
    {
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("%s","Detailed items");
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Hero gats:%4d",lvl->stats.hero_gates_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Dung hearts:%4d",lvl->stats.dn_hearts_count);
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Torches:%6d",lvl->stats.torches_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Dn specials:%4d",lvl->stats.dng_specboxes_count);
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Statues:%6d",lvl->stats.statues_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Spell books:%4d",lvl->stats.spellbooks_count);
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Gold tngs:%4d",lvl->stats.gold_things_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Creatr lairs:%3d",lvl->stats.crtr_lairs_count);
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Furniture:%4d",lvl->stats.furniture_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Room things:%4d",lvl->stats.room_things_count);
    }
    return scr_row;
}

int display_tng_subtiles(int scr_row, int scr_col,int ty,int tx)
{
    set_cursor_pos(scr_row, scr_col);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(".tng entries");
    int i,k;
    unsigned char *thing;
    int entry_size=6;
    scr_row+=2;
    for (k=0; k<3; k++)
    {
        int sy=ty*3+k;
        for (i=0; i<3; i++)
        {
            int sx=tx*3+i;
            screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
            set_cursor_pos(scr_row+1, scr_col+i*entry_size);
            unsigned int obj_num=get_object_subnums(lvl,sx,sy);
            if (obj_num > 1)
            {
                if (scrmode->mode==MD_TNG)
                  screen_printf("(%d/%d)", mdtng->vistng[i][k]+1, obj_num);
                else
                  screen_printf("(%d)", obj_num);
            }

            set_cursor_pos(scr_row, scr_col+i*entry_size);
            short obj_type=get_object_type(lvl,sx,sy,mdtng->vistng[i][k]);
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
                  thing=get_object(lvl,sx,sy,mdtng->vistng[i][k]);
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
            if ((i==mapmode->subtl_x) && (k==mapmode->subtl_y) && (scrmode->mode==MD_TNG))
                color=get_tng_display_color(obj_type,obj_owner,true);
            else
                color=get_tng_display_color(obj_type,obj_owner,false);
            screen_setcolor(color);
            screen_printf("%s",type_text);
        }
        scr_row+=2;
    }
    return scr_row;
}

int get_tng_display_color(short obj_type,unsigned char obj_owner,short marked)
{
    int col;
    if (marked)
      col=PRINT_COLOR_WHITE_ON_RED;
    else
      col=PRINT_COLOR_LGREY_ON_BLACK;
    switch (obj_type)
    {
    case OBJECT_TYPE_ACTNPT:
    case OBJECT_TYPE_STLIGHT:
        return col+35;
    case OBJECT_TYPE_THING:
        return col+30+obj_owner;
    default:
        return col+36;
    }
}

void tng_makeitem(int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing=create_item_adv(lvl,sx,sy,stype_idx);
    thing_add(lvl,thing);
    mdtng->vistng[mapmode->subtl_x][mapmode->subtl_y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
}

void tng_change_height(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_height)
{
    unsigned char *obj=get_object(lvl,sx,sy,z);
    short obj_type=get_object_type(lvl,sx,sy,z);
    int height,subheight;
    if (obj==NULL)
    {
        message_error("Cannot find object");
        return;
    }
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      height=get_stlight_tilepos_h(obj);
      subheight=get_stlight_subtpos_h(obj)+delta_height;
      break;
    case OBJECT_TYPE_ACTNPT:
      message_error("Action points have no height");
      return;
    case OBJECT_TYPE_THING:
      height=get_thing_tilepos_h(obj);
      subheight=get_thing_subtpos_h(obj)+delta_height;
      break;
    default:
      message_error("Cannot recognize object");
      return;
      break;
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
    if (height>=MAP_SUBNUM_H)
    {
        height=MAP_SUBNUM_H-1;
        subheight=255;
        message_error("Object height limit reached");
    } else
    if (height<0)
    {
        height=0;
        subheight=0;
        message_error("Object height limit reached");
    } else
    {
        message_info("Object height adjusted");
    }
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
      set_stlight_tilepos_h(obj,height);
      set_stlight_subtpos_h(obj,subheight);
      break;
    case OBJECT_TYPE_THING:
      set_thing_tilepos_h(obj,height);
      set_thing_subtpos_h(obj,subheight);
      break;
    }
}
