/******************************************************************************/
// textmenu.c - Console-based menu interface module.
/******************************************************************************/
// Author:  Tomasz Lis
// Created: 31 May 2008

// Purpose:
//   Defines functions used to create menu on text screen

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <slang.h>

#include "textmenu.h"

#include "../libadikted/globals.h"
#include "input_kb.h"
#include "output_scr.h"
#include "var_utils.h"

inline union _value menu_value_num(long num)
{ union _value ret; ret.num=num; return ret; }

inline union _value menu_value_longptr(long *longptr)
{ union _value ret; ret.longptr=longptr; return ret; }

/*
 * Allocates memory and fills new menu item
 */
menuitem menu_new_item(int x, int y, const char *label, int type,
        union _value value, void *param)
{
    menuitem ret;
    ret = malloc (sizeof (struct _menuitem));
    ret->type=type;
    ret->x=x;
    ret->y=y;
    ret->label=(char *)label;
    ret->value=value;
    ret->param=param;
    switch (type)
    {
    case MIT_BOOLEAN:
        menu_item_limits(ret,false,true);
        break;
    case MIT_NUMERIC:
        menu_item_limits(ret,DK_LONG_MIN,DK_LONG_MAX);
        break;
    case MIT_CHOICE:
        // Note: for CHOICE, the limit.min is set to # of elements in array,
        // and limit.max is set to width of largest element
        if (ret->param!=NULL)
        {
          int max_len=1;
          int i=0;
          while (((char **)ret->param)[i]!=NULL)
          {
            int nlen=strlen(((char **)ret->param)[i]);
            if (max_len<nlen) max_len=nlen;
            i++;
          }
          menu_item_limits(ret,i,max_len);
        } else
        {
          menu_item_limits(ret,0,4);
        }
        break;
    default:
        menu_item_limits(ret,0,DK_LONG_MAX);
        break;
    }
    return ret;
}

void menu_item_limits(menuitem dest, long min, long max)
{
    dest->limit.min=min;
    dest->limit.max=max;
}

/*
 * Adds menuitem into menu
 */
void add_item_to_menu(menu dest, menuitem src)
{
    int i, j, x, y, p, ip, jp;
    int n;
    dest->nitems++;
    dest->items=realloc(dest->items, dest->nitems*sizeof(menuitem));
    if (!dest->items)
    die ("add_item_to_menu: Out of memory");
    dest->items[dest->nitems-1]=src;
    
    src->num=dest->nitems-1;
    src->up=-1;
    src->down=-1;
    src->left=-1;
    src->right=-1;

    if (src->type==MIT_STATIC)
      return;
    
    if ((dest->first==-1) || (src->y < dest->items[dest->first]->y) ||
      ((src->y == dest->items[dest->first]->y) && 
       (src->x < dest->items[dest->first]->x)))
    dest->first=src->num;
    
    if ((dest->last==-1) || (src->y > dest->items[dest->last]->y) ||
      ((src->y == dest->items[dest->last]->y) && 
       (src->x > dest->items[dest->last]->x)))
    dest->last=src->num;
    
    n=0;
    for (i=0; i < src->num; i++)
      if (dest->items[i]->type != MIT_STATIC)
        n++;
    if (n<1)
    {
      src->up=src->num;
      src->down=src->num;
      src->left=src->num;
      src->right=src->num;
    } else
    if (n==1)
    {
      for (i=0; i < src->num; i++)
      {
        if (dest->items[i]->type != MIT_STATIC)
          n=i;
      }
      src->up=n;
      src->down=n;
      src->left=n;
      src->right=n;
      dest->items[n]->up=src->num;
      dest->items[n]->down=src->num;
      dest->items[n]->left=src->num;
      dest->items[n]->right=src->num;
    } else
    {
      x = src->x;
      y = src->y;
      p = (x<<8)+y;
      for (i=0; src->up==-1 && i < src->num; i++)
      {
        if (dest->items[i]->type==MIT_STATIC)
          continue;
        ip = (dest->items[i]->x<<8)+dest->items[i]->y;
        if (ip==p)
        {
          //MESSAGE!! printf ("Item location: %d,%d\n", x, y);
          die ("add_item_to_menu: Two items at same place!");
        }
        j = dest->items[i]->up;
        if (j==-1)
          die ("add_item_to_menu: This shouldn't happen - no up");
        jp = (dest->items[j]->x<<8)+dest->items[j]->y;
        if ((jp < ip && p < ip && p > jp) ||
          ((jp > ip) && (p < ip || p > jp)))
        {
          src->up=j;
          src->down=i;
          dest->items[i]->up=src->num;
          dest->items[j]->down=src->num;
        }
      }
      p = (y<<8)+x;
      for (i=0; (src->left==-1) && (i<src->num); i++)
      {
        if (dest->items[i]->type==MIT_STATIC)
          continue;
        ip = (dest->items[i]->y<<8)+dest->items[i]->x;
        j = dest->items[i]->left;
        if (j==-1)
          die ("add_item_to_menu: This shouldn't happen - no left");
        jp = (dest->items[j]->y<<8)+dest->items[j]->x;
        if ((jp < ip && p < ip && p > jp) ||
          ((jp > ip) && (p < ip || p > jp)))
        {
          src->left=j;
          src->right=i;
          dest->items[i]->left=src->num;
          dest->items[j]->right=src->num;
        }
      }
    }
}

