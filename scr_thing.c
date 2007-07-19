/*
 * scr_thing.c
 *
 * Defines functions for initializing and displaying the level thing screen.
 *
 */

#include "scr_thing.h"

#include "globals.h"
#include "output_scr.h"
#include "action.h"
#include "obj_utils.h"
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
      screen_printf_toeol(" ");
    }
    if (vistng[sx][sy] < lvl->tng_subnums[cx*3+sx][cy*3+sy])
      display_thing (lvl->tng_lookup[cx*3+sx][cy*3+sy][vistng[sx][sy]], 
                   cols+3, 1);
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
                        dat_high[cx*3+i][cy*3+j],
                        dat_low[cx*3+i][cy*3+j]);
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
            if (lvl->tng_subnums[cx*3+i][cy*3+j] > 1)
            {
                if (mode==MD_TNG)
                  screen_printf("(%d/%d)", vistng[i][j]+1,
                              lvl->tng_subnums[cx*3+i][cy*3+j]);
                else
                  screen_printf("(%d)   ", lvl->tng_subnums[cx*3+i][cy*3+j]);
            }
            else
                screen_printf("%s","     ");
            set_cursor_pos(all_rows-8+j*2, cols+20+i*6);
            if (i==sx && j==sy && mode==MD_TNG)
                col=10;
            else
                col=0;
            if ((vistng[i][j]) < (lvl->tng_subnums[cx*3+i][cy*3+j]))
            {
                thing=lvl->tng_lookup[cx*3+i][cy*3+j][vistng[i][j]];
                if (is_action_thing(thing))
                {
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s","action");
                }
                else
                switch (thing[6])
                {
                case THING_TYPE_CREATURE:
                  col += 30+thing[8];
                  screen_setcolor(col);
                  screen_printf("%s", get_creature_subtype_shortname(thing[7]));
                  break;
                case THING_TYPE_ROOMEFFECT:
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s", "effct");
                  break;
                case THING_TYPE_TRAP:
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s", "trap");
                  break;
                case THING_TYPE_DOOR:
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s", "door");
                  break;
                case THING_TYPE_ITEM:
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s",get_item_subtype_shortname(thing[7]));
                  break;
                default:
                  col+=35;
                  screen_setcolor(col);
                  screen_printf("%s","thing");
                  break;
                }
            }
            else
            {
                col+=36;
                screen_setcolor(col);
                screen_printf("%s","     ");
            }
          }
      }
    }
}
    
char get_thing_char (int x, int y)
{
    switch (lvl->tng_nums[x][y])
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
      return (char)(lvl->tng_nums[x][y]+'0');
      break;
    default:
      return '+';
    }
}

void display_thing (unsigned char *thing, int x, int y)
{
    int m, i;

    if (is_action_thing(thing))
    {
      display_action_point(thing, x, y);
      return;
    }
    screen_setcolor(0);
    set_cursor_pos(y, x);
    screen_printf("%s","Thing data block:");
    for (m=0; m < 3; m++)
    {
      set_cursor_pos(m+y+1, x);
      for (i=0; i < 7; i++)
          screen_printf("%02X ", (unsigned int)thing[m*7+i]);
      SLsmg_erase_eol();
    }
    set_cursor_pos(y+5, x);
    screen_printf("Subtile: %3d,%3d (Tile: %2d, %2d)", thing[1], thing[3],
              thing[1]/3, thing[3]/3);
    set_cursor_pos(y+6, x);
    screen_printf("Position within subtile: %3d, %3d", thing[0], thing[2]);
    set_cursor_pos(y+7, x);
    screen_printf("Thing type: %s",get_thing_type_fullname(thing[6]));
    
    /* Creature */
    if (thing[6]==THING_TYPE_CREATURE)
    {
      set_cursor_pos(y+8, x);
      screen_printf("Type: %s", get_creature_subtype_fullname(thing[7]));
      set_cursor_pos(y+9, x);
      screen_printf("Level: %d", thing[14]+1);
      set_cursor_pos(y+10, x);
      screen_printf("Owner: %s", get_owner_type_fullname(thing[8]));
    }
    /* Trap */
    if (thing[6]==THING_TYPE_TRAP)
    {
      set_cursor_pos(y+8, x);
      screen_printf("Type : %s", get_trap_subtype_fullname(thing[7]));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(thing[8]));
    }
    // Door
    if (thing[6]==THING_TYPE_DOOR)
    {
      set_cursor_pos(y+8, x);
      screen_printf("Type : %s", get_door_subtype_fullname(thing[7]));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(thing[8]));
      set_cursor_pos(y+10, x);
      char *lock_state;
      if (thing[14])
          lock_state="Locked";
      else
          lock_state="Unlocked";
      screen_printf("%s",lock_state);
    }
    // Room effect
    if (thing[6]==THING_TYPE_ROOMEFFECT)
    {
      set_cursor_pos(y+8, x);
      screen_printf("Type : %s",get_roomeffect_subtype_fullname(thing[7]));
    }
    // Object
    if (thing[6]==THING_TYPE_ITEM)
    {
      set_cursor_pos(y+8, x);
      screen_printf("Type: %s",get_item_subtype_fullname(thing[7]));
      set_cursor_pos(y+9, x);
      screen_printf("Owner: %s", get_owner_type_fullname(thing[8]));
      set_cursor_pos(y+10, x);
      screen_printf("Category: %s",get_item_category_fullname(thing[7]));
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
