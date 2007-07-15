/*
 * action.c   keyboard actions definition for AdiktEd
 *
 * Defines what to do when specific keys are stoked. Covers
 * all "modes" of work
 *
 */
#include "action.h"
#include "globals.h"

void proc_key(void);

static void slbactions (int key);
static void creatureactions (int key);
static int cursor_actions (int key);
static void tngactions (int key);
static void clmactions (int key);
static void helpactions (int key);
static unsigned int get_key (void);
static void slb_place_room (unsigned char room);
static void start_help (void);
static void end_help (void);
static void slbposcheck(void);
static void curposcheck (void);
static void (*actions [])(int)={slbactions, tngactions, 
    creatureactions, helpactions, clmactions};
static void change_ownership (unsigned char purchaser);

/*
 * Gets and processes a key stroke.
 */
void proc_key (void) 
{
    static char mapname[80];
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
      if ((mode!=MD_HELP)&&(mode!=MD_CRTR))
	    finished=true;
      else
        actions[mode](17); // Grotty but it'll work
      break;
    case KEY_CTRL_L:
      if (get_str ("Enter map number/name to load: ", mapname))
      {
        if (!*mapname)
        {
          sprintf (message, "Load cancelled");
          break;
        }
        free_map();
        sprintf (message, "Map %s loaded", mapname);
        if (strchr (mapname, '.'))
          *strchr (mapname, '.')=0;
        if (atoi (mapname))
		  sprintf (mapname, "%s"SEPARATOR"map%.5d", filebase, atoi (mapname));
        load_map (mapname);
      }
      break;
    case KEY_CTRL_N:
      free_map();
      start_new_map();
      strcpy (message, "New map started");
      break;
    case KEY_CTRL_S:
      if (mode==MD_HELP || mode==MD_CRTR)
	    return;
      if (get_str ("Enter map name: ", mapname))
      {
        if (!*mapname)
        {
          sprintf (message, "Save cancelled");
          break;
        }
        if (strchr(mapname, '.'))
          *strchr(mapname, '.')=0;
        save_map (mapname);
      } else
      {
        SLtt_beep();
      }
      break;
    default:
        //Sending the action to a function corresponding to actual screen
        actions[mode](g);
    }
}

/*
 * Function for retrieving single key from keyboard.
 */
