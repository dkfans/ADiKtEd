/******************************************************************************/
// scr_help.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  14 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Defines functions for initializing and displaying the help screen.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_help.h"

#include "../libadikted/adikted.h"
#include "scr_actn.h"
#include "output_scr.h"
#include "input_kb.h"
#include "var_utils.h"

// Help variables

const char *help_filename="map.hlp";
const int itm_desc_rows=8;

short init_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing help variable
    workdata->help=(struct HELP_DATA *)malloc(sizeof(struct HELP_DATA));
    struct HELP_DATA *help=workdata->help;
    if (help==NULL)
     die("init_help: Cannot allocate memory.");
    help->formode=0;
    help->y=0;
    help->rows=0;
    help->text=NULL;
    help->drows=0;
    help->desc=NULL;

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
    help->srchkeyrows=0;
    help->srchkey=NULL;
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
    help->srchrows=0;
    help->srch=NULL;
    help->rwrkrows=0;
    help->rwrk=NULL;
    help->lmaprows=0;
    help->lmap=NULL;
    help->smaprows=0;
    help->smap=NULL;
    help->tiprows=0;
    help->tips=NULL;
    help->compassrows=0;
    help->compass=NULL;
    help->mcompassrows=0;
    help->mcompass=NULL;
    help->cubedescrows=0;
    help->cubedesc=NULL;
    help->slabdescrows=0;
    help->slabdesc=NULL;
    help->crtrdescrows=0;
    help->crtrdesc=NULL;
    help->itmtdescrows=0;
    help->itmtdesc=NULL;
    help->txtrdescrows=0;
    help->txtrdesc=NULL;
    help->efctrows=0;
    help->efct=NULL;
    help->efctkeyrows=0;
    help->efctkey=NULL;
    help->efctdescrows=0;
    help->efctdesc=NULL;
    help->traprows=0;
    help->trap=NULL;
    help->trapkeyrows=0;
    help->trapkey=NULL;
    help->trapdescrows=0;
    help->trapdesc=NULL;
    help->cclmdescrows=0;
    help->cclmdesc=NULL;

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
      if ((buffer[0]=='[') && (buffer[strlen(buffer)-1]==']'))
      {
          what = match_title(help,title, n);
          if (n && what)
          {
            *what = (char **)malloc(n*sizeof(char *));
            if ((*what)==NULL)
                die("init_help: Out of memory");
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
      if ((buffer[0]=='[') && (buffer[strlen(buffer)-1]==']'))
      {
          strcpy (title, buffer+1);
          title[strlen(title)-1]=0;
          what=match_title (help,title, -1);
          n=0;
      }
      else
      {
          if (what)
          {
            (*what)[n]=(char *)malloc((strlen (buffer)+1)*sizeof(char));
            if (!(*what)[n])
                die("init_help: Out of memory");
            strcpy ((*what)[n], buffer);
          }
          n++;
      }
    }
    fseek(fp, (long)0, SEEK_SET);
    fclose(fp);
    return true;
}

/*
 * Deallocates memory for the help screen.
 */
void free_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  message_log(" free_help: starting");
  if ((workdata==NULL)||(workdata->help==NULL)) return;
  //We could do more than that...
  free(workdata->help);
  workdata->help=NULL;
}

/*
 * Covers actions from the help screen.
 */
void actions_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int *y=&(workdata->help->y);
    int rows=workdata->help->rows;
    switch (key)
    {
    case KEY_UP:
      (*y)--;
      break;
    case KEY_DOWN:
      (*y)++;
      break;
    case KEY_PGUP:
      (*y)-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      (*y)+=scrmode->rows-1;
      break;
    default:
      end_help(scrmode,workdata);
    }
    if ((*y)+scrmode->rows > rows)
      (*y)=rows-scrmode->rows;
    if ((*y) < 0)
      (*y)=0;
}

