/******************************************************************************/
/** @file msg_log.c
 * Maintaining of message log.
 * @par Purpose:
 *     Procedures for logging messages into file, and holding them
 *     to print on screen.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     25 Apr 2008 - 29 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "msg_log.h"

#include "globals.h"

char *message_prv;
char *message;
short message_hold;
unsigned int message_getcount;
char *msgout_fname;

/**
 * Global state prevents massive fopen()/fclose()
 */
#define ALLOW_GLOBAL_MSGOUT_FP  1
/* #define ALLOW_GLOBAL_MSGOUT_FP  0 */
FILE *msgout_global_fp;


/**
 * Get/ensure opened global log file.
 */
static FILE* ensure_log_file()
{
#if ALLOW_GLOBAL_MSGOUT_FP
    if ( msgout_global_fp == NULL )
        msgout_global_fp = fopen(msgout_fname,"ab");
    return msgout_global_fp;
#else
    return fopen(msgout_fname,"ab");
#endif
}

static void write_log_file( FILE* msgout_fp ) {
#if ALLOW_GLOBAL_MSGOUT_FP
    fflush( msgout_fp );
#else
    fclose( msgout_fp );
#endif
}

/**
 * Close global log file.
 */
static void close_log_file()
{
#if ALLOW_GLOBAL_MSGOUT_FP
    if ( msgout_global_fp == NULL ) return;
    fclose( msgout_global_fp );
    msgout_global_fp = NULL;
#else
    /// global state disabled -- do nothing
#endif
}

/**
 * Only logs the message, without showing on screen.
 * The va_list version - mainly for internal use.
 * @param format Specifies the string pattern.
 * @param val List of arguments used in the pattern.
 */
void message_log_vl(const char *format, va_list val)
{
    if (msgout_fname==NULL) return;
    FILE *msgout_fp;
    msgout_fp = ensure_log_file();
    if (msgout_fp!=NULL)
    {
      /* Write to log file if it is opened */
      vfprintf(msgout_fp, format, val);
      fprintf(msgout_fp,"\r\n");
      write_log_file( msgout_fp );
    }
}

/**
 * Simplified function for message logging.
 * Allows writing only single string.
 * @param str Specifies the exact to log string.
 */
void message_log_simp(const char *str)
{
    if (msgout_fname==NULL) return;
    FILE *msgout_fp;
    msgout_fp = ensure_log_file();
    /* Write to log file if it is opened */
    if (msgout_fp!=NULL)
    {
      fprintf(msgout_fp, "%s\r\n",str);
      write_log_file( msgout_fp );
    }
}

/**
 * Only logs the message, without showing on screen.
 * Standard version - allows formatted message.
 * @param format Specifies the string pattern.
 * @param ... List of arguments used in the pattern.
 */
void message_log(const char *format, ...)
{
    if (msgout_fname==NULL) return;
    va_list val;
    va_start(val, format);
    message_log_vl(format, val);
    va_end(val);
}

/**
 * Logs the message and prints it into stderr.
 * Sets message_hold, so other messages can't overwrite it.
 * @param format Specifies the string pattern.
 * @param ... List of arguments used in the pattern.
 */
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
    /* Write to log file if it is prepared */
    message_log_simp(msg);
    /* Store the message */
    message_prv=message;
    message=msg;
    message_hold=true;
    message_getcount=0;
}

/**
 * Returns if the message buffer is empty.
 * @return Returns true if the message buffer is empty.
 */
short message_is_empty(void)
{
     if ((message!=NULL)&&(message[0]>'\0')) return false;
     return true;
}

/**
 * Logs the message and prints it into message buffer.
 * The message is set into buffer only if message_hold is not set.
 * @param format Specifies the string pattern.
 * @param ... List of arguments used in the pattern.
 */
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
    /* Write to log file if it is prepared */
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

/**
 * Sets the new message even if previous one was marked to be held.
 * The new message is a standard information - hold is disabled.
 * @param format Specifies the string pattern.
 * @param ... List of arguments used in the pattern.
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
    /* Write to log file if it is prepared */
    message_log_simp(msg);
    /* Update message variables */
    message_prv=message;
    message=msg;
    message_hold=false;
    message_getcount=0;
}

/**
 * Returns the value of message_hold.
 * This value informs if the buffer need to be released before
 * any further logging.
 * @return Returns message_hold, which true or false.
 */
short message_hold_get(void)
{
      return message_hold;
}

/**
 * Returns the value of message_getcount.
 * This value informs how many times the message_get() was called
 * for the current message.
 * @return Returns message_get call count.
 */
unsigned int message_getcount_get(void)
{
      return message_getcount;
}

/**
 * Releases the message buffer, so it can store next message.
 * Call to this function is reqired after message_error call,
 * to allow logging further messages.
 */
void message_release(void)
{
      message_hold=false;
}

/**
 * Returns pointer to the buffer with the last message.
 * @return Returns pointer to the last message string.
 */
char *message_get(void)
{
     message_getcount++;
     return message;
}

/**
 * Returns pointer to the buffer with the previous message.
 * @return Returns pointer to the last message string.
 */
char *message_get_prev(void)
{
     return message_prv;
}

/**
 * Sets message log file name. Rewrites it, then writes header and two
 * last messages.
 * @param fname The file name under which log is written.
 * @return Returns true if the log was created, otherwise false.
 */
short set_msglog_fname(char *fname)
{
    if ((fname==NULL)||(fname[0]=='\0'))
    {
        msgout_fname=NULL;
        return false;
    }

    close_log_file();

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

/**
 * Clears message log variables without freeing memory (drops any pointers).
 */
void init_messages(void)
{
  message=NULL;
  message_prv=NULL;
  message_hold=false;
  message_getcount=0;
  msgout_fname=NULL;
  msgout_global_fp = NULL;
}

/**
 * Frees memory allocated for messages. The pointers are not cleared.
 */
void free_messages(void)
{
    free(message_prv);
    free(message);
    free(msgout_fname);
    close_log_file();
}
