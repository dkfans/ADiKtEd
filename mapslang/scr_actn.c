/******************************************************************************/
// scr_actn.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines functions for initializing and displaying all the level screens,
//   and also for actions - reading keyboard input and executing commands.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_actn.h"

#include <math.h>
#include <string.h>
#include "../libadikted/adikted.h"
#include "var_utils.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_thing.h"
#include "scr_help.h"
#include "scr_slab.h"
#include "scr_clm.h"
#include "scr_list.h"
#include "scr_txted.h"
#include "scr_rwrk.h"
#include "scr_cube.h"
#include "textmenu.h"
#include "scr_txtgen.h"

void (*actions [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *,int)={
     actions_mdslab, actions_mdtng,  actions_crcrtr,  actions_critem,
     actions_help,   actions_mdclm,  actions_scrpt,  actions_mdtextr,
     actions_mdcclm, actions_mdcube, actions_mdslbl, actions_mdrwrk,
     actions_mdsrch, actions_mdlmap, actions_mdsmap, actions_mdgrafit,
     actions_crefct, actions_crtrap, actions_editem, actions_edcrtr,
     actions_edefct, actions_edtrap, };

// Drawing functions for modes. You can fill new entries
// with "draw_mdempty" till you create proper draw function.
void (*mddraw [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *)={
     draw_mdslab, draw_mdtng,  draw_crcrtr,  draw_critem,
     draw_help,   draw_mdclm,  draw_scrpt,  draw_mdtextr,
     draw_mdcclm, draw_mdcube, draw_mdslbl, draw_mdrwrk,
     draw_mdsrch, draw_mdlmap, draw_mdsmap, draw_mdgrafit,
     draw_crefct, draw_crtrap, draw_editem, draw_edcrtr,
     draw_edefct, draw_edtrap, };

short (*mdstart [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *)={
     start_mdslab, start_mdtng,  start_crcrtr,  start_critem,
     start_help,   start_mdclm,  start_scrpt,  start_mdtextr,
     start_mdcclm, start_mdcube, start_mdslbl, start_mdrwrk,
     start_mdsrch, start_mdlmap, start_mdsmap, start_mdgrafit,
     start_crefct, start_crtrap, start_editem, start_edcrtr,
     start_edefct, start_edtrap, };

void (*mdend [])(struct SCRMODE_DATA *,struct WORKMODE_DATA *)={
     end_mdslab, end_mdtng,  end_crcrtr,  end_critem,
     end_help,   end_mdclm,  end_scrpt,  end_mdtextr,
     end_mdcclm, end_mdcube, end_mdslbl, end_mdrwrk,
     end_mdsrch, end_mdlmap, end_mdsmap, end_mdgrafit,
     end_crefct, end_crtrap, end_editem, end_edcrtr,
     end_edefct, end_edtrap, };

// Max. 5 chars mode names
const char *modenames[]={
     "Slab",  "Thing", "ACrtr", "AItem",
     "Help",  "Clmn",  "Scrpt", "Textr",
     "CClm",  "CCube", "SlbLs", "Rewrk",
     "Srch",  "OMap",  "SMap",  "Grft",
     "AEfct", "ATrap", "EItem", "ECrtr",
     "EEfct", "ETrap", "(bad)",};
// longer mode names
const char *longmodenames[]={
     "slab", "thing", "add creature","add item",
     "help", "column", "script", "texture",
     "cust.column","cust.cubes","slab list","rework",
     "search", "open map", "save map", "graffiti",
     "add effect", "add trap", "edit item", "edit creatre",
     "edit effect", "edit trap", "(bad)",};

const char *string_input_msg[]={ "",
     "Map number/name to load:",
     "Map number/name to save:",
     "Graffiti text:",
     "(bad)",
      };

// indicates if the main program loop should end
short finished;

/*
 * Initializes basic structures and dynamic variables, which may be needed
 * on parameters reading process. Only the basic structures are inited here.
 */
void init_levscr_basics(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" init_levscr_basics: Started");
    // variable that informs if main program loop should finish
    finished=false;
    // random num gen seed selection
    srand(time(0));
    //Creating and clearing screen mode info variable
    *scrmode=(struct SCRMODE_DATA *)malloc(sizeof(struct SCRMODE_DATA));
    if ((*scrmode)==NULL)
     die("init_levscr: Cannot allocate memory.");
    clear_scrmode(*scrmode);
    workdata->mapmode=(struct MAPMODE_DATA *)malloc(sizeof(struct MAPMODE_DATA));
    if (workdata->mapmode==NULL)
     die("init_levscr: Cannot allocate memory.");
    // Copy map size in tiles from level
    workdata->mapmode->tlsize.x=workdata->lvl->tlsize.x;
    workdata->mapmode->tlsize.y=workdata->lvl->tlsize.y;
    message_log(" init_levscr_basics: map size set to %dx%d",workdata->mapmode->tlsize.x,workdata->mapmode->tlsize.y);
    { //allocating mapmode sub-structures
      int i;
      workdata->mapmode->hilight = (int **)malloc(workdata->mapmode->tlsize.y*sizeof(int *));
      if (workdata->mapmode->hilight==NULL)
          die("init_levscr: Out of memory");
      for (i=0; i < workdata->mapmode->tlsize.y; i++)
      {
        workdata->mapmode->hilight[i] = (int *)malloc(workdata->mapmode->tlsize.x*sizeof(int));
        if (workdata->mapmode->hilight[i]==NULL)
          die("init_levscr: Out of memory");
      }
      workdata->mapmode->brighten = (int **)malloc(workdata->mapmode->tlsize.y*sizeof(int *));
      if (workdata->mapmode->brighten==NULL)
          die("init_levscr: Out of memory");
      for (i=0; i < workdata->mapmode->tlsize.y; i++)
      {
        workdata->mapmode->brighten[i] = (int *)malloc(workdata->mapmode->tlsize.x*sizeof(int));
        if (workdata->mapmode->brighten[i]==NULL)
          die("init_levscr: Out of memory");
      }
        if (!level_init(&(workdata->mapmode->preview),MFV_DKGOLD,NULL))
          die("init_levscr: Error creating preview structure");
    }
    clear_mapmode(workdata->mapmode);
    // optns - options which are copied to level structure
    workdata->optns=(struct LEVOPTIONS *)malloc(sizeof(struct LEVOPTIONS));
    if (workdata->optns==NULL)
     die("init_levscr: Cannot allocate memory.");
    level_clear_options(workdata->optns);
    // ipanel - information panel configuration
    workdata->ipanel=(struct INFOPANEL_DATA *)malloc(sizeof(struct INFOPANEL_DATA));
    if (workdata->ipanel==NULL)
     die("init_levscr: Cannot allocate memory.");
    clear_infopanel(workdata->ipanel);
    // init modes - create text editor structures
    if (!init_scrpt(*scrmode,workdata))
     die("init_levscr: init_scrpt returned with error.");
    int i;
    (*scrmode)->automated_commands=malloc(READ_BUFSIZE*sizeof(unsigned int));
    for (i=0; i < READ_BUFSIZE; i++)
      (*scrmode)->automated_commands[i]=0;
    message_log(" init_levscr_basics: Finished");
}

/*
 * Initializes the screen to work with Adikted.
 */
void init_levscr_modes(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" init_levscr_modes: starting");
    // initilaize screen support library
    if (scrmode->screen_enabled)
      screen_init();
    // and keyboard input
    if (scrmode->input_enabled)
      input_init();
    // init modes - read help file
    init_help(scrmode,workdata);
    // init modes - create list structures
    init_list(scrmode,workdata);
    // init modes - create column structures
    init_mdclm(scrmode,workdata);
    // init modes - create thing structures
    init_mdtng(scrmode,workdata);
    // init modes - create cubes structures
    init_mdcube(scrmode,workdata);
    // initialize slab mode
    //(this includes slbkey required for all screens containing map)
    init_mdslab(scrmode,workdata);
    // Init rework mode
    init_mdrwrk(scrmode,workdata);
    // Init script generator in script mode
    init_scrptgen(scrmode,workdata);
    // Copy options into level preview structure
    level_set_options(workdata->mapmode->preview,workdata->optns);
    // Note: the modes which are not initied here, are initied earlier
    // in init_levscr_basics
    message_log(" init_levscr_modes: finished");
}

void clear_scrmode(struct SCRMODE_DATA *scrmode)
{
    scrmode->keycols=40;
    scrmode->mode=MD_SLB;
    //This will disable drawing until variables are updated
    scrmode->rows=0;
    scrmode->cols=0;
    scrmode->clipbrd=NULL;
    scrmode->clip_count=0;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
    scrmode->screen_enabled=true;
    scrmode->input_enabled=true;
}

void clear_mapmode(struct MAPMODE_DATA *mapmode)
{
    set_marking_disab(mapmode);
    set_painting_disab(mapmode);
    mapmode->paintownr=PLAYER_UNSET;
    mapmode->paintroom=SLAB_TYPE_ROCK;
    mapmode->screen.x=0;
    mapmode->screen.y=0;
    mapmode->map.x=0;
    mapmode->map.y=0;
    mapmode->subtl.x=1;
    mapmode->subtl.y=1;
    mapmode->level_preview=LPREV_LOAD;
    mapmode->show_obj_range=1;
    mapmode->traps_list_on_create=1;
    mapmode->effectgen_list_on_create=1;
    mapmode->items_list_on_create=1;
    mapmode->creature_list_on_create=1;
    mapmode->eetype=EE_NONE;
    clear_highlight(mapmode);
    clear_brighten(mapmode);
    level_free(mapmode->preview);
    level_clear(mapmode->preview);
}

void clear_infopanel(struct INFOPANEL_DATA *ipanel)
{
    ipanel->mode=PV_MODE;
    ipanel->dat_view_mode=1;
    ipanel->display_float_pos=1;
}

void free_levscr(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" free_levscr: starting");
    // free modes
    free_help(*scrmode,workdata);
    free_list(*scrmode,workdata);
    free_mdcube(*scrmode,workdata);
    free_scrpt(*scrmode,workdata);
    free_mdclm(*scrmode,workdata);
    free_mdtng(*scrmode,workdata);
    free_mdslab(*scrmode,workdata);
    free_mdrwrk(*scrmode,workdata);
    // Main screen variables
    message_log(" free_levscr: mode variables freed");
    //Freeing mapmode structure
    if (workdata->mapmode->hilight!=NULL)
    {
      int i;
      for (i=0; i<workdata->mapmode->tlsize.y; i++)
          free(workdata->mapmode->hilight[i]);
      free(workdata->mapmode->hilight);
    }
    level_free(workdata->mapmode->preview);
    level_deinit(&(workdata->mapmode->preview));
    free(workdata->mapmode);
    workdata->mapmode=NULL;
    free((*scrmode)->automated_commands);
    free(*scrmode);
    *scrmode=NULL;
    // Shutting down screen support
    screen_done();
    message_log(" free_levscr: finished");
}

