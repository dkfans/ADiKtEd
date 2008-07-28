/******************************************************************************/
// memfile.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Jon Skeet
// Created:  05 Oct 1997
// Modified: Tomasz Lis

// Purpose:
//   Functions for reading RNC files and storing in memory.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "memfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include "dernc.h"


/**
 * Creates new MEMORY_FILE structure.
 * @param mfile Double pointer to MEMORY_FILE structure.
 * @param alloc_len Initial size of allocated memory.
 * @return Returns MFILE_OK, or negative error code.
 *     On error, the *mfile pointer is set to NULL.
 */
short memfile_new(struct MEMORY_FILE **mfile, unsigned long alloc_len)
{
  (*mfile)=malloc(sizeof(struct MEMORY_FILE));
  if ((*mfile)==NULL) return MFILE_MALLOC_ERR;
  (*mfile)->len=0;
  (*mfile)->pos=0;
  (*mfile)->alloc_len=alloc_len;
  (*mfile)->alloc_delta=0;
  (*mfile)->errcode=MFILE_OK;
  if (alloc_len>0)
  {
    (*mfile)->content=malloc(alloc_len);
    if ((*mfile)->content == NULL)
    {
        free(*mfile);
        (*mfile)=NULL;
        return MFILE_MALLOC_ERR;
    }
  } else
  {
    (*mfile)->content=NULL;
  }
  return MFILE_OK;
}

/**
 * Frees the MEMORY_FILE structure.
 * @param mfile Double pointer to MEMORY_FILE structure.
 * @return Returns MFILE_OK, or negative error code.
 */
short memfile_free(struct MEMORY_FILE **mfile)
{
  if ((*mfile)!=NULL)
  {
//message_log("  memfile_free: content %X",(*mfile)->content);
      free((*mfile)->content);
      free((*mfile));
  }
  (*mfile)=NULL;
  return MFILE_OK;
}

/**
 * Enlarges data buffer allocated for MEMORY_FILE.
 * @param mfile Pointer to MEMORY_FILE structure.
 * @param alloc_len The minimal length allocated for buffer.
 * @return Returns MFILE_OK, or negative error code.
 */
short memfile_growalloc(struct MEMORY_FILE *mfile, unsigned long alloc_len)
{
  if (mfile->alloc_len < alloc_len)
  {
      mfile->content=realloc(mfile->content,alloc_len+mfile->alloc_delta);
      if (mfile->content==NULL)
      {
          mfile->alloc_len=0;
          mfile->len=0;
          mfile->errcode=MFILE_MALLOC_ERR;
          return mfile->errcode;
      }
      mfile->alloc_len=alloc_len+mfile->alloc_delta;
  }
  mfile->errcode=MFILE_OK;
  return mfile->errcode;
}

/**
 * Adds data from buffer to the MEMORY_FILE.
 * @param mfile Pointer to MEMORY_FILE structure.
 * @param buf The input buffer.
 * @param buf_len Length of the input buffer.
 * @return Returns MFILE_OK, or negative error code.
 */
short memfile_add(struct MEMORY_FILE *mfile,
    const unsigned char *buf,unsigned long buf_len)
{
    // Nothing to add case
    if (buf_len==0)
    {
      mfile->errcode=MFILE_OK;
      return mfile->errcode;
    }
    if (memfile_growalloc(mfile,mfile->len+buf_len)!=MFILE_OK)
      return mfile->errcode;
    memcpy(mfile->content+mfile->len,buf,buf_len);
    mfile->len+=buf_len;
    mfile->errcode=MFILE_OK;
    return mfile->errcode;
}

/**
 * Sets data from buffer as MEMORY_FILE.
 * The MEMORY_FILE content is directly replaced by the buffer.
 * Input buffer is not copied - the direct pointer is set as content.
 * @param mfile Pointer to MEMORY_FILE structure.
 * @param buf The input buffer.
 * @param len Length of the file in the input buffer.
 * @param alloc_len Allocated length of the input buffer.
 * @return Returns MFILE_OK, or negative error code.
 */
short memfile_set(struct MEMORY_FILE *mfile,
    unsigned char *buf,unsigned long len,unsigned long alloc_len)
{
    mfile->pos=0;
    free(mfile->content);
    if (((len>0)||(alloc_len>0))&&(buf==NULL))
    {
      mfile->content=NULL;
      mfile->len=0;
      mfile->pos=0;
      mfile->alloc_len=0;
      mfile->errcode=MFILE_INTERNAL;
      return mfile->errcode;
    }
    mfile->content=buf;
    mfile->len=len;
    mfile->alloc_len=alloc_len;
    mfile->errcode=MFILE_OK;
    return mfile->errcode;
}

