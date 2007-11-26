
#include "globals.h"
#include "dernc.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "internal.h"


char *filebase;

// 0 for high-low, 1 for low-high
int datmode=0;

int vistng[3][3]; // Which number are we looking at on each subtile

int default_clm=0; // Have we already set up the clm entries

struct memory_file read_file (char *iname);

/*
 * Read a file, possibly compressed, and decompress it if necessary.
 * Warning: it may destroy the file name from input variable
 */
struct memory_file read_file (char *iname)
{
    FILE *ifp;
    long plen, ulen;
    void *packed, *unpacked;
    char buffer[80];
    struct memory_file ret;

    ifp = fopen(iname, "rb");
    //Prepare "short" filename - no path, max. 32 chars
    char *ifname=strrchr(iname,'\\');
    if (ifname==NULL)
        ifname=iname;
    if (strlen(ifname) > 31)
          ifname[32]=0;
    //If cannot open
    if (!ifp) 
    {
      ret.content=NULL;
      ret.len=-1;
      return ret;
    }
    fseek (ifp, 0L, SEEK_END);
    plen = ftell (ifp);
    rewind (ifp);
    packed = malloc(plen);
    if (packed==NULL)
    {
      sprintf (buffer, "read_file: Cannot allocate memory for \"%s\".",ifname);
      die(buffer);
    }
    fread (packed, 1, plen, ifp);
    fclose (ifp);
    ulen = rnc_ulen (packed);
    if (ulen < 0) 
    {
      if (ulen == -1) // File wasn't RNC to start with
      {
          ret.content = (unsigned char *)packed;
          ret.len = plen;
          return ret;
      }
      sprintf (buffer, "read_file: %s", rnc_error (ulen));
      die(buffer);
    }
    unpacked = malloc(ulen);
    if (unpacked==NULL)
    {
      sprintf (buffer, "read_file: Cannot alloc mem to decompress \"%s\".",ifname);
      die(buffer);
    }
    ulen = rnc_unpack (packed, unpacked, 0);
    if (ulen < 0)
    {
      sprintf (buffer, "read_file: %s", rnc_error (ulen));
      die (buffer);
    }

    free (packed);
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
    return ret;
}

