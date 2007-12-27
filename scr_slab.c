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


// Variables

MDSLAB_DATA *mdslab;

//Slab keys
char *slbkey;

/*
 * Initializes variables for the mdslab screen.
 */
short init_mdslab(void)
{
    //Creating mdslab variable
    mdslab=(MDSLAB_DATA *)malloc(sizeof(MDSLAB_DATA));
    if (mdslab==NULL)
     die("init_mdslab: Cannot allocate memory.");
    //Initialize keys for displaying and putting slabs
    init_mdslab_keys();
    return true;
}

/*
 * Deallocates memory for the mdslab screen.
 */
void free_mdslab(void)
{
  free(mdslab);
  free_mdslab_keys();
}


/*
 * Covers actions from the slab screen.
 */
void actions_mdslab(int key)
{
    int d;
    message_release();
    if (!cursor_actions(key))
    {
        int tx=mapmode->mapx+mapmode->screenx;
        int ty=mapmode->mapy+mapmode->screeny;
        switch (key)
        {
        case 'u': // Update all things/dat/clm/w?b
          update_slab_owners(lvl);
          update_datclm_for_whole_map(lvl);
          message_info("DAT/CLM/W?B entries updated");
          break;
        case 'v': // Verify whole map
          level_verify(lvl,NULL);
          break;
        case KEY_TAB:
          end_mdslab();
          start_mdtng(lvl);
          message_info("Thing mode activated");
          break;
        case 'c':
          end_mdslab();
          start_mdclm(lvl);
          message_info("Column mode activated");
          break;
        case KEY_CTRL_SPACE:
          if (mapmode->mark)
          {
            mapmode->mark=false;
          } else
          {
            mapmode->mark=true;
            mapmode->paintmode=false;
            mapmode->marky=ty;
            mapmode->markx=tx;
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
        case 'a': // Change graffiti orientation
//            slb_place_graffiti(lvl,tx,ty,ORIENT_WE);
            slb_next_graffiti_orient(lvl,graffiti_idx(lvl,tx,ty));
          break;
        case 'd': // Add/view graffiti down
            slb_place_graffiti(lvl,tx,ty,ORIENT_NS);
          break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            mapmode->paintown=key-'0';
            change_ownership ((char)(key-'0'));
            message_info("Slab owner changed");
            break;
        case '[':
          d = graffiti_idx(lvl,tx,ty);
          if (d>=0)
          {
            struct DK_GRAFFITI *graf;
            graf=get_graffiti(lvl,d);
            int nheight=graf->height-1;
            if (set_graffiti_height(graf,nheight)==nheight)
            {
              message_info("Height decreased");
              graffiti_update_columns(lvl,d);
            } else
              message_error("Value limit reached");
          } else
            message_error("Nothing to decrease");
          break;
        case ']':
          d = graffiti_idx(lvl,tx,ty);
          if (d>=0)
          {
            struct DK_GRAFFITI *graf;
            graf=get_graffiti(lvl,d);
            int nheight=graf->height+1;
            if (set_graffiti_height(graf,nheight)==nheight)
            {
              message_info("Height increased");
              graffiti_update_columns(lvl,d);
            } else
              message_error("Value limit reached");
          } else
            message_error("Nothing to increase");
          break;
        case 'z':
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
              slb_place_room(mdslab->placekeys[key]);
              if (mapmode->paintmode)
                mapmode->paintroom=mdslab->placekeys[key];
            } else
            {
            message_info("Unrecognized slb key code: %d",key);
            speaker_beep();
        }
      }
    }
    slbposcheck();
}

/*
 * Action function - start the mdslab mode.
 */
short start_mdslab(struct LEVEL *lvl)
{
    scrmode->mode=MD_SLB;
    return true;
}

/*
 * Action function - end the mdslab mode.
 */
void end_mdslab()
{
    mapmode->mark=false;
    mapmode->panel_mode=PV_MODE;
}

/*
 * Action subfunction - check position in slb mode.
 */
void slbposcheck(void)
{
    if (mapmode->paintmode)
    {
      if (mapmode->paintroom != 255)
          slb_place_room(mapmode->paintroom);
      if (mapmode->paintown >= 0)
          change_ownership ((char)(mapmode->paintown));
    }
}

/*
 * Action function - place room in slb mode.
 * Places complete room, including things and CLM update
 * (these updates can be disabled).
 */
void slb_place_room(unsigned char room)
{
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;

    // Sanity check, almost certainly unneeded
    if ((tx >= MAP_SIZE_X) || (tx >= MAP_SIZE_Y) ||
        (tx < 0) || (ty < 0))
      return;
    int x, y;
    unsigned char oldslb;
    if (!mapmode->mark)
    {
      oldslb = get_tile_slab(lvl,tx,ty);
      set_tile_slab(lvl,tx,ty,room);
      if (obj_auto_update)
        update_obj_for_square_radius1(lvl,tx,ty);
      if (datclm_auto_update)
        update_datclm_for_square_radius1(lvl,tx,ty);
      return;
    }
    // And another - this time for marking mode
    if ((mapmode->markr>MAP_MAXINDEX_X) || (mapmode->markb>MAP_MAXINDEX_Y) ||
       (mapmode->markl<0) || (mapmode->markt<0))
      return;
    for (x=mapmode->markl; x <= mapmode->markr; x++)
      for (y=mapmode->markt; y <= mapmode->markb; y++)
      {
          oldslb = get_tile_slab(lvl,x,y);
          set_tile_slab(lvl,x,y,room);
      }
    if (obj_auto_update)
      update_obj_for_square(lvl, mapmode->markl-1, mapmode->markr+1, mapmode->markt-1, mapmode->markb+1);
    if (datclm_auto_update)
      update_datclm_for_square(lvl, mapmode->markl-1, mapmode->markr+1, mapmode->markt-1, mapmode->markb+1);
    mapmode->mark=false;
}