static unsigned int get_key (void)
{
    unsigned int ret;
#if defined(unix) && !defined(GO32)
    if (update_required)
	update();
    safe_update = true;
#endif
    ret = SLkp_getkey();
#if defined(unix) && !defined(GO32)
    safe_update = FALSE;
#endif
    return ret;
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
    *message=0;
    
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
	    strcpy (creatinput, "");
	    sprintf (message, "Creature number: ");
	    break;
	  case 'b' :
	    thing = create_object (cx, cy, 0xb);
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'h' : // Add Dungeon heart
	    thing = create_object (cx, cy, 5);
	    thing[5]=3; // Raise it up a bit
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'H' : // Add hero gate
	    thing = create_object (cx, cy, 0x31);
	    thing[8]=4;
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'a' : // Add action point
	    thing = create_action_point (cx, cy);
	    add_thing (thing);
	    sprintf (message, "Added action point %d", 
		     (thing[20]<<8)+thing[19]);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'k' : // Copy thing to clipboard
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]==0xff) // Action point
		{
		    strcpy (message, "Can't copy action points to clipboard.");
		    SLtt_beep();
		    break;
		}
		memcpy (tngclipboard, thing+4, 17);
	    }
	    else
	    {
		strcpy (message, "Can't copy to clipboard: nothing selected.");
		SLtt_beep();
	    }
	    break;
	  case 'y' : // Create thing from clipboard
	    if (tngclipboard[2])
	    {
		thing = create_thing (cx, cy);
		memcpy (thing+4, tngclipboard, 17);
		// Make sure the co-ordinates are preserved for objects
		if (thing[2]==1)
		{
		    thing [12]=(85*(mapy+screeny)+(mapx+screenx)) >> 8;
		    thing [11]=((85*(mapy+screeny)+(mapx+screenx)) & 255);
		}
		add_thing (thing);
		vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    }
	    else
	    {
		strcpy (message, "Can't create thing: clipboard is empty.");
		SLtt_beep();
	    }
	    break;
	  case 'u' : // Update all things
	    create_clmdattng();
	    break;
	  case 'l' : // Lock / unlock a door
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]==9)
		    thing[14]=1-thing[14];
		if ((slb[cx/3][cy/3]&0xfe) == thing[7]*2+40) // Update the slab
		    slb[cx/3][cy/3] = thing[7]*2+40+thing[14];
	    }
	    break;
	  case 'd': // Dungeon special
	    thing = create_object (cx, cy, 0x56);
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'g': // Gold
	    thing = create_object (cx, cy, 3);
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'G': // Gold
	    thing = create_object (cx, cy, 6);
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'o' : // Change ownership of creature/trap/special/spell
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		thing[8]++;
		if (thing[8]==6)
		    thing[8]=0;
	    }
	    break;
	  case 'S' : // Change type of creature
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]!=5)
		    break;
		thing[7]++;
		if (thing[7]==32)
		    thing[7]=1;
	    }
	    break;
	  case 'X' : // Change type of creature
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]!=5)
		    break;
		thing[7]--;
		if (!thing[7])
		    thing[7]=31;
	    }
	    break;
	  case 's' : // Change level of creature/trap/special/spell
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]==5 && thing[14]<9)
		    thing[14]++;
		if (thing[6]==8 && thing[7]<6)
		    thing[7]++;
		if (thing[6]==1 && thing[7]>=0x56 && thing[7]<=0x5d)
		{
		    thing[7]++;
		    if (thing[7]==0x5e)
			thing[7]=0x56;
		}
		if (is_spell (thing))
		{
		    thing[7]++;
		    if (thing[7]==0x18)
			thing[7]=0x2d;
		    if (thing[7]==0x31)
			thing[7]=0x86;
		    if (thing[7]==0x87)
			thing[7]=0xb;
		}
		if (thing[6]==1 && thing[7]>=0x5e && thing[7] <= 0x63)
		{
		    thing[7]++;
		    if (thing[7]==0x64)
			thing[7]=0x5e;
		}
	    }
	    break;
	  case 'x' : // Change level of creature/trap/special/spell
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		thing = tng_lookup [cx][cy][vistng[sx][sy]];
		if (thing[6]==5 && thing[14])
		    thing[14]--;
		if (thing[6]==8 && thing[7]>1)
		    thing[7]--;
		if (thing[6]==1 && thing[7]>=0x54 && thing[7]<=0x5d)
		{
		    thing[7]--;
		    if (thing[7]==0x55)
			thing[7]=0x5d;
		}
		if (is_spell (thing))
		{
		    thing[7]--;
		    if (thing[7]==0x2c)
			thing[7]=0x17;
		    if (thing[7]==0xa)
			thing[7]=0x86;
		    if (thing[7]==0x85)
			thing[7]=0x30;
		}
		if (thing[6]==1 && thing[7]>=0x5e && thing[7] <= 0x63)
		{
		    thing[7]--;
		    if (thing[7]==0x5d)
			thing[7]=0x63;
		}
	    }
	    break;
	  case 't' : // Create trap
	    thing = create_object (cx, cy, 1);
	    thing[6]=8; // Make it a trap
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case 'T' : // Create trap box
	    thing = create_object (cx, cy, 0x5e);
	    add_thing (thing);
	    vistng[sx][sy]=tng_subnums[cx][cy]-1;
	    break;
	  case '/' : // Cycle through highlighted things
	    if (tng_subnums[cx][cy])
		vistng[sx][sy]=(vistng[sx][sy]+1)%tng_subnums[cx][cy];
	    break;
	  case 127 : // delete
	    if (vistng[sx][sy] < tng_subnums[cx][cy])
	    {
		if (is_action_point (cx, cy, vistng[sx][sy]))
		    strcpy (message, "Action point deleted");
		else
		    strcpy (message, "Thing deleted");
		delete_thing (cx, cy, vistng[sx][sy]);
		vistng[sx][sy]--;
		if (vistng[sx][sy]<0)
		    vistng[sx][sy]=0;
	    }
	    break;
	  default:
	    SLtt_beep();
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
    *message=0;
    
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
	    SLtt_beep();
	}
    }
}

/*
 * Covers actions from the creature screen.
 */
