#include "globals.h"

int get_str (char *prompt, char *buf);
void init_slang(void);
void die (char *x);
void done(void);
void draw_scr (void);

static void mark_check(void);
static char *mode_status (int mode);
static void get_screen_size (void);
static void draw_creature (void);
static void draw_help(void);
static void draw_clm(void);
static void draw_slb (void);
static void draw_tng (void);
static void display_thing (unsigned char *thing, int x, int y);
static void display_action_point (unsigned char *point, int x, int y);
static void display_tngdat (void);
static char get_thing_char (int x, int y);

#ifdef MSDOS
#define ABORT 34		       /* scan code for ^G */
#else
#define ABORT 7			       /* character code for ^G */
#endif

#if defined(unix) && !defined(GO32)
int sigwinch (int sigtype);
void update (void);
volatile int safe_update, update_required;
#endif

int slang_going=0;

/*
 * Draw the screen
 */
void draw_scr (void)
{
    if (mark)
	mark_check();
    SLtt_set_cursor_visibility (0);
    rows = SLtt_Screen_Rows - 2;
    cols = SLtt_Screen_Cols - keycols;
    /* If we don't have much room at all, just forget it! */
    if (cols < 0)
	return;
    SLsmg_gotorc (SLtt_Screen_Rows-2, 0);
    SLsmg_set_color(2);
    /* Hope there's enough room for it :) */
    if (SLtt_Screen_Cols > 70)
	SLsmg_printf("Dungeon Keeper Map Editor    %5.5s mode  %s", 
		     modenames[mode], mode_status (mode));
    SLsmg_erase_eol();
    SLsmg_set_color (0);
    SLsmg_gotorc (SLtt_Screen_Rows-1, 0);
    if (!message[0] && is_graffiti (screenx+mapx, screeny+mapy)!=-1)
    {
	strcpy (message, "Graffiti: ");
	strncpy (message+10, get_graffiti (is_graffiti 
					   (screenx+mapx, screeny+mapy)), 60);
	message[70]=0;
    }
    SLsmg_write_string (message);
    SLsmg_erase_eol();
    switch (mode)
    {
      case MD_SLB:
	draw_slb();
	break;
      case MD_TNG:
	draw_tng();
	break;
      case MD_CRTR:
	draw_creature();
	break;
      case MD_HELP:
	draw_help();
	break;
      case MD_CLM:
	draw_clm();
      default:
	break;
    }
    SLsmg_gotorc (SLtt_Screen_Rows-1, SLtt_Screen_Cols-1);
    SLsmg_refresh ();
}

static void display_tngdat (void)
{
    int i, j;
    int cx, cy;
    int col;
    unsigned char *thing;

    cx = screenx+mapx;
    cy = screeny+mapy;
    
    /* Display .dat stuff for this tile */
    if (SLtt_Screen_Rows > 7)
    {
	SLsmg_set_color (0);
	SLsmg_gotorc (SLtt_Screen_Rows-10, cols+3);
	SLsmg_write_string (".dat entries");
	for (i=0; i < 3; i++)
	{
	    for (j=0; j < 3; j++)
	    {
		SLsmg_gotorc (SLtt_Screen_Rows-8+j*2, cols+3+i*5);
		
		if (i==sx && j==sy && mode==MD_CLM)
		    col=45;
		else
		    col=0;
		SLsmg_set_color (col);
		if (datmode==0)
		    SLsmg_printf ("%02X%02X",
			      dat_high[cx*3+i][cy*3+j],
			      dat_low[cx*3+i][cy*3+j]);
	    }
	}
	SLsmg_gotorc (SLtt_Screen_Rows-10, cols+20);
	SLsmg_set_color (0);
	SLsmg_write_string (".tng entries");
	for (i=0; i < 3; i++)
	{
	    for (j=0; j < 3; j++)
	    {
		SLsmg_set_color (0);
		SLsmg_gotorc (SLtt_Screen_Rows-8+j*2, cols+20+i*6);
		SLsmg_write_string ("     ");
		SLsmg_gotorc (SLtt_Screen_Rows-7+j*2, cols+20+i*6);
		if (tng_subnums [cx*3+i][cy*3+j] > 1)
		{
		    if (mode==MD_TNG)
			SLsmg_printf ("(%d/%d)", vistng[i][j]+1,
				      tng_subnums[cx*3+i][cy*3+j]);
		    else
			SLsmg_printf ("(%d)   ", tng_subnums[cx*3+i][cy*3+j]);
		}
		else
		    SLsmg_write_string ("     ");
		SLsmg_gotorc (SLtt_Screen_Rows-8+j*2, cols+20+i*6);
		if (i==sx && j==sy && mode==MD_TNG)
		    col=10;
		else
		    col=0;
		if (vistng[i][j] < tng_subnums[cx*3+i][cy*3+j])
		{
		    thing=tng_lookup[cx*3+i][cy*3+j][vistng[i][j]];
		    if (is_action_thing (thing))
		    {
			col+=35;
			SLsmg_set_color (col);
			SLsmg_printf ("action");
		    }
		    else if (thing[6]==5)
		    {
			col += 30+thing[8];
			SLsmg_set_color (col);
			SLsmg_printf ("%s", creature_names[thing[7]]);
		    }
		    else
		    {
			col+=35;
			SLsmg_set_color (col);
			SLsmg_printf ("thing");
		    }
		}
		else
		{
		    col+=36;
		    SLsmg_set_color (col);
		    SLsmg_write_string ("     ");
		}
	    }
	}
    }
}
    