/*
 * Gets highlight color of a tile.
 */
int get_tile_highlight(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty)
{
    if (mapmode->hilight==NULL) return 0;
    //Bounding position
    if ((tx>=mapmode->tlsize.x)||(ty>=mapmode->tlsize.y)) return 0;
    return mapmode->hilight[tx][ty];
}

/*
 * Sets a new value to highlight color.
 */
void set_tile_highlight(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty, int nval)
{
    //Bounding position
    if ((tx>=mapmode->tlsize.x)||(ty>=mapmode->tlsize.y)) return;
    mapmode->hilight[tx][ty]=nval;
}

/*
 * Returns if a tile should be brightened.
 */
short get_tile_brighten(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty)
{
    if (mapmode->brighten==NULL) return false;
    //Bounding position
    if ((tx>=mapmode->tlsize.x)||(ty>=mapmode->tlsize.y)) return false;
    return mapmode->brighten[tx][ty];
}

/*
 * Sets a new value to brighten array.
 */
void set_tile_brighten(struct MAPMODE_DATA *mapmode, unsigned int tx, unsigned int ty, short nval)
{
    //Bounding position
    if ((tx>=mapmode->tlsize.x)||(ty>=mapmode->tlsize.y)) return;
    mapmode->brighten[tx][ty]=nval;
}

void clear_highlight(struct MAPMODE_DATA *mapmode)
{
    int i,k;
    for (k=0;k<mapmode->tlsize.y;k++)
      for (i=0;i<mapmode->tlsize.x;i++)
        mapmode->hilight[i][k]=0;
}

void clear_brighten(struct MAPMODE_DATA *mapmode)
{
    int i,k;
    for (k=0;k<mapmode->tlsize.y;k++)
      for (i=0;i<mapmode->tlsize.x;i++)
        mapmode->brighten[i][k]=false;
}

void update_brighten(struct LEVEL *lvl,struct MAPMODE_DATA *mapmode)
{
    //Estimating number of objects we'll have to compute
    unsigned int ranged_obj=get_lgt_total_count(lvl)+get_apt_total_count(lvl)+
        get_tng_total_count(lvl)/6;
    //If this may take more than second, display information
    if (ranged_obj>96)
      popup_show("Updating object ranges for whole map","Sweeping through all objects can take some time. Please wait...");
    const int arr_entries_x=mapmode->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=mapmode->tlsize.x*MAP_SUBNUM_Y;
    clear_brighten(mapmode);
    if (!mapmode->show_obj_range) return;
    int curr_sx, curr_sy, i;
    for (curr_sx=0; curr_sx < arr_entries_x; curr_sx++)
      for (curr_sy=0; curr_sy < arr_entries_y; curr_sy++)
      {
          int last_obj=get_thing_subnums(lvl,curr_sx,curr_sy)-1;
          for (i=last_obj; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,curr_sx,curr_sy,i);
            if (is_effectgen(thing))
            {
              set_brighten_for_thing(mapmode,thing);
            }
          }
          last_obj=get_actnpt_subnums(lvl,curr_sx,curr_sy)-1;
          for (i=last_obj; i>=0; i--)
          {
            unsigned char *obj=get_actnpt(lvl,curr_sx,curr_sy,i);
            set_brighten_for_actnpt(mapmode,obj);
          }
          last_obj=get_stlight_subnums(lvl,curr_sx,curr_sy)-1;
          for (i=last_obj; i>=0; i--)
          {
            unsigned char *obj=get_stlight(lvl,curr_sx,curr_sy,i);
            set_brighten_for_stlight(mapmode,obj);
          }
      }
}

void set_brighten_for_thing(struct MAPMODE_DATA *mapmode,unsigned char *thing)
{
  if (thing==NULL) return;
  if (!mapmode->show_obj_range) return;
  set_brighten_for_range(mapmode,get_thing_subtile_x(thing),
      get_thing_subtile_y(thing),get_thing_range_adv(thing));
}

void set_brighten_for_actnpt(struct MAPMODE_DATA *mapmode,unsigned char *actnpt)
{
  if (actnpt==NULL) return;
  if (!mapmode->show_obj_range) return;
  set_brighten_for_range(mapmode,get_actnpt_subtile_x(actnpt),
      get_actnpt_subtile_y(actnpt),get_actnpt_range_adv(actnpt));
}

void set_brighten_for_stlight(struct MAPMODE_DATA *mapmode,unsigned char *stlight)
{
  if (stlight==NULL) return;
  if (!mapmode->show_obj_range) return;
  set_brighten_for_range(mapmode,get_stlight_subtile_x(stlight),
      get_stlight_subtile_y(stlight),get_stlight_range_adv(stlight));
}

