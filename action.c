/*
 * action.c   keyboard actions definition for AdiktEd
 *
 * Defines what to do when specific keys are stoked. Covers
 * all "modes" of work
 *
 */
#include "action.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "obj_actnpts.h"
#include "input_kb.h"
#include "scr_help.h"
#include "scr_level.h"
#include "internal.h"

void proc_key(void);

static void slbactions (int key);
static void creatureactions (int key);
static void itemtypeactions (int key);
static short cursor_actions (int key);
static void tngactions (int key);
static void clmactions (int key);
static void helpactions (int key);
static void slb_place_room (unsigned char room);
void start_help(void);
void end_help(void);
void start_list(void);
void end_list(void);
static void slbposcheck(void);
static void curposcheck (void);
static void (*actions [])(int)={slbactions, tngactions, 
    creatureactions, itemtypeactions, helpactions, clmactions};
static void change_ownership (unsigned char purchaser);

const char *modenames[]={"Slab", "Thing", "+body", "Item", "Help", "Clm"};

// indicates if the main program loop should end
short finished;
// main program working mode
int mode=MD_SLB;

//Marking variables
short mark=false;
int markl, markr, markt, markb, markx, marky;

//Painting properties
short paintmode=false;
short paintown=false;
unsigned char paintroom=0;

// Size of the screen area where map is drawn
int rows;
int cols;

// Location of cursor on screen (where appropriate)
int screenx=0, screeny=0;
// Location of top left corner of screen in map (where appropriate
int mapx=0, mapy=0;

// Which subtile is being considered in thing and data modes
int sx=1, sy=1;

//Clipboard
char tngclipboard[TNGCLIPBRD_SIZE];

/*
 * Gets and processes a key stroke.
 */
void proc_key (void) 
{
    static char usrinput[80];
    unsigned int g;
    g = get_key();
    // Decoding "universal keys" - global actions
    // which should work in every screen
    //Performing actions, or sending the keycode elswhere
    switch (g)
    {
    case KEY_F1:
      if (mode != MD_HELP)
        start_help();
        break;
    case KEY_CTRL_Q:
      if ((mode!=MD_HELP)&&(mode!=MD_CRTR)&&(mode!=MD_ITMT))
          finished=true;
      else
        actions[mode](KEY_CTRL_Q); // Grotty but it'll work
      break;
    case KEY_CTRL_L:
      if (get_str ("Enter map number/name to load: ", usrinput))
      {
        message_info_force("Loading map...");
        if (format_map_fname(lvl->fname,usrinput))
        {
          free_map();
          load_map(lvl);
          message_info("Map \"%s\" loaded", lvl->fname);
        } else
          message_error("Map loading cancelled");
      } else
      {
        speaker_beep();
      }
      break;
    case KEY_CTRL_S:
      if ((mode==MD_HELP) || (mode==MD_CRTR) || (mode==MD_ITMT))
        break;
      if (get_str("Enter map number/name to save: ", usrinput))
      {
        message_info_force("Saving map...");
        if (format_map_fname(lvl->fname,usrinput))
        {
          save_map(lvl);
          message_info("Map \"%s\" saved", lvl->fname);
        } else
          message_error("Map saving cancelled");
      } else
      {
        speaker_beep();
      }
      break;
    case KEY_CTRL_N:
      free_map();
      start_new_map(lvl);
      message_info_force("New map started");
      break;
    case KEY_CTRL_R:
      free_map();
      generate_random_map(lvl);
      message_info_force("Map generation completed.");
      break;
    default:
        //Sending the action to a function corresponding to actual screen
        actions[mode](g);
    }
}

/*
 * Covers actions from the help screen.
 */
static void helpactions(int key)
{
    switch (key)
    {
    case KEY_UP:
      helpy--;
      break;
    case KEY_DOWN:
      helpy++;
      break;
    case KEY_PGUP:
      helpy-=rows-1;
      break;
    case KEY_PGDOWN:
      helpy+=rows-1;
      break;
    default:
      end_help();
    }
    if (helpy+rows > helprows)
      helpy=helprows-rows;
    if (helpy < 0)
      helpy=0;
}

