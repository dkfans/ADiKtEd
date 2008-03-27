/*
 * scr_slab.c
 *
 * Defines functions for initializing and displaying the slab screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_slab.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "lev_data.h"
#include "scr_help.h"
#include "obj_things.h"
#include "obj_slabs.h"
#include "graffiti.h"
#include "scr_thing.h"


// Variables

struct MDSLAB_DATA *mdslab;

//Slab keys
char *slbkey;

/*
 * Initializes variables for the mdslab screen.
 */
short init_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    //Creating mdslab variable
    mdslab=(struct MDSLAB_DATA *)malloc(sizeof(struct MDSLAB_DATA));
    if (mdslab==NULL)
     die("init_mdslab: Cannot allocate memory.");
    //Initialize keys for displaying and putting slabs
    init_mdslab_keys(scrmode,mapmode);
    return true;
}

/*
 * Deallocates memory for the mdslab screen.
 */
void free_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
  free(mdslab);
  free_mdslab_keys(scrmode,mapmode);
}

/*
 * Covers actions from the slab screen.
 */
void actions_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key)
{
    int d;
    char *msg;
    message_release();
    if (!cursor_actions(scrmode,mapmode,lvl,key))
    {
        int tx=mapmode->map.x+mapmode->screen.x;
        int ty=mapmode->map.y+mapmode->screen.y;
        switch (key)
        {
        case KEY_ENTER:
          mdstart[MD_SLBL](scrmode,mapmode,lvl);
          break;
        case KEY_U: // Update all things/dat/clm/w?b
          update_slab_owners(lvl);
          update_datclm_for_whole_map(lvl);
          message_info("All DAT/CLM/W?B entries updated.");
          break;
        case KEY_V: // Verify whole map
          level_verify_with_highlight(lvl,mapmode);
          break;
        case KEY_TAB:
          end_mdslab(scrmode,mapmode,lvl);
          mdstart[MD_TNG](scrmode,mapmode,lvl);
          message_info("Thing mode activated");
          break;
        case KEY_C:
          end_mdslab(scrmode,mapmode,lvl);
          mdstart[MD_CLM](scrmode,mapmode,lvl);
          message_info("Column mode activated");
          break;
        case KEY_R:
          end_mdslab(scrmode,mapmode,lvl);
          mdstart[MD_RWRK](scrmode,mapmode,lvl);
          message_info("Rework mode activated");
          break;
        case KEY_CTRL_X:
          msg=malloc(LINEMSG_SIZE*sizeof(char));
          level_generate_random_extension(lvl,msg);
          message_info("Random extension: %s",msg);
          free(msg);
          break;
        case KEY_CTRL_SPACE:
          if (mapmode->mark)
          {
            mapmode->mark=false;
          } else
          {
            mapmode->mark=true;
            mapmode->paintmode=false;
            mapmode->markp.y=ty;
            mapmode->markp.x=tx;
          }
          message_info("Mark mode %s",mapmode->mark?"on":"off");
          break;
        case KEY_DEL: // Delete graffiti if there is any here
          d = graffiti_idx(lvl,tx,ty);
          if (d>=0)
          {
            message_info("Graffiti deleted");
            graffiti_clear_from_columns(lvl,d);
            graffiti_del(lvl,d);
          } else
            message_error("Nothing to delete");
          break;
        case KEY_A: // Change graffiti orientation
            slb_next_graffiti_orient(lvl,graffiti_idx(lvl,tx,ty));
          break;
        case KEY_D: // Add/view graffiti down
            if ((mapmode->mark) || (mapmode->paintmode))
            {
              message_error("Can't draw graffiti whilst painting or marking");
              return;
            }
            mdstart[MD_GRFT](scrmode,mapmode,lvl);
          break;
        case KEY_NUM0:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER0);
            break;
        case KEY_NUM1:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER1);
            break;
        case KEY_NUM2:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER2);
            break;
        case KEY_NUM3:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER3);
            break;
        case KEY_NUM4:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER_GOOD);
            break;
        case KEY_NUM5:
            slb_change_ownership(scrmode,mapmode,lvl,PLAYER_UNSET);
            break;
        case KEY_SQRBRCKTL:
          d = graffiti_idx(lvl,tx,ty);
          if (d>=0)
          {
            slb_change_graffiti_height(lvl,d,-1);
          } else
            message_error("Nothing to decrease");
          break;
        case KEY_SQRBRCKTR:
          d = graffiti_idx(lvl,tx,ty);
          if (d>=0)
          {
            slb_change_graffiti_height(lvl,d,1);
          } else
            message_error("Nothing to increase");
          break;
        case KEY_Z:
            if (mapmode->paintmode==false)
            {
              mapmode->paintmode=true;
              mapmode->paintown=-1;
              mapmode->paintroom=255;
              mapmode->mark=false;
            } else
            {
              mapmode->paintmode=false;
            }
            message_info("Paint mode %s",mapmode->paintmode?"on":"off");
            break;
          case KEY_ESCAPE:
            message_info(get_random_tip());
            break;
          default:
            if ((key < mdslab->placenkeys) && (mdslab->placekeys[key]!=255))
            {
              slb_place_room(lvl,mapmode,mdslab->placekeys[key]);
              if (mapmode->paintmode)
                mapmode->paintroom=mdslab->placekeys[key];
            } else
            {
            message_info("Unrecognized slb key code: %d",key);
            speaker_beep();
        }
      }
    }
    slbposcheck(scrmode,mapmode,lvl);
}

