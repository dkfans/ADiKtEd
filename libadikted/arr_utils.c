/******************************************************************************/
// arr_utils.c - Dungeon Keeper Tools.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  15 Nov 2007

// Purpose:
//   Utility functions for working with arrays and strings.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "arr_utils.h"

#include "globals.h"
#include <stdarg.h>

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

/*
 * Trimms white & control character from string end
 */
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
 * Creates map filename from given string.
 * Returns true on success.
 */
short format_map_fname(char *fname, const char *usrinput,const char *levels_path)
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
 * free() it after use. Returns NULL on error.
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
    if (len>0)
        retname=(char *)malloc(len+1);
    else
        retname=NULL;
    if (retname==NULL)
    {
        message_error("prepare_short_fname: Cannot allocate memory.");
        return NULL;
    }
    strncpy(retname,start,len);
    retname[len]='\0';
    return retname;
}

/*
 * Formats data file name. Returns true on success.
 */
short format_data_fname(char **fullname, const char *data_path, const char *format, ...)
{
    va_list val;
    va_start(val, format);
    char *file = (char *)malloc(DISKPATH_SIZE*sizeof(char));
    if (file!=NULL)
        vsprintf(file, format, val);
    va_end(val);
    if (file==NULL)
        return false;
    *fullname = (char *)malloc(strlen(file)+strlen(data_path)+4);
    if (*fullname==NULL)
    {
        message_error("format_data_fname: Out of memory.");
        return false;
    }
    sprintf(*fullname, "%s"SEPARATOR"%s", data_path, file);
    free(file);
    if (strlen(*fullname)>strlen(data_path)) return true;
    return false;
}

/*
 * Searches for position of an item in array of unsigned short
 */
int arr_ushort_pos(const unsigned short *arr,unsigned short arr_item,int array_count)
{
    int i;
    for (i=0;i<array_count;i++)
      if (arr[i]==arr_item) return i;
    return -1;
}
