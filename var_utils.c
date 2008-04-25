/******************************************************************************/
// var_utils.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  15 Nov 2007

// Purpose:
//   Various utility functions.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "var_utils.h"

#include "globals.h"
#include <stdarg.h>
#include "input_kb.h"
#include "output_scr.h"
#include "scr_actn.h"

char *message_prv;
char *message;
short message_is_warn;
char *msgout_fname;

char *levels_path;
char *data_path;

/*
 * Strips control characters from end of the string
 */
void strip_crlf(char *string_in)
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

void trim_right(char *string_in)
{
    int i;
    unsigned char *string = (unsigned char *) string_in;
    
    for(i=strlen((char *)string)-1;i>=0;i--)
    {
        if(string[i]<=32)
            string[i]=0;
        else
            break;
    }
}

/*
 * Only logs the message, without showing on screen.
 * the va_list and inline version.
 */
inline void message_log_vl(const char *format, va_list val)
{
    if (msgout_fname==NULL) return;
    FILE *msgout_fp;
    msgout_fp=fopen(msgout_fname,"ab");
    if (msgout_fp!=NULL)
    {
      // Write to log file if it is opened
      vfprintf(msgout_fp, format, val);
      fprintf(msgout_fp,"\r\n");
      fclose(msgout_fp);
    }
}

/*
 * Simplified function for message logging; is inline and
 * allows writing only single string.
 */
inline void message_log_simp(const char *str)
{
    if (msgout_fname==NULL) return;
    FILE *msgout_fp;
    msgout_fp=fopen(msgout_fname,"ab");
    // Write to log file if it is opened
    if (msgout_fp!=NULL)
    {
      fprintf(msgout_fp, "%s\r\n",str);
      fclose(msgout_fp);
    }
}

/*
 * Only logs the message, without showing on screen.
 * Standard version - allows formatted message.
 */
void message_log(const char *format, ...)
{
    if (msgout_fname==NULL) return;
    va_list val;
    va_start(val, format);
    message_log_vl(format, val);
    va_end(val);
}

void message_error(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL)
    {
        msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
        if (msg==NULL)
            die("message_error: Cannot allocate memory");
    }
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=true;
    // Write to log file if it is prepared
    message_log_simp(message);
    speaker_beep();
}

short message_is_empty(void)
{
     if ((message!=NULL)&&(message[0]>'\0')) return false;
     return true;
}

void message_info(const char *format, ...)
{
    if ((!message_is_empty())&&(message_is_warn)) return;
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL)
    {
        msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
        if (msg==NULL)
            die("message_info: Cannot allocate memory");
    }
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=false;
    // Write to log file if it is prepared
    message_log_simp(message);
}

void message_info_force(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL)
    {
        msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
        if (msg==NULL)
            die("message_info_force: Cannot allocate memory");
    }
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=false;
    // Write to log file if it is prepared
    message_log_simp(message);
}

void message_release(void)
{
      message_is_warn=false;
}

char *message_get(void)
{
     return message;
}

/*
 * Popups are temporary messages, visible only until next screen redraw.
 */
void popup_show(const char *title,const char *format, ...)
{
    char *msg;
    msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    if (msg==NULL)
        die("popup_show: Cannot allocate memory");
    va_list val;
    va_start(val, format);
    vsprintf(msg, format, val);
    va_end(val);
    int rows=get_screen_rows();
    int cols=get_screen_cols();
    int textlen=max(strlen(title),strlen(msg));
    int window_rows=4;
    int window_cols=3*cols/4;
    if (textlen+2>window_cols)
        window_cols=textlen+2;
    if (window_cols>cols-2)
        window_cols=max(cols-2,0);
    int posx=(cols-window_cols)>>1;
    int posy=(rows-window_rows)>>1;
    screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
    screen_draw_window(posy,posx,window_rows,window_cols,1,bsDouble);
    int title_x=(cols-strlen(title)-2)>>1;
    if (title_x<posx) title_x=posx;
    set_cursor_pos(posy,title_x);
    screen_printf(" %s ",title);
    set_cursor_pos(posy+2,posx+1);
    screen_printf("%s",msg);
    screen_refresh();
    // Write to log file if it is prepared
    message_log_simp(msg);
    free(msg);
}

/*
 * Creates map filename from given string.
 */
