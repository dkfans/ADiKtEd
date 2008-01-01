/*
 * scr_help.c
 *
 * Defines functions for initializing and displaying the help screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_help.h"

#include "globals.h"
#include "scr_actn.h"
#include "output_scr.h"
#include "input_kb.h"

// Help variables

const char *help_filename="map.hlp";

HELP_DATA *help;

short init_help(void)
{
    //Creating and clearing help variable
    help=(HELP_DATA *)malloc(sizeof(HELP_DATA));
    if (help==NULL)
     die("init_help: Cannot allocate memory.");
    help->formode=0;
    help->y=0;
    help->rows=0;
    help->text=NULL;

    help->clmrows=0;
    help->clm=NULL;
    help->slbkeyrows=0;
    help->slbkey=NULL;
    help->tngkeyrows=0;
    help->tngkey=NULL;
    help->crtkeyrows=0;
    help->crtkey=NULL;
    help->itmtkeyrows=0;
    help->itmtkey=NULL;
    help->txtrkeyrows=0;
    help->txtrkey=NULL;
    help->cclmkeyrows=0;
    help->cclmkey=NULL;
    help->cubekeyrows=0;
    help->cubekey=NULL;
    help->slblkeyrows=0;
    help->slblkey=NULL;
    help->slbrows=0;
    help->slb=NULL;
    help->tngrows=0;
    help->tng=NULL;
    help->crtrows=0;
    help->crt=NULL;
    help->itmtrows=0;
    help->itmt=NULL;
    help->scrprows=0;
    help->scrp=NULL;
    help->txtrrows=0;
    help->txtr=NULL;
    help->cclmrows=0;
    help->cclm=NULL;
    help->cuberows=0;
    help->cube=NULL;
    help->slblrows=0;
    help->slbl=NULL;
    help->tiprows=0;
    help->tips=NULL;
    help->compassrows=0;
    help->compass=NULL;

    char buffer[READ_BUFSIZE];
    char title[READ_BUFSIZE];
    int n=0;
    char ***what;
    FILE *fp;
    strcpy (title, "");
    fp = fopen (help_filename, "rb");
    // Non-fatal error
    if (!fp)
    {
      message_info("Help file \"%s\" not found.",help_filename);
      return false;
    }
    while (fgets(buffer, READ_BUFSIZE-1, fp))
    {
      strip_crlf(buffer);
      if (buffer[0]=='[' && buffer[strlen(buffer)-1]==']')
      {
          what = match_title (title, n);
          if (n && what)
          {
            *what = (char **)malloc(n*sizeof (char *));
            if (!*what)
                die ("init_help: Out of memory");
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
    while (fgets (buffer, READ_BUFSIZE-1, fp))
    {
      strip_crlf(buffer);
      if (buffer[0]=='[' && buffer[strlen(buffer)-1]==']')
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
            (*what)[n]=(char *)malloc((strlen (buffer)+1)*sizeof(char));
            if (!(*what)[n])
                die ("init_help: Out of memory");
            strcpy ((*what)[n], buffer);
          }
          n++;
      }
    }
    fseek (fp, (long)0, SEEK_SET);
    fclose (fp);
    return true;
}

/*
 * Deallocates memory for the help screen.
 */
void free_help(void)
{
  free(help);
}

/*
 * Covers actions from the help screen.
 */
void actions_help(int key)
{
    switch (key)
    {
    case KEY_UP:
      help->y--;
      break;
    case KEY_DOWN:
      help->y++;
      break;
    case KEY_PGUP:
      help->y-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      help->y+=scrmode->rows-1;
      break;
    default:
      end_help();
    }
    if (help->y+scrmode->rows > help->rows)
      help->y=help->rows-scrmode->rows;
    if (help->y < 0)
      help->y=0;
}