/**
 * Frees the MEMORY_FILE structure, leaving content.
 * @param mfile Double pointer to MEMORY_FILE structure.
 * @return Returns the MEMORY_FILE content.
 */
unsigned char *memfile_leave_content(struct MEMORY_FILE **mfile)
{
  unsigned char *content=NULL;
  if ((*mfile)!=NULL)
  {
      content=(*mfile)->content;
      free((*mfile));
  }
  (*mfile)=NULL;
  return content;
}

/**
 * Read a file, possibly compressed, and decompress it if necessary.
 * Requires MEMORY_FILE to be preallocated.
 * @see memfile_new
 * @param mfile Pointer to MEMORY_FILE structure, which will contain the file.
 * @param fname The input file name.
 * @param max_size Maximum acceptable input file size.
 * @return Returns MFILE_OK, or negative error code.
 */
short memfile_read(struct MEMORY_FILE *mfile,const char *fname,unsigned long max_size)
{
    if ((mfile==NULL) || (fname==NULL))
        return MFILE_INTERNAL;
    FILE *ifp;
    unsigned long plen, ulen;
    unsigned long alloc_plen;
    void *packed;
    ifp = fopen(fname, "rb");
    //If cannot open
    if (ifp==NULL)
    {
      mfile->errcode=MFILE_CANNOT_OPEN;
      return mfile->errcode;
    }
    fseek(ifp, 0L, SEEK_END);
    plen = ftell(ifp);
    //message_log("end %d",plen);
    if (((long)plen == -1L) || (plen > MAX_FILE_SIZE))
    {
      fclose(ifp);
      mfile->errcode=MFILE_SIZE_ERR;
      return mfile->errcode;
    }
    // Make sure we have enough bytes allocated to check file type
    //additional 8 bytes in buffer are for safety
    if (plen<SIZEOF_RNC_HEADER)
      alloc_plen=SIZEOF_RNC_HEADER+8;
    else
      alloc_plen=plen+8;
    rewind(ifp);
    packed = malloc(alloc_plen);
    if (packed==NULL)
    {
      fclose(ifp);
      mfile->errcode=MFILE_MALLOC_ERR;
      return mfile->errcode;
    }
    unsigned long rdlen;
    rdlen=fread(packed, 1, plen, ifp);
//message_log("plen %d rdlen %d alloc_plen %d",plen,rdlen,alloc_plen);
    if (ferror(ifp))
    {
      fclose(ifp);
      free(packed);
      mfile->errcode=MFILE_READ_ERR;
      return mfile->errcode;
    }
    fclose(ifp);
    // Fill the buffer end with zeros
    if (rdlen<alloc_plen)
        memset(packed+rdlen,'\0',alloc_plen-rdlen);
    ulen = rnc_ulen(packed);
    if ((long)ulen == RNC_FILE_IS_NOT_RNC) // File wasn't RNC to start with
    {
      if (mfile->len > 0)
      {
          memfile_add(mfile,packed,plen);
          free(packed);
      } else
          memfile_set(mfile,packed,plen,alloc_plen);
      return mfile->errcode;
    }
    if (ulen>MAX_FILE_SIZE)
    {
      free(packed);
      mfile->errcode=MFILE_SIZE_ERR;
      return mfile->errcode;
    }
    // Check if the input file isn't truncated
    plen = rnc_plen(packed);
    if (((long)plen > 0) && (plen >= alloc_plen) && (plen < MAX_FILE_SIZE))
    {
      unsigned long alloc_new=plen+8;
//message_log("packed buffer truncated - enlarging to %d",alloc_new);
      packed=realloc(packed,alloc_new);
      if (packed==NULL)
      {
          mfile->errcode=MFILE_MALLOC_ERR;
          return mfile->errcode;
      }
      memset(packed+alloc_plen,'\0',alloc_new-alloc_plen);
      alloc_plen=alloc_new;
    }
    unsigned long alloc_ulen;
    void *unpacked;
    alloc_ulen=ulen+8;
//message_log("ulen %d alloc_ulen %d",ulen,alloc_ulen);
    unpacked = malloc(alloc_ulen);
    if (unpacked==NULL)
    {
      free(packed);
      mfile->errcode=MFILE_MALLOC_ERR;
      return mfile->errcode;
    }

    ulen = rnc_unpack(packed, unpacked, RNC_IGNORE_NONE);
//memset(unpacked,0,ulen);
//message_log("  read_file: packed %X unpacked %X delta %d",packed,unpacked,unpacked-packed);
    free(packed);
    // We assume there is less than 32 error messages
    if ( ((long)ulen < 0) && ((long)ulen > -32) )
    {
      free(unpacked);
      mfile->errcode=(short)ulen;
//message_log("  read_file: returning unpack error code");
      return mfile->errcode;
    }

    if (mfile->len>0)
    {
        memfile_add(mfile,unpacked,ulen);
        free(unpacked);
    } else
        memfile_set(mfile,unpacked,ulen,alloc_ulen);
//message_log("  read_file: returning");
    return mfile->errcode;
}