/*
 * Allocates memory and initializes new menu
 */
menu new_menu(char *title)
{
    menu ret;
    ret = malloc (sizeof (struct _menu));
    if (ret==NULL)
      die("new_menu: Can't allocate memory");
    ret->nitems=0;
    ret->items=NULL;
    ret->back=NULL;
    ret->title=title;
    ret->first=-1;
    ret->last=-1;
    return ret;
}    

struct workmenu *new_workmenu(menu src)
{
    struct workmenu *ret;
    ret = (struct workmenu *)malloc(sizeof(struct workmenu));
    if (ret==NULL)
      die("new_workmenu: Can't allocate memory");
    // Where we are
    ret->cmenu=src;
    ret->citem=0;
    ret->process_special=NULL;
    ret->item_open=false;
    ret->limit_min=0;
    ret->limit_max=255;
    ret->choicen=0;
    ret->choicecols=1;
    ret->choicew=64;
    ret->edit_value[0]='\0';
    ret->mleft=0;
    ret->mtop=0;
    ret->mwidth=64;
    ret->mheight=32;
    workmenu_enter(ret,src);
    return ret;
}    

/*
 * Changes the current menu
 */
short workmenu_enter(struct workmenu *currmnu,menu nmenu)
{
    if (currmnu==NULL) return false;
    currmnu->item_open=false;
    menu pmenu=currmnu->cmenu;
    if (nmenu==NULL) return false;
    currmnu->cmenu = nmenu;
    if (currmnu->cmenu!=NULL)
    {
        currmnu->citem = currmnu->cmenu->first;
        if (pmenu!=NULL)
        {
          int it;
          for (it=nmenu->first;it<nmenu->last;it++)
            if ((nmenu->items[it]->type==MIT_MENU)&&(nmenu->items[it]->param==pmenu))
            {
              currmnu->citem = it;
              break;
            }
        }
        workmenu_compute_size_rect(currmnu);
    } else
    {
        currmnu->citem = 0;
    }
    return true;
}

/*
 * Reacts on menu key events
 */
short action_menukey(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key)
{
    if (currmnu==NULL) return false;
    menu m;
    int it;
    m = currmnu->cmenu;
    if (m==NULL) return false;
    it = currmnu->citem;
    if ((it<0) || (it>=m->nitems))
    {  //Handling error situation
        currmnu->item_open=false;
        workmenu_enter(currmnu,m->back);
        speaker_beep();
        return true;
    }

    if (currmnu->item_open)
    { // Key on opened item
      switch (m->items[it]->type)
      {
      case MIT_CHOICE:
          if (action_menukey_choice(workdata,currmnu,key))
            return true;
          break;
      case MIT_NUMERIC:
          if (action_menukey_numeric(workdata,currmnu,key))
            return true;
          break;
      default: // Bad state
          currmnu->item_open=false;
          break;
      }
    } else
    { // Key when no item is opened
      if (action_menukey_treemove(workdata,currmnu,key))
          return true;
    }
    return false;
}

/*
 * Reacts on menu numeric value key events
 */
