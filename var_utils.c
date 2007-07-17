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

void die(char *x)
{
    done();
    fprintf(stderr, "%s\n", x);
    exit(1);
}

/*
 * Clean up all the stuff that init() did.
 */
void done(void)
{
    screen_done();
    input_done();
}
