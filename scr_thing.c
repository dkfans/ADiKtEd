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
short init_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    //Creating and clearing mdtng variable
    mdtng=(MDTNG_DATA *)malloc(sizeof(MDTNG_DATA));
    if (mdtng==NULL)
     die("init_mdtng: Cannot allocate memory.");
    mdtng->obj_ranges_changed=true;
    return true;
}

/*
 * Deallocates memory for the tng screen.
 */
void free_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
  free(mdtng);
}

void change_visited_tile(struct MAPMODE_DATA *mapmode)
{
    int i,j;
    for (i=0; i<MAP_SUBNUM_X; i++)
      for (j=0; j<MAP_SUBNUM_Y; j++)
        mdtng->vistng[i][j]=0;
}

/*
 * Covers actions from the tng screen.
 */
void actions_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key)
{
    int sx, sy;
    unsigned char *thing;
    sx = (mapmode->map.x+mapmode->screen.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->map.y+mapmode->screen.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    message_release();
    
    if (!cursor_actions(scrmode,mapmode,lvl,key))
    if (!subtl_select_actions(mapmode,key))
    {
      switch (key)
      {
        case KEY_TAB:
          end_mdtng(scrmode,mapmode,lvl);
          mdstart[MD_SLB](scrmode,mapmode,lvl);
          message_info("Slab mode activated");
          break;
        case KEY_C: //add creature
          if (mapmode->creature_list_on_create)
            mdstart[MD_CRTR](scrmode,mapmode,lvl);
          else
            tng_makecreature(scrmode,mapmode,lvl,sx,sy,CREATR_SUBTP_WIZRD);
          break;
        case KEY_I: //add item
          mdstart[MD_CITM](scrmode,mapmode,lvl);
          break;
        case KEY_B: // create spell
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_SPELLHOE);
          break;
        case KEY_H: // Add Dungeon heart
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_DNHEART);
          break;
        case KEY_SHIFT_H: // Add hero gate
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_HEROGATE);
          break;
        case KEY_D: // Dungeon special
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_SPREVMAP);
          break;
        case KEY_G: // Gold
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_GOLDCHEST);
          break;
        case KEY_SHIFT_G: // Gold
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_GOLD);
          break;
        case KEY_F: // Food
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_CHICKNSTB);
          break;
        case KEY_R: // create torch
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_TORCH);
          break;
        case KEY_P: // Poles and prison bars
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_FLAGPOST);
          break;
        case KEY_SHIFT_T: // Create trap box
          tng_makeitem(scrmode,mapmode,lvl,sx,sy,ITEM_SUBTYPE_TBBOULDER);
          break;
        case KEY_E: // Add room effect
          if (mapmode->roomeffect_list_on_create)
            mdstart[MD_CEFC](scrmode,mapmode,lvl);
          else
            tng_makeroomeffect(scrmode,mapmode,lvl,sx,sy,ROOMEFC_SUBTP_DRIPWTR);
          break;
        case KEY_A: // Add action point
          thing = create_actnpt(lvl, sx, sy);
          actnpt_add(lvl,thing);
          message_info("Added action point %d",(unsigned int)get_actnpt_number(thing));
          mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_ACTNPT);
          set_brighten_for_actnpt(mapmode,thing);
          break;
        case KEY_SHIFT_L: // Add static light
          thing = create_stlight(sx, sy);
          stlight_add(lvl,thing);
          message_info("Added static light");
          mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_STLIGHT);
          set_brighten_for_stlight(mapmode,thing);
          break;
        case KEY_K: // Copy thing to clipboard
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
            short obj_type=get_object_type(lvl,sx,sy,visiting_z);
            thing=get_object(lvl,sx,sy,visiting_z);
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
                thing = create_thing_copy(lvl,sx,sy,clip_itm->data);
                thing_add(lvl,thing);
                message_info("Thing pasted from clipboard at subtile %d,%d",sx,sy);
                if (is_roomeffect(thing))
                    set_brighten_for_thing(mapmode,thing);
                mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
                break;
            case OBJECT_TYPE_ACTNPT:
                thing = create_actnpt_copy(sx,sy,clip_itm->data);
                actnpt_add(lvl,thing);
                set_brighten_for_actnpt(mapmode,thing);
                message_info("Action point pasted from clipboard at subtile %d,%d",sx,sy);
                mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_ACTNPT);
                break;
            case OBJECT_TYPE_STLIGHT:
                thing = create_stlight_copy(sx,sy,clip_itm->data);
                stlight_add(lvl,thing);
                message_info("Static light pasted from clipboard at subtile %d,%d",sx,sy);
                set_brighten_for_stlight(mapmode,thing);
                mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_STLIGHT);
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
          int prev_tng_rmv=lvl->stats.things_removed;
          int prev_tng_add=lvl->stats.things_added;
          update_slab_owners(lvl);
          update_obj_for_whole_map(lvl);
          update_obj_subpos_and_height_for_whole_map(lvl);
          message_info("Auto-maintained TNG entries updated, %u added, %u removed.",
              lvl->stats.things_added-prev_tng_add,lvl->stats.things_removed-prev_tng_rmv);
          mdtng->obj_ranges_changed=true;
          change_visited_tile(mapmode);
          };break;
        case KEY_V: // Verify whole map
          level_verify_with_highlight(lvl,mapmode);
          break;
        case KEY_T: // Create trap
          if (mapmode->traps_list_on_create)
            mdstart[MD_CTRP](scrmode,mapmode,lvl);
          else
            tng_maketrap(scrmode,mapmode,lvl,sx,sy,TRAP_SUBTYPE_BOULDER);
          break;
        case KEY_L: // Lock / unlock a door
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
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
          };break;
        case KEY_O: // Change ownership of creature/trap/special/spell
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
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
        case KEY_SHIFT_S:
        case KEY_SHIFT_X: // Change type of creature
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
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
              if (key==KEY_SHIFT_S)
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
        case KEY_S: // Change level of creature/trap/special/spell/lair/room efct
          action_inc_object_level(scrmode,mapmode,lvl);
          break;
        case KEY_X: // Change level of creature/trap/special/spell/lair/room efct
          action_dec_object_level(scrmode,mapmode,lvl);
          break;
        case KEY_SLASH: // Cycle through highlighted things
          {
          unsigned int obj_num=get_object_subnums(lvl,sx,sy);
          if (obj_num > 0)
            mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=(mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]+1)%(obj_num);
          };break;
        case KEY_SQRBRCKTL:
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
            tng_change_height(scrmode,mapmode,lvl,sx,sy,visiting_z,-64);
          };break;
        case KEY_SQRBRCKTR:
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
            tng_change_height(scrmode,mapmode,lvl,sx,sy, visiting_z,+64);
          };break;

        case KEY_NMINUS:
        case KEY_UNDERLN:
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
            tng_change_range(scrmode,mapmode,lvl,sx,sy,visiting_z,-64);
          };break;
        case KEY_NPLUS:
        case KEY_EQUAL:
          {
            int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
            tng_change_range(scrmode,mapmode,lvl,sx,sy, visiting_z,+64);
          };break;

        case KEY_DEL: // delete
          action_delete_object(scrmode,mapmode,lvl);
          break;
        case KEY_ENTER:
          action_edit_object(scrmode,mapmode,lvl);
          break;
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
short start_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    mapmode->mark=false;
    change_visited_tile(mapmode);
    mdtng->obj_ranges_changed=true;
    scrmode->usrinput_type=SI_NONE;
    scrmode->mode=MD_TNG;
    return true;
}

