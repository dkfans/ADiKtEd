/*
 * lev_files.c
 *
 * Defines functions for loading and writing levels from/to disk.
 *
 */

#include "lev_files.h"

#include "globals.h"
#include "internal.h"

void write_tng(LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i, j, k;
    fp = fopen (fname, "wb");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return;
    }
    fputc ((tng_tot-apt_tot) & 255, fp);
    fputc ((tng_tot-apt_tot)>>8, fp);
    for (i=0; i < 255; i++)
      for (j=0; j < 255; j++)
          for (k=0; k < lvl->tng_subnums[i][j]; k++)
            if (!is_action_point(i,j,k))
                fwrite (lvl->tng_lookup[i][j][k], 21, 1, fp);
    fclose (fp);
}