void set_brighten_for_range(struct MAPMODE_DATA *mapmode,
    unsigned int pos_x,unsigned int pos_y,unsigned int rng)
{
  unsigned int til_rng=rng/MAP_SUBNUM_X+((rng%MAP_SUBNUM_X)>0);
  int tx_start,tx_end;
  tx_start=(pos_x/MAP_SUBNUM_X)-til_rng;
  tx_end=(pos_x/MAP_SUBNUM_X)+til_rng+1;
  if (tx_start<0) tx_start=0;
  if (tx_end>mapmode->tlsize.x) tx_end=mapmode->tlsize.x;
  int ty_start,ty_end;
  ty_start=(pos_y/MAP_SUBNUM_Y)-til_rng;
  ty_end=pos_y/MAP_SUBNUM_Y+til_rng+1;
  if (ty_start<0) ty_start=0;
  if (ty_end>mapmode->tlsize.y) ty_end=mapmode->tlsize.y;
  unsigned int tx,ty;
  for (tx=tx_start;tx<tx_end;tx++)
    for (ty=ty_start;ty<ty_end;ty++)
    {
      float sx=tx*MAP_SUBNUM_X+1;
      float sy=ty*MAP_SUBNUM_Y+1;
      float distance=sqrt(pow((float)pos_x-sx,2)+pow((float)pos_y-sy,2))*256.f;
      if (distance<=rng)
        mapmode->brighten[tx][ty]=true;
    }
}

void draw_eegg(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    if (mapmode->eetype!=EE_TLBIRTH)
        return;
    if (rnd(100)<90)
        return;
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();
    int row=(all_rows-20)/2 + (6-rnd(7));
    int col=(all_cols-38)/2 + (6-rnd(7));
    screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
    set_cursor_pos(row, col+12);
    screen_printf(".__--\"\"\"--__.");
    row++;
    set_cursor_pos(row,col+8);
    screen_printf("._--'");
    set_cursor_pos(row,col+24);
    screen_printf("'--_.");
    row++;
    set_cursor_pos(row,col+6);
    screen_printf(".\"");
    set_cursor_pos(row,col+29);
    screen_printf("\".");
    row++;
    set_cursor_pos(row,col+4);
    screen_printf(".\"|\"-.");
    set_cursor_pos(row,col+27);
    screen_printf(".-\"|\".");
    row++;
    set_cursor_pos(row,col+3);
    screen_printf("/");
    set_cursor_pos(row,col+7);
    screen_printf("\\");
    set_cursor_pos(row,col+10);
    screen_printf("\"-.");
    set_cursor_pos(row,col+24);
    screen_printf(".-\"");
    set_cursor_pos(row,col+29);
    screen_printf("/");
    set_cursor_pos(row,col+33);
    screen_printf("\\");
    row++;
    set_cursor_pos(row,col+2);
    screen_printf("/");
    set_cursor_pos(row,col+8);
    screen_printf("|");
    set_cursor_pos(row,col+13);
    screen_printf("'.");
    set_cursor_pos(row,col+22);
    screen_printf(".'");
    set_cursor_pos(row,col+28);
    screen_printf("|");
    set_cursor_pos(row,col+34);
    screen_printf("\\");
    row++;
    set_cursor_pos(row,col+1);
    screen_printf(".'");
    set_cursor_pos(row,col+8);
    screen_printf("|");
    set_cursor_pos(row,col+15);
    screen_printf("\"-.");
    set_cursor_pos(row,col+19);
    screen_printf(".-\"");
    set_cursor_pos(row,col+28);
    screen_printf("|");
    set_cursor_pos(row,col+34);
    screen_printf("'.");
    row++;
    set_cursor_pos(row,col+1);
    screen_printf("/");
    set_cursor_pos(row,col+9);
    screen_printf("\\");
    set_cursor_pos(row,col+16);
    screen_printf(".-v-.");
    set_cursor_pos(row,col+27);
    screen_printf("/");
    set_cursor_pos(row,col+35);
    screen_printf("\\");
    row++;
    set_cursor_pos(row,col);
    screen_printf("\"");
    set_cursor_pos(row,col+10);
    screen_printf("|");
    set_cursor_pos(row,col+14);
    screen_printf(".\"");
    set_cursor_pos(row,col+21);
    screen_printf("\".");
    set_cursor_pos(row,col+26);
    screen_printf("|");
    set_cursor_pos(row,col+36);
    screen_printf("\"");
    row++;
    set_cursor_pos(row,col);
    screen_printf("|");
    set_cursor_pos(row,col+11);
    screen_printf(")-'");
    set_cursor_pos(row,col+23);
    screen_printf("\"-(");
    set_cursor_pos(row,col+36);
    screen_printf("|");
    row++;
    set_cursor_pos(row,col);
    screen_printf("|");
    set_cursor_pos(row,col+8);
    screen_printf(".-\"");
    set_cursor_pos(row,col+12);
    screen_printf("|");
    set_cursor_pos(row,col+24);
    screen_printf("|");
    set_cursor_pos(row,col+26);
    screen_printf("\"-.");
    set_cursor_pos(row,col+36);
    screen_printf("|");
    row++;
    set_cursor_pos(row,col);
    screen_printf("'.");
    set_cursor_pos(row,col+6);
    screen_printf(".\"");
    set_cursor_pos(row,col+12);
    screen_printf("|");
    set_cursor_pos(row,col+24);
    screen_printf("|");
    set_cursor_pos(row,col+29);
    screen_printf("\".");
    set_cursor_pos(row,col+35);
    screen_printf(".'");
    row++;
    set_cursor_pos(row,col+1);
    screen_printf("\\");
    set_cursor_pos(row,col+3);
    screen_printf(".-\"");
    set_cursor_pos(row,col+13);
    screen_printf("\\");
    set_cursor_pos(row,col+23);
    screen_printf("/");
    set_cursor_pos(row,col+31);
    screen_printf("\"-.");
    set_cursor_pos(row,col+35);
    screen_printf("/");
    row++;
    set_cursor_pos(row, col+1);
    screen_printf("'=-----------+-------+-----------='");
    row++;
    set_cursor_pos(row,col+2);
    screen_printf("\\");
    set_cursor_pos(row,col+14);
    screen_printf("|");
    set_cursor_pos(row,col+22);
    screen_printf("|");
    set_cursor_pos(row,col+34);
    screen_printf("/");
    row++;
    set_cursor_pos(row,col+3);
    screen_printf("\\");
    set_cursor_pos(row,col+15);
    screen_printf("|");
    set_cursor_pos(row,col+21);
    screen_printf("|");
    set_cursor_pos(row,col+33);
    screen_printf("/");
    row++;
    set_cursor_pos(row,col+4);
    screen_printf("'.");
    set_cursor_pos(row,col+16);
    screen_printf("|");
    set_cursor_pos(row,col+20);
    screen_printf("|");
    set_cursor_pos(row,col+31);
    screen_printf(".'");
    row++;
    set_cursor_pos(row,col+6);
    screen_printf("'.");
    set_cursor_pos(row,col+16);
    screen_printf("|");
    set_cursor_pos(row,col+20);
    screen_printf("|");
    set_cursor_pos(row,col+29);
    screen_printf(".'");
    row++;
    set_cursor_pos(row,col+8);
    screen_printf("\"--__.");
    set_cursor_pos(row,col+17);
    screen_printf("\\");
    set_cursor_pos(row,col+19);
    screen_printf("/");
    set_cursor_pos(row,col+23);
    screen_printf(".__--\"");
    row++;
    set_cursor_pos(row, col+13);
    screen_printf("'\"-__V__-\"'");
    row++;
}

/*
 * Draw the whole screen; Draws bottom lines and calls proper function
 * to draw rest of the screen.
 */
void draw_levscr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" draw_levscr: starting");
    drawdata.scrmode=scrmode;
    drawdata.workdata=workdata;
    if (is_marking_enab(workdata->mapmode))
      mark_check(scrmode,workdata->mapmode);
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();
    scrmode->rows = all_rows-2;
    scrmode->cols = all_cols-scrmode->keycols;
    //If we shouldn't draw on screen - just exit.
    if (!scrmode->screen_enabled) return;
    // If we don't have room at all, just forget it!
    if (scrmode->cols < 0) return;

    set_cursor_pos(scrmode->rows, 0);
    screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
    // Hope there's enough room for it :)
    char *divline_str;
    divline_str=get_lif_name_text(workdata->lvl);
    if ((divline_str==NULL)||(divline_str[0]=='\0'))
        divline_str="Dungeon Keeper Map Editor";
    if (all_cols>70)
      screen_printf_toeol("%25s    %5.5s mode  %s", divline_str,
                 modenames[scrmode->mode], mode_status(scrmode,workdata,scrmode->mode));

    int tx=workdata->mapmode->screen.x+workdata->mapmode->map.x;
    int ty=workdata->mapmode->screen.y+workdata->mapmode->map.y;
    set_cursor_pos(all_rows-1, 0);
    if ((message_hold_get())&&(message_getcount_get()<1))
    {
        speaker_beep();
        screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
    } else
    {
        screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    }
    screen_printf_toeol(message_get());
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    message_log(" draw_levscr: executing screen-specific subfunction");
    mddraw[scrmode->mode%MODES_COUNT](scrmode,workdata);
    screen_refresh();
    message_log(" draw_levscr: finished");
}

