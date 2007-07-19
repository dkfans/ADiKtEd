
#include "globals.h"
#include "dernc.h"
#include "lev_data.h"
#include "obj_utils.h"
#include "internal.h"


/* Note that I store action points as things in here, and separate them
 * out at save time. This makes lots of things a lot easier */

unsigned int apt_tot; /* Total number of action points */
unsigned int tng_tot; /* Number of things in total */
unsigned char **action_used=NULL; 
/* Which action point numbers are taken, and where their data is */

/* 
 * Note we assume there will be less than 1024 action points. I think
 * this should do... the most DK uses is 13
 */

/* 
 * Exceptionally grotty hack - we never need the actual data
 * stored in the .dat file, only what the high and low bytes should
 * be. So long as we remember this when we generate the "standard"
 * dungeon things, we'll be fine
 */
unsigned char **dat_low=NULL;
unsigned char **dat_high=NULL;

char *filebase;

int vistng[3][3]; /* Which number are we looking at on each subtile */

/* 0 for high-low, 1 for low-high */
int datmode=0;

int default_clm=0; /* Have we already set up the clm entries */

struct memory_file read_file (char *iname);

/* Read a file, possibly compressed, and decompress it if necessary */
struct memory_file read_file (char *iname)
{
    FILE *ifp;
    long plen, ulen;
    void *packed, *unpacked;
    char buffer[80];
    struct memory_file ret;

    ifp = fopen(iname, "rb");
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
    if (!packed) 
      die("read_file: Out of memory.");
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
      if (strlen (iname) > 40)
          iname[40]=0;
      sprintf (buffer, "read_file: %s", rnc_error (ulen));
      die (buffer);
    }
    unpacked = malloc(ulen);
    if (!unpacked) 
      die ("read_file: Out of memory.");
    ulen = rnc_unpack (packed, unpacked, 0);
    if (ulen < 0)
    {
      if (strlen (iname) > 40)
          iname[40]=0;
      sprintf (buffer, "read_file: %s", rnc_error (ulen));
      die (buffer);
    }

    free (packed);
    ret.content=(unsigned char *)unpacked;
    ret.len=ulen;
    return ret;
}