void show_cursor (char cur)
{
    SLsmg_gotorc (screeny, screenx);
    SLsmg_set_color (9);
    SLsmg_write_char (cur);
    SLsmg_gotorc (SLtt_Screen_Rows-1, SLtt_Screen_Cols-1);
}

static void draw_slb (void)
{
    int i, j, k, g;
    int col;
    char *t;
    static char *hilite[4]={"wood", "braced", "iron", "magic"};
    
    for (i=0; i < rows; i++)
    {
	SLsmg_set_color(0);
	SLsmg_gotorc (i, 0);
	if (i+mapy > 84)
	{
	    for (j=0; j < cols; j++)
		SLsmg_write_char (' ');
	}
	else
	{
	    for (j=0; j < cols; j++)
	    {
		if (j+mapx <= 84)
		{
		    g = is_graffiti (j+mapx, i+mapy);
		    /* Are we marking? */
		    if (g==-1)
		    {
			col = own[j+mapx][i+mapy]+10;
			if (slb[j+mapx][i+mapy]==0 ||
			    slb[j+mapx][i+mapy]==2 ||
			    slb[j+mapx][i+mapy]==3)
			    col=16;
		    }
		    else
			col=7;
		    if (mark && j+mapx >= markl && j+mapx <= markr
			&& i+mapy >= markt && i+mapy <= markb)
			col+=10;
		    SLsmg_set_color(col);
		    if (g==-1)
			SLsmg_write_char (slbkey[slb[j+mapx][i+mapy]]);
		    else
			SLsmg_write_char ('@');
		}
		else
		    SLsmg_write_char (' ');
	    }
	}
	SLsmg_set_color (2);
	SLsmg_write_char (' ');
	SLsmg_write_char (' ');
	SLsmg_set_color (0);
	SLsmg_write_char (' ');
	if (i < slbkeyhelprows)
	{
	    SLsmg_write_string (slbkeyhelp[i]);
	    SLsmg_erase_eol();
	    SLsmg_set_color (5);
	    t = slbkeyhelp[i]+1;
	    while ((t = strchr (t, ':')))
	    {
		SLsmg_gotorc (i, cols+2+(t-slbkeyhelp[i]));
		SLsmg_write_char (*(t-1));
		t++;
	    }
	    /* Grotty hack time */
	    for (k=0; k < 4; k++)
	    {
		if ((t=strstr (slbkeyhelp[i], hilite[k])))
		{
		    SLsmg_gotorc (i, cols+3+(t-slbkeyhelp[i]));
		    SLsmg_write_char (hilite[k][0]);
		}
	    }
	}
	else
	    SLsmg_erase_eol();
    }
    display_tngdat();
    if (is_graffiti(screenx+mapx, screeny+mapy)==-1)
	show_cursor (slbkey[slb[screenx+mapx][screeny+mapy]]);
    else
	show_cursor ('@');
}