/*
 * Action function - end the thing mode.
 */
void end_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    mapmode->panel_mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
}

/*
 * Draws screen for the thing mode.
 */
void draw_mdtng(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    if (mdtng->obj_ranges_changed)
    {
      update_brighten(lvl,mapmode);
      mdtng->obj_ranges_changed=false;
    }
    draw_map_area(scrmode,mapmode,lvl,true,false,true);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(scrmode,mapmode,lvl,mapmode->panel_mode);
    else
      draw_mdtng_panel(scrmode,mapmode,lvl);
    draw_map_cursor(scrmode,mapmode,lvl,true,false,true);
    return;
}

void draw_mdtng_panel(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int sx, sy;
    sx = (mapmode->screen.x+mapmode->map.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->screen.y+mapmode->map.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    int k=0;
    short obj_type=get_object_type(lvl,sx,sy,mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]);
    char *obj=get_object(lvl,sx,sy,mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]);
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
      display_thing(obj, scr_col1, 1, scrmode->rows);
      break;
    default:
      k=display_mode_keyhelp(k,scr_col1,scrmode->rows-TNGDAT_ROWS,scrmode->mode,0);
      display_obj_stats(scrmode,mapmode,lvl,k,scr_col1);
      break;
    }
    display_rpanel_bottom(scrmode,mapmode,lvl);
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