short action_menukey_numeric(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key)
{
    menu m;
    int it;
    m = currmnu->cmenu;
    it = currmnu->citem;
    int len;
    len = strlen(currmnu->edit_value);
    switch (key)
    {
    case KEY_ESCAPE:
          currmnu->item_open=false;
          return true;
    case KEY_ENTER:
          if (len>0)
              *(m->items[it]->value.longptr)=atoi(currmnu->edit_value);
          currmnu->item_open=false;
          return true;
    case KEY_DEL:
    case KEY_BACKSP:
          if (len>0)
              currmnu->edit_value[len-1]='\0';
          return true;
/*
    case KEY_PGDOWN: // PgDown on opened item
          return true;
    case KEY_PGUP: // PgUp on opened item
          return true;
    case KEY_HOME:
          return true;
    case KEY_END:
          return true;
*/
    case KEY_NUM0:
    case KEY_NUM1:
    case KEY_NUM2:
    case KEY_NUM3:
    case KEY_NUM4:
    case KEY_NUM5:
    case KEY_NUM6:
    case KEY_NUM7:
    case KEY_NUM8:
    case KEY_NUM9:
        if (len<11)
        {
          currmnu->edit_value[len]=key;
          currmnu->edit_value[len+1]='\0';
          long val=atoi(currmnu->edit_value);
          if (val>currmnu->limit_max)
          {
            currmnu->edit_value[len]='\0';
            speaker_beep();
          }
        } else
        {
          speaker_beep();
        }
        return true;
    case KEY_NMINUS:
        if ((len==0)&&(currmnu->limit_min<0))
        {
          currmnu->edit_value[len]='-';
          currmnu->edit_value[len+1]='\0';
        }
        return true;
    }
    return false;
}

/*
 * Reacts on menu choice value key events
 */
short action_menukey_choice(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key)
{
    menu m;
    int it;
    m = currmnu->cmenu;
    it = currmnu->citem;
    switch (key)
    {
    case KEY_DEL:
    case KEY_BACKSP:
    case KEY_ESCAPE:
          currmnu->item_open=false;
          return true;
    case KEY_ENTER:
          (*m->items[it]->value.longptr)=currmnu->limit_min+currmnu->choicen;
          currmnu->item_open=false;
          return true;

    case KEY_UP:
        if (currmnu->choicen)
          currmnu->choicen--;
        else
          currmnu->choicen=currmnu->limit_max-currmnu->limit_min-1;
        return true;
    case KEY_DOWN:
        currmnu->choicen++;
        if (currmnu->choicen>=currmnu->limit_max-currmnu->limit_min)
          currmnu->choicen=0;
        return true;
    case KEY_RIGHT:
        currmnu->choicen+=10;
        if (currmnu->choicen >= currmnu->limit_max-currmnu->limit_min)
          currmnu->choicen-=(currmnu->limit_max-currmnu->limit_min);
        if (currmnu->choicen >= currmnu->limit_max-currmnu->limit_min)
          currmnu->choicen=0;
        return true;
    case KEY_LEFT:
        currmnu->choicen-=10;
        if (currmnu->choicen < 0)
          currmnu->choicen=(currmnu->limit_max-currmnu->limit_min)+currmnu->choicen;
        if ((currmnu->choicen<0) || (currmnu->choicen >= currmnu->limit_max-currmnu->limit_min))
          currmnu->choicen=currmnu->limit_max-currmnu->limit_min-1;
        return true;
    case KEY_PGDOWN: // PgDown on opened item
          currmnu->choicen+=10;
          if (currmnu->choicen >= currmnu->limit_max-currmnu->limit_min)
              currmnu->choicen=currmnu->limit_max-currmnu->limit_min-1;
          return true;
    case KEY_PGUP: // PgUp on opened item
          currmnu->choicen-=10;
          if (currmnu->choicen < 0)
              currmnu->choicen = 0;
          return true;
    case KEY_HOME:
          currmnu->choicen = 0;
          return true;
    case KEY_END:
          currmnu->choicen=currmnu->limit_max-currmnu->limit_min-1;
          return true;
    }
    return false;
}

/*
 * Reacts on menu tree movement key events
 */