/*
 * Action function - start the mdslab mode.
 */
short start_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    scrmode->mode=MD_SLB;
    scrmode->usrinput_type=SI_NONE;
    return true;
}

/*
 * Action function - end the mdslab mode.
 */
void end_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    mapmode->mark=false;
    mapmode->paintmode=false;
    mapmode->panel_mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
}

/*
 * Action subfunction - check position in slb mode.
 */
void slbposcheck(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    if (mapmode->paintmode)
    {
      if (mapmode->paintroom != 255)
          slb_place_room(lvl,mapmode,mapmode->paintroom);
      if (mapmode->paintown >= 0)
          change_ownership(mapmode,lvl,(char)(mapmode->paintown));
    }
}

/*
 * Action function - place room in slb mode.
 * Places complete room, including things and CLM update
 * (these updates can be disabled).
 */
void slb_place_room(struct LEVEL *lvl,struct MAPMODE_DATA *mapmode,unsigned char room)
{
    int markr,markb;
    int markl,markt;
    if (mapmode->mark)
    {
      markr=mapmode->markr.r;
      markb=mapmode->markr.b;
      markl=mapmode->markr.l;
      markt=mapmode->markr.t;
    } else
    {
      markr=mapmode->screen.x+mapmode->map.x;
      markb=mapmode->screen.y+mapmode->map.y;
      markl=markr;
      markt=markb;
    }
    // Sanity check, almost certainly unneeded
    if ((markr>MAP_MAXINDEX_X) || (markb>MAP_MAXINDEX_Y) ||
       (markl<0) || (markt<0))
    {
      message_error("Map coordinates out of bounds");
      return;
    }
    int tile_x,tile_y;
    for (tile_x=markl; tile_x<=markr; tile_x++)
      for (tile_y=markt; tile_y<=markb; tile_y++)
      {
//          unsigned char oldslb;
//          oldslb = get_tile_slab(lvl,tile_x,tile_y);
          set_tile_slab(lvl,tile_x,tile_y,room);
      }
    if (obj_auto_update)
      update_obj_for_square(lvl, markl-1, markr+1, markt-1, markb+1);
    if (datclm_auto_update)
      update_datclm_for_square(lvl, markl-1, markr+1, markt-1, markb+1);
    if (obj_auto_update)
      update_obj_subpos_and_height_for_square(lvl, markl-1, markr+1, markt-1, markb+1);
    mapmode->mark=false;
}

