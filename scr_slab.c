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
    static char graf[READ_BUFSIZE];
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
          start_mdtng();
          message_info("Thing mode activated");
          break;
        case 'c':
          end_mdslab();
          start_mdclm();
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
          d = is_graffiti(tx,ty);
          if (d != -1)
          {
            message_info("Graffiti deleted");
            delete_graffiti(d);
          } else
            message_error("Nothing to delete");
          break;
        case 'a': // Add/view graffiti across
        case 'd': // Add/view graffiti down
          if ((mapmode->mark) || (mapmode->paintmode))
          {
            message_error("Can't draw graffiti whilst painting or marking");
            return;
          }
          d = (key=='d' ? 1 : 0);
          if (get_str ("Enter graffiti: ", graf))
            if (!add_graffiti (tx, ty, graf, d))
            {
               message_error("Not enough room for graffiti!");
            }
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
short start_mdslab()
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

/*
 * Init function - creates arrays for keyboard shortcuts in slab mode.
 */
void init_mdslab_keys()
{
    int i=0;
    // Arrays storing keyboard shortcuts and corresponding rooms
    // These must end with zero
    static const unsigned int room_keys[]={
      't', 'L', 'H',
      'T', 'l', '%',
      'W', 'G', 'P',
      'O', 'B', 'e',
      'g', 'S', 's',
      'h', '#', '.',
      '-', ' ', '/',
      '^', '\\', '&',
      '*', '(', '$',
      '!', '~', '`',
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

    static char got[]="#$./^\\&*()- =~E?t?l?P?O?T?h?W?S?e?g?H?L?B?wwbbiimm?%!G";
    const int MAX_NUM_KEYS=256;
    slbkey=(char *)malloc(MAX_NUM_KEYS);
    if (!slbkey)
      die("init_slbkey: Out of memory");

    int l = strlen(got);
    for (i=0; i < l; i++)
      slbkey[i]=got[i];
    for (i=l-1; i < MAX_NUM_KEYS; i++)
      slbkey[i]='?';
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
void draw_mdslab(void)
{
    draw_map_area(lvl,true,true,false);
    if (mapmode->panel_mode!=PV_MODE)
    {
      draw_forced_panel(lvl,mapmode->panel_mode);
    } else
    {
      int scr_row=display_mode_keyhelp(0,scrmode->cols+3,scrmode->mode);
      display_tngdat();
    }
    draw_map_cursor(lvl,true,true,false);
}