short action_menukey_treemove(struct WORKMODE_DATA *workdata,struct workmenu *currmnu,int key)
{
    menu m;
    int it;
    m = currmnu->cmenu;
    it = currmnu->citem;
    switch (key)
    {
    case KEY_UP:
        currmnu->citem=m->items[it]->up;
        return true;
    case KEY_DOWN:
        currmnu->citem=m->items[it]->down;
        return true;
    case KEY_RIGHT:
        currmnu->citem=m->items[it]->right;
        return true;
    case KEY_LEFT:
        currmnu->citem=m->items[it]->left;
        return true;
    case KEY_HOME:
        currmnu->citem=m->first;
        return true;
    case KEY_END:
        currmnu->citem=m->last;
        return true;
    case KEY_PGUP:
        {
          int i;
          for (i=0;i<6;i++)
          {
            currmnu->citem=m->items[it]->up;
            it = currmnu->citem;
          }
        };return true;
    case KEY_PGDOWN:
        {
          int i;
          for (i=0;i<6;i++)
          {
            currmnu->citem=m->items[it]->down;
            it = currmnu->citem;
          }
        };return true;
    case KEY_ESCAPE:
        if (workmenu_enter(currmnu,m->back))
          return true;
        return false;
    case KEY_ENTER:
        switch (m->items[it]->type)
        {
        case MIT_MENU:
          {
            workmenu_enter(currmnu,(menu)m->items[it]->param);
          };break;
        case MIT_CHOICE:
          {
            currmnu->item_open=true;
            currmnu->choicen=0;
            char **c;
            c = (char **)m->items[it]->param;
            if (c==NULL) break;
            currmnu->limit_min=0;
            while (!c[currmnu->limit_min])
              currmnu->limit_min++;
            currmnu->limit_max=currmnu->limit_min;
            currmnu->choicew=0;
            while (c[currmnu->limit_max])
            {
              if (strlen(c[currmnu->limit_max]) > currmnu->choicew)
                currmnu->choicew=strlen(c[currmnu->limit_max]);
              currmnu->limit_max++;
            }
            currmnu->choicew++;
            currmnu->choicecols = (currmnu->limit_max-currmnu->limit_min+9)/10;
          };break;
        case MIT_NUMERIC:
            currmnu->item_open=true;
            currmnu->limit_min=m->items[it]->limit.min;
            currmnu->limit_max=m->items[it]->limit.max;
            sprintf (currmnu->edit_value, "%ld", *(m->items[it]->value.longptr));
            break;
        case MIT_BOOLEAN:
             if (*(m->items[it]->value.longptr) == m->items[it]->limit.min)
              *(m->items[it]->value.longptr)=m->items[it]->limit.max;
             else
              *(m->items[it]->value.longptr)=m->items[it]->limit.min;
            break;
        case MIT_SPECIAL:
            if (currmnu->process_special!=NULL)
                currmnu->process_special(workdata,m->items[it]->value,m->items[it]->param);
            break;
        }
        return true;
    case KEY_DEL:
    case KEY_BACKSP:
        // del/backspace when no item is opened
        if (m->back)
        {
          currmnu->cmenu = m->back;
          currmnu->citem = currmnu->cmenu->first;
        }
        return true;
    }
    return false;
}

short workmenu_compute_size_rect(struct workmenu *currmnu)
{
    if (currmnu==NULL) return false;
    menu m;
    m = currmnu->cmenu;
    if (m==NULL) return false;
    int cols=strlen(m->title)+3;
    int rows=1;
    int it;
    for (it=0;it<m->nitems;it++)
    {
        menuitem itm=m->items[it];
        int ncols=itm->x+strlen(itm->label)+1;
        switch (itm->type)
        {
        case MIT_NUMERIC:
          {
              int val=itm->limit.max;
              int digts=0;
              while (val!=0)
              {
                digts++;
                val=(val/10);
              }
              ncols+=digts+1;
          };break;
        case MIT_BOOLEAN:
            ncols+=4; // length of "Yes "
            break;
        case MIT_CHOICE:
             ncols+=itm->limit.max+1; //For CHOICE, the max length is stored here
             break;
        }
        if (cols<ncols) cols=ncols;
        if (rows<(itm->y+1)) rows=(itm->y+1);
    }
    currmnu->mwidth=cols;
    currmnu->mheight=rows;
}

