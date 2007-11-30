/*
 * scr_thing.c
 *
 * Defines functions for initializing and displaying the level thing screen,
 * includes also creature and item type subscreens.
 *
 */

#include "scr_thing.h"

#include "globals.h"
#include "output_scr.h"
#include "action.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "scr_help.h"
#include "internal.h"

void draw_tng(void)
{
    int i, j;
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
                screen_setcolor(lvl->own[j+mapx][i+mapy]+10);
                screen_printchr(get_thing_char(mapx+j, mapy+i));
            }
            else
                screen_printchr(' ');
          }
      }
      screen_setcolor(2);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(0);
      screen_printf_toeol("");
    }
    short obj_type=get_object_type(lvl, cx*3+sx,cy*3+sy,vistng[sx][sy]);
    char *obj=get_object(lvl, cx*3+sx,cy*3+sy,vistng[sx][sy]);
    switch (obj_type)
    {
    case OBJECT_TYPE_ACTNPT:
      display_action_point(obj, cols+3, 1);
      break;
    case OBJECT_TYPE_THING:
      display_thing (obj, cols+3, 1);
      break;
    }
    display_tngdat();
    show_cursor(get_thing_char (screenx+mapx, screeny+mapy));
    return;
}

void display_tngdat(void)
{
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();
    int i, j;
    int cx, cy;
    int col;
    unsigned char *thing;

    cx = screenx+mapx;
    cy = screeny+mapy;
    
    /* Display .dat stuff for this tile */
    if (all_rows > 7)
    {
      screen_setcolor(0);
      set_cursor_pos(all_rows-10, cols+3);
      screen_printf("%s",".dat entries");
      for (i=0; i < 3; i++)
      {
          for (j=0; j < 3; j++)
          {
            set_cursor_pos(all_rows-8+j*2, cols+3+i*5);
            
            if (i==sx && j==sy && mode==MD_CLM)
                col=45;
            else
                col=0;
            screen_setcolor(col);
            if (datmode==0)
                screen_printf("%02X%02X",
                        (unsigned int)lvl->dat_high[cx*3+i][cy*3+j],
                        (unsigned int)lvl->dat_low[cx*3+i][cy*3+j]);
          }
      }
      set_cursor_pos(all_rows-10, cols+20);
      screen_setcolor(0);
      screen_printf("%s",".tng entries");
      for (i=0; i < 3; i++)
      {
          for (j=0; j < 3; j++)
          {
            screen_setcolor(0);
            set_cursor_pos(all_rows-8+j*2, cols+20+i*6);
            screen_printf("%s","     ");
            set_cursor_pos(all_rows-7+j*2, cols+20+i*6);
            unsigned int obj_num=get_object_subnums(lvl,cx*3+i,cy*3+j);
            if (obj_num > 1)
            {
                if (mode==MD_TNG)
                  screen_printf("(%d/%d)", vistng[i][j]+1, obj_num);
                else
                  screen_printf("(%d)   ", obj_num);
            }
            else
                screen_printf("%s","     ");
            set_cursor_pos(all_rows-8+j*2, cols+20+i*6);
            if ((i==sx) && (j==sy) && (mode==MD_TNG))
                col=10;
            else
                col=0;
            short obj_type=get_object_type(lvl,cx*3+i,cy*3+j,vistng[i][j]);
            switch (obj_type)
            {
              case OBJECT_TYPE_ACTNPT:
                {
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s","action");
                };break;
              case OBJECT_TYPE_THING:
                {
                  thing=get_object(lvl,cx*3+i,cy*3+j,vistng[i][j]);
                  switch (get_thing_type(thing))
                  {
                  case THING_TYPE_CREATURE:
                    col += 30+get_thing_owner(thing);
                    screen_setcolor(col);
                    screen_printf("%s", get_creature_subtype_shortname(get_thing_subtype(thing)));
                    break;
                  case THING_TYPE_ITEM:
                    col+=35;
                    screen_setcolor(col);
                    screen_printf("%s",get_item_subtype_shortname(get_thing_subtype(thing)));
                    break;
                  default:
                    col+=35;
                    screen_setcolor(col);
                    screen_printf("%s",get_thing_type_shortname(get_thing_type(thing)));
                    break;
                  }
                };break;
              default:
                {
                    col+=36;
                    screen_setcolor(col);
                    screen_printf("%s","     ");
                };break;
            }
          }
      }
    }
}
    