/*
 * Draws screen for new modes which do not have real drawing function yet.
 */
void draw_mdempty(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" draw_mdempty: starting");
    int tx,ty;
    tx=workdata->mapmode->map.x+workdata->mapmode->screen.x;
    ty=workdata->mapmode->map.y+workdata->mapmode->screen.y;
    draw_map_area(scrmode,workdata->mapmode,workdata->lvl,true,false,false);
    int scr_row=0;
    int scr_col=scrmode->cols+3;
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("This mode is unfinished.");
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("No drawing function yet.");
    scr_row++;
    scr_row=display_mode_keyhelp(workdata->help,scr_row,scr_col,scrmode->rows,scrmode->mode,0);
    message_log(" draw_mdempty: finished");
}

/*
 * Draws the right panel with contents set by panel_mode.
 */
void draw_forced_panel(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata, short panel_mode)
{
  int i;
  int scr_row=0;
  int scr_col=scrmode->cols+2;
  switch (panel_mode)
  {
  case PV_COMPS:
    if (scrmode->rows >= workdata->help->compassrows+TNGDAT_ROWS)
    {
      for (i=0; i<workdata->help->compassrows; i++)
          draw_help_line(scr_row++,scr_col,workdata->help->compass[i],DHFLAG_USE_COLORS);
    } else
    {
      for (i=0; i<workdata->help->mcompassrows; i++)
          draw_help_line(scr_row++,scr_col,workdata->help->mcompass[i],DHFLAG_USE_COLORS);
    }
    display_rpanel_bottom(scrmode,workdata);
    break;
  case PV_SLB:
    draw_mdslab_panel(scrmode,workdata);
    break;
  case PV_TNG:
    draw_mdtng_panel(scrmode,workdata);
    break;
  default:
    break;
  }
}

/*
 * Sets variables for drawing marked area in marking mode.
 */
void mark_check(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    int tx=mapmode->screen.x+mapmode->map.x;
    int ty=mapmode->screen.y+mapmode->map.y;
    if (!mapmode->mark)
      return;
    if (mapmode->markp.x <= tx)
    {
      mapmode->markr.l = mapmode->markp.x;
      mapmode->markr.r = tx;
    }
    else
    {
      mapmode->markr.r = mapmode->markp.x;
      mapmode->markr.l = tx;
    }
    if (mapmode->markp.y <= ty)
    {
      mapmode->markr.t = mapmode->markp.y;
      mapmode->markr.b = ty;
    }
    else
    {
      mapmode->markr.b = mapmode->markp.y;
      mapmode->markr.t = ty;
    }
}

char *mode_status(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int mode)
{
    int tx=workdata->mapmode->screen.x+workdata->mapmode->map.x;
    int ty=workdata->mapmode->screen.y+workdata->mapmode->map.y;
    static char buffer[LINEMSG_SIZE];
    switch (mode)
    {
    case MD_CLM:
    case MD_TNG:
      sprintf (buffer, "   Position: %2d,%2d", tx,ty);
      break;
    case MD_SLB:
      sprintf (buffer, "   Position: %2d,%2d", tx,ty);
      if (is_marking_enab(workdata->mapmode))
          strcat (buffer, " (Marking)");
      else if (is_painting_enab(workdata->mapmode))
          strcat (buffer, " (Painting)");
      break;
    case MD_HELP:
      sprintf (buffer, "   (Help for %s mode)", longmodenames[workdata->help->formode]);
      break;
    case MD_CRTR:
      sprintf (buffer, "   (Selecting kind)");
      break;
    case MD_CITM:
    case MD_TXTR:
    case MD_SLBL:
    case MD_SRCH:
      sprintf (buffer, "   (Selecting type)");
      break;
    case MD_LMAP:
    case MD_SMAP:
      sprintf (buffer, "   (Selecting map)");
      break;
    case MD_GRFT:
      sprintf (buffer, "   (Entering text)");
      break;
    default:
      strcpy (buffer, "    (unknown mode)");
      break;
    case MD_SCRP:
      if ((workdata->editor->gen_flags&SGF_ENABLED)==SGF_ENABLED)
        sprintf (buffer, "   (Script generator)");
      else
        sprintf (buffer, "   Text Line: %4d", workdata->editor->y+1);
      break;
    }
    return buffer;
}

/*
 * Returns color in which the specified tile should be drawn.
 * The color depends on tile owner, but also marking mode.
 */
int get_draw_map_tile_color(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int tx,int ty,short special,short darken_fg,short brighten_bg)
{
    int g;
    if ((tx<0)||(tx>=lvl->tlsize.x)) return PRINT_COLOR_GREY_ON_BLACK;
    if ((ty<0)||(ty>=lvl->tlsize.y)) return PRINT_COLOR_GREY_ON_BLACK;
    //If highlighted - nothing more matters
    int hilight=get_tile_highlight(mapmode,tx,ty);
    if (hilight>0) return hilight;
    int own=get_tile_owner(lvl,tx,ty);
    short marked=((is_marking_enab(mapmode)) && (tx>=mapmode->markr.l) && (tx<=mapmode->markr.r)
                        && (ty>=mapmode->markr.t) && (ty<=mapmode->markr.b));
    int col;
    if (special)
    {
      if (marked)
        return PRINT_COLOR_MAGENT_ON_LGREY;
      else
      {
        switch (own)
        {
        case PLAYER0:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_LRED;
          else
            return PRINT_COLOR_LMAGENT_ON_RED;
        case PLAYER1:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_LBLUE;
          else
            return PRINT_COLOR_LMAGENT_ON_BLUE;
        case PLAYER2:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_LGREEN;
          else
            return PRINT_COLOR_LMAGENT_ON_GREEN;
        case PLAYER3:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_YELLOW;
          else
            return PRINT_COLOR_LMAGENT_ON_BROWN;
        case PLAYER_GOOD:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_LCYAN;
          else
            return PRINT_COLOR_LMAGENT_ON_CYAN;
        default:
        case PLAYER_UNSET:
          if (brighten_bg)
            return PRINT_COLOR_LMAGENT_ON_GREY;
          else
            return PRINT_COLOR_LMAGENT_ON_BLACK;
        }
      }
    } else
    {
      return get_screen_color_owned(own,marked,darken_fg,brighten_bg);
    }
}

int get_screen_color_owned(unsigned char owner,short marked,short darken_fg,short brighten_bg)
{

    if (marked)
    {
        switch (owner)
        {
        case PLAYER0:return PRINT_COLOR_RED_ON_WHITE;
        case PLAYER1:return PRINT_COLOR_BLUE_ON_WHITE;
        case PLAYER2:return PRINT_COLOR_GREEN_ON_WHITE;
        case PLAYER3:return PRINT_COLOR_BROWN_ON_WHITE;
        case PLAYER_GOOD:return PRINT_COLOR_CYAN_ON_WHITE;
        default:
        case PLAYER_UNSET:return PRINT_COLOR_BLACK_ON_LGREY;
        }
    } else
    {
        switch (owner)
        {
        case PLAYER0:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_LRED;
            else
              return PRINT_COLOR_WHITE_ON_LRED;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_RED;
            else
              return PRINT_COLOR_WHITE_ON_RED;
          }
        case PLAYER1:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_LBLUE;
            else
              return PRINT_COLOR_WHITE_ON_LBLUE;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_BLUE;
            else
              return PRINT_COLOR_WHITE_ON_BLUE;
          }
        case PLAYER2:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_LGREEN;
            else
              return PRINT_COLOR_WHITE_ON_LGREEN;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_GREEN;
            else
              return PRINT_COLOR_WHITE_ON_GREEN;
          }
        case PLAYER3:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_YELLOW;
            else
              return PRINT_COLOR_WHITE_ON_YELLOW;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_BROWN;
            else
              return PRINT_COLOR_WHITE_ON_BROWN;
          }
        case PLAYER_GOOD:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_LCYAN;
            else
              return PRINT_COLOR_WHITE_ON_LCYAN;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_CYAN;
            else
              return PRINT_COLOR_WHITE_ON_CYAN;
          }
        default:
        case PLAYER_UNSET:
          if (brighten_bg)
          {
            if (darken_fg)
              return PRINT_COLOR_LGREY_ON_GREY;
            else
              return PRINT_COLOR_WHITE_ON_GREY;
          } else
          {
            if (darken_fg)
              return PRINT_COLOR_GREY_ON_BLACK;
            else
              return PRINT_COLOR_LGREY_ON_BLACK;
          }
        }
    }

}