static void draw_clm (void)
{
    int i, j, g;
    int col;
    unsigned char *c;
    
    int cx, cy;

    cx = screenx+mapx;
    cy = screeny+mapy;

    for (i=0; i < rows; i++)
    {
	SLsmg_set_color(0);
	SLsmg_gotorc (i, 0);
	if (i+mapy > 84)
	{
	    for (j=0; j < cols; j++)
		SLsmg_write_char (' ');
	}
	else
	{
	    for (j=0; j < cols; j++)
	    {
		if (j+mapx <= 84)
		{
		    g = is_graffiti (j+mapx, i+mapy);
		    /* Are we marking? */
		    if (g==-1)
		    {
			col = own[j+mapx][i+mapy]+10;
			if (slb[j+mapx][i+mapy]==0 ||
			    slb[j+mapx][i+mapy]==2 ||
			    slb[j+mapx][i+mapy]==3)
			    col=16;
		    }
		    else
			col=7;
		    if (mark && j+mapx >= markl && j+mapx <= markr
			&& i+mapy >= markt && i+mapy <= markb)
			col+=10;
		    SLsmg_set_color(col);
		    if (g==-1)
			SLsmg_write_char (slbkey[slb[j+mapx][i+mapy]]);
		    else
			SLsmg_write_char ('@');
		}
		else
		    SLsmg_write_char (' ');
	    }
	}
	SLsmg_set_color (2);
	SLsmg_write_char (' ');
	SLsmg_write_char (' ');
	SLsmg_set_color (0);
	SLsmg_write_char (' ');
	SLsmg_erase_eol();
    }
    SLsmg_set_color (0);
    SLsmg_gotorc (1, cols+3);
    c = clm [0x10000-((dat_high[cx*3+sx][cy*3+sy]<<8)+
		      dat_low[cx*3+sx][cy*3+sy])];
    SLsmg_printf ("Use: %04X ", c[0]+(c[1]<<8));
    SLsmg_gotorc (1, cols+23);
    SLsmg_printf ("Permanent: %d", c[2]&1);
    SLsmg_gotorc (2, cols+3);
    SLsmg_printf ("Lintel: %d", (c[2]>>1)&7);
    SLsmg_gotorc (2, cols+23);
    SLsmg_printf ("Height: %X", (c[2]>>4)&15);
    SLsmg_gotorc (3, cols+3);
    SLsmg_printf ("Solid mask: %04X", c[3]+(c[4]<<8));
    SLsmg_gotorc (3, cols+23);
    SLsmg_printf ("Orientation: %02X", c[7]);
    SLsmg_gotorc (4, cols+3);
    SLsmg_printf ("Base block: %03X", c[5]+(c[6]<<8));
    for (i=0; i < 8; i++)
    {
	SLsmg_gotorc (5+i, cols+3);
	SLsmg_printf ("Cube %d: %03X", i, c[8+i*2]+(c[9+i*2]<<8));
    }
    display_tngdat();
    if (is_graffiti(screenx+mapx, screeny+mapy)==-1)
	show_cursor (slbkey[slb[screenx+mapx][screeny+mapy]]);
    else
	show_cursor ('@');
}

/*
 * Get the screen size.
 */
static void get_screen_size (void)
{
    int r = 0, c = 0;

#ifdef TIOCGWINSZ
    struct winsize wind_struct;

    if ((ioctl(1,TIOCGWINSZ,&wind_struct) == 0)
	|| (ioctl(0, TIOCGWINSZ, &wind_struct) == 0)
	|| (ioctl(2, TIOCGWINSZ, &wind_struct) == 0)) {
        c = (int) wind_struct.ws_col;
        r = (int) wind_struct.ws_row;
    }
#elif defined(MSDOS)
    union REGS regs;

    regs.h.ah = 0x0F;
    int86 (0x10, &regs, &regs);
    c = regs.h.ah;

    regs.x.ax = 0x1130, regs.h.bh = 0;
    int86 (0x10, &regs, &regs);
    r = regs.h.dl + 1;
#endif

    if ((r <= 0) || (r > 200)) r = 24;
    if ((c <= 0) || (c > 250)) c = 80;
    SLtt_Screen_Rows = r;
    SLtt_Screen_Cols = c;
}