static void creatureactions(int key)
{
    int l;
    unsigned char *monster;
    
    l = strlen (creatinput);
    switch (key)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (l<2)
      {
        creatinput[l]=key;
        creatinput[l+1]=0;
      }	else
      {
	    SLtt_beep();
      }
      break;
    case KEY_BACKSP:
    case KEY_DEL:
      if (l)
        creatinput [l-1]=0;
      break;
    case KEY_ENTER:
      l = atoi (creatinput);
      if (!l)
      {
        strcpy (message, "");
        mode=MD_TNG;
        break;
      }
      if (l > 31)
      {
        SLtt_beep();
        break;
      }
      monster = create_thing ((mapx+screenx)*3+sx, (mapy+screeny)*3+sy);
      monster[5]=1;
      monster[6]=5;
      monster[7]=l;
      monster[8]=own[mapx+screenx][mapy+screeny];
      add_thing(monster);
      // Copy the monster to the clipboard
      memcpy (tngclipboard, monster+4, 17);
      // Show the new monster
      vistng[sx][sy]=tng_subnums[(mapx+screenx)*3+sx]
        [(mapy+screeny)*3+sy]-1;
      strcpy (message, "Creature added");
      mode=MD_TNG;
      break;
    default:
      SLtt_beep();
    }
    sprintf (message, "Creature number: %s", creatinput);
}

/*
 * Covers actions from the slab screen.
 */
static void slbactions(int key)
{
    int d;
    static char graf[80];
    *message=0;
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
          strcpy(message, "Mark mode off");
        } else
        {
          mark=true;
          paintmode=false;
          marky=mapy+screeny;
          markx=mapx+screenx;
          strcpy(message, "Mark mode on");
        }
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
          SLtt_beep();
          strcpy (message, "Can't draw graffiti whilst painting or"
              " marking");
          return;
        }
        d = (key=='d' ? 1 : 0);
        if (get_str ("Enter graffiti: ", graf))
          if (!add_graffiti (mapx+screenx, mapy+screeny, graf, d))
          {
            SLtt_beep();
            strcpy (message, "Not enough room for graffiti!");
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
	      strcpy (message, "Paint mode on");
	    } else
	    {
	      strcpy (message, "Paint mode off");
	      paintmode=false;
	    }
	    break;
	  default:
	    if (key < slbplacenkeys && slbplacekeys[key]!=255)
	    {
	      slb_place_room(slbplacekeys[key]);
	      if (paintmode)
	        paintroom=slbplacekeys[key];
	    } else
	    {
	      SLtt_beep();
        }
	}
    }
    slbposcheck();
}

/*
 * Covers cursor actions from non-help screens.
 */
static int cursor_actions (int key)
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
	mapx=85;
	screenx=85;
	break;
      case 5132 : // ctrl+page up
	mapy=0;
	screeny=0;
	break;
      case 5118 : // ctrl+page down
	mapy=85;
	screeny=85;
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
	return 0;
    }
    curposcheck();
    if ((omx+osx != screenx+mapx) || (omy+osy != screeny+mapy))
	// As we've moved, reset which things we're looking at
    for (i=0; i < 3; i++)
      for (j=0; j < 3; j++)
        vistng[i][j]=0;
    return 1;
}

/*
 * Action function - start the help mode.
 */
static void start_help ()
{
    strcpy (message,"Use arrow keys and page up/down to move, "
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
static void end_help (void)
{
    mode=helpformode;
    *message=0;
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
      oldslb = slb[screenx+mapx][screeny+mapy];
      slb[screenx+mapx][screeny+mapy]=room;
      update_square (screenx+mapx, screeny+mapy);
      return;
    }
    // And another...
    if ((markr>84) || (markb>84) ||	(markl<0) || (markt<0))
	return;
    for (x=markl; x <= markr; x++)
    {
	for (y=markt; y <= markb; y++)
	{
	    oldslb = slb[x][y];
	    slb[x][y]=room;
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
    if (screenx+mapx > 84 || screeny+mapy > 84 ||
	screeny+mapy < 0 || screeny+mapy < 0)
	return;
    if (!mark)
    {
	own[screenx+mapx][screeny+mapy]=purchaser;
	update_square (screenx+mapx, screeny+mapy);
	return;
    }
    // And another...
    if (markr > 84 || markb > 84 ||
	markl < 0 || markt < 0)
	return;
    for (x=markl; x <= markr; x++)
	for (y=markt; y <= markb; y++)
	    own[x][y]=purchaser;
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
    if (mapx+cols > 85)
	mapx = 85-cols;
    if (mapy+rows > 85)
	mapy = 85-rows;
    if (mapx < 0)
	mapx=0;
    if (mapy < 0)
	mapy=0;
}