/*
 * Action sub-function - change the owner of slab.
 */
void change_ownership(struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,unsigned char purchaser)
{
    int tx=mapmode->screen.x+mapmode->map.x;
    int ty=mapmode->screen.y+mapmode->map.y;
    // Sanity check, almost certainly unneeded
    if ((tx >= MAP_SIZE_X) || (tx >= MAP_SIZE_Y) ||
        (tx < 0) || (ty < 0))
      return;
    if (!mapmode->mark)
    {
      set_tile_owner(lvl,tx,ty,purchaser);
      if (obj_auto_update)
        update_obj_for_square_radius1(lvl,tx,ty);
      if (datclm_auto_update)
        update_datclm_for_square_radius1(lvl,tx,ty);
      if (obj_auto_update)
        update_obj_subpos_and_height_for_square_radius1(lvl,tx,ty);
      return;
    }
    // another sanity check - this time for marking mode
    if ((mapmode->markr.r > MAP_MAXINDEX_X) || (mapmode->markr.b > MAP_MAXINDEX_Y) ||
      (mapmode->markr.l < 0) || (mapmode->markr.t < 0))
      return;
    for (tx=mapmode->markr.l; tx<=mapmode->markr.r; tx++)
      for (ty=mapmode->markr.t; ty<=mapmode->markr.b; ty++)
          set_tile_owner(lvl,tx,ty,purchaser);
    if (obj_auto_update)
      update_obj_for_square(lvl, mapmode->markr.l-1, mapmode->markr.r+1, mapmode->markr.t-1, mapmode->markr.b+1);
    if (datclm_auto_update)
      update_datclm_for_square(lvl, mapmode->markr.l-1, mapmode->markr.r+1, mapmode->markr.t-1, mapmode->markr.b+1);
    if (obj_auto_update)
      update_obj_subpos_and_height_for_square(lvl, mapmode->markr.l-1, mapmode->markr.r+1, mapmode->markr.t-1, mapmode->markr.b+1);
    mapmode->mark=false;
}

/*
 * Action function - change the owner of a slab and display message.
 */