int display_thing(unsigned char *thing, int scr_col, int scr_row,int max_row)
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
    set_cursor_pos(scr_row++, scr_col);
    int tpos_x=get_thing_subtile_x(thing);
    int tpos_y=get_thing_subtile_y(thing);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", tpos_x, tpos_y,
              tpos_x/MAP_SUBNUM_X, tpos_y/MAP_SUBNUM_Y);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("Position within subtile: %3d, %3d",
              get_thing_subtpos_x(thing), get_thing_subtpos_y(thing));
    set_cursor_pos(scr_row++, scr_col);
    unsigned char type_idx=get_thing_type(thing);
    screen_printf("Thing type: ");
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf(get_thing_type_fullname(type_idx));
    set_cursor_pos(scr_row++, scr_col);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf("Altitude: %3d within subtile %d",
        get_thing_subtpos_h(thing),get_thing_subtile_h(thing));
    // Only room effects and doors have sensitive tile.
    // but Traps, doors and creatures may have a number written in same way
    // on this position - to check
    if ((type_idx==THING_TYPE_ROOMEFFECT)||(type_idx==THING_TYPE_ITEM))
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
      if (init_item_desc(MD_CRTR,stype_idx-1) && (help->drows<=(max_row-scr_row)))
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
      if (init_item_desc(MD_CTRP,stype_idx-1) && (help->drows<=(max_row-scr_row)))
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
    // Room effect
    case THING_TYPE_ROOMEFFECT:
    {
      unsigned short stype_idx=get_thing_subtype(thing);
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("Kind : ");
      screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
      screen_printf(get_roomeffect_subtype_fullname(stype_idx));
      set_cursor_pos(scr_row++, scr_col);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Range: %3d subtiles and %3d",
          get_thing_range_subtile(thing),get_thing_range_subtpos(thing));
      scr_row++;
      if (init_item_desc(MD_EFCT,stype_idx-1) && (help->drows<=(max_row-scr_row)))
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
          unsigned int cnum=-(char)get_thing_level(thing);
          set_cursor_pos(scr_row-1, scr_col+20);
          screen_printf("Number: %d", cnum);
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
      if (init_item_desc(MD_CITM,stype_idx-1) && (help->drows<=(max_row-scr_row)))
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