/*
 * Initialise stuff at the beginning of the program: mostly the
 * SLang terminal and screen management library.
 */
void init_slang(void) 
{
    SLtt_get_terminfo();

   // SLkp_init assumes that SLtt_get_terminfo has been called.
   if (SLkp_init() == -1)
     {
        fprintf(stderr, "axe: SLkp_init: returned error code\n");
        exit(1);
     }

    //SLang_init_tty initializes terminal for reading single characters
    //definition: int SLang_init_tty (int abort_char, int flow_ctrl, int opost)
    if (SLang_init_tty(ABORT,1,0) == -1) 
    {
        fprintf(stderr, "axe: SLang_init_tty: returned error code\n");
        exit(1);
    }
    SLang_set_abort_signal(NULL);

    get_screen_size ();
    SLtt_Use_Ansi_Colors = true;
#if defined(unix) && !defined(GO32)
    signal (SIGWINCH, (void *) sigwinch);
#endif

    if (SLsmg_init_smg()!=0)
    {
      fprintf(stderr, "adikted: SLsmg_init_smg: returned error code\n");
      SLang_reset_tty ();
      exit (1);
    }

    SLtt_set_color (0, "buffer", "lightgray", "black");
    SLtt_set_color (1, "select", "black", "lightgray");
    SLtt_set_color (2, "status", "yellow", "blue");
    SLtt_set_color (3, "escape", "brightred", "black");
    SLtt_set_color (4, "invalid", "yellow", "black");
    SLtt_set_color (5, "invalid", "white", "black");
    SLtt_set_color (7, "graffiti", "brightmagenta", "black");
    SLtt_set_color (9, "cursor", "red", "white");
    
    /* Used in slb/tng main display */    
    SLtt_set_color (10,"Keeper 0","white","red");
    SLtt_set_color (11,"Keeper 1","white","cyan");
    SLtt_set_color (12,"Keeper 2","white","green");
    SLtt_set_color (13,"Keeper 3","white","brown");
    SLtt_set_color (14,"Keeper 4","white","blue");
    SLtt_set_color (15,"Unclaimed","lightgray","black");
    SLtt_set_color (16,"Dirt/rock","gray","black");
    SLtt_set_color (17, "graffiti", "magenta", "white");
    SLtt_set_color (20,"Keeper 0","red","white");
    SLtt_set_color (21,"Keeper 1","cyan","white");
    SLtt_set_color (22,"Keeper 2","green", "white");
    SLtt_set_color (23,"Keeper 3","yellow", "lightgray");
    SLtt_set_color (24,"Keeper 4","blue", "lightgray");
    SLtt_set_color (25,"Unclaimed", "black", "lightgray");
    SLtt_set_color (26,"Dirt/rock","black", "lightgray");
    
    /* Used in dat display */
    SLtt_set_color (30,"Keeper 0","brightred","black");
    SLtt_set_color (31,"Keeper 1","brightcyan","black");
    SLtt_set_color (33,"Keeper 2","brightgreen", "black");
    SLtt_set_color (33,"Keeper 3","yellow", "black");
    SLtt_set_color (34,"Keeper 4","brightblue", "black");
    SLtt_set_color (35,"Unclaimed", "lightgray", "black");
    SLtt_set_color (36,"Blank","lightgray","black");
    SLtt_set_color (40,"Keeper 0","white","red");
    SLtt_set_color (41,"Keeper 1","white","cyan");
    SLtt_set_color (42,"Keeper 2","white","green");
    SLtt_set_color (43,"Keeper 3","white","brown");
    SLtt_set_color (44,"Keeper 4","white","blue");
    SLtt_set_color (45,"Unclaimed","black","lightgray");
    SLtt_set_color (46,"Blank","black","lightgray");

    SLang_flush_input(); 

    slang_going=1;
}