/**
 * Read a file, possibly compressed, and decompress it if necessary.
 * Automatically creates new MEMORY_FILE at start.
 * @see memfile_read
 * @param mfile Double pointer to MEMORY_FILE structure, which will contain the file.
 * @param fname The input file name.
 * @param max_size Maximum acceptable input file size.
 * @return Returns MFILE_OK, or negative error code.
 *     On error, the *mfile is set to NULL.
 */
short memfile_readnew(struct MEMORY_FILE **mfile,const char *fname,unsigned long max_size)
{
    short errcode;
    errcode = memfile_new(mfile,0);
    if (errcode != MFILE_OK)
        return errcode;
    //message_log("  load_dat: before read_file");
    errcode = memfile_read((*mfile),fname,max_size);
    if (errcode != MFILE_OK)
        memfile_free(mfile);
    return errcode;
}

char *memfile_error(int errcode)
{
    static char *const errors[] = {
	"No error",
	"Cannot open file",
	"Cannot allocate memory",
	"Wrong file size",
	"Data read error",
	"Internal error",
	"Unknown error",
    };
    if ((errcode<0)&&(errcode>=-16))
    {
      return rnc_error(errcode);
    }
    errcode = MFILE_CANNOT_OPEN+1 - errcode;
    if (errcode < 0)
	errcode = 0;
    if (errcode > sizeof(errors)/sizeof(*errors) - 1)
      errcode = sizeof(errors)/sizeof(*errors) - 1;
	return errors[errcode];
}

/*
 * Read a file, possibly compressed, and decompress it if necessary.
 * @param iname The input file name.
 * @return The MEMORY_FILE structure, containing the file.
struct MEMORY_FILE read_file(const char *iname)
{
    FILE *ifp;
    long plen, ulen;
    unsigned long alloc_len;
    void *packed;
    void *unpacked;
    struct MEMORY_FILE ret;
    ifp = fopen(iname, "rb");
    //If cannot open
    if (ifp==NULL)
    {
      ret.errcode=MFILE_CANNOT_OPEN;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    fseek(ifp, 0L, SEEK_END);
    plen = ftell(ifp);
    //message_log("end %d",plen);
    if (((long)plen==-1L)||(plen>MAX_FILE_SIZE))
    {
      ret.errcode=MFILE_SIZE_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    // Make sure we have enough bytes allocated to check file type
    //additional 8 bytes in buffer are for safety
    if (plen<12)
      alloc_len=12+8;
    else
      alloc_len=plen;
    rewind(ifp);
    packed = malloc(alloc_len);
    if (packed==NULL)
    {
      ret.errcode=MFILE_MALLOC_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    //message_log("allocated %d",alloc_len);
    unsigned long rdlen;
    rdlen=fread(packed, 1, plen, ifp);
    if (ferror(ifp))
    {
      fclose(ifp);
      free(packed);
      ret.errcode=MFILE_READ_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    fclose(ifp);
    // Fill the buffer end with zeros
    if (rdlen<alloc_len)
        memset(packed+rdlen,'\0',alloc_len-rdlen);
    ulen = rnc_ulen(packed);
    if ((long)ulen == RNC_FILE_IS_NOT_RNC) // File wasn't RNC to start with
    {
      ret.errcode=MFILE_OK;
      ret.content = (unsigned char *)packed;
      ret.len = plen;
      return ret;
    }
    if (ulen>MAX_FILE_SIZE)
    {
      free(packed);
      ret.errcode=MFILE_SIZE_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    // Check if the input file isn't truncated
    plen = rnc_plen(packed);
    if ((plen>0)&&(plen>=alloc_len))
      packed=realloc(packed,plen+8);
    unpacked = malloc(ulen+8);
    if ((packed==NULL)||(unpacked==NULL))
    {
      free(packed);
      free(unpacked);
      ret.errcode=MFILE_MALLOC_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    ulen = rnc_unpack(packed, unpacked, RNC_IGNORE_NONE);
    free(packed);
    // We assume there is less than 32 error messages
    if ( ((long)ulen < 0) && ((long)ulen > -32) )
    {
      free(unpacked);
      ret.errcode=(long)ulen;
      ret.content=NULL;
      ret.len=0;
//message_log("  read_file: returning unpack error code");
      return ret;
    }
    ret.errcode=MFILE_OK;
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
//message_log("  read_file: returning MFILE_OK");
    return ret;
}
 */
