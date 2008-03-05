/*
 * var_utils.c
 *
 * Various utility functions.
 *
 */

#include "var_utils.h"

#include "globals.h"
#include <stdarg.h>
#include "input_kb.h"
#include "output_scr.h"

char *message_prv;
char *message;
short message_is_warn;
FILE *msgout_fp;

char *levels_path;
char *data_path;

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

void message_error(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL) msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=true;
    if (msgout_fp!=NULL)
      fprintf(msgout_fp,"%s\n",message);
    speaker_beep();
}

short message_is_empty()
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
    if (msg==NULL) msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=false;
    if (msgout_fp!=NULL)
      fprintf(msgout_fp,"%s\n",message);
}

void message_info_force(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL) msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
    message_prv=message;
    message=msg;
    vsprintf(message, format, val);
    va_end(val);
    message_is_warn=false;
    if (msgout_fp!=NULL)
      fprintf(msgout_fp,"%s\n",message);
}

void message_release()
{
      message_is_warn=false;
}

char *message_get()
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
      if (msgout_fp!=NULL)
        fprintf(msgout_fp,"%s\n",message);
      free(msg);
}

short format_map_fname(char *fname, char *usrinput)
{
    // if the file does not have a path
    if (strstr(usrinput,SEPARATOR)==NULL)
    {
          //Then it shouldn't have any dots (no extension)
          char *dotpos=strrchr(usrinput,'.');
          if (dotpos!=NULL)
          {
              *dotpos='\0';
              sprintf(fname, "%s", usrinput);
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

short format_data_fname(char **fullname, const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *file = (char *)malloc(DISKPATH_SIZE*sizeof(char));
    vsprintf(file, format, val);
    va_end(val);
    *fullname = (char *)malloc(strlen(file)+strlen(data_path)+4);
    if (*fullname==NULL)
      die ("format_data_fname: Out of memory.");
    sprintf(*fullname, "%s"SEPARATOR"%s", data_path, file);
    free(file);
    if (strlen(*fullname)>strlen(data_path)) return true;
    return false;
}

short set_msglog_fname(char *fname)
{
    msgout_fp=fopen(fname,"wb");
    if (msgout_fp!=NULL)
    {
      fprintf(msgout_fp,"aDiKtEd message log file\n");
      return true;
    }
    return false;
}

void init_messages()
{
  message=NULL;
  message_prv=NULL;
  msgout_fp=NULL;
}

void free_messages()
{
    free(message_prv);
    free(message);
    if (msgout_fp!=NULL)
      fclose(msgout_fp);
}

void die(const char *format, ...)
{
      done();
      va_list val;
      va_start(val, format);
      vfprintf(stderr, format, val);
      va_end(val);
      fprintf(stderr, "\n");
      exit(1);
}

/*
 * Clean up all the stuff that init() did.
 */
void done(void)
{
    level_deinit();
    free_levscr();
    free_messages();
    input_done();
}

int rnd (int range)
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
