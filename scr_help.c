/*
 * scr_help.c
 *
 * Defines functions for initializing and displaying the help screen.
 *
 */

#include "scr_help.h"

#include "globals.h"
#include "action.h"

// Help variables

int helpformode=0;
int helpy=0;

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

void init_help (void)
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
      strip_crlf(buffer);
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

char ***match_title (char *title, int n)
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
}

extern void draw_help(void)
{
    int i;
    screen_setcolor(0);
    for (i=0; i < rows; i++)
    {
      set_cursor_pos(i,0);
      char *help_line="";
      if (helpy+i < helprows)
          help_line=helptext[helpy+i];
      screen_printf_toeol("%s",help_line);
    }
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