/*
 * Action function - change the owner of item mode.
 */
void change_ownership(unsigned char purchaser)
{
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    // Sanity check, almost certainly unneeded
    if ((tx >= MAP_SIZE_X) || (tx >= MAP_SIZE_Y) ||
        (tx < 0) || (ty < 0))
      return;
    if (!mapmode->mark)
    {
      set_tile_owner(lvl,tx,ty,purchaser);
      if (datclm_auto_update)
        update_datclm_for_square_radius1(lvl,tx,ty);
      return;
    }
    // another sanity check - this time for marking mode
    if ((mapmode->markr > MAP_MAXINDEX_X) || (mapmode->markb > MAP_MAXINDEX_Y) ||
      (mapmode->markl < 0) || (mapmode->markt < 0))
      return;
    for (tx=mapmode->markl; tx<=mapmode->markr; tx++)
      for (ty=mapmode->markt; ty<=mapmode->markb; ty++)
          set_tile_owner(lvl,tx,ty,purchaser);
    if (datclm_auto_update)
      update_datclm_for_square(lvl, mapmode->markl-1, mapmode->markr+1, mapmode->markt-1, mapmode->markb+1);
    mapmode->mark=false;
}

void slb_place_graffiti(struct LEVEL *lvl,int tx, int ty,unsigned short orient)
{
    static char graf_text[READ_BUFSIZE];
    if ((mapmode->mark) || (mapmode->paintmode))
    {
        message_error("Can't draw graffiti whilst painting or marking");
        return;
    }
    if (get_str("Enter graffiti: ", graf_text))
    {
      int graf_idx;
      graf_idx=graffiti_add(lvl,tx,ty,0,graf_text,GRAFF_FONT_ADICLSSC,orient,0x0184);
      if (graf_idx<0)
      {
        message_error("Cannot add graffiti(not enought room?)");
        return;
      }
      graffiti_update_columns(lvl,graf_idx);
      message_info("Graffiti placed");
    }
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
    short result=set_graffiti_orientation(graf,new_orient);
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
void init_mdslab_keys()
{
    int i;
    // Arrays storing keyboard shortcuts and corresponding rooms
    // These must end with zero
    static const unsigned int room_keys[]={
      't', 'L', 'H',
      'T', 'l', '%',
      'W', 'G', 'P',
      'O', 'B', 'e',
      'g', 's', 'S',
      'h', '#', '.',
      '-', ' ', '/',
      '^', '\\', '&',
      '*', '(', '$',
      '!', '`', '~',
      '=', 'w', 'b',
      'i', 'm', 'E',
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
      die ("init_keys: Number of rooms doesn't match number of keys");
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
      slbkey[room_types[i]]=room_keys[i];
      i++;
    }
    // Hack for some slabs that are not in room_types[], 
    // or if we just want another char to view
    slbkey[SLAB_TYPE_DOORWOOD2]='w';
    slbkey[SLAB_TYPE_DOORBRACE2]='b';
    slbkey[SLAB_TYPE_DOORIRON2]='i';
    slbkey[SLAB_TYPE_DOORMAGIC2]='m';
// Old way - delete pending
//    static char got[]="#$./^\\&*()- =~E?t?l?P?O?T?h?W?S?e?g?H?L?B?wwbbiimm?%!G";
}

/*
 * Frees memory allocated by init_mdslab_keys. Should be called in free_mdslab().
 */
void free_mdslab_keys()
{
    free(mdslab->placekeys);
    free(slbkey);
}

/*
 * Draws screen for the mdslab mode.
 */
void draw_mdslab()
{
    int tx,ty;
    tx=mapmode->mapx+mapmode->screenx;
    ty=mapmode->mapy+mapmode->screeny;
    draw_map_area(lvl,true,true,false);
    if (mapmode->panel_mode!=PV_MODE)
    {
      draw_forced_panel(lvl,mapmode->panel_mode);
    } else
    {
      int scr_row=0;
      int scr_col=scrmode->cols+3;
      int graff_idx=graffiti_idx(lvl,tx,ty);
      if (graff_idx<0)
      {
        scr_row=display_mode_keyhelp(scr_row,scr_col,scrmode->mode);
      } else
      {
        scr_row=display_graffiti(lvl,scr_row,scr_col,graff_idx);
      }
      scr_row++;
      set_cursor_pos(scr_row++, scr_col);
      unsigned short slb_type=get_tile_slab(lvl,tx,ty);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Slab: ");
      screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
      screen_printf("%s",get_slab_fullname(slb_type));
      display_tngdat();
    }
    draw_map_cursor(lvl,true,true,false);
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
    screen_printf("Position: %d,%d",graf->tx,graf->ty);
    set_cursor_pos(scr_row++, scr_col2);
    screen_printf("End: %d,%d",graf->fin_tx,graf->fin_ty);
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
