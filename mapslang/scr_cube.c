/******************************************************************************/
// scr_cube.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  27 Oct 2007

// Purpose:
//   Defines functions for initializing and displaying the cube screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_cube.h"

#include "../libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_list.h"
#include "scr_help.h"

#include "var_utils.h"

/*
 * Initializes variables for the mdcube screen.
 * Note: init_list must be called too for the mode to work.
 */
short init_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing mdcube variable
    workdata->mdcube=(struct MDCUBE_DATA *)malloc(sizeof(struct MDCUBE_DATA));
    if (workdata->mdcube==NULL)
     die("init_mdcube: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the mdcube screen.
 */
void free_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  free(workdata->mdcube);
}

/*
 * Covers actions from custom cubes screen.
 */
void actions_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
   //TODO
   //- create mode variable for storing column
   //- load the column at start, and add to level as "custom" at end
   //- draw the "column selector" in right panel
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_mdcube(scrmode,workdata);
          message_info("Customization cancelled");
          break;
        case KEY_ENTER:
          //TODO
          end_mdcube(scrmode,workdata);
          message_info("Column cubes set");
          break;
        default:
          message_info("Unrecognized custom cubes key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start the mdcube mode.
 */
short start_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    short result;
    if (workdata==NULL) return false;
    result=start_list(scrmode,workdata,MD_CUBE);
    workdata->mdcube->cube_idx=1;
    workdata->list->pos=8;
    return result;
}

/*
 * Action function - end the mdcube mode.
 */
void end_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

/*
 * Draws screen for the mdcube mode.
 */
void draw_mdcube(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    // Note: get_cubedesc_head shouldn't be used here, because it may contain names
    // which are too long for the list (without shortcuts)
    draw_numbered_list(get_cube_fullname,scrmode,workdata,
        0,CUBE_MAX_INDEX,20);
    show_no_cursor();
}