#if defined(unix) && !defined(GO32)
int sigwinch (int sigtype) {
    if (safe_update)
	update();
    else
	update_required = TRUE;
    signal (SIGWINCH, (void *) sigwinch);
    return 0;
}

void update (void) 
{
    SLsmg_reset_smg ();
    get_screen_size ();
    SLsmg_init_smg ();
    draw_scr ();
}
#endif

/*
 * Clean up all the stuff that init() did.
 */
void done(void) 
{
    SLtt_set_cursor_visibility (1);
    SLsmg_reset_smg ();
    SLang_reset_tty ();
    slang_going=0;
}

void die (char *x)
{
    if (slang_going)
	done();
    fprintf (stderr, "%s\n", x);
    exit (1);
}

static char get_thing_char (int x, int y)
{
    switch (tng_nums[x][y])
    {
      case 0 :
	return ' ';
	break;
      case 1 :
      case 2 :
      case 3 :
      case 4 :
      case 5 :
      case 6 :
      case 7 :
      case 8 :
      case 9 :
	return (char)(tng_nums[x][y]+'0');
	break;
      default:
	return '+';
    }
    /* Daft, but we need it to keep the compiler happy */
    return ' ';
}

static void draw_help (void)
{
    int i;
    
    SLsmg_set_color (0);
    for (i=0; i < rows; i++)
    {
	SLsmg_gotorc(i,0);
	if (helpy+i < helprows)
	    SLsmg_write_string (helptext[helpy+i]);
	SLsmg_erase_eol();
    }
    SLsmg_gotorc (SLtt_Screen_Rows-1, SLtt_Screen_Cols-1);
}

static void draw_creature (void)
{
    int i;
    for (i=0; i < rows; i++)
    {
	SLsmg_set_color(0);
	SLsmg_gotorc (i, 0);
	/* Just do two columns. If this mucks up, so be it */
	if (i < 16)
	{
	    SLsmg_printf ("%2d: %-16.16s", i+1, creature_fullnames[i+1]);
	    if (i+17 < 32)
		SLsmg_printf ("%2d: %-16.16s", i+17, creature_fullnames[i+17]);
	}
	SLsmg_erase_eol();
	SLsmg_gotorc (i, cols);
	SLsmg_set_color (2);
	SLsmg_write_char (' ');
	SLsmg_write_char (' ');
	SLsmg_set_color (0);
	SLsmg_write_char (' ');
	if (i < crtkeyhelprows)
	    SLsmg_write_string (crtkeyhelp[i]);
	SLsmg_erase_eol();
    }
    SLsmg_gotorc (SLtt_Screen_Rows-1, 17+strlen (creatinput));
}

static void draw_tng (void)
{
    int i, j;
    int cx, cy;

    cx = screenx+mapx;
    cy = screeny+mapy;

    for (i=0; i < rows; i++)
    {
	SLsmg_set_color(0);
	SLsmg_gotorc (i, 0);
	if (i+mapy > 84)
	{
	    for (j=0; j < cols; j++)
		SLsmg_write_char (' ');
	}
	else
	{
	    for (j=0; j < cols; j++)
	    {
		if (j+mapx <= 84)
		{
		    SLsmg_set_color(own[j+mapx][i+mapy]+10);
		    SLsmg_write_char (get_thing_char(mapx+j, mapy+i));
		}
		else
		    SLsmg_write_char (' ');
	    }
	}
	SLsmg_set_color (2);
	SLsmg_write_char (' ');
	SLsmg_write_char (' ');
	SLsmg_set_color (0);
	SLsmg_write_char (' ');
	SLsmg_erase_eol();
    }
    if (vistng[sx][sy] < tng_subnums[cx*3+sx][cy*3+sy])
	display_thing (tng_lookup[cx*3+sx][cy*3+sy][vistng[sx][sy]], 
		       cols+3, 1);
    display_tngdat();
    show_cursor (get_thing_char (screenx+mapx, screeny+mapy));
    return;
}