char ***match_title(char *title, int n)
{
    static const char *titles[]={
            "slbkeyhelp", "tngkeyhelp",   //0,1
            "crtkeyhelp", "itmtkeyhelp",  //2,3
            "slbhelp", "tnghelp",         //4,5
            "crthelp", "itmhelp",         //6,7
            "clmhelp", "tips",            //8,9
            "scrphelp", "txtrhelp",       //10,11
            "cclmhelp", "cubehelp",       //12,13
            "txtrkeyhelp", "cclmkeyhelp", //14,15
            "cubekeyhelp",                //16
            "slblhelp", "slblkeyhelp",    //17,18
            "compass", NULL};             //19
    int i=0;

    while (titles[i] && strcmp (titles[i], title))
      i++;
    switch (i)
    {
      case 0 :
      if (n!=-1)
          help->slbkeyrows=n;
      return &(help->slbkey);
      case 1 :
      if (n!=-1)
          help->tngkeyrows=n;
      return &(help->tngkey);
      case 2 :
      if (n!=-1)
          help->crtkeyrows=n;
      return &(help->crtkey);
      case 3 :
      if (n!=-1)
          help->itmtkeyrows=n;
      return &(help->itmtkey);
      case 4 :
      if (n!=-1)
          help->slbrows=n;
      return &(help->slb);
      case 5 :
      if (n!=-1)
          help->tngrows=n;
      return &(help->tng);
      case 6 :
      if (n!=-1)
          help->crtrows=n;
      return &(help->crt);
      case 7 :
      if (n!=-1)
          help->itmtrows=n;
      return &(help->itmt);
      case 8 :
      if (n!=-1)
          help->clmrows=n;
      return &(help->clm);
      case 9 :
      if (n!=-1)
          help->tiprows=n;
      return &(help->tips);
      case 10:
      if (n!=-1)
          help->scrprows=n;
      return &(help->scrp);
      case 11:
      if (n!=-1)
          help->txtrrows=n;
      return &(help->txtr);
      case 12:
      if (n!=-1)
          help->cclmrows=n;
      return &(help->cclm);
      case 13:
      if (n!=-1)
          help->cuberows=n;
      return &(help->cube);
      case 14:
      if (n!=-1)
          help->txtrkeyrows=n;
      return &(help->txtrkey);
      case 15:
      if (n!=-1)
          help->cclmkeyrows=n;
      return &(help->cclmkey);
      case 16:
      if (n!=-1)
          help->cubekeyrows=n;
      return &(help->cubekey);
      case 17:
      if (n!=-1)
          help->slblrows=n;
      return &(help->slbl);
      case 18:
      if (n!=-1)
          help->slblkeyrows=n;
      return &(help->slblkey);
      case 19:
      if (n!=-1)
          help->compassrows=n;
      return &(help->compass);
      default :
      return NULL;
    }
}

/*
 * Action function - start the help mode.
 */
short start_help(struct LEVEL *lvl)
{
    help->formode=scrmode->mode;
    help->y=0;
    switch (help->formode)
    {
      case MD_SLB:
        help->rows=help->slbrows;
        help->text=help->slb;
        break;
      case MD_CRTR:
        help->rows=help->crtrows;
        help->text=help->crt;
        break;
      case MD_ITMT:
        help->rows=help->itmtrows;
        help->text=help->itmt;
        break;
      case MD_TNG:
        help->rows=help->tngrows;
        help->text=help->tng;
        break;
      case MD_CLM:
        help->rows=help->clmrows;
        help->text=help->clm;
        break;
      case MD_SCRP:
        help->rows=help->scrprows;
        help->text=help->scrp;
        break;
      case MD_TXTR:
        help->rows=help->txtrrows;
        help->text=help->txtr;
        break;
      case MD_CCLM:
        help->rows=help->cclmrows;
        help->text=help->cclm;
        break;
      case MD_CUBE:
        help->rows=help->cuberows;
        help->text=help->cube;
        break;
      case MD_SLBL:
        help->rows=help->slblrows;
        help->text=help->slbl;
        break;
      default:
        help->rows=0;
        help->text=NULL;
        break;
    }
    if ((help->rows<1)||(help->text==NULL))
    {
      message_error("Cannot find help data for current screen.");
      return false;
    }
    message_info_force("Use arrow keys and page up/down to move, "
      "any other key to return.");
    scrmode->mode=MD_HELP;
    return true;
}

/*
 * Action function - end the help mode.
 */