/*
 * Covers actions from the tng screen.
 */
static void tngactions(int key)
{
    int cx, cy;
    unsigned char *thing;
    cx = (mapx+screenx)*3+sx;
    cy = (mapy+screeny)*3+sy;
    message_release();
    
    if (!cursor_actions (key))
    {
      switch (key)
      {
        case KEY_TAB:
          mode=MD_SLB;
          break;
        case '1':
          sx=0;
          sy=2;
          break;
        case '2':
          sx=1;
          sy=2;
          break;
        case '3':
          sx=2;
          sy=2;
          break;
        case '4':
          sx=0;
          sy=1;
          break;
        case '5':
          sx=1;
          sy=1;
          break;
        case '6':
          sx=2;
          sy=1;
          break;
        case '7':
          sx=0;
          sy=0;
          break;
        case '8':
          sx=1;
          sy=0;
          break;
        case '9':
          sx=2;
          sy=0;
          break;
        case 'c':
          mode=MD_CRTR;
          start_list();
          break;
        case 'i':
          mode=MD_ITMT;
          start_list();
          break;
        case 'b' :
          thing = create_item(cx, cy, ITEM_SUBTYPE_SPELLHOE);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'h' : // Add Dungeon heart
          thing = create_item (cx, cy, ITEM_SUBTYPE_DNHEART);
          set_thing_tilepos_h(thing,3); // Raise it up a bit
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'H' : // Add hero gate
          thing = create_item (cx, cy, ITEM_SUBTYPE_HEROGATE);
          set_thing_owner(thing,4);
          thing_add(lvl,thing);
          //Hero gate number
          lvl->stats.hero_gates_count++;
          thing[14]=(char)(-lvl->stats.hero_gates_count);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'a' : // Add action point
          thing = create_actnpt(cx, cy);
          actnpt_add(lvl,thing);
          message_info_force("Added action point %d",(thing[20]<<8)+thing[19]);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'k' : // Copy thing to clipboard
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing=get_object(lvl,cx,cy,vistng[sx][sy]);
              memcpy (tngclipboard, thing+4, 17);
              message_info("Object now in clipboard.");
              break;
            case OBJECT_TYPE_ACTNPT:
              message_error("Can't copy action points to clipboard.");
              break;
            default:
              message_error("Can't copy to clipboard: nothing selected.");
              break;
            }
          };break;
        case 'y' : // Create thing from clipboard
          if (tngclipboard[2])
          {
            thing = create_thing (cx, cy);
            memcpy (thing+4, tngclipboard, 17);
            // Make sure the coordinates are preserved for objects
            if (get_thing_subtpos_y(thing)==1)
            {
                thing[12]=(MAP_SIZE_X*(mapy+screeny)+(mapx+screenx)) >> 8;
                thing[11]=((MAP_SIZE_X*(mapy+screeny)+(mapx+screenx)) & 255);
            }
            thing_add(lvl,thing);
            vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          } else
          {
            message_error("Can't create thing: clipboard is empty.");
          };break;
        case 'u' : // Update all things
          create_clmdattng();
          break;
        case 'l' : // Lock / unlock a door
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing=get_object(lvl,cx,cy,vistng[sx][sy]);
              if (get_thing_type(thing)==THING_TYPE_DOOR)
                  thing[14]=1-thing[14];
              break;
            default:
              break;
            }
            if ((lvl->slb[cx/MAP_SUBNUM_Y][cy/MAP_SUBNUM_Y]&0xfe) == get_thing_subtype(thing)*2+40) // Update the slab
                lvl->slb[cx/MAP_SUBNUM_Y][cy/MAP_SUBNUM_Y] = get_thing_subtype(thing)*2+40+thing[14];
          };break;
        case 'd': // Dungeon special
          thing = create_item (cx, cy, ITEM_SUBTYPE_SPREVMAP);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'g': // Gold
          thing = create_item (cx, cy, ITEM_SUBTYPE_GOLDCHEST);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'G': // Gold
          thing = create_item (cx, cy, ITEM_SUBTYPE_GOLD);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'e': // Add room effect
          thing = create_roomeffect(cx, cy, ROOMEFC_SUBTP_DRIPWTR);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'o' : // Change ownership of creature/trap/special/spell
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,cx,cy,vistng[sx][sy]);
              set_thing_owner(thing,get_player_next(get_thing_owner(thing)));
              break;
            default:
              message_error("Can't change owner: no \"thing\" selected.");
              break;
            }
          };break;
        case 'S' : // Change type of creature
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,cx,cy,vistng[sx][sy]);
              if (!is_creature(thing)) break;
              set_thing_subtype(thing,get_creature_next(get_thing_subtype(thing)));
              break;
            default:
              message_error("Can't change type: no \"thing\" selected.");
              break;
            }
          };break;
        case 'X' : // Change type of creature
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,cx,cy,vistng[sx][sy]);
              if (!is_creature(thing)) break;
              set_thing_subtype(thing,get_creature_prev(get_thing_subtype(thing)));
              break;
            default:
              message_error("Can't change type: no \"thing\" selected.");
              break;
            }
          };break;
        case 's' : // Change level of creature/trap/special/spell/lair/room efct
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,cx,cy,vistng[sx][sy]);
              if ((get_thing_type(thing)==THING_TYPE_CREATURE) && (thing[14]<9))
                  thing[14]++;
              else
              if (is_trap(thing))
                  set_thing_subtype(thing,get_trap_next(get_thing_subtype(thing)));
              else
              if (is_dngspecbox(thing))
                  set_thing_subtype(thing,get_dngspecbox_next(get_thing_subtype(thing)));
              else
              if (is_spellbook(thing))
                  set_thing_subtype(thing,get_spellbook_next(get_thing_subtype(thing)));
              else
              if (is_trapbox(thing))
                  set_thing_subtype(thing,get_trapbox_next(get_thing_subtype(thing)));
              else
              if (is_crtrlair(thing))
                  set_thing_subtype(thing,get_crtrlair_next(get_thing_subtype(thing)));
              else
              if (is_roomeffect(thing))
                  set_thing_subtype(thing,get_roomeffect_next(get_thing_subtype(thing)));
              else
                  message_error("This item has no level nor type.");
              break;
            default:
              message_error("Can't change level: no thing selected.");
              break;
            }
          };break;
        case 'x' : // Change level of creature/trap/special/spell/lair/room efct
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_THING:
              thing = get_object(lvl,cx,cy,vistng[sx][sy]);
              if ((get_thing_type(thing)==THING_TYPE_CREATURE) && (thing[14]>0))
                  thing[14]--;
              else
              if (is_trap(thing))
                  set_thing_subtype(thing,get_trap_prev(get_thing_subtype(thing)));
              else
              if (is_dngspecbox(thing))
                  set_thing_subtype(thing,get_dngspecbox_prev(get_thing_subtype(thing)));
              else
              if (is_spellbook(thing))
                  set_thing_subtype(thing,get_spellbook_prev(get_thing_subtype(thing)));
              else
              if (is_trapbox(thing))
                  set_thing_subtype(thing,get_trapbox_prev(get_thing_subtype(thing)));
              else
              if (is_crtrlair(thing))
                  set_thing_subtype(thing,get_crtrlair_prev(get_thing_subtype(thing)));
              else
              if (is_roomeffect(thing))
                  set_thing_subtype(thing,get_roomeffect_prev(get_thing_subtype(thing)));
              else
                  message_error("This item has no level nor type.");
              break;
            default:
              message_error("Can't change level: no thing selected.");
              break;
            }
          };break;
        case 't' : // Create trap
          thing = create_trap (cx, cy, TRAP_SUBTYPE_BOULDER);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case 'T' : // Create trap box
          thing = create_item (cx, cy, ITEM_SUBTYPE_TBBOULDER);
          thing_add(lvl,thing);
          vistng[sx][sy]=get_thing_subnums(lvl,cx,cy)-1;
          break;
        case '/' : // Cycle through highlighted things
          {
          unsigned int obj_num=get_object_subnums(lvl,cx,cy);
          if (obj_num > 0)
            vistng[sx][sy]=(vistng[sx][sy]+1)%(obj_num);
          };break;
        case KEY_DEL: // delete
          {
            short obj_type=get_object_type(lvl,cx,cy,vistng[sx][sy]);
            switch (obj_type)
            {
            case OBJECT_TYPE_ACTNPT:
              object_del(lvl,cx, cy, vistng[sx][sy]);
              message_info_force("Action point deleted.");
              break;
            case OBJECT_TYPE_THING:
              object_del(lvl,cx, cy, vistng[sx][sy]);
              message_info_force("Thing deleted.");
              break;
            default:
              message_error("Nothing to delete.");
              break;
            }
            if (vistng[sx][sy]>0) vistng[sx][sy]--;
          };break;
        default:
          message_info("Unrecognized tng key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from the claim screen.
 */
static void clmactions (int key)
{
    int cx, cy;

    cx = (mapx+screenx)*3+sx;
    cy = (mapy+screeny)*3+sy;
    message_release();
    
    if (!cursor_actions (key))
    {
      switch (key)
      {
        case KEY_TAB:
          mode=MD_SLB;
          break;
        case '1':
          sx=0;
          sy=2;
          break;
        case '2':
          sx=1;
          sy=2;
          break;
        case '3':
          sx=2;
          sy=2;
          break;
        case '4':
          sx=0;
          sy=1;
          break;
        case '5':
          sx=1;
          sy=1;
          break;
        case '6':
          sx=2;
          sy=1;
          break;
        case '7':
          sx=0;
          sy=0;
          break;
        case '8':
          sx=1;
          sy=0;
          break;
        case '9':
          sx=2;
          sy=0;
          break;
        case 'u': // Update all things
          create_clmdattng();
          break;
        default:
          message_info("Unrecognized clm key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from all screens with numbered list.
 */
short list_actions(int key)
{
    int num_rows=numbered_list_items/numbered_list_cols
                        + ((numbered_list_items%numbered_list_cols)>0);
    switch (key)
    {
    case KEY_UP:
      numbered_list_pos--;
      break;
    case KEY_DOWN:
      numbered_list_pos++;
      break;
    case KEY_LEFT:
      numbered_list_pos-=numbered_list_items/numbered_list_cols
                        + ((numbered_list_items%numbered_list_cols)>0);
      break;
    case KEY_RIGHT:
      numbered_list_pos+=numbered_list_items/numbered_list_cols
                        + ((numbered_list_items%numbered_list_cols)>0);
      break;
    case KEY_PGUP:
      numbered_list_pos-=rows-1;
      break;
    case KEY_PGDOWN:
      numbered_list_pos+=rows-1;
      break;
    default:
      return false;
    }
    if (numbered_list_pos >= numbered_list_items)
      numbered_list_pos=numbered_list_items-1;
    if (numbered_list_pos<0) numbered_list_pos=0;
    //Compute the row where selected item is
    int curr_row=numbered_list_pos%num_rows;
    if (numbered_list_y>curr_row) numbered_list_y=curr_row;
    if (numbered_list_y+rows<curr_row+1) numbered_list_y=curr_row-rows+1;
    if (numbered_list_y+rows > num_rows+1)
      numbered_list_y=num_rows-rows+1;
    if (numbered_list_y<0) numbered_list_y=0;
    return true;
}

/*
 * Covers actions from the creature screen.
 */
static void creatureactions(int key)
{
    unsigned char *thing;
    if (!list_actions(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
          message_info("Adding creature cancelled");
          mode=MD_TNG;
          break;
        case KEY_ENTER:
          thing = create_creature((mapx+screenx)*3+sx, (mapy+screeny)*3+sy,numbered_list_pos+1);
          thing[5]=1;
          thing[6]=5;
          thing[8]=lvl->own[mapx+screenx][mapy+screeny];
          thing_add(lvl,thing);
          // Show the new thing
          vistng[sx][sy]=get_thing_subnums(lvl,(mapx+screenx)*3+sx,(mapy+screeny)*3+sy)-1;
          end_list();
          message_info("Creature added");
          mode=MD_TNG;
          break;
        default:
          message_info("Unrecognized item key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from the item type screen.
 */
static void itemtypeactions(int key)
{
    unsigned char *thing;
    if (!list_actions(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
          message_info("Adding thing cancelled");
          mode=MD_TNG;
          break;
        case KEY_ENTER:
          thing = create_item((mapx+screenx)*3+sx, (mapy+screeny)*3+sy,numbered_list_pos+1);
          thing[8]=lvl->own[mapx+screenx][mapy+screeny];
          thing_add(lvl,thing);
          // Show the new thing
          vistng[sx][sy]=get_thing_subnums(lvl,(mapx+screenx)*3+sx,(mapy+screeny)*3+sy)-1;
          end_list();
          message_info("Item added");
          mode=MD_TNG;
          break;
        default:
          message_info("Unrecognized item key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from the slab screen.
 */
static void slbactions(int key)
{
    int d;
    static char graf[80];
    message_release();
    if (!cursor_actions(key))
    {
      switch (key)
      {
      case 'u': // Update all things
        create_clmdattng();
        break;
      case KEY_TAB:
        mode=MD_TNG;
        break;
      case 'c':
        mode=MD_CLM;
        break;
      case KEY_CTRL_SPACE:
        if (mark)
        {
          mark=false;
        } else
        {
          mark=true;
          paintmode=false;
          marky=mapy+screeny;
          markx=mapx+screenx;
        }
        message_info("Mark mode %s",mark?"on":"off");
        break;
      case KEY_DEL: // Delete graffiti if there is any here
        d = is_graffiti(mapx+screenx, mapy+screeny);
        if (d != -1)
          delete_graffiti (d);
        break;
      case 'a': // Add/view graffiti across
      case 'd': // Add/view graffiti down
        if (mark || paintmode)
        {
          message_error("Can't draw graffiti whilst painting or marking");
          return;
        }
        d = (key=='d' ? 1 : 0);
        if (get_str ("Enter graffiti: ", graf))
          if (!add_graffiti (mapx+screenx, mapy+screeny, graf, d))
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
          paintown=key-'0';
          change_ownership ((char)(key-'0'));
          break;
        case 'z':
          if (paintmode==false)
          {
            paintmode=true;
            paintown=-1;
            paintroom=255;
            mark=false;
          } else
          {
            paintmode=false;
          }
        message_info("Paint mode %s",paintmode?"on":"off");
          break;
        default:
          if (key < slbplacenkeys && slbplacekeys[key]!=255)
          {
            slb_place_room(slbplacekeys[key]);
            if (paintmode)
              paintroom=slbplacekeys[key];
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
 * Covers cursor actions from non-help screens.
 */
static short cursor_actions(int key)
{
    int osx, osy, omx, omy;
    int i, j;
    
    osx=screenx;
    osy=screeny;
    omx=mapx;
    omy=mapy;
    
    switch (key)
    {
#if !defined (unix) || defined (GO32)      
//TODO: fix these keycodes!
      case 5141 : // ctrl+arrow up
      screeny-=5;
      break;
      case 5145 : // ctrl+arrow down
      screeny+=5;
      break;
      case 5115 : // ctrl+arrow left
      screenx-=5;
      break;
      case 5116 : // ctrl+arrow right
      screenx+=5;
      break;
      case 5152 : // alt+arrow up
      screeny-=15;
      break;
      case 5160 : // alt+arrow down
      screeny+=15;
      break;
      case 5155 : // alt+arrow left
      screenx-=15;
      break;
      case 5157 : // alt+arrow right
      screenx+=15;
      break;
      case 5119 : // ctrl+home
      mapx=0;
      screenx=0;
      break;
      case 5117 : // ctrl+end
      mapx=MAP_SIZE_X;
      screenx=MAP_SIZE_X;
      break;
      case 5132 : // ctrl+page up
      mapy=0;
      screeny=0;
      break;
      case 5118 : // ctrl+page down
      mapy=MAP_SIZE_Y;
      screeny=MAP_SIZE_Y;
      break;
#endif      
      case KEY_UP:
      screeny--;
      break;
      case KEY_DOWN:
      screeny++;
      break;
      case KEY_RIGHT:
      screenx++;
      break;
      case KEY_LEFT:
      screenx--;
      break;
      case KEY_END:
      screenx+=10;
      break;
      case KEY_HOME:
      screenx-=10;
      break;
      case KEY_PGUP:
      screeny-=10;
      break;
      case KEY_PGDOWN:
      screeny+=10;
      break;
      default:
      return false;
    }
    curposcheck();
    if ((omx+osx != screenx+mapx) || (omy+osy != screeny+mapy))
      // As we've moved, reset which things we're looking at
    for (i=0; i < 3; i++)
      for (j=0; j < 3; j++)
        vistng[i][j]=0;
    return true;
}

/*
 * Action function - start the help mode.
 */
void start_help()
{
    message_info_force("Use arrow keys and page up/down to move, "
      "any other key to return.");
    helpformode=mode;
    helpy=0;
    switch (helpformode)
    {
      case MD_SLB:
        helprows=slbhelprows;
        helptext=slbhelp;
        break;
      case MD_CRTR:
        helprows=crthelprows;
        helptext=crthelp;
        break;
      case MD_ITMT:
        helprows=itmthelprows;
        helptext=itmthelp;
        break;
      case MD_TNG:
        helprows=tnghelprows;
        helptext=tnghelp;
        break;
      case MD_CLM:
        helprows=clmhelprows;
        helptext=clmhelp;
        break;
    }
    mode=MD_HELP;
}

/*
 * Action function - end the help mode.
 */
void end_help()
{
    mode=helpformode;
    message_release();
    message_info("Returned to last work mode.");
}

/*
 * Action function - start any of the the list modes.
 */
void start_list()
{
    numbered_list_y=0; //the first visible row
    numbered_list_pos=0; // selected item position (rel. to screen top)
    numbered_list_items=8; //number of items in the list
    numbered_list_cols=2; //number of columns in the item list
    message_info("Use arrow keys and page up/down to move, "
      "enter to choose.");
}

/*
 * Action function - end any of the the list modes.
 */
void end_list()
{
    message_release();
}

/*
 * Action subfunction - check position in slb mode.
 */
static void slbposcheck(void)
{
    if (paintmode)
    {
      if (paintroom != 255)
          slb_place_room (paintroom);
      if (paintown >= 0)
          change_ownership ((char)paintown);
    }
}

/*
 * Action function - place room in slb mode.
 */
static void slb_place_room (unsigned char room)
{
    int x, y;
    int oldslb;

    // Sanity check, almost certainly unneeded
    if ((screenx+mapx >= MAP_SIZE_X) || (screeny+mapy >= MAP_SIZE_Y) ||
        (screeny+mapy < 0) || (screeny+mapy < 0))
      return;
    if (!mark)
    {
      oldslb = lvl->slb[screenx+mapx][screeny+mapy];
      lvl->slb[screenx+mapx][screeny+mapy]=room;
      update_square (screenx+mapx, screeny+mapy);
      return;
    }
    // And another...
    if ((markr>MAP_MAXINDEX_X) || (markb>MAP_MAXINDEX_Y) || (markl<0) || (markt<0))
      return;
    for (x=markl; x <= markr; x++)
    {
      for (y=markt; y <= markb; y++)
      {
          oldslb = lvl->slb[x][y];
          lvl->slb[x][y]=room;
      }
    }
    for (x=markl-1; x <= markr+1; x++)
      for (y=markt-1; y <= markb+1; y++)
          update_tngdat (x, y);
    mark=false;
}

/*
 * Action function - change the owner of item mode.
 */
static void change_ownership(unsigned char purchaser)
{
    int x, y;
    // Sanity check, almost certainly unneeded
    if ((screenx+mapx > MAP_MAXINDEX_X) || (screeny+mapy > MAP_MAXINDEX_Y) ||
      (screeny+mapy < 0) || (screeny+mapy < 0))
      return;
    if (!mark)
    {
      lvl->own[screenx+mapx][screeny+mapy]=purchaser;
      update_square (screenx+mapx, screeny+mapy);
      return;
    }
    // And another...
    if ((markr > MAP_MAXINDEX_X) || (markb > MAP_MAXINDEX_Y) ||
      (markl < 0) || (markt < 0))
      return;
    for (x=markl; x <= markr; x++)
      for (y=markt; y <= markb; y++)
          lvl->own[x][y]=purchaser;
    for (x=markl-1; x <= markr+1; x++)
      for (y=markt-1; y <= markb+1; y++)
          update_tngdat (x, y);
    mark=false;
}

/*
 * Action subfunction - check cursor position.
 */
static void curposcheck(void)
{
    if (screenx < 0)
    {
      mapx+=screenx;
      screenx=0;
    }
    if (screeny < 0)
    {
      mapy+=screeny;
      screeny=0;
    }
    if (screeny >= rows)
    {
      mapy+=screeny-(rows-1);
      screeny=rows-1;
    }
    if (screenx >= cols)
    {
      mapx+=screenx-(cols-1);
      screenx = cols-1;
    }
    if (mapx+cols > MAP_SIZE_X)
      mapx = MAP_SIZE_X-cols;
    if (mapy+rows > MAP_SIZE_Y)
      mapy = MAP_SIZE_Y-rows;
    if (mapx < 0)
      mapx=0;
    if (mapy < 0)
      mapy=0;
}

/*
 * Init function - creates arrays for keyboard shortcuts.
 */
void init_keys(void)
{
    int i=0;
    // Arrays storing keyboard shortcuts and corresponding rooms
    // These must end with zero
    static const unsigned int keys[]={'t', 'L', 'H', 'T', 'l', '%',
      'W', 'G', 'P', 'O', 'B', 'e', 'g', 'S', 's', 'h', '#', '.', '-',
      ' ', '/', '^', '\\', '&', '*', '(', '$', '!', '~', '=', 
      'w', 'b', 'i', 'm', 'E', 0};
    static const unsigned char rooms[]={16, 38, 36, 24, 18, 51,
      28, 53, 20, 22, 40, 32, 34, 30, 30, 26, 0, 2, 10, 
      11, 3, 4, 5, 6, 7, 8, 1, 52, 13, 12, 42, 44, 46, 48, 14, 0};

    if (sizeof(keys)/sizeof(*keys) != sizeof(rooms)/sizeof(*rooms))
      die ("init_keys: Number of rooms doesn't match number of keys");

    while (keys[i])
    {
      if (keys[i] > slbplacenkeys)
        slbplacenkeys=keys[i];
      i++;
    }
    slbplacenkeys++;
    slbplacekeys=(unsigned char *)malloc(slbplacenkeys*sizeof(char));
    if (!slbplacekeys)
      die("init_keys: Can't alloc memory for key table");

     // I don't *think* 255 is used by slb... if it is, we can always put
     // the key in manually in slbactions
    for (i=0; i < slbplacenkeys; i++)
      slbplacekeys[i]=255;
    i=0;
    while (keys[i])
    {
      slbplacekeys[keys[i]]=rooms[i];
      i++;
    }
}