/*
 * Reduces slab to basic type, for displaying map without rooms
 */
unsigned char simplify_map_slab(unsigned short slab)
{
  if (slab==SLAB_TYPE_ROCK) return SLAB_TYPE_ROCK;
  if (slab_is_tall(slab)) return SLAB_TYPE_EARTH;
  return SLAB_TYPE_CLAIMED;
}

/*
 * Returns a character that should de drawn on map in current mode.
 */
int get_draw_map_tile_char(const struct MAPMODE_DATA *mapmode,const struct LEVEL *lvl,
    int tx,int ty,short show_ground,short show_rooms,short show_things,short force_at)
{
    char out_ch;
    unsigned short slab;
    if (show_things)
      out_ch=get_thing_char(lvl,tx,ty);
    else
      out_ch=' ';
    if ((out_ch==' ')&&((show_ground)||(show_rooms)))
    {
      slab=get_tile_slab(lvl,tx,ty);
      if (!show_rooms) slab=simplify_map_slab(slab);
      if ((force_at)||(mapmode->slbkey==NULL))
        out_ch='@';
      else
        out_ch=mapmode->slbkey[slab];
    }
    return out_ch;
}

/*
 * Draws the map area for all modes that has map display.
 * Also clears the right panel.
 */
void draw_map_area(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,short show_ground,short show_rooms,short show_things)
{
    int i, k;
    for (k=0; k<scrmode->rows; k++)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(k,0);
      int ty=mapmode->map.y+k;
      if (ty >= mapmode->tlsize.y)
      {
          for (i=0; i<scrmode->cols; i++)
            screen_printchr(' ');
      }
      else
      {
          for (i=0; i<scrmode->cols; i++)
          {
            int tx=mapmode->map.x+i;
            if (tx < lvl->tlsize.x)
            {
              char out_ch;
              int g;
              short has_ccol;
              short darken_fg;
              short brighten_bg;
              if (show_rooms)
              {
                  g = graffiti_idx(lvl,tx,ty);
                  has_ccol = slab_has_custom_columns(lvl,tx,ty);
                  unsigned short slab=get_tile_slab(lvl,tx,ty);
                  darken_fg=(slab==SLAB_TYPE_ROCK)||(slab==SLAB_TYPE_LAVA);
              } else
              {
                  g = -1;
                  has_ccol = false;
                  darken_fg=(get_object_tilnums(lvl,tx,ty)==0);
              }
              if (show_things)
              {
                  brighten_bg=get_tile_brighten(mapmode,tx,ty);
              } else
              {
                  brighten_bg=false;
              }
              screen_setcolor(get_draw_map_tile_color(scrmode,mapmode,lvl,tx,ty,has_ccol,darken_fg,brighten_bg));
              out_ch=get_draw_map_tile_char(mapmode,lvl,tx,ty,show_ground,show_rooms,show_things,(g>=0));
              screen_printchr(out_ch);
            } else
            {
              screen_printchr(' ');
            }
          }
      }
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
    }
    draw_eegg(scrmode,mapmode);
}

/*
 * Shows cursor on the map screen.
 */
void draw_map_cursor(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,short show_ground,short show_rooms,short show_things)
{
    int tx=mapmode->screen.x+mapmode->map.x;
    int ty=mapmode->screen.y+mapmode->map.y;
    char out_ch;
    int g;
    if (show_rooms)
      g = graffiti_idx(lvl,tx,ty);
    else
      g = -1;
    out_ch=get_draw_map_tile_char(mapmode,lvl,tx,ty,show_ground,show_rooms,show_things,(g>=0));
    show_cursor(scrmode,mapmode,out_ch);
}

/*
 * Shows map cursor at position from mapmode; the cursor is a character
 * same as in the current map background written in different colors.
 */
