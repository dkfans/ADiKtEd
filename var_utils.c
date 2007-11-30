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
}

void message_release()
{
      message_is_warn=false;
}

char *message_get()
{
     return message;
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
              sprintf (fname, "%s"SEPARATOR"map%.5d", filebase, atoi(usrinput));
          else
              sprintf(fname, "%s"SEPARATOR"%s", filebase, usrinput);
          }
    } else
        // File is with path
    {
        sprintf(fname, "%s", usrinput);
    }
    if (strlen(fname)>0) return true;
    return false;
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
    screen_done();
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
