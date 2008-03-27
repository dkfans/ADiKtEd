/*
 * memfile.c
 *
 * Functions for reading RNC files and storing in memory.
 *
 */

#include "memfile.h"

#include "globals.h"
#include "var_utils.h"
#include "dernc.h"

/*
 * Read a file, possibly compressed, and decompress it if necessary.
 */
struct MEMORY_FILE read_file(char *iname)
{
    FILE *ifp;
    unsigned long plen, ulen, alloc_len;
    void *packed, *unpacked;
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
    if (((long)plen==-1L)||(plen>MAX_FILE_SIZE))
    {
      ret.errcode=MFILE_SIZE_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    // Make sure we have enough bytes allocated to check file type
    if (plen<12)
      alloc_len=12;
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
    fread(packed, 1, plen, ifp);
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
    ulen = rnc_ulen(packed);
    if ((long)ulen == RNC_FILE_IS_NOT_RNC) // File wasn't RNC to start with
    {
      ret.errcode=MFILE_OK;
      ret.content = (unsigned char *)packed;
      ret.len = plen;
      return ret;
    } else
    if (ulen>MAX_FILE_SIZE)
    {
      ret.errcode=MFILE_SIZE_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    unpacked = malloc(ulen);
    if (unpacked==NULL)
    {
      ret.errcode=MFILE_MALLOC_ERR;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    ulen = rnc_unpack(packed, unpacked, 0);
    free(packed);
    // We assume there is less than 32 error messages
    if ( ((long)ulen < 0) && ((long)ulen > -32) )
    {
      free(unpacked);
      ret.errcode=(long)ulen;
      ret.content=NULL;
      ret.len=0;
      return ret;
    }
    ret.errcode=MFILE_OK;
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
    return ret;
}

char *read_file_error(int errcode)
{
    static char *const errors[] = {
	"No error",
	"Cannot open file",
	"Cannot allocate memory",
	"Wrong file size",
	"Data read error",
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
struct MEMORY_FILE read_file(char *iname)
{
    FILE *ifp;
    long plen, ulen, alloc_len;
    void *packed, *unpacked;
    struct MEMORY_FILE ret;
    char *ifname;

    ifp = fopen(iname, "rb");
    //If cannot open
    if (!ifp) 
    {
      ret.content=(unsigned char *)malloc(LINEMSG_SIZE);
      if (ret.content!=NULL)
        ifname=prepare_short_fname(iname,24);
      else
        ifname=NULL;
      if (ifname!=NULL)
        sprintf(ret.content,"Cannot open file \"%s\".",ifname);
      free(ifname);
      ret.len=MEMRD_ERROR;
      return ret;
    }
    fseek(ifp, 0L, SEEK_END);
    plen = ftell(ifp);
    // Make sure we have enough bytes allocated to check file type
    if (plen<8)
      alloc_len=8;
    else
      alloc_len=plen;
    rewind(ifp);
    packed = malloc(alloc_len);
    if (packed==NULL)
    {
      ret.content=(unsigned char *)malloc(LINEMSG_SIZE);
      if (ret.content!=NULL)
        ifname=prepare_short_fname(iname,24);
      else
        ifname=NULL;
      if (ifname!=NULL)
        sprintf(ret.content,"Cannot allocate memory for \"%s\".",ifname);
      free(ifname);
      ret.len=MEMRD_ERROR;
      return ret;
    }
    fread (packed, 1, plen, ifp);
    fclose(ifp);
    ulen = rnc_ulen(packed);
    if (ulen < 0) 
    {
      if (ulen == RNC_FILE_IS_NOT_RNC) // File wasn't RNC to start with
      {
          ret.content = (unsigned char *)packed;
          ret.len = plen;
          return ret;
      }
      ret.content=(unsigned char *)malloc(LINEMSG_SIZE);
      if (ret.content!=NULL)
        ifname=prepare_short_fname(iname,24);
      else
        ifname=NULL;
      if (ifname!=NULL)
        sprintf(ret.content,"File check: %s in \"%s\"",rnc_error(ulen),ifname);
      free(ifname);
      ret.len=MEMRD_ERROR;
      return ret;
    }
    unpacked = malloc(ulen);
    if (unpacked==NULL)
    {
      ret.content=(unsigned char *)malloc(LINEMSG_SIZE);
      if (ret.content!=NULL)
        ifname=prepare_short_fname(iname,24);
      else
        ifname=NULL;
      if (ifname!=NULL)
        sprintf(ret.content,"Cannot alloc mem to decompress \"%s\".",ifname);
      free(ifname);
      ret.len=MEMRD_ERROR;
      return ret;
    }
    ulen = rnc_unpack(packed, unpacked, 0);
    if (ulen < 0)
    {
      ret.content=(unsigned char *)malloc(LINEMSG_SIZE);
      if (ret.content!=NULL)
        sprintf(ret.content,"Decompression: %s",rnc_error(ulen));
      ret.len=MEMRD_ERROR;
      return ret;
    }
    free(packed);
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
    return ret;
}
*/