void show_cursor(const struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,char cur)
{
    set_cursor_pos(mapmode->screen.y, mapmode->screen.x);
    screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
    screen_printchr(cur);
    set_cursor_visibility(false);
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

/*
 * Shows map cursor at given position; the cursor is a character
 * from the parameter written in highlighted colors.
 */
void show_cursor_at(const struct SCRMODE_DATA *scrmode,int row,int col,char cur)
{
    set_cursor_pos(row,col);
    screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
    screen_printchr(cur);
    set_cursor_visibility(false);
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

/*
 * Used when the screen should be drawn without any cursor
 * (ie. if a whole string, like button, is selected)
 */
void show_no_cursor()
{
    set_cursor_visibility(false);
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

void draw_rpanel_usrinput(const struct SCRMODE_DATA *scrmode,const char *inp_message)
{
    int scr_col;
    int scr_row;
    // Draw the user input
    scr_col=scrmode->cols+3;
    scr_row=scrmode->rows-USRINPUT_ROWS;
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row, scr_col);
    screen_printf("%s",inp_message);
    scr_row+=2;
    set_cursor_pos(scr_row, scr_col);
    screen_printf("%s",scrmode->usrinput);
    if ( (scrmode->usrinput_pos>=0) && (scrmode->usrinput_pos<scrmode->keycols) )
    {
      char chr=scrmode->usrinput[scrmode->usrinput_pos];
      if ((chr<32)||(chr>126)) chr=' ';
      show_cursor_at(scrmode, scr_row, scr_col+scrmode->usrinput_pos, chr);
    } else
    {
      show_no_cursor();
    }
}

/*
 * Draws bottom part of the right panel. This contains DAT/TNG entries,
 * or string input field.
 */
void display_rpanel_bottom(const struct SCRMODE_DATA *scrmode,const struct WORKMODE_DATA *workdata)
{
    int tx, ty;
    tx = workdata->mapmode->screen.x+workdata->mapmode->map.x;
    ty = workdata->mapmode->screen.y+workdata->mapmode->map.y;
    if (scrmode->usrinput_type==SI_NONE)
    {
      short compressed;
      int scr_row;
      int tngdat_rows_min=(TNGDAT_ROWS>>1)+(TNGDAT_ROWS%1);
      if (scrmode->rows > TNGDAT_ROWS)
      {
        compressed=false;
        scr_row=scrmode->rows-TNGDAT_ROWS;
      } else
      {
        compressed=true;
        scr_row=scrmode->rows-tngdat_rows_min;
      }
      if (scrmode->rows >= tngdat_rows_min)
      {
        int scr_col=scrmode->cols+3;
        if (workdata->ipanel->dat_view_mode!=0)
        {
          display_dat_subtiles(scrmode,workdata,scr_row,scr_col,compressed,ty,tx);
          scr_col+=17;
        }
        display_tng_subtiles(scrmode,workdata,workdata->lvl,scr_row,scr_col,compressed,ty,tx);
      }
      show_no_cursor();
    } else
    {
      draw_rpanel_usrinput(scrmode,string_input_msg[scrmode->usrinput_type]);
    }
}
    
/*
 * Displays key help for given mode. Key help is visible on right
 * panel in some work modes.
 */
int display_mode_keyhelp(struct HELP_DATA *help, int scr_row, int scr_col,
    int max_row,int mode,int itm_idx)
{
    int i;
    if (init_key_help(help,mode) && (help->rows<=(max_row-scr_row)))
    {
      for (i=0; i<help->rows; i++)
          draw_help_line(scr_row++,scr_col,help->text[i],DHFLAG_USE_COLORS);
    }
    if (init_item_desc(help,mode,itm_idx) && (help->drows<=(max_row-scr_row)))
    {
      for (i=0; i<help->drows; i++)
          draw_help_line(scr_row++,scr_col,help->desc[i],DHFLAG_USE_COLORS);
    }
    return scr_row;
}

/*
 * Changes the work mode. Reinits mode even if the new one is same as old.
 */
int change_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int new_mode)
{
  short prevmode=scrmode->mode;
  mdend[prevmode%MODES_COUNT](scrmode,workdata);
  short result=mdstart[new_mode%MODES_COUNT](scrmode,workdata);
  if (!result)
      mdstart[prevmode%MODES_COUNT](scrmode,workdata);
  return result;
}

/*
 * Returns if the work mode is "simple". You can't save, load or exit
 * directly from "simple" modes.
 */
short is_simple_mode(int mode)
{
    if ((mode==MD_SLB)  || (mode==MD_TNG)  ||
        (mode==MD_CLM)  || (mode==MD_SCRP) ||
        (mode==MD_RWRK) )
      return false;
    return true;
}

/*
 * Main action function - gets and processes a key stroke.
 */
void proc_key(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" proc_key: starting");
    unsigned int g;
    if (scrmode->automated_commands[0]!=0)
    {
      g=scrmode->automated_commands[0];
      memmove(scrmode->automated_commands,scrmode->automated_commands+1,(READ_BUFSIZE-1)*sizeof(unsigned int));
    } else
    {
      if (!scrmode->input_enabled)
      {
        finished=true;
        return;
      }
      g = get_key();
    }
    // Decoding "universal keys" - global actions
    // which should work in every screen
    //Performing actions, or sending the keycode elswhere
    message_log(" proc_key: got keycode %u",g);
    switch (g)
    {
    case KEY_F1:
      action_enter_help_mode(scrmode,workdata);
      break;
    case KEY_CTRL_Q:
      action_quit_program(scrmode,workdata);
      break;
    case KEY_CTRL_L:
      action_enter_mapload_mode(scrmode,workdata);
      break;
    case KEY_F7:
      action_load_map_quick(scrmode,workdata);
      break;
    case KEY_CTRL_S:
      action_enter_mapsave_mode(scrmode,workdata);
      break;
    case KEY_F5:
      action_save_map_quick(scrmode,workdata);
      break;
    case KEY_CTRL_N:
      action_create_new_map(scrmode,workdata);
      break;
    case KEY_CTRL_R:
      action_create_random_backgnd(scrmode,workdata);
      break;
    case KEY_CTRL_P:
      action_toggle_compass_rose(scrmode,workdata);
      break;
    case KEY_CTRL_U:
      action_toggle_datclm_aupdate(scrmode,workdata);
      break;
    case KEY_CTRL_T:
      action_enter_script_mode(scrmode,workdata);
      break;
    case KEY_CTRL_F:
      action_enter_search_mode(scrmode,workdata);
      break;
    case KEY_CTRL_B:
      action_generate_bitmap(scrmode,workdata);
      break;
    case KEY_CTRL_E:
      action_enter_texture_mode(scrmode,workdata);
      break;

    default:
      {
        message_log(" proc_key: executing screen-specific subfunction");
        //Sending the action to a function corresponding to actual screen
        actions[scrmode->mode%MODES_COUNT](scrmode,workdata,g);
      };break;
    }
    inc_info_usr_cmds_count(workdata->lvl);
    message_log(" proc_key: finished");
}

/*
 * Action function - covers cursor actions from non-help screens.
 */
short cursor_actions(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int osx, osy, omx, omy;
    int i, j;
    struct IPOINT_2D *screen=&(workdata->mapmode->screen);
    struct IPOINT_2D *map=&(workdata->mapmode->map);
    osx=screen->x;
    osy=screen->y;
    omx=map->x;
    omy=map->y;
    
    switch (key)
    {
    case KEY_CTRL_UP: // ctrl+arrow up
      screen->y-=5;
      message_log(" cursor_actions: KEY_CTRL_UP");
      break;
    case KEY_CTRL_DOWN: // ctrl+arrow down
      screen->y+=5;
      message_log(" cursor_actions: KEY_CTRL_DOWN");
      break;
    case KEY_CTRL_LEFT: // ctrl+arrow left
      screen->x-=5;
      message_log(" cursor_actions: KEY_CTRL_LEFT");
      break;
    case KEY_CTRL_RIGHT: // ctrl+arrow right
      screen->x+=5;
      message_log(" cursor_actions: KEY_CTRL_RIGHT");
      break;
    case KEY_ALT_UP: // alt+arrow up
      screen->y-=15;
      message_log(" cursor_actions: KEY_ALT_UP");
      break;
    case KEY_ALT_DOWN: // alt+arrow down
      screen->y+=15;
      message_log(" cursor_actions: KEY_ALT_DOWN");
      break;
    case KEY_ALT_LEFT: // alt+arrow left
      screen->x-=15;
      message_log(" cursor_actions: KEY_ALT_LEFT");
      break;
    case KEY_ALT_RIGHT: // alt+arrow right
      screen->x+=15;
      message_log(" cursor_actions: KEY_ALT_RIGHT");
      break;
    case KEY_CTRL_HOME: // ctrl+home
      map->x=0;
      screen->x=0;
      message_log(" cursor_actions: KEY_CTRL_HOME");
      break;
    case KEY_CTRL_END: // ctrl+end
      map->x=workdata->lvl->tlsize.x;
      screen->x=workdata->lvl->tlsize.x;
      message_log(" cursor_actions: KEY_CTRL_END");
      break;
    case KEY_CTRL_PGUP: // ctrl+page up
      map->y=0;
      screen->y=0;
      message_log(" cursor_actions: KEY_CTRL_PGUP");
      break;
    case KEY_CTRL_PGDOWN: // ctrl+page down
      map->y=workdata->lvl->tlsize.y;
      screen->y=workdata->lvl->tlsize.y;
      message_log(" cursor_actions: KEY_CTRL_PGDOWN");
      break;
    case KEY_UP:
      screen->y--;
      break;
    case KEY_DOWN:
      screen->y++;
      break;
    case KEY_RIGHT:
      screen->x++;
      break;
    case KEY_LEFT:
      screen->x--;
      break;
    case KEY_END:
      screen->x+=10;
      break;
    case KEY_HOME:
      screen->x-=10;
      break;
    case KEY_PGUP:
      screen->y-=10;
      break;
    case KEY_PGDOWN:
      screen->y+=10;
      break;
      default:
      return false;
    }
    curposcheck(scrmode,workdata->mapmode);
    int tx=screen->x+map->x;
    int ty=screen->y+map->y;
    if ((omx+osx != tx) || (omy+osy != ty))
    {
      // As we've moved, reset which object we're looking at
      change_visited_tile(workdata);
      message_log(" cursor_actions: map pos is now (%d,%d)",tx,ty);
    }
    return true;
}

/*
 * Action function - allows selecting subtiles with digits from numpad.
 */
short subtl_select_actions(struct MAPMODE_DATA *mapmode,int key)
{
      switch (key)
      {
        case KEY_NUM1:
          mapmode->subtl.x=0;
          mapmode->subtl.y=2;
          break;
        case KEY_NUM2:
          mapmode->subtl.x=1;
          mapmode->subtl.y=2;
          break;
        case KEY_NUM3:
          mapmode->subtl.x=2;
          mapmode->subtl.y=2;
          break;
        case KEY_NUM4:
          mapmode->subtl.x=0;
          mapmode->subtl.y=1;
          break;
        case KEY_NUM5:
          mapmode->subtl.x=1;
          mapmode->subtl.y=1;
          break;
        case KEY_NUM6:
          mapmode->subtl.x=2;
          mapmode->subtl.y=1;
          break;
        case KEY_NUM7:
          mapmode->subtl.x=0;
          mapmode->subtl.y=0;
          break;
        case KEY_NUM8:
          mapmode->subtl.x=1;
          mapmode->subtl.y=0;
          break;
        case KEY_NUM9:
          mapmode->subtl.x=2;
          mapmode->subtl.y=0;
          break;
      default:
      return false;
    }
    message_log(" subtl_select_actions: active subtile is now (%d,%d)",
            (int)(mapmode->subtl.x),(int)(mapmode->subtl.y));
    return true;
}

