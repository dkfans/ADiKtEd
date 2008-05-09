/******************************************************************************/
// msg_log.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  25 Apr 2008

// Purpose:
//   Maintaining of message log.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "msg_log.h"

#include "globals.h"

char *message_prv;
char *message;
short message_hold;
unsigned int message_getcount;
char *msgout_fname;

/*
 * Only logs the message, without showing on screen.
 * the va_list and inline version.
 */
void message_log_vl(const char *format, va_list val)
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
void message_log_simp(const char *str)
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
        {
            fprintf(stderr, "message_error: Cannot allocate memory\n");
            return;
        }
    }
    vsprintf(msg, format, val);
    va_end(val);
    // Write to log file if it is prepared
    message_log_simp(msg);
    // Store the message
    message_prv=message;
    message=msg;
    message_hold=true;
    message_getcount=0;
}

short message_is_empty(void)
{
     if ((message!=NULL)&&(message[0]>'\0')) return false;
     return true;
}

void message_info(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if ((msg==NULL)||(message_hold))
    {
        msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
        if (msg==NULL)
        {
            fprintf(stderr, "message_info: Cannot allocate memory\n");
            return;
        }
    }
    vsprintf(msg, format, val);
    va_end(val);
    // Write to log file if it is prepared
    message_log_simp(msg);
    if ((!message_is_empty())&&(message_hold))
    {
      free(msg);
    } else
    {
      message_prv=message;
      message=msg;
      message_hold=false;
      message_getcount=0;
    }
}

/*
 * Sets the new message even if previous one was marked to be held.
 * The new message is a standard information - hold is disabled.
 */
void message_info_force(const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *msg=message_prv;
    if (msg==NULL)
    {
        msg=(char *)malloc(LINEMSG_SIZE*sizeof(char));
        if (msg==NULL)
        {
            fprintf(stderr, "message_info_force: Cannot allocate memory\n");
            return;
        }
    }
    vsprintf(msg, format, val);
    va_end(val);
    // Write to log file if it is prepared
    message_log_simp(msg);
    // Update message variables
    message_prv=message;
    message=msg;
    message_hold=false;
    message_getcount=0;
}

short message_hold_get(void)
{
      return message_hold;
}

unsigned int message_getcount_get(void)
{
      return message_getcount;
}

void message_release(void)
{
      message_hold=false;
}

char *message_get(void)
{
     message_getcount++;
     return message;
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
  message_hold=false;
  message_getcount=0;
  msgout_fname=NULL;
}

void free_messages(void)
{
    free(message_prv);
    free(message);
    free(msgout_fname);
}