char ***match_title(struct HELP_DATA *help,const char *title, const int n)
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
            "srchhelp", "srchkeyhelp",    //19,20
            "rwrkhelp", "lmaphelp",       //21,22
            "smaphelp",                   //23
            "grfthelp", "grftkeyhelp",    //24,25
            "compass", "mcompass",        //26,27
            "cubedesc", "slabdesc",       //28,29
            "crtrdesc", "itmtdesc",       //30,31
            "txtrdesc", "cclmdesc",       //32,33
            "efcthelp", "traphelp",       //34,35
            "efctkeyhelp", "trapkeyhelp", //36,37
            "efctdesc", "trapdesc",       //38,39
            NULL,};
    int i=0;

    while (titles[i] && strcmp(titles[i], title))
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
          help->srchrows=n;
        return &(help->srch);
      case 20:
        if (n!=-1)
          help->srchkeyrows=n;
        return &(help->srchkey);
      case 21:
        if (n!=-1)
          help->rwrkrows=n;
        return &(help->rwrk);
      case 22:
        if (n!=-1)
          help->lmaprows=n;
        return &(help->lmap);
      case 23:
        if (n!=-1)
          help->smaprows=n;
        return &(help->smap);
      case 24:
        if (n!=-1)
          help->grftrows=n;
        return &(help->grft);
      case 25:
        if (n!=-1)
          help->grftkeyrows=n;
        return &(help->grftkey);
      case 26:
        if (n!=-1)
          help->compassrows=n;
        return &(help->compass);
      case 27:
        if (n!=-1)
          help->mcompassrows=n;
        return &(help->mcompass);
      case 28:
        if (n!=-1)
          help->cubedescrows=n;
        return &(help->cubedesc);
      case 29:
        if (n!=-1)
          help->slabdescrows=n;
        return &(help->slabdesc);
      case 30:
        if (n!=-1)
          help->crtrdescrows=n;
        return &(help->crtrdesc);
      case 31:
        if (n!=-1)
          help->itmtdescrows=n;
        return &(help->itmtdesc);
      case 32:
        if (n!=-1)
          help->txtrdescrows=n;
        return &(help->txtrdesc);
      case 33:
        if (n!=-1)
          help->cclmdescrows=n;
        return &(help->cclmdesc);
      case 34:
        if (n!=-1)
          help->efctrows=n;
        return &(help->efct);
      case 35:
        if (n!=-1)
          help->traprows=n;
        return &(help->trap);
      case 36:
        if (n!=-1)
          help->efctkeyrows=n;
        return &(help->efctkey);
      case 37:
        if (n!=-1)
          help->trapkeyrows=n;
        return &(help->trapkey);
      case 38:
        if (n!=-1)
          help->efctdescrows=n;
        return &(help->efctdesc);
      case 39:
        if (n!=-1)
          help->trapdescrows=n;
        return &(help->trapdesc);
      default:
        return NULL;
    }
}

/*
 * Action function - start the help mode.
 */
short start_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata==NULL) return false;
    struct HELP_DATA *help=workdata->help;
    help->formode=scrmode->mode;
    help->y=0;
    help->drows=0;
    help->desc=NULL;
    switch (help->formode)
    {
      case MD_SLB:
        help->rows=help->slbrows;
        help->text=help->slb;
        break;
      case MD_CRTR:
      case MD_ECRT:
        help->rows=help->crtrows;
        help->text=help->crt;
        break;
      case MD_CITM:
      case MD_EITM:
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
      case MD_SRCH:
        help->rows=help->srchrows;
        help->text=help->srch;
        break;
      case MD_RWRK:
        help->rows=help->rwrkrows;
        help->text=help->rwrk;
        break;
      case MD_LMAP:
        help->rows=help->lmaprows;
        help->text=help->lmap;
        break;
      case MD_SMAP:
        help->rows=help->smaprows;
        help->text=help->smap;
        break;
      case MD_GRFT:
        help->rows=help->grftrows;
        help->text=help->grft;
        break;
      case MD_CEFC:
      case MD_EFCT:
        help->rows=help->efctrows;
        help->text=help->efct;
        break;
      case MD_CTRP:
      case MD_ETRP:
        help->rows=help->traprows;
        help->text=help->trap;
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
    inc_info_usr_mdswtch_count(workdata->lvl);
    return true;
}

/*
 * Action function - end the help mode.
 */
void end_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (scrmode->mode!=workdata->help->formode)
      scrmode->mode=workdata->help->formode;
    else
      scrmode->mode=MD_SLB;
    message_release();
    message_info("Returned to last work mode.");
}

void draw_help(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    int y=workdata->help->y;
    int i;
    for (i=0; i < scrmode->rows; i++)
    {
      char *help_line="";
      if (y+i < workdata->help->rows)
          help_line=workdata->help->text[y+i];
      draw_help_line(i,0,help_line,DHFLAG_USE_COLORS);
    }
    show_no_cursor();
}