void end_help()
{
    if (scrmode->mode!=help->formode)
      scrmode->mode=help->formode;
    else
      scrmode->mode=MD_SLB;
    message_release();
    message_info("Returned to last work mode.");
}

void draw_help()
{
    int i;
    for (i=0; i < scrmode->rows; i++)
    {
      char *help_line="";
      if (help->y+i < help->rows)
          help_line=help->text[help->y+i];
      draw_help_line(i,0,help_line);
    }
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

void draw_help_line(int posy,int posx,char *text)
{
      char *line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
      set_cursor_pos(posy,posx);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      char *lpos=text;
      char *lend;
      do {
        lend=strchr(lpos,'\\');
        if (lend==NULL)
        {
          strncpy(line,lpos,LINEMSG_SIZE);
          line[LINEMSG_SIZE-1]=0;
        }
        else
        {
          int llen=lend-lpos;
          if (llen>=LINEMSG_SIZE) llen=LINEMSG_SIZE-1;
          if (llen>0)
          {
            strncpy(line,lpos,llen+1);
            line[llen]=0;
          } else
            line[0]=0;
        }
        screen_printf("%s",line);
        if (lend!=NULL)
          if (strlen(lend)>0)
          {
              if (lend[1]=='\\')
              {
                screen_printf("\\");
              } else
              if (lend[1]=='w')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
              } else
              if (lend[1]=='s')
              {
                screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
              } else
              if (lend[1]=='i')
              {
                screen_setcolor(PRINT_COLOR_BLACK_ON_LGREY);
              } else
              if (lend[1]=='r')
              {
                screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
              } else
              if (lend[1]=='y')
              {
                screen_setcolor(PRINT_COLOR_YELLOW_ON_BLACK);
              } else
              if (lend[1]=='m')
              {
                screen_setcolor(PRINT_COLOR_LMAGENT_ON_BLACK);
              } else
              if (lend[1]=='c')
              {
                screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
              } else
              if (lend[1]=='b')
              {
                screen_setcolor(PRINT_COLOR_LBLUE_ON_BLACK);
              } else

              if (lend[1]=='D')
              {
                screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
              } else

              if (lend[1]=='P')
              {
                screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
              } else
              if (lend[1]=='R')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_RED);
              }
              if (lend[1]=='C')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_CYAN);
              }
              if (lend[1]=='G')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_GREEN);
              }
              if (lend[1]=='Y')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_BROWN);
              }
              if (lend[1]=='B')
              {
                screen_setcolor(PRINT_COLOR_WHITE_ON_BLUE);
              }

              lend++;
          }

        lpos=lend+1;
      } while (lend!=NULL);
      screen_printf_toeol("");
      free(line);
}

char *get_random_tip()
{
    if ((help->tiprows<1)||(help->tips==NULL))
      return "No tip, sorry.";
    int num=rnd(help->tiprows);
    return help->tips[num];
}

short init_key_help(int mode)
{
    switch (mode)
    {
      case MD_SLB:
        help->rows=help->slbkeyrows;
        help->text=help->slbkey;
        break;
      case MD_CRTR:
        help->rows=help->crtkeyrows;
        help->text=help->crtkey;
        break;
      case MD_ITMT:
        help->rows=help->itmtkeyrows;
        help->text=help->itmtkey;
        break;
      case MD_TNG:
        help->rows=help->tngkeyrows;
        help->text=help->tngkey;
        break;
      case MD_TXTR:
        help->rows=help->txtrkeyrows;
        help->text=help->txtrkey;
        break;
      case MD_CCLM:
        help->rows=help->cclmkeyrows;
        help->text=help->cclmkey;
        break;
      case MD_CUBE:
        help->rows=help->cubekeyrows;
        help->text=help->cubekey;
        break;
      case MD_SLBL:
        help->rows=help->slblkeyrows;
        help->text=help->slblkey;
        break;
      case MD_CLM:
      case MD_SCRP:
      default:
        help->rows=0;
        help->text=NULL;
        break;
    }
    if ((help->rows<1)||(help->text==NULL))
    {
      return false;
    }
    return true;
}