void slb_change_ownership(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,unsigned char purchaser)
{
    mapmode->paintown=purchaser;
    change_ownership(mapmode,lvl,purchaser);
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
void init_mdslab_keys(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
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
    mdslab->placenkeys=1;
    i=0;
    while (room_keys[i])
    {
      if (room_keys[i] > mdslab->placenkeys)
        mdslab->placenkeys=room_keys[i];
      i++;
    }
    mdslab->placenkeys++;
    mdslab->placekeys=(unsigned char *)malloc(mdslab->placenkeys*sizeof(char));
    if (!mdslab->placekeys)
      die("init_keys: Can't alloc memory for key table");

     // I don't *think* 255 is used by slb... if it is, we can always put
     // the key in manually in slbactions
    for (i=0; i < mdslab->placenkeys; i++)
      mdslab->placekeys[i]=255;
    i=0;
    while (room_keys[i])
    {
      mdslab->placekeys[room_keys[i]]=room_types[i];
      i++;
    }

    const int MAX_NUM_KEYS=256;
    slbkey=(char *)malloc(MAX_NUM_KEYS);
    if (!slbkey)
      die("init_slbkey: Out of memory");
    for (i=0; i < MAX_NUM_KEYS; i++)
      slbkey[i]='?';
    i=0;
    while (room_keys[i])
    {
      slbkey[room_types[i]]=key_to_ascii(room_keys[i]);
      i++;
    }
    // Hack for some slabs that are not in room_types[], 
    // or if we just want another char to view
    slbkey[SLAB_TYPE_DOORWOOD2]='w';
    slbkey[SLAB_TYPE_DOORBRACE2]='b';
    slbkey[SLAB_TYPE_DOORIRON2]='i';
    slbkey[SLAB_TYPE_DOORMAGIC2]='m';
}

/*
 * Frees memory allocated by init_mdslab_keys. Should be called in free_mdslab().
 */
void free_mdslab_keys(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode)
{
    free(mdslab->placekeys);
    free(slbkey);
}

/*
 * Draws screen for the mdslab mode.
 */
void draw_mdslab(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    draw_map_area(scrmode,mapmode,lvl,true,true,false);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(scrmode,mapmode,lvl,mapmode->panel_mode);
    else
      draw_mdslab_panel(scrmode,mapmode,lvl);
    draw_map_cursor(scrmode,mapmode,lvl,true,true,false);
}

void draw_mdslab_panel(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    int tx,ty;
    tx=mapmode->map.x+mapmode->screen.x;
    ty=mapmode->map.y+mapmode->screen.y;
    int scr_row=0;
    int scr_col=scrmode->cols+3;
    int graff_idx=graffiti_idx(lvl,tx,ty);
    if (graff_idx<0)
    {
        scr_row=display_mode_keyhelp(scr_row,scr_col,scrmode->rows-TNGDAT_ROWS-2,scrmode->mode,0);
    } else
    {
        scr_row=display_graffiti(lvl,scr_row,scr_col,graff_idx);
    }
    if (scrmode->rows >= scr_row+TNGDAT_ROWS+3)
    {
        set_cursor_pos(scrmode->rows-TNGDAT_ROWS-2, scr_col);
        unsigned short slb_type=get_tile_slab(lvl,tx,ty);
        screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
        screen_printf(".slb entry:%3d ",slb_type);
        screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
        screen_printf("%s",get_slab_fullname(slb_type));
    }
    display_rpanel_bottom(scrmode,mapmode,lvl);
}

int display_graffiti(struct LEVEL *lvl,int scr_row, int scr_col,int graff_idx)
{
    int scr_col1=scr_col+2;
    int scr_col2=scr_col+20;
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("%s %d","Graffiti",graff_idx+1);
    if (graff_idx>=lvl->graffiti_count) return scr_row;
    struct DK_GRAFFITI *graf=lvl->graffiti[graff_idx];
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
short start_mdgrafit(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    scrmode->mode=MD_GRFT;
    scrmode->usrinput_type=SI_GRAFT;
    message_info("Graffiti mode started");
    return true;
}

/*
 * Action function - end the mdgrafit mode.
 */
void end_mdgrafit(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    mapmode->panel_mode=PV_MODE;
    scrmode->usrinput_type=SI_NONE;
    scrmode->usrinput_pos=0;
    scrmode->usrinput[0]='\0';
    scrmode->mode=MD_SLB;
}

/*
 * Draws screen for the mdgrafit mode.
 */
void draw_mdgrafit(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl)
{
    draw_map_area(scrmode,mapmode,lvl,true,true,false);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(scrmode,mapmode,lvl,mapmode->panel_mode);
    else
      draw_mdslab_panel(scrmode,mapmode,lvl);
}


/*
 * Covers actions from the graffiti input screen.
 */
void actions_mdgrafit(struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LEVEL *lvl,int key)
{
    message_release();
    if (!string_get_actions(scrmode,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          mdend[MD_GRFT](scrmode,mapmode,lvl);
          message_info("Graffiti creation cancelled");
          break;
        case KEY_ENTER:
          {
          int tx,ty;
          tx=mapmode->map.x+mapmode->screen.x;
          ty=mapmode->map.y+mapmode->screen.y;
          int graf_idx;
          graf_idx=graffiti_add(lvl,tx,ty,0,scrmode->usrinput,GRAFF_FONT_ADICLSSC,ORIENT_NS,0x0184);
          mdend[MD_GRFT](scrmode,mapmode,lvl);
          if (graf_idx<0)
          {
            message_error("Cannot add graffiti(not enought room?)");
            break;
          }
          graffiti_update_columns(lvl,graf_idx);
          message_info("Graffiti placed");
          };break;
        default:
          message_info("Unrecognized \"%s\" key code: %d",longmodenames[MD_GRFT],key);
          speaker_beep();
      }
    }
}