static void display_thing (unsigned char *thing, int x, int y)
{
    int m, i;

    if (is_action_thing(thing))
    {
	display_action_point(thing, x, y);
	return;
    }
    SLsmg_set_color (0);
    SLsmg_gotorc (y, x);
    SLsmg_write_string ("Thing data block:");
    for (m=0; m < 3; m++)
    {
	SLsmg_gotorc (m+y+1, x);
	for (i=0; i < 7; i++)
	    SLsmg_printf ("%02X ", (unsigned int)thing[m*7+i]);
	SLsmg_erase_eol();
    }
    SLsmg_gotorc (y+5, x);
    SLsmg_printf ("Subtile: %3d,%3d (Tile: %2d, %2d)", thing[1], thing[3],
		  thing[1]/3, thing[3]/3);
    SLsmg_gotorc (y+6, x);
    SLsmg_printf ("Position within subtile: %3d, %3d", thing[0], thing[2]);
    SLsmg_gotorc (y+7, x);
    SLsmg_write_string ("Thing type: ");
    
    /* Creature */
    if (thing[6]==5)
    {
	SLsmg_write_string ("Creature");
	SLsmg_gotorc (y+8, x);
	SLsmg_printf ("Type: %s", creature_fullnames[thing[7]]);
	SLsmg_gotorc (y+9, x);
	SLsmg_printf ("Level: %d", thing[14]+1);
	SLsmg_gotorc (y+10, x);
	SLsmg_printf ("Owner: %s", owners[thing[8]]);
    }
    /* Trap */
    if (thing[6]==8)
    {
	SLsmg_write_string ("Trap");
	SLsmg_gotorc (y+8, x);
	SLsmg_printf ("Type : %s", traps[thing[7]]);
	SLsmg_gotorc (y+9, x);
	SLsmg_printf ("Owner: %s", owners[thing[8]]);
    }
    /* Door */
    if (thing[6]==9)
    {
	SLsmg_write_string ("Door");
	SLsmg_gotorc (y+8, x);
	SLsmg_printf ("Type : %s", doors[thing[7]]);
	SLsmg_gotorc (y+9, x);
	SLsmg_printf ("Owner: %s", owners[thing[8]]);
	SLsmg_gotorc (y+10, x);
	if (thing[14])
	    SLsmg_printf ("Locked");
	else
	    SLsmg_printf ("Unlocked");
    }
    /* Room effect */
    if (thing[6]==7)
    {
	SLsmg_write_string ("Room effect");
	SLsmg_gotorc (y+8, x);
	switch (thing[7])
	{
	  case 2 :
	    SLsmg_write_string ("Dripping water");
	    break;
	  case 4:
	    SLsmg_write_string ("Dry ice");
	    break;
	  case 5:
	    SLsmg_write_string ("Dry ice");
	}
    }
    /* Object */
    if (thing[6]==1)
    {
	SLsmg_write_string ("Object");
	SLsmg_gotorc (y+8, x);
	SLsmg_write_string ("Type: ");
	switch (thing[7])
	{
	  case 2 :
	    SLsmg_write_string ("Torch");
	    break;
	  case 3 :
	    SLsmg_write_string ("Gold (500)");
	    break;
	  case 4 :
	    SLsmg_write_string ("Temple statue");
	    break;
	  case 5 :
	    SLsmg_write_string ("Dungeon heart");
	    break;
	  case 6 :
	    SLsmg_write_string ("Gold (250)");
	    break;
	  case 7:
	    SLsmg_write_string ("Unlit torch");
	    break;
	  case 8:
	    SLsmg_write_string ("Statue");
	    break;
	  case 9:
	    SLsmg_write_string ("Chicken");
	    break;
	  case 0xb:
	    SLsmg_write_string ("Hand of evil spell");
	    break;
	  case 0xc:
	    SLsmg_write_string ("Create imp spell");
	    break;
	  case 0xd:
	    SLsmg_write_string ("Must obey spell");
	    break;
	  case 0xe:
	    SLsmg_write_string ("Slap spell");
	    break;
	  case 0xf:
	    SLsmg_write_string ("Sight of evil spell");
	    break;
	  case 0x10:
	    SLsmg_write_string ("Call to arms spell");
	    break;
	  case 0x11:
	    SLsmg_write_string ("Cave in spell");
	    break;
	  case 0x12:
	    SLsmg_write_string ("Heal creature spell");
	    break;
	  case 0x13:
	    SLsmg_write_string ("Hold audience spell");
	    break;
	  case 0x14:
	    SLsmg_write_string ("Lightning spell");
	    break;
	  case 0x15:
	    SLsmg_write_string ("Speed creature spell");
	    break;
	  case 0x16:
	    SLsmg_write_string ("Protect creature spell");
	    break;
	  case 0x17:
	    SLsmg_write_string ("Conceal creature spell");
	    break;
	  case 0x1a:
	    SLsmg_write_string ("Anvil");
	    break;
	  case 0x1b:
	    SLsmg_write_string ("Prison bar");
	    break;
	  case 0x1c :
	    SLsmg_write_string ("Candlestick");
	    break;
	  case 0x1d :
	    SLsmg_write_string ("Gravestone");
	    break;
	  case 0x1e:
	    SLsmg_write_string ("Statue");
	    break;
	  case 0x1f :
	    SLsmg_write_string ("Training post");
	    break;
	  case 0x20 :
	    SLsmg_write_string ("Torture spike");
	    break;
	  case 0x28:
	    SLsmg_write_string ("Chicken");
	    break;
	  case 0x29:
	    SLsmg_write_string ("Chicken");
	    break;
	  case 0x2b:
	    SLsmg_write_string ("Chicken");
	    break;
	  case 0x2c:
	    SLsmg_write_string ("Spinning key");
	    break;
	  case 0x2d:
	    SLsmg_write_string ("Disease spell");
	    break;
	  case 0x2e:
	    SLsmg_write_string ("Chicken spell");
	    break;
	  case 0x2f:
	    SLsmg_write_string ("Destroy walls spell");
	    break;
	  case 0x30:
	    SLsmg_write_string ("Time bomb spell");
	    break;
	  case 0x31:
	    SLsmg_write_string ("Hero gate");
	    break;
	  case 0x56 : 
	    SLsmg_write_string ("Reveal map");
	    break;
	  case 0x57:
	    SLsmg_write_string ("Resurrect creature");
	    break;
	  case 0x58 :
	    SLsmg_write_string ("Transfer creature");
	    break;
	  case 0x59:
	    SLsmg_write_string ("Steal hero");
	    break;
	  case 0x5a:
	    SLsmg_write_string ("Multiply creatures");
	    break;
	  case 0x5b :
	    SLsmg_write_string ("Increase level");
	    break;
	  case 0x5c :
	    SLsmg_write_string ("Make safe");
	    break;
	  case 0x5d:
	    SLsmg_write_string ("Reveal hidden world");
	    break;
	  case 0x5e:
	    SLsmg_write_string ("Boulder trap box");
	    break;
	  case 0x5f:
	    SLsmg_write_string ("Alarm trap box");
	    break;
	  case 0x60:
	    SLsmg_write_string ("Poison gas trap box");
	    break;
	  case 0x61:
	    SLsmg_write_string ("Lightning trap box");
	    break;
	  case 0x62:
	    SLsmg_write_string ("Word of Power trap box");
	    break;
	  case 0x63:
	    SLsmg_write_string ("Lava trap box");
	    break;
	  case 0x6f :
	    SLsmg_write_string ("Heart flame (red)");
	    break;
	  case 0x71 :
	    SLsmg_write_string ("Scavenger");
	    break;
	  case 0x72 :
	    SLsmg_write_string ("Workshop machine");
	    break;
	  case 0x73:
	    SLsmg_write_string ("Flag (red)");
	    break;
	  case 0x74:
	    SLsmg_write_string ("Flag (blue)");
	    break;
	  case 0x75:
	    SLsmg_write_string ("Flag (green)");
	    break;
	  case 0x76:
	    SLsmg_write_string ("Flag (yellow)");
	    break;
	  case 0x77:
	    SLsmg_write_string ("Flagpost");
	    break;
	  case 0x78 :
	    SLsmg_write_string ("Heart flame (blue)");
	    break;
	  case 0x79 :
	    SLsmg_write_string ("Heart flame (green)");
	    break;
	  case 0x7a :
	    SLsmg_write_string ("Heart flame (yellow)");
	    break;
	  case 0x7d :
	    SLsmg_write_string ("Torturer");
	    break;
	  case 0x81 :
	    SLsmg_write_string ("Statue");
	    break;
	  case 0x86 :
	    SLsmg_write_string ("Armageddon");
	    break;
	  default :
	    SLsmg_write_string ("Unknown");
	    break;
	}
	SLsmg_gotorc (y+9, x);
	SLsmg_printf ("Owner: %s", owners[thing[8]]);
    }
}

