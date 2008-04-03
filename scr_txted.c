/*
 * scr_txted.c
 *
 * Defines functions for initializing and displaying the scrpt screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_txted.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "lev_data.h"

// Variables

TXTED_DATA *editor;

/*
 * Initializes variables for the scrpt screen.
 */
short init_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    //Creating and clearing scrpt variable
    editor=(TXTED_DATA *)malloc(sizeof(TXTED_DATA));
    if (editor==NULL)
     die("init_scrpt: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the scrpt screen.
 */
void free_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
  free(editor);
}


/*
 * Covers actions from the scrpt screen.
 */
void actions_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key)
{
    switch (key)
    {
    case KEY_UP:
      editor->y--;
      break;
    case KEY_DOWN:
      editor->y++;
      break;
    case KEY_PGUP:
      editor->y-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      editor->y+=scrmode->rows-1;
      break;
    case KEY_U:
      recompute_script_levels(&(lvl->script));
      recompose_script(&(lvl->script));
      break;
    case KEY_TAB:
    case KEY_ESCAPE:
      end_scrpt(scrmode,mapmode,lvl);
      message_info("Returned to last work mode");
      break;
    }
    if (editor->y+scrmode->rows > editor->rows)
      editor->y=editor->rows-scrmode->rows;
    if (editor->y < 0)
      editor->y=0;
}

/*
 * Action function - start the scrpt mode.
 */
short start_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    editor->prevmode=scrmode->mode;
    editor->y=0;
    editor->rows=lvl->script.lines_count;
    editor->text=lvl->script.txt;
    editor->script_cmds=lvl->script.list;
    scrmode->mode=MD_SCRP;
    return true;
}

/*
 * Action function - end the scrpt mode.
 */
void end_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    lvl->script.txt=editor->text;
    lvl->script.lines_count=editor->rows;
    lvl->script.list=editor->script_cmds;
    editor->y=0;
    editor->rows=0;
    editor->text=NULL;
    if (scrmode->mode!=editor->prevmode)
      scrmode->mode=editor->prevmode;
    else
      scrmode->mode=MD_SLB;
}

/*
 * Draws screen for the scrpt mode.
 */
void draw_scrpt(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int i;
    for (i=0; i < scrmode->rows; i++)
    {
      char *txt_line="";
      if (editor->y+i < editor->rows)
          txt_line=editor->text[editor->y+i];
      set_cursor_pos(i,0);
      screen_printf_toeol("%s",txt_line);
    }
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}