char get_thing_char (int x, int y)
{
    switch (get_object_tilnums(lvl,x,y))
    {
    case  0:
      return ' ';
      break;
    case  1:
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

void display_thing (unsigned char *thing, int x, int y)
{
    int m, i;

    screen_setcolor(0);
    set_cursor_pos(y, x);
    screen_printf("%s","Thing data block:");
    for (m=0; m < 3; m++)
    {
      set_cursor_pos(m+y+1, x);
      for (i=0; i < 7; i++)
          screen_printf("%02X ", (unsigned int)thing[m*7+i]);
      screen_printf_toeol("");
    }
    set_cursor_pos(y+5, x);
    int tpos_x=get_thing_tilepos_x(thing);
    int tpos_y=get_thing_tilepos_y(thing);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", tpos_x, tpos_y,
              tpos_x/MAP_SUBNUM_X, tpos_y/MAP_SUBNUM_Y);
    set_cursor_pos(y+6, x);
    screen_printf("Position within subtile: %3d, %3d",
              get_thing_subtpos_x(thing), get_thing_subtpos_y(thing));
    set_cursor_pos(y+7, x);
    screen_printf("Thing type: %s",get_thing_type_fullname(get_thing_type(thing)));

    switch (get_thing_type(thing))
    {
    // Creature
    case THING_TYPE_CREATURE:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Kind: %s", get_creature_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y+9, x);
      screen_printf("Level: %d", thing[14]+1);
      set_cursor_pos(y+10, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
    };break;
    // Trap
    case THING_TYPE_TRAP:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Kind : %s", get_trap_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
    };break;
    // Door
    case THING_TYPE_DOOR:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Kind : %s", get_door_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
      set_cursor_pos(y+10, x);
      char *lock_state;
      if (thing[14])
          lock_state="Locked";
      else
          lock_state="Unlocked";
      screen_printf("%s",lock_state);
    };break;
    // Room effect
    case THING_TYPE_ROOMEFFECT:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Kind : %s",get_roomeffect_subtype_fullname(get_thing_subtype(thing)));
    };break;
    // Object
    case THING_TYPE_ITEM:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Kind: %s",get_item_subtype_fullname(get_thing_subtype(thing)));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(get_thing_owner(thing)));
      set_cursor_pos(y+10, x);
      screen_printf("Category: %s",get_item_category_fullname(get_thing_subtype(thing)));
    };break;
    default:
    {
      set_cursor_pos(y+8, x);
      screen_printf("Unrecognized thing!");
      set_cursor_pos(y+9, x);
      screen_printf("Type: %d", (int)get_thing_type(thing));
    };break;
    }
}

void display_action_point (unsigned char *point, int x, int y)
{
    screen_setcolor(0);
    set_cursor_pos(y, x);
    screen_printf("Action point number %d:", point[19]+(point[20]<<8));
    set_cursor_pos(y+1, x);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", point[1], point[3],
              point[1]/3, point[3]/3);
    set_cursor_pos(y+2, x);
    screen_printf("Position within subtile: %3d, %3d", point[0], point[2]);
    set_cursor_pos(y+3, x);
    screen_printf("Unknown byte pair: %02X %02X", point[17], point[18]);
}

void draw_creatrkind()
{
    draw_numbered_list(get_creature_subtype_fullname,1,CREATR_SUBTP_FLOAT,16,
        crtkeyhelprows,crtkeyhelp);
    set_cursor_pos(get_screen_rows()-1, 17);
}

void draw_itemtype()
{
    draw_numbered_list(get_item_subtype_fullname,1,ITEM_SUBTYPE_SPELLARMG,18,
        itmtkeyhelprows,itmtkeyhelp);
    set_cursor_pos(get_screen_rows()-1, 17);
}