/*
 * Action function - allows reading a string from keyboard.
 */
short string_get_actions(struct SCRMODE_DATA *scrmode,int key,short *text_changed)
{
    unsigned int maxlen = scrmode->keycols-4;  // limit length
    unsigned int len = strlen(scrmode->usrinput);
    if (scrmode->usrinput_pos>len)
      scrmode->usrinput_pos=len;
    if (scrmode->usrinput_pos<0)
      scrmode->usrinput_pos=0;
    if ((key>=32) && (key<=126))
    {
        if (len < maxlen)
        {
            int i;
            for (i=len-1;i>=scrmode->usrinput_pos;i--)
            {
                scrmode->usrinput[i+1]=scrmode->usrinput[i];
            }
            scrmode->usrinput[scrmode->usrinput_pos] = key;
            scrmode->usrinput_pos++;
            len++;
            scrmode->usrinput[len] = '\0';
            (*text_changed)=true;
        } else
        {
            (*text_changed)=false;
            speaker_beep();
        }
    } else
    switch (key)
    {
    case KEY_CTRL_U:             // ^U kill line
        len=0;
        scrmode->usrinput[0] = '\0';
        scrmode->usrinput_pos=0;
        (*text_changed)=true;
        break;
    case KEY_DEL:
        if (len>0)
        {
            int i;
            for (i=scrmode->usrinput_pos+1;i<len;i++)
            {
                scrmode->usrinput[i-1]=scrmode->usrinput[i];
            }
            len--;
            scrmode->usrinput[len] = '\0';
            (*text_changed)=true;
        } else
        {
            speaker_beep();
            (*text_changed)=false;
        }
        break;
    case KEY_BACKSP:
        if (len>0)
        {
            int i=scrmode->usrinput_pos;
            if (i<1) i=1;
            while (i<len)
            {
                scrmode->usrinput[i-1]=scrmode->usrinput[i];
                i++;
            }
            if (scrmode->usrinput_pos>0)
              scrmode->usrinput_pos--;
            len--;
            scrmode->usrinput[len] = '\0';
            (*text_changed)=true;
        } else
        {
            speaker_beep();
            (*text_changed)=false;
        }
        break;
    case KEY_RIGHT:
      if (scrmode->usrinput_pos<len)
        scrmode->usrinput_pos++;
      (*text_changed)=false;
      break;
    case KEY_LEFT:
      if (scrmode->usrinput_pos>0)
        scrmode->usrinput_pos--;
      (*text_changed)=false;
      break;
    default:
      (*text_changed)=false;
      return false;
    }
    message_log(" string_get_actions: action performed");
    return true;
}


/*
 * Action subfunction - check cursor position.
 */
void curposcheck(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    if (mapmode->screen.x < 0)
    {
      mapmode->map.x+=mapmode->screen.x;
      mapmode->screen.x=0;
    }
    if (mapmode->screen.y < 0)
    {
      mapmode->map.y+=mapmode->screen.y;
      mapmode->screen.y=0;
    }
    if (mapmode->screen.y >= scrmode->rows)
    {
      mapmode->map.y+=mapmode->screen.y-(scrmode->rows-1);
      mapmode->screen.y=scrmode->rows-1;
    }
    if (mapmode->screen.x >= scrmode->cols)
    {
      mapmode->map.x+=mapmode->screen.x-(scrmode->cols-1);
      mapmode->screen.x = scrmode->cols-1;
    }
    if (mapmode->map.x+scrmode->cols > mapmode->tlsize.x)
      mapmode->map.x = mapmode->tlsize.x-scrmode->cols;
    if (mapmode->map.y+scrmode->rows > mapmode->tlsize.y)
      mapmode->map.y = mapmode->tlsize.y-scrmode->rows;
    if (mapmode->map.x < 0)
      mapmode->map.x=0;
    if (mapmode->map.y < 0)
      mapmode->map.y=0;
}

/*
 * Returns current map subtile in 'subpos'.
 */
short get_map_subtile_pos(struct MAPMODE_DATA *mapmode,struct IPOINT_2D *subpos)
{
    if ((mapmode==NULL)||(subpos==NULL))
        return false;
    struct IPOINT_2D *screen=&(mapmode->screen);
    struct IPOINT_2D *map=&(mapmode->map);
    struct IPOINT_2D *subtl=&(mapmode->subtl);
    subpos->x = (map->x+screen->x)*MAP_SUBNUM_X+subtl->x;
    subpos->y = (map->y+screen->y)*MAP_SUBNUM_Y+subtl->y;
    return true;
}

/*
 * Returns current map tile in 'tilpos'.
 */
short get_map_tile_pos(struct MAPMODE_DATA *mapmode,struct IPOINT_2D *tilpos)
{
    if ((mapmode==NULL)||(tilpos==NULL))
        return false;
    struct IPOINT_2D *screen=&(mapmode->screen);
    struct IPOINT_2D *map=&(mapmode->map);
    tilpos->x = (map->x+screen->x);
    tilpos->y = (map->y+screen->y);
    return true;
}

void action_enter_help_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
      if (scrmode->mode != MD_HELP)
        mdstart[MD_HELP](scrmode,workdata);
}

void action_quit_program(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
          message_info("You can't quit from here.");
          return;
    }
    message_log(" proc_key: setting finished to true");
    //TODO: maybe we should ask to save unsaved data?
    finished=true;
//    actions[scrmode->mode%MODES_COUNT](KEY_CTRL_Q); // Grotty but it'll work
}

void action_enter_mapload_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't load from here.");
        return;
    }
    mdstart[MD_LMAP](scrmode,workdata);
}

void action_load_map_quick(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't load from here.");
        return;
    }
    if (strlen(get_lvl_fname(workdata->lvl))>0)
    {
          popup_show("Reloading map","Reading map files. Please wait...");
          free_map(workdata->lvl);
          set_lvl_savfname(workdata->lvl,"");
          user_load_map(workdata->lvl,true);
          clear_highlight(workdata->mapmode);
          workdata->mdtng->obj_ranges_changed=true;
          change_mode(scrmode,workdata,scrmode->mode);
          message_info("Map \"%s\" reloaded", get_lvl_fname(workdata->lvl));
    } else
          message_error("Map name is empty, cannot load last loaded.");
}

void action_enter_mapsave_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't save from here.");
        return;
    }
    mdstart[MD_SMAP](scrmode,workdata);
}

void action_save_map_quick(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't save from here.");
        return;
    }
    if (strlen(get_lvl_savfname(workdata->lvl))>0)
    {
        popup_show("Saving map","Writing map files. Please wait...");
        user_save_map(workdata->lvl,0);
        message_info("Map \"%s\" saved", get_lvl_savfname(workdata->lvl));
    } else
    {
          message_error("Map name is empty, please save as.");
    }
}

void action_create_new_map(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't clear map from here.");
        return;
    }
    popup_show("Clearing map","Generating empty map. Please wait...");
    free_map(workdata->lvl);
    start_new_map(workdata->lvl);
    clear_highlight(workdata->mapmode);
    workdata->mdtng->obj_ranges_changed=true;
    change_mode(scrmode,workdata,scrmode->mode);
    message_info_force("New map started");
}

void action_create_random_backgnd(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't randomize map from here.");
        return;
    }
    popup_show("Randomizing map","Generating random map. Please wait...");
    free_map(workdata->lvl);
    generate_random_map(workdata->lvl);
    clear_highlight(workdata->mapmode);
    workdata->mdtng->obj_ranges_changed=true;
    change_mode(scrmode,workdata,scrmode->mode);
    message_info_force("Map generation completed");
}