void draw_help_line(int posy,int posx,char *text,short flags)
{
      char *line=(char *)malloc(LINEMSG_SIZE*sizeof(char));
      set_cursor_pos(posy,posx);
      if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
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
              switch (lend[1])
              {
              case '\\':
                screen_printf("\\");
                break;
              case 'w':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_BLACK);
                break;
              case 's':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
                break;
              case 'i':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_BLACK_ON_LGREY);
                break;
              case 'r':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LRED_ON_BLACK);
                break;
              case 'y':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_YELLOW_ON_BLACK);
                break;
              case 'm':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LMAGENT_ON_BLACK);
                break;
              case 'c':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LCYAN_ON_BLACK);
                break;
              case 'b':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LBLUE_ON_BLACK);
                break;
              case 'g':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
                break;
              case 'D':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
                break;
              case 'P':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
                break;
              case 'R':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_RED);
                break;
              case 'C':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_CYAN);
                break;
              case 'G':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_GREEN);
                break;
              case 'Y':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_BROWN);
                break;
              case 'B':
                if ((flags&DHFLAG_USE_COLORS)==DHFLAG_USE_COLORS)
                  screen_setcolor(PRINT_COLOR_WHITE_ON_BLUE);
                break;
              }
              lend++;
          }

        lpos=lend+1;
      } while (lend!=NULL);
      screen_printf_toeol("");
      free(line);
}

char *get_random_tip(const struct HELP_DATA *help)
{
    if ((help->tiprows<1)||(help->tips==NULL))
      return "No tip, sorry.";
    int num=rnd(help->tiprows);
    return help->tips[num];
}

short init_key_help(struct HELP_DATA *help,int mode)
{
    help->drows=0;
    help->desc=NULL;
    switch (mode)
    {
      case MD_SLB:
        help->rows=help->slbkeyrows;
        help->text=help->slbkey;
        break;
      case MD_CRTR:
      case MD_ECRT:
        help->rows=help->crtkeyrows;
        help->text=help->crtkey;
        break;
      case MD_CITM:
      case MD_EITM:
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
      case MD_SRCH:
        help->rows=help->srchkeyrows;
        help->text=help->srchkey;
        break;
      case MD_GRFT:
        help->rows=help->grftkeyrows;
        help->text=help->grftkey;
        break;
      case MD_CEFC:
      case MD_EFCT:
        help->rows=help->efctkeyrows;
        help->text=help->efctkey;
        break;
      case MD_CTRP:
      case MD_ETRP:
        help->rows=help->trapkeyrows;
        help->text=help->trapkey;
        break;
      case MD_CLM:
      case MD_SCRP:
      case MD_RWRK:
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

short init_item_desc(struct HELP_DATA *help,int mode,int itm_idx)
{
    if (itm_idx<0) return false;
    unsigned long pos=itm_desc_rows*itm_idx;
    help->drows=0;
    help->desc=NULL;
    switch (mode)
    {
      case MD_CUBE:
        if (help->cubedescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->cubedesc[pos]);
        }
        break;
      case MD_SLBL:
        if (help->slabdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->slabdesc[pos]);
        }
        break;
      case MD_CRTR:
      case MD_ECRT:
        if (help->crtrdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->crtrdesc[pos]);
        }
        break;
      case MD_CITM:
      case MD_EITM:
        if (help->itmtdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->itmtdesc[pos]);
        }
        break;

      case MD_CEFC:
      case MD_EFCT:
        if (help->efctdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->efctdesc[pos]);
        }
        break;
      case MD_CTRP:
      case MD_ETRP:
        if (help->trapdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->trapdesc[pos]);
        }
        break;
      case MD_TXTR:
        if (help->txtrdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->txtrdesc[pos]);
        }
        break;
      case MD_CCLM:
        if (help->cclmdescrows >= pos+itm_desc_rows)
        {
          help->drows=itm_desc_rows;
          help->desc=&(help->cclmdesc[pos]);
        }
        break;
      default:
        break;
    }
    // Remove empty lines at end
    if (help->desc!=NULL)
    {
      while (help->desc[help->drows-1][0]=='\0')
      {
        help->drows--;
        if (help->drows<1) break;
      }
    }
    if ((help->drows<1)||(help->desc==NULL))
    {
      return false;
    }
    return true;
}

/*
 * Returns cube type name as text
 */
char *get_cubedesc_head(struct WORKMODE_DATA *workdata,unsigned short idx)
{
    unsigned long pos=itm_desc_rows*idx;
    if (workdata->help->cubedescrows >= pos+itm_desc_rows)
      return (char *)workdata->help->cubedesc[pos];
    else
      return "unknown cube";
}