int display_action_point (unsigned char *actnpt, int x, int y)
{
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(y++, x);
    screen_printf("Action point data block:");
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
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf("Action point");
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(" number ");
    screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
    screen_printf("%d", get_actnpt_number(actnpt));
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(":");
    set_cursor_pos(y++, x);
    int subtlpos_x=get_actnpt_subtile_x(actnpt);
    int subtlpos_y=get_actnpt_subtile_y(actnpt);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", subtlpos_x, subtlpos_y,
        subtlpos_x/MAP_SUBNUM_X, subtlpos_y/MAP_SUBNUM_Y);
    set_cursor_pos(y++, x);
    screen_printf("Position within subtile: %3d, %3d",
        get_actnpt_subtpos_x(actnpt), get_actnpt_subtpos_y(actnpt));
    set_cursor_pos(y++, x);
    screen_printf("Range: %3d subtiles and %3d",
        get_actnpt_range_subtile(actnpt),get_actnpt_range_subtpos(actnpt));
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
    screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
    screen_printf("Static light");
    set_cursor_pos(y++, x);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    int subtlpos_x=get_stlight_subtile_x(stlight);
    int subtlpos_y=get_stlight_subtile_y(stlight);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", subtlpos_x, subtlpos_y,
        subtlpos_x/MAP_SUBNUM_X, subtlpos_y/MAP_SUBNUM_Y);
    set_cursor_pos(y++, x);
    screen_printf("Position within subtile: %3d, %3d",
        (unsigned int)get_stlight_subtpos_x(stlight), (unsigned int)get_stlight_subtpos_y(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Altitude: %3d within subtile %d",
        (unsigned int)get_stlight_subtpos_h(stlight),(unsigned int)get_stlight_subtile_h(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Range: %3d subtiles and %d",
        (unsigned int)get_stlight_range_subtile(stlight),(unsigned int)get_stlight_range_subtpos(stlight));
    set_cursor_pos(y++, x);
    screen_printf("Intensivity: ");
    screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
    screen_printf("%3d",(unsigned int)get_stlight_intensivity(stlight));
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    return y;
}

int display_obj_stats(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,const struct LEVEL *lvl, int scr_row, int scr_col)
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
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("Room things:%4d",lvl->stats.room_things_count);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("Items:%4d",lvl->stats.items_count);
    }
    if (scrmode->rows >= scr_row+TNGDAT_ROWS+9)
    {
      set_cursor_pos(scr_row++, scr_col);
      screen_printf("%s","Detailed items");
      set_cursor_pos(scr_row, scr_col2);
      screen_printf("Hero gates:%4d",lvl->stats.hero_gates_count);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf("Dung hearts:%4d",lvl->stats.dn_hearts_count);

      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_ITEMEFFCT),
          lvl->stats.things_count[THING_CATEGR_ITEMEFFCT]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_CREATLAIR),
          lvl->stats.things_count[THING_CATEGR_CREATLAIR]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_SPECIALBOX),
          lvl->stats.things_count[THING_CATEGR_SPECIALBOX]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_SPELLBOOK),
          lvl->stats.things_count[THING_CATEGR_SPELLBOOK]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_WRKSHOPBOX),
          lvl->stats.things_count[THING_CATEGR_WRKSHOPBOX]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_SPINNTNG),
          lvl->stats.things_count[THING_CATEGR_SPINNTNG]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_FOOD),
          lvl->stats.things_count[THING_CATEGR_FOOD]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_GOLD),
          lvl->stats.things_count[THING_CATEGR_GOLD]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_TORCHCNDL),
          lvl->stats.things_count[THING_CATEGR_TORCHCNDL]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_HEARTFLAME),
          lvl->stats.things_count[THING_CATEGR_HEARTFLAME]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_POLEBAR),
          lvl->stats.things_count[THING_CATEGR_POLEBAR]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_STATUE),
          lvl->stats.things_count[THING_CATEGR_STATUE]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_FURNITURE),
          lvl->stats.things_count[THING_CATEGR_FURNITURE]);
      set_cursor_pos(scr_row++, scr_col2);
      screen_printf("%9s:%5d",get_thing_category_shortname(THING_CATEGR_ROOMEQUIP),
          lvl->stats.things_count[THING_CATEGR_ROOMEQUIP]);
      set_cursor_pos(scr_row, scr_col1);
      screen_printf("%9s:%6d",get_thing_category_shortname(THING_CATEGR_PWHAND),
          lvl->stats.things_count[THING_CATEGR_PWHAND]);
      scr_row++;
    }
    return scr_row;
}