short format_map_fname(char *fname, const char *usrinput)
{
    // if the file does not have a path
    if (strstr(usrinput,SEPARATOR)==NULL)
    {
          //Then it shouldn't have any dots (no extension)
          char *dotpos=strrchr(usrinput,'.');
          if (dotpos!=NULL)
          {
              int len=dotpos-usrinput;
              strncpy(fname,usrinput,len);
              fname[len]='\0';
          } else
          // If there are no dots or separators, maybe it's a number of map, not filename
          {
          if (atoi(usrinput))
              sprintf (fname, "%s"SEPARATOR"map%.5d", levels_path, atoi(usrinput));
          else
              sprintf(fname, "%s"SEPARATOR"%s", levels_path, usrinput);
          }
    } else
        // File is with path
    {
        sprintf(fname, "%s", usrinput);
    }
    if (strlen(fname)>0) return true;
    return false;
}

/*
 * Prepares short version of the given filename; result can't be longer
 * than maxlen. Allocates memory for the returned string - you should
 * free() it after use.
 */
char *prepare_short_fname(char *fname, unsigned int maxlen)
{
    char *start;
    start=strrchr(fname,SEPARATOR[0]);
    if (start==NULL)
        start=fname;
    else
        start++;
    if (start[0]=='\0')
        start=fname;
    unsigned int len=strlen(start);
    if (len > maxlen)
          len=maxlen;
    char *retname;
    retname=(char *)malloc(len+1);
    if (retname==NULL)
      die("prepare_short_fname: Cannot allocate memory.");
    strncpy(retname,start,len);
    retname[len]='\0';
    return retname;
}

short format_data_fname(char **fullname, const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *file = (char *)malloc(DISKPATH_SIZE*sizeof(char));
    vsprintf(file, format, val);
    va_end(val);
    *fullname = (char *)malloc(strlen(file)+strlen(data_path)+4);
    if (*fullname==NULL)
      die("format_data_fname: Out of memory.");
    sprintf(*fullname, "%s"SEPARATOR"%s", data_path, file);
    free(file);
    if (strlen(*fullname)>strlen(data_path)) return true;
    return false;
}

/*
 * Sets message log file name. Rewrites it, then writes header and two
 * last messages.
 */
short set_msglog_fname(char *fname)
{
    if ((fname==NULL)||(fname[0]=='\0'))
    {
        msgout_fname=NULL;
        return false;
    }
    FILE *msgout_fp;
    msgout_fname=strdup(fname);
    msgout_fp=fopen(msgout_fname,"wb");
    if (msgout_fp!=NULL)
    {
      fprintf(msgout_fp,"%s message log file\r\n",PROGRAM_NAME);
      if (message_prv!=NULL)
        fprintf(msgout_fp,"%s\r\n",message_prv);
      if (message!=NULL)
        fprintf(msgout_fp,"%s\r\n",message);
      fclose(msgout_fp);
      return true;
    }
    free(msgout_fname);
    msgout_fname=NULL;
    return false;
}

/*
 * Clears message log variables without freeing memory (drops any pointers).
 */
void init_messages(void)
{
  message=NULL;
  message_prv=NULL;
  message_is_warn=false;
  msgout_fname=NULL;
}

void free_messages(void)
{
    free(message_prv);
    free(message);
    free(msgout_fname);
}

void die(const char *format, ...)
{
      done(&(drawdata.scrmode),drawdata.workdata);
      va_list val;
      va_start(val, format);
      vfprintf(stderr, format, val);
      // Write to log file if it is prepared
      message_log_vl(format, val);
      va_end(val);
      fprintf(stderr, "\n");
      message_log_simp(PROGRAM_NAME " has died");
      free_messages();
      exit(1);
}

/*
 * Clean up all the stuff that init() did.
 */
void done(struct SCRMODE_DATA **scrmode,struct WORKMODE_DATA *workdata)
{
    if (workdata!=NULL)
    {
        if (workdata->lvl!=NULL)
          level_deinit(&(workdata->lvl));
        if (scrmode!=NULL)
          free_levscr(scrmode,workdata);
    }
    // Write to log file if it is prepared
    message_log_simp(PROGRAM_NAME " work is done");
    free_messages();
    input_done();
}

int rnd(int range)
{
    return (int) (((float)range)*rand()/(RAND_MAX+1.0));
}

int arr_ushort_pos(const unsigned short *arr,unsigned short slab_type,int array_count)
{
    int i;
    for (i=0;i<array_count;i++)
      if (arr[i]==slab_type) return i;
    return -1;
}