static void display_action_point (unsigned char *point, int x, int y)
{
    SLsmg_set_color (0);
    SLsmg_gotorc (y, x);
    SLsmg_printf ("Action point number %d:", point[19]+(point[20]<<8));
    SLsmg_gotorc (y+1, x);
    SLsmg_printf ("Subtile: %3d,%3d (Tile: %2d, %2d)", point[1], point[3],
		  point[1]/3, point[3]/3);
    SLsmg_gotorc (y+2, x);
    SLsmg_printf ("Position within subtile: %3d, %3d", point[0], point[2]);
    SLsmg_gotorc (y+3, x);
    SLsmg_printf ("Unknown byte pair: %02X %02X", point[17], point[18]);
}

static char *mode_status (int mode)
{
    static char buffer[256];
    static const char *longmodenames[]={"slab", "thing", "add creature"};
    switch (mode)
    {
      case MD_CLM:
      case MD_TNG:
	sprintf (buffer, "   Position: %2d,%2d", mapx+screenx,
		 mapy+screeny);
	break;
      case MD_SLB:
	sprintf (buffer, "   Position: %2d,%2d", mapx+screenx,
		 mapy+screeny);
	if (mark)
	    strcat (buffer, " (Marking)");
	else if (paintmode)
	    strcat (buffer, " (Painting)");
	break;
      case MD_HELP:
	sprintf (buffer, "   (Help for %s mode)", longmodenames[helpformode]);
	break;
      default:
	strcpy (buffer, "");
	break;
    }
    return buffer;
}