int display_tng_subtiles(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,
    const struct LEVEL *lvl, int scr_row, int scr_col,short compressed,int ty,int tx)
{
    set_cursor_pos(scr_row, scr_col);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf(".tng entries");
    int i,k;
    unsigned char *thing;
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
            if ((i==mapmode->subtl.x) && (k==mapmode->subtl.y) && (scrmode->mode==MD_TNG))
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

unsigned char *tng_makeitem(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing=create_item_adv(lvl,sx,sy,stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create thing");
        return NULL;
    }
    thing_add(lvl,thing);
    mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
    message_info("Item added: %s",get_item_subtype_fullname(stype_idx));
    return thing;
}

unsigned char *tng_makeroomeffect(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_roomeffect(lvl,sx,sy,stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create Room Effect");
        return NULL;
    }
    thing_add(lvl,thing);
    message_info("Room Effect added to map at (%d,%d)",sx,sy);
    mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
    set_brighten_for_thing(mapmode,thing);
    return thing;
}

unsigned char *tng_maketrap(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_trap (lvl, sx, sy, stype_idx);
    if (thing==NULL)
    {
        message_error("Cannot create Trap");
        return NULL;
    }
    thing_add(lvl,thing);
    message_info("Trap added to map at (%d,%d)",sx,sy);
    mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
    return thing;
}

unsigned char *tng_makecreature(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int sx,int sy,unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_creature(lvl,sx,sy,stype_idx);
    thing_add(lvl,thing);
    // Show the new thing
    mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subtl_last(lvl,sx,sy,OBJECT_TYPE_THING);
    message_info("Creature added to map at (%d,%d)",sx,sy);
    return thing;
}

void tng_change_height(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_height)
{
    unsigned char *obj=get_object(lvl,sx,sy,z);
    short obj_type=get_object_type(lvl,sx,sy,z);
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
      tile_limit=MAP_SIZE_X/2;
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
      set_brighten_for_actnpt(mapmode,obj);
      break;
    case OBJECT_TYPE_THING:
      set_thing_subtile_h(obj,height);
      set_thing_subtpos_h(obj,subheight);
      break;
    }
}

void tng_change_range(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_range)
{
    unsigned char *obj=get_object(lvl,sx,sy,z);
    short obj_type=get_object_type(lvl,sx,sy,z);
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
      tile_limit=MAP_SIZE_X/2;
      break;
    case OBJECT_TYPE_ACTNPT:
      rng=get_actnpt_range_subtile(obj);
      subrng=get_actnpt_range_subtpos(obj)+delta_range;
      tile_limit=MAP_SIZE_X/2;
      break;
    case OBJECT_TYPE_THING:
      rng=get_thing_range_subtile(obj);
      subrng=get_thing_range_subtpos(obj)+delta_range;
      tile_limit=MAP_SIZE_X/2;
      break;
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
          set_brighten_for_stlight(mapmode,obj);
      else
          mdtng->obj_ranges_changed=true;
      break;
    case OBJECT_TYPE_ACTNPT:
      set_actnpt_range_subtile(obj,rng);
      set_actnpt_range_subtpos(obj,subrng);
      if (delta_range>0)
          set_brighten_for_actnpt(mapmode,obj);
      else
          mdtng->obj_ranges_changed=true;
      break;
    case OBJECT_TYPE_THING:
      set_thing_range_subtile(obj,rng);
      set_thing_range_subtpos(obj,subrng);
      if (delta_range>0)
          set_brighten_for_thing(mapmode,obj);
      else
          mdtng->obj_ranges_changed=true;
      break;
    }
}

