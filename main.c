/*
 * main.c   contains main() and initialization functions
 */

#include "main.h"
#include "action.h"
#include "globals.h"

int finished;
int mode=MD_SLB;
int mark=false;
int helpformode=0;
int helpy=0;

int rows;
int cols;

/* Amount of width the key takes up */
int keycols=40;

/* Location of cursor on screen (where appropriate) */
int screenx=0, screeny=0;
/* Location of top left corner of screen in map (where appropriate */
int mapx=0, mapy=0;

/* Which subtile is being considered in thing and data modes */
int sx=1, sy=1;

int markl, markr, markt, markb, markx, marky;

/* 0 for high-low, 1 for low-high */
int datmode=0;

int default_clm=0; /* Have we already set up the clm entries */

int vistng[3][3]; /* Which number are we looking at on each subtile */

char creatinput[5];

/* Global map variables */

unsigned char **slb;
unsigned char **own;
unsigned char **wib;

/* Note that I store action points as things in here, and separate them
 * out at save time. This makes lots of things a lot easier */

unsigned int apt_tot; /* Total number of action points */
unsigned int tng_tot; /* Number of things in total */
unsigned char **action_used=NULL; 
/* Which action point numbers are taken, and where their data is */

/* 
 * Note we assume there will be less than 1024 action points. I think
 * this should do... the most DK uses is 13
 */

unsigned char ****tng_lookup=NULL; /* Index to things by subtile */
unsigned short **tng_nums=NULL; /* Number of things in a tile */
unsigned short **tng_subnums=NULL; /* Number of things in a subtile */

/* 
 * Exceptionally grotty hack - we never need the actual data
 * stored in the .dat file, only what the high and low bytes should
 * be. So long as we remember this when we generate the "standard"
 * dungeon things, we'll be fine
 */
unsigned char **dat_low=NULL;
unsigned char **dat_high=NULL;

unsigned char **clm=NULL; /* clm file */

char *slbkey;
unsigned char *slbplacekeys;
int slbplacenkeys;
int paintown=false;
unsigned char paintroom=0;
int paintmode=0;
char *filebase;
char message[LINEMSG_SIZE];
char tngclipboard[TNGCLIPBRD_SIZE];

/* Help stuff */

int clmhelprows=0;
char **clmhelp=NULL;
int slbkeyhelprows=0;
char **slbkeyhelp=NULL;
int crtkeyhelprows=0;
char **crtkeyhelp=NULL;
int slbhelprows=0;
char **slbhelp=NULL;
int tnghelprows=0;
char **tnghelp=NULL;
int crthelprows=0;
char **crthelp=NULL;
int helprows=0;
char **helptext=NULL;

int main(int argc, char **argv)
// Main function; initializes variables, loads map from commandline
// parameters and enters the main program loop.
{
    char mapname[DISKPATH_SIZE];
    // Do something else here
#if defined(unix) && !defined (GO32)
    filebase="levels";
#else
    filebase="levels";
#endif
    // variable that informs if main program loop should finish
    finished=false;
    // say the clipboard is empty to start with
    tngclipboard[2]=0;
    // initialize slbkey array
    init_slbkey();
    // read help file
    init_help();
    // initialize keyboard shortcuts
    init_keys();
    // read configuration file
    read_init();
    // Interpreting command line parameters
    if ((argc==2) && (atoi(argv[1])>0))
      snprintf(mapname, DISKPATH_SIZE, "%s%smap%.5d", filebase,SEPARATOR, atoi(argv[1]));
    else
    if (argc==2)
      strncpy(mapname,argv[1],DISKPATH_SIZE);
    else
      strcpy(mapname,"");
    //Loading map
    load_map(mapname);
    init_slang();
    create_default_clm();
    check_doors();
    do 
    {
      draw_scr();
      proc_key();
    } while (!finished);
    done();
    return 0;
}

static void read_init(void)
{
    char buffer[READ_BUFSIZE];
    char *p;
    int l;
    
    FILE *fp;
    fp = fopen ("map.ini", "rb");
    // If we can't get anything, warn but don't die
    if (!fp)
    {
      strncpy (message, "Couldn't open map.ini, defaults loaded.",LINEMSG_SIZE);
      return;
    } else
    {
      strncpy (message, "Welcome to Adikted. Press F1 for help.",LINEMSG_SIZE);
    }
    while (fgets(buffer, READ_BUFSIZE-1, fp))
    {
      //Searchinf for a line containing equal sign
      p = strchr (buffer, '=');
      if (p==NULL)
	    continue;
      strip_crlf(buffer);
      (*p)=0;
      p++;
      while ((*p==' ')||(*p=='\t')) p++;
	if (strchr (buffer, ' '))
	    *strchr (buffer, ' ')=0;
	if (!strcmp(buffer, "datmode"))
	    datmode=atoi (p);
	if (!strcmp(buffer, "filebase"))
	{
	    filebase=strdup(p);
	    if (strchr(filebase,' '))
		*strchr(filebase,' ')=0;
	    l = strlen (filebase);
	    if (l)
		if (filebase[l-1]==SEPARATOR[0])
		    filebase[l-1]=0;
	}
    }
}