void draw_menuscr(struct workmenu *currmnu,int tot_rows,int tot_cols)
{
    if (currmnu==NULL) return;
    menu m;
    m = currmnu->cmenu;
    if (m==NULL) return;
    currmnu->mleft=(tot_cols-currmnu->mwidth)>>1;
    if (currmnu->mleft<0) currmnu->mleft=0;
    currmnu->mtop=(tot_rows-currmnu->mheight)>>1;
    if (currmnu->mtop<0) currmnu->mtop=0;
    //set_cursor_pos(0,0);
    //screen_printf("%ld %ld",currmnu->mwidth,currmnu->mheight);

    int rows=tot_rows-currmnu->mtop;
    int cols=tot_cols-currmnu->mleft;

    screen_setcolor(PRINT_COLOR_WHITE_ON_BLUE);
    screen_draw_window(currmnu->mtop,currmnu->mleft,currmnu->mheight,currmnu->mwidth,1,bsDouble);
    set_cursor_pos(currmnu->mtop, currmnu->mleft+((currmnu->mwidth-strlen(m->title))>>1));
    screen_printf(" %s ",m->title);
    screen_setcolor(PRINT_COLOR_GREY_ON_BLUE);
    int citem=currmnu->citem;
    int i;
    for (i=0; i < m->nitems; i++)
    {
      set_cursor_pos(currmnu->mtop+m->items[i]->y, currmnu->mleft+m->items[i]->x);
      if ((citem==i) && ((m->items[i]->type==MIT_MENU)
                   || (m->items[i]->type==MIT_SPECIAL)))
        screen_setcolor(PRINT_COLOR_WHITE_ON_GREY);
      screen_printf("%s ",m->items[i]->label);
      if (citem==i)
        screen_setcolor(PRINT_COLOR_WHITE_ON_GREY);
      else
        screen_setcolor(PRINT_COLOR_WHITE_ON_BLUE);
      switch (m->items[i]->type)
      {
      case MIT_NUMERIC:
        if ((currmnu->item_open) && (citem == i))
        {
          screen_printf("%s",currmnu->edit_value);
        } else
        {
          long *ptr=m->items[i]->value.longptr;
          if (ptr!=NULL)
            screen_printf("%ld", *ptr);
          else
            screen_printf("BAD");
        }
        break;
      case MIT_BOOLEAN:
        {
          long *ptr=m->items[i]->value.longptr;
          if (ptr!=NULL)
            screen_printf("%s",(*ptr == m->items[i]->limit.min) ? "No" : "Yes");
          else
            screen_printf("BAD");
        }
        break;
      case MIT_CHOICE:
        {
          char **param=(char **)m->items[i]->param;
          long *ptr=m->items[i]->value.longptr;
          if ((ptr!=NULL)&&(param!=NULL)&&(*ptr<m->items[i]->limit.min))
            screen_printf("%s",param[*ptr]);
          else
            screen_printf("BAD");
        }
        break;
      }
      screen_setcolor(PRINT_COLOR_GREY_ON_BLUE);
    }

    if ((citem<0) || (citem>=m->nitems)) return;
    menuitem it;
    it = m->items[citem];
    if ((currmnu->item_open) && (it->type==MIT_CHOICE))
    {
      int cr, cx, cy;
    
      screen_setcolor(PRINT_COLOR_YELLOW_ON_RED);
      cr = currmnu->limit_max-currmnu->limit_min > 10 ? 10 : currmnu->limit_max-currmnu->limit_min;
      cy = ((rows-cr)>>1);
      cx = ((cols-currmnu->choicecols*currmnu->choicew)>>1);
      SLsmg_fill_region (cy-1, cx-1, 
               cr+2, currmnu->choicew*currmnu->choicecols+2, ' ');
      for (i=0; i < currmnu->limit_max-currmnu->limit_min; i++)
      {
        set_cursor_pos((i%10)+cy, (i/10)*currmnu->choicew+cx);
        if (i==currmnu->choicen)
          screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
        else
          screen_setcolor(PRINT_COLOR_YELLOW_ON_RED);
        screen_printf("%s",((char **)it->param)[i+currmnu->limit_min]);
      }
    }

}

void draw_menu_cursor(struct workmenu *currmnu,int rows,int cols)
{
    if (currmnu==NULL) return;
    menu m;
    m = currmnu->cmenu;
    if (m==NULL) return;
    int citem=currmnu->citem;
    menuitem it;
    if ((citem<0) || (citem>=m->nitems))
      it = NULL;
    else
      it = m->items[citem];
    if ((currmnu->item_open) && (it!=NULL) && (it->type==MIT_NUMERIC))
    {
      set_cursor_pos(currmnu->mtop+it->y, currmnu->mleft+it->x+strlen(it->label)+1+strlen(currmnu->edit_value));
      set_cursor_visibility(true);
    } else
    {
      set_cursor_pos(rows-1, cols-1);
      set_cursor_visibility(false);
    }
}