void action_edit_object(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int sx, sy;
    sx = (mapmode->map.x+mapmode->screen.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->map.y+mapmode->screen.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
    short obj_type=get_object_type(lvl,sx,sy,visiting_z);
    unsigned char *thing;
    thing = get_object(lvl,sx,sy,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
      {
        unsigned char type_idx=get_thing_type(thing);
        switch (type_idx)
        {
        case THING_TYPE_CREATURE:
            mdstart[MD_ECRT](scrmode,mapmode,lvl);
            break;
        case THING_TYPE_TRAP:
            mdstart[MD_ETRP](scrmode,mapmode,lvl);
            break;
        case THING_TYPE_ITEM:
            mdstart[MD_EITM](scrmode,mapmode,lvl);
            break;
        case THING_TYPE_ROOMEFFECT:
            mdstart[MD_EFCT](scrmode,mapmode,lvl);
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

void action_delete_object(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int sx, sy;
    sx = (mapmode->map.x+mapmode->screen.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->map.y+mapmode->screen.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
    short obj_type=get_object_type(lvl,sx,sy,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_STLIGHT:
        object_del(lvl,sx,sy, visiting_z);
        message_info_force("Static light deleted.");
        mdtng->obj_ranges_changed=true;
        break;
    case OBJECT_TYPE_ACTNPT:
        object_del(lvl,sx,sy, visiting_z);
        message_info_force("Action point deleted.");
        mdtng->obj_ranges_changed=true;
        break;
    case OBJECT_TYPE_THING:
        object_del(lvl,sx,sy, visiting_z);
        message_info_force("Thing deleted.");
        mdtng->obj_ranges_changed=true;
        break;
    default:
        message_error("Nothing to delete.");
        break;
    }
    if (visiting_z>0)
        mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]--;
    else
        mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y]=get_object_subnums(lvl,sx,sy)-1;
}

void action_inc_object_level(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int sx, sy;
    sx = (mapmode->map.x+mapmode->screen.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->map.y+mapmode->screen.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    unsigned char *thing;
    unsigned char *stlight;
    int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
    short obj_type=get_object_type(lvl,sx,sy,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
        thing = get_object(lvl,sx,sy,visiting_z);
        if (is_creature(thing))
        {
            if (get_thing_level(thing)<9)
            {
                set_thing_level(thing,get_thing_level(thing)+1);
                message_info("Creature level increased.");
            } else
                message_error("Creature level limit reached.");
        } else
        if (switch_thing_subtype(thing,true))
        {
            message_info("Item type switched to next.");
        } else
            message_error("This item has no level/type, or its limit is reached.");
        break;
    case OBJECT_TYPE_STLIGHT:
        stlight = get_object(lvl,sx,sy,visiting_z);
        unsigned int intens=get_stlight_intensivity(stlight);
        if (intens<255)
        {
            if (intens<127)
              intens++;
            else
              intens+=4;
            if (intens>255) intens=255;
            set_stlight_intensivity(stlight,intens);
        } else
        {
            message_error("Maximum static light intensivity reached.");
        }
        break;
    default:
        message_error("Can't change level: no thing selected.");
        break;
    }
}

void action_dec_object_level(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int sx, sy;
    sx = (mapmode->map.x+mapmode->screen.x)*MAP_SUBNUM_X+mapmode->subtl.x;
    sy = (mapmode->map.y+mapmode->screen.y)*MAP_SUBNUM_Y+mapmode->subtl.y;
    unsigned char *thing;
    unsigned char *stlight;
    int visiting_z=mdtng->vistng[mapmode->subtl.x][mapmode->subtl.y];
    short obj_type=get_object_type(lvl,sx,sy,visiting_z);
    switch (obj_type)
    {
    case OBJECT_TYPE_THING:
        thing = get_object(lvl,sx,sy,visiting_z);
        if (is_creature(thing))
        {
            if (get_thing_level(thing)>0)
            {
                set_thing_level(thing,get_thing_level(thing)-1);
                message_info("Creature level decreased.");
            } else
            message_error("Creature level limit reached.");
        } else
        if (switch_thing_subtype(thing,false))
        {
            message_info("Item type switched to previous.");
        } else
            message_error("This item has no level/type, or its limit is reached.");
        break;
    case OBJECT_TYPE_STLIGHT:
        stlight = get_object(lvl,sx,sy,visiting_z);
        unsigned int intens=get_stlight_intensivity(stlight);
        if (intens>1)
        {
            intens--;
            set_stlight_intensivity(stlight,intens);
        } else
        {
            message_error("Minimum static light intensivity reached.");
        }
        break;
    default:
        message_error("Can't change level: no thing selected.");
        break;
    }
}