/*
 * Get a string, in the "minibuffer". Return TRUE on success, FALSE
 * on break. Possibly syntax-highlight the entered string for
 * backslash-escapes, depending on the "highlight" parameter.
 */
int get_str (char *prompt, char *buf) 
{
    int maxlen = 79 - strlen(prompt);  /* limit to 80 - who cares? :) */
    int len = 0;
    int c;

    for (EVER) 
    {
	SLsmg_gotorc (SLtt_Screen_Rows-1, 0);
	SLsmg_set_color (0);
	SLsmg_write_string (prompt);
	SLsmg_write_nchars (buf, len);

	SLsmg_set_color (0);
	SLsmg_erase_eol();
	SLsmg_refresh();
#if defined(unix) && !defined(GO32)
	if (update_required)
	    update();
	safe_update = TRUE;
#endif
	c = SLang_getkey();
#if defined(unix) && !defined(GO32)
	safe_update = FALSE;
#endif
	if (c == 13)
	{
	    buf[len] = '\0';
	    return true;
	} 
	else if (c == 27 || c == 7)
	{
 	    SLtt_beep();
	    SLKeyBoard_Quit = 0;
            SLang_set_error (0);
	    strcpy (message, "User break");
	    return false;
	}
	
	if (c >= 32 && c <= 126)
	{
	    if (len < maxlen)
		buf[len++] = c;
	    else
		SLtt_beep();
	}

	if ((c == 127 || c == 8) && len > 0)
	    len--;

	if (c == 'U'-'@')	       /* ^U kill line */
	    len = 0;
    }
}

static void mark_check(void)
{
    if (!mark)
	return;
    if (markx <= mapx+screenx)
    {
	markl = markx;
	markr = mapx+screenx;
    }
    else
    {
	markr = markx;
	markl = mapx+screenx;
    }
    if (marky <= mapy+screeny)
    {
	markt = marky;
	markb = mapy+screeny;
    }
    else
    {
	markb = marky;
	markt = mapy+screeny;
    }
}

