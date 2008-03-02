/*
 * scr_cube.c
 *
 * Defines functions for initializing and displaying the cube screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_cube.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "obj_column.h"
#include "scr_actn.h"
#include "scr_list.h"

// Variables

MDCUBE_DATA *mdcube;

/*
 * Initializes variables for the mdcube screen.
 * Note: init_list must be called too for the mode to work.
 */
short init_mdcube(void)
{
    //Creating and clearing mdcube variable
    mdcube=(MDCUBE_DATA *)malloc(sizeof(MDCUBE_DATA));
    if (mdcube==NULL)
     die("init_mdcube: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the mdcube screen.
 */
void free_mdcube(void)
{
  free(mdcube);
}

/*
 * Covers actions from custom cubes screen.
 */
void actions_mdcube(int key)
{
   //TODO
   //- create mode variable for storing column
   //- load the column at start, and add to level as "custom" at end
   //- draw the "column selector" in right panel
    message_release();
    if (!actions_list(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          end_mdcube();
          message_info("Customization cancelled");
          break;
        case KEY_ENTER:
          //TODO
          end_mdcube();
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
short start_mdcube(struct LEVEL *lvl)
{
    short result;
    result=start_list(lvl,MD_CUBE);
    mdcube->cube_idx=1;
    list->pos=8;
    return result;
}

/*
 * Action function - end the mdcube mode.
 */
void end_mdcube()
{
    end_list();
}

/*
 * Draws screen for the mdcube mode.
 */
void draw_mdcube()
{
    draw_numbered_list(get_cube_fullname,0,CUBE_MAX_INDEX,20);
    set_cursor_pos(get_screen_rows()-1, 17);
}