void action_toggle_compass_rose(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("Go back to map view first.");
        return;
    }
    if (workdata->ipanel->mode!=PV_COMPS)
        workdata->ipanel->mode=PV_COMPS;
    else
        workdata->ipanel->mode=PV_MODE;
    message_info_force("Compass rose mode %s",(workdata->ipanel->mode==PV_COMPS)?"enabled":"disabled");
}

void action_toggle_datclm_aupdate(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    short nval=switch_datclm_auto_update(workdata->lvl);
    if (nval)
        message_info_force("Automatic update of DAT/CLM/WIB enabled");
    else
        message_info_force("Auto DAT/CLM/WIB update disabled - manual with \"u\"");
}

void action_enter_script_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't view script from here.");
    } else
    if (scrmode->mode==MD_SCRP)
    {
        message_info("Entering script generator.");
        start_scrptgen(scrmode,workdata);
    } else
    {
        change_mode(scrmode,workdata,MD_SCRP);
    }
}

void action_enter_search_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't search from this mode.");
    } else
    {
        change_mode(scrmode,workdata,MD_SRCH);
    }
}

void action_generate_bitmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't generate BMP from this mode.");
    } else
    {
        popup_show("Generating bitmap","Creating and writing BMP file. Please wait...");
        save_nfo_file(workdata->lvl);
        generate_map_bitmap_mapfname(workdata->lvl);
    }
}

void action_enter_texture_mode(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (is_simple_mode(scrmode->mode))
    {
        message_info("You can't change texture from here.");
    } else
    {
        mdstart[MD_TXTR](scrmode,workdata);
    }
}

/*
 * Gets index of the item from clipboard which is object.
 * Objects are action points, lights and things.
 * If cannot find, returns NULL.
 */
struct CLIPBOARD *get_clipboard_object(struct SCRMODE_DATA *scrmode,int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        int dtype=scrmode->clipbrd[i].dtype;
        if ((dtype==OBJECT_TYPE_ACTNPT)||(dtype==OBJECT_TYPE_THING)||
            (dtype==OBJECT_TYPE_STLIGHT))
          cidx++;
        if (cidx==idx)
          return &(scrmode->clipbrd[i]);
    }
    return NULL;
}

short is_painting_enab(struct MAPMODE_DATA *mapmode)
{
    return (mapmode->paintmode&PNTMD_ENAB==PNTMD_ENAB);
}

short is_painting_slab(struct MAPMODE_DATA *mapmode)
{
    if ((mapmode->paintmode&PNTMD_ENAB)!=PNTMD_ENAB)
      return false;
    if ((mapmode->paintmode&PNTMD_SLAB)==PNTMD_SLAB)
        return true;
    if ((mapmode->paintmode&PNTMD_RNDWALL)==PNTMD_RNDWALL)
        return true;
    return false;
}

short is_painting_ownr(struct MAPMODE_DATA *mapmode)
{
    const short cmpr=PNTMD_ENAB|PNTMD_OWNR;
    return ((mapmode->paintmode&cmpr)==cmpr);
}

unsigned short get_painting_slab(struct MAPMODE_DATA *mapmode)
{
    if ((mapmode->paintmode&PNTMD_RNDWALL)==PNTMD_RNDWALL)
        return get_random_wall_slab();
    return mapmode->paintroom;
}

unsigned char get_painting_ownr(struct MAPMODE_DATA *mapmode)
{
    return mapmode->paintownr;
}

void set_painting_enab(struct MAPMODE_DATA *mapmode)
{
    mapmode->paintmode=PNTMD_ENAB;
    mapmode->mark=false;
}

void set_painting_slab(struct MAPMODE_DATA *mapmode,const unsigned short slab)
{
    mapmode->paintmode&=(~PNTMD_RNDWALL);
    mapmode->paintmode|=(PNTMD_ENAB|PNTMD_SLAB);
    mapmode->paintroom=slab;
    mapmode->mark=false;
}

void set_painting_ownr(struct MAPMODE_DATA *mapmode,const unsigned char owner)
{
    mapmode->paintmode|=(PNTMD_ENAB|PNTMD_OWNR);
    mapmode->paintownr=owner;
    mapmode->mark=false;
}

void set_painting_rndwall(struct MAPMODE_DATA *mapmode)
{
    mapmode->paintmode&=(~PNTMD_SLAB);
    mapmode->paintmode|=(PNTMD_ENAB|PNTMD_RNDWALL);
    mapmode->mark=false;
}

void set_painting_disab(struct MAPMODE_DATA *mapmode)
{
    mapmode->paintmode=PNTMD_NONE;
}

short is_marking_enab(struct MAPMODE_DATA *mapmode)
{
    return (mapmode->mark);
}

void set_marking_disab(struct MAPMODE_DATA *mapmode)
{
    mapmode->mark=false;
}

void set_marking_start(struct MAPMODE_DATA *mapmode,int x,int y)
{
    mapmode->mark=true;
    mapmode->markp.x=x;
    mapmode->markp.y=y;
    mapmode->paintmode=PNTMD_NONE;
}

/*
 * Gets the slab item from clipboard.
 */
unsigned char *get_clipboard_slab(struct SCRMODE_DATA *scrmode,int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_SLAB)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Gets the DAT list item from clipboard.
 */
unsigned char *get_clipboard_datlst(struct SCRMODE_DATA *scrmode,int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_DATLST)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Gets the column item from clipboard.
 */
unsigned char *get_clipboard_column(struct SCRMODE_DATA *scrmode,int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_COLUMN)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Adds any object/slab/datlist/column to the clipboard,
 * pasting the given pointer as clipboard item.
 * Returns index of new item.
 */
int add_clipboard_any(struct SCRMODE_DATA *scrmode,char *obj,int obj_type)
{
    if (obj==NULL) return 0;
    int obj_idx=scrmode->clip_count;
    scrmode->clipbrd=realloc(scrmode->clipbrd,(obj_idx+1)*sizeof(struct CLIPBOARD));
    if (scrmode->clipbrd==NULL)
        die("add_clipboard_any: Cannot allocate clipboard memory");
    scrmode->clip_count=obj_idx+1;
    scrmode->clipbrd[obj_idx].data=obj;
    scrmode->clipbrd[obj_idx].dtype=obj_type;
    return obj_idx;
}

/*
 * Replaces clipboard with given object/slab/datlist/column,
 * pasting the given pointer as clipboard item.
 * Returns index of new item, which is zero.
 */
int put_clipboard_any(struct SCRMODE_DATA *scrmode,char *obj,int obj_type)
{
    clear_clipboard(scrmode);
    return add_clipboard_any(scrmode,obj,obj_type);
}

/*
 * Clears all object from clipboard (frees the memory).
 */
void clear_clipboard(struct SCRMODE_DATA *scrmode)
{
     int i;
     for (i=scrmode->clip_count-1;i>=0;i--)
         free(scrmode->clipbrd[i].data);
     scrmode->clip_count=0;
     free(scrmode->clipbrd);
     scrmode->clipbrd=NULL;
    message_log(" clear_clipboard: done");
}

/*
 * Copies thing to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_thing(struct SCRMODE_DATA *scrmode,unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_TNG_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_TNG_REC);
    return put_clipboard_any(scrmode,obj_cp,OBJECT_TYPE_THING);
}

/*
 * Copies action point to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_actnpt(struct SCRMODE_DATA *scrmode,unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_APT_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_APT_REC);
    return put_clipboard_any(scrmode,obj_cp,OBJECT_TYPE_ACTNPT);
}

/*
 * Copies static light to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_stlight(struct SCRMODE_DATA *scrmode,unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_LGT_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_LGT_REC);
    return put_clipboard_any(scrmode,obj_cp,OBJECT_TYPE_STLIGHT);
}


short level_verify_with_highlight(struct WORKMODE_DATA *workdata)
{
    message_log(" level_verify_with_highlight: starting");
    struct IPOINT_2D errpt={-1,-1};
    short result;
    clear_highlight(workdata->mapmode);
    result=level_verify(workdata->lvl,NULL,&errpt);
    if ((errpt.x>=0) && (errpt.y>=0))
      set_tile_highlight(workdata->mapmode, errpt.x, errpt.y, PRINT_COLOR_LRED_ON_YELLOW);
    message_log(" level_verify_with_highlight: finished");
    return result;
}