static void init_keys (void)
{
    int i=0;
    //Arrays storing keyboard shortcuts and corresponding rooms
    static const unsigned int keys[]={'t', 'L', 'H', 'T', 'l', '%',
	'W', 'G', 'P', 'O', 'B', 'e', 'g', 'S', 's', 'h', '#', '.', '-',
	' ', '/', '^', '\\', '&', '*', '(', '$', '!', '~', '=', 
	'w', 'b', 'i', 'm', 'E', 0};
    static const unsigned char rooms[]={16, 38, 36, 24, 18, 51,
	28, 53, 20, 22, 40, 32, 34, 30, 30, 26, 0, 2, 10, 
	11, 3, 4, 5, 6, 7, 8, 1, 52, 13, 12, 42, 44, 46, 48, 14, 0};

    if (sizeof(keys)/sizeof(*keys) != sizeof(rooms)/sizeof(*rooms))
	die ("Number of rooms doesn't match number of keys in init_room");

    while (keys[i])
    {
	if (keys[i] > slbplacenkeys)
	    slbplacenkeys=keys[i];
	i++;
    }
    slbplacenkeys++;
    slbplacekeys=(unsigned char *)malloc(slbplacenkeys*sizeof(char));
    if (!slbplacekeys)
	die ("Can't alloc memory for key table");

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

static void init_help (void)
{
    char buffer [256];
    char title[256];
    int n=0;
    char ***what;
    FILE *fp;
    strcpy (title, "");
    fp = fopen ("map.hlp", "rb");
    // Non-fatal error
    if (!fp)
	return;
    while (fgets (buffer, 255, fp))
    {
	strip_crlf (buffer);
	if (buffer[0]=='[' && buffer[strlen (buffer)-1]==']')
	{
	    what = match_title (title, n);
	    if (n && what)
	    {
		*what = (char **)malloc (n*sizeof (char *));
		if (!*what)
		    die ("Out of memory");
	    }
	    n=0;
	    strcpy (title, buffer+1);
	    title[strlen(title)-1]=0;
	}
	else
	    n++;
    }
    fseek (fp, (long)0, SEEK_SET);
    strcpy (title, "");
    what=NULL;
    n=0;
    while (fgets (buffer, 255, fp))
    {
	strip_crlf(buffer);
	if (buffer[0]=='[' && buffer[strlen (buffer)-1]==']')
	{
	    strcpy (title, buffer+1);
	    title[strlen(title)-1]=0;
	    what=match_title (title, -1);
	    n=0;
	}
	else
	{
	    if (what)
	    {
		(*what)[n]=(char *)malloc ((strlen (buffer)+1)*sizeof(char));
		if (!(*what)[n])
		    die ("Out of memory");
		strcpy ((*what)[n], buffer);
	    }
	    n++;
	}
    }
    fseek (fp, (long)0, SEEK_SET);
    fclose (fp);
}

static char ***match_title (char *title, int n)
{
    static char *titles[]={"slbkeyhelp", "tngkeyhelp", "crtkeyhelp", 
	"slbhelp", "tnghelp", "crthelp", "clmhelp", NULL};
    int i=0;

    while (titles[i] && strcmp (titles[i], title))
	i++;
    switch (i)
    {
      case 0 :
	if (n!=-1)
	    slbkeyhelprows=n;
	return &slbkeyhelp;
      case 2 :
	if (n!=-1)
	    crtkeyhelprows=n;
	return &crtkeyhelp;
      case 3 :
	if (n!=-1)
	    slbhelprows=n;
	return &slbhelp;
      case 4 :
	if (n!=-1)
	    tnghelprows=n;
	return &tnghelp;
      case 5 :
	if (n!=-1)
	    crthelprows=n;
	return &crthelp;
      case 6 :
	if (n!=-1)
	    clmhelprows=n;
	return &clmhelp;
      default :
	return NULL;
    }
    /* Never reached, but what the hell :) */
    return NULL;
}

static void strip_crlf(char *string_in)
{
    int i;
    unsigned char *string = (unsigned char *) string_in;
    
    for(i=strlen((char *)string)-1;i>=0;i--)
    {
        if(string[i]<32)
            string[i]=0;
        else
            break;
    }
}

static void init_slbkey(void)
{
    static char got[]="#$./^\\&*()- =~E?t?l?P?O?T?h?W?S?e?g?H?L?B?wwbbiimm?%!G";
    int i;
    int l;
    slbkey=(char *)malloc (256);
    if (!slbkey)
	die ("Out of memory");
    l = strlen (got);
    for (i=0; i < 256; i++)
	slbkey[i]=got[i];
    for (i=l; i < 256; i++)
	slbkey[i]='?';
}

