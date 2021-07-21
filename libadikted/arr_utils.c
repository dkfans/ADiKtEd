/******************************************************************************/
/** @file arr_utils.c
 * Dungeon Keeper Array Tools.
 * @par Purpose:
 *     Utility functions for working with arrays and strings.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     15 Nov 2007 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "arr_utils.h"

#include "globals.h"
#include <stdarg.h>
#include "msg_log.h"

/**
 * Strips control characters from end of a string.
 * @see trim_right
 * @param string_in the string which is stripped.
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

/**
 * Trimms white & control character from string end.
 * @see strip_crlf
 * @param string_in The string which is trimmed.
 * @return Returns string_in, which is the trimmed string.
 */
char *trim_right(char *string_in)
{
    int i;
    unsigned char *string = (unsigned char *) string_in;
    
    for(i=strlen((char *)string)-1;i>=0;i--)
    {
        if(string[i]<=32)
            string[i]='\0';
        else
            break;
    }
    return (char *)string;
}

/**
 * Trimms white & control character from string start.
 * @see trim_right
 * @param string_in The string which is trimmed.
 * @return Returns string_in+n, where n is amount of characters trimmed.
 */
char *trim_left(char *string_in)
{
    unsigned char *string = (unsigned char *) string_in;
    
    while (string[0]!='\0')
    {
        if(string[0]<=32)
            string++;
        else
            break;
    }
    return (char *)string;
}

/**
 * Duplicates a string without quotes. Allocates memory
 * and copies null-terminated string, skipping any quotes
 * at start/end of the source.
 * Any white/control character outside of quotes are skipped.
 * All white/control characters inside quotes are preserved.
 * @param src The string which is duplicated.
 * @return The duplicate, or NULL on error.
 */
char *strdup_noquot(char *src)
{
     char *trimsrc;
     trimsrc=src;
     while ((trimsrc[0]>0)&&(trimsrc[0]<=32)) trimsrc++;
     if ((trimsrc[0]=='\"')||(trimsrc[0]=='\''))
         trimsrc++;
     else
         trimsrc=src;
     int len=strlen(trimsrc);
     int trimlen;
     trimlen=len;
     while (((unsigned char)(trimsrc[trimlen-1])<=32)) trimlen--;
     if ((trimsrc[trimlen-1]=='\"')||(trimsrc[trimlen-1]=='\''))
         trimlen--;
     else
         trimlen=len;
     char *dest=malloc(trimlen+1);
     if (dest==NULL) return NULL;
     strncpy(dest,trimsrc,trimlen);
     dest[trimlen]='\0';
     return dest;
}

/**
 * Duplicates a string with trimming. Allocates memory
 * and copies null-terminated string, skipping any
 * white/control characters at start/end of the source.
 * @param src The string which is duplicated.
 * @return The duplicate, or NULL on error.
 */
char *strdup_trim(char *src)
{
     char *trimsrc;
     trimsrc=src;
     while ((trimsrc[0]>0)&&(trimsrc[0]<=32)) trimsrc++;
     int len=strlen(trimsrc);
     int trimlen;
     trimlen=len;
     while ((((unsigned char)(trimsrc[trimlen-1]))<=32)) trimlen--;
     char *dest=malloc(trimlen+1);
     if (dest==NULL) return NULL;
     strncpy(dest,trimsrc,trimlen);
     dest[trimlen]='\0';
     return dest;
}

/**
 * Returns file name only from given filename with path.
 * @see prepare_short_fname
 * @param pathname The source filename, possibly with path.
 * @return Pointer to the name in pathname string, with path skipped.
 */
char *filename_from_path(char *pathname)
{
    char *fname = NULL;
    if (pathname)
    {
        fname = strrchr (pathname, '/') + 1;
        if (!fname)
            fname = strrchr (pathname, '\\') + 1;
    }
    if (!fname)
        fname=pathname;
return fname;
}

/**
 * Creates map filename from given string.
 * @see format_data_fname
 * @param fname The output filename.
 * @param usrinput The input string used to create filename.
 * @param levels_path Directory path to the folder containing levels.
 * @return Returns true on success.
 */
short format_map_fname(char *fname, const char *usrinput,const char *levels_path)
{
    /* if the file does not have a path */
    short has_path=(strchr(usrinput,'\\')!=NULL);
    if (!has_path)
        has_path=(strchr(usrinput,'/')!=NULL);
    if (!has_path)
    {
          const char *usedpath;
          if ((levels_path==NULL)||(levels_path[0]=='\0'))
              usedpath=".";
          else
              usedpath=levels_path;
          /* Then it shouldn't have any dots (no extension) */
          char *dotpos=strrchr(usrinput,'.');
          if (dotpos!=NULL)
          {
              int len=dotpos-usrinput;
              strncpy(fname,usrinput,len);
              fname[len]='\0';
          } else
          /* If there are no dots or separators, maybe it's a number of map, not filename */
          {
          if (atoi(usrinput))
              sprintf (fname, "%s"SEPARATOR"map%.5d", usedpath, atoi(usrinput));
          else
              sprintf(fname, "%s"SEPARATOR"%s", usedpath, usrinput);
          }
    } else
        /* File is with path */
    {
        sprintf(fname, "%s", usrinput);
    }
    if (strlen(fname)>0) return true;
    return false;
}

/**
 * Prepares short version of the given filename. Resulting string is
 * never longer than maxlen.
 * Allocates memory for the returned string - you should
 * free() it after use.
 * @param fname The input (possibly long) file name.
 * @param maxlen Maximal size of the returned output string.
 * @return Returns the short file name, or NULL on error.
 */
char *prepare_short_fname(const char *fname, unsigned int maxlen)
{
    const char *start;
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
    strncpy( retname, start, len );
    retname[len]='\0';
    return retname;
}

/**
 * Formats data file name. Allocates memory and prints formatted
 * file name into it.
 * fullname Pointer to the returned file name.
 * data_path Path to the data directory, inserted before filename.
 * format The string format used to create file name.
 * @see format_map_fname
 * @return Returns true on success.
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

/**
 * Searches for position of an item in array of unsigned short.
 * @param arr Pointer to the source array.
 * @param arr_item The searched item.
 * @param array_count Size of the array (number of elements in it).
 * @return Returns the position, or -1 if cannot find.
 */
int arr_ushort_pos(const unsigned short *arr,unsigned short arr_item,int array_count)
{
    int i;
    for (i=0;i<array_count;i++)
      if (arr[i]==arr_item) return i;
    return -1;
}
