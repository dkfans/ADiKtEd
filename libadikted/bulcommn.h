/******************************************************************************/
/** @file bulcommn.h
 * Various functions used in many tools for Bullfrog games.
 * @par Purpose:
 *     Header file. Defines exported routines from bulcommn.c.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     10 Mar 2005 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef BULL_BULCOMMN_H
#define BULL_BULCOMMN_H

#include "globals.h"

/* Routines */

DLLIMPORT unsigned int rnd(const unsigned int range);

DLLIMPORT short write_bmp_fn_idx (const char *fname, int width, int height, const unsigned char *pal, 
		const char *data, int red, int green, int blue, int mult);
DLLIMPORT short write_bmp_fp_idx (FILE *out, int width, int height, 
        const unsigned char *pal, const char *data,
        int red, int green, int blue, int mult);
DLLIMPORT short write_bmp_fn_24b (const char *fname, int width, int height, const char *data);
DLLIMPORT short write_bmp_fp_24b (FILE *out, int width, int height, const char *data);

DLLIMPORT int read_palette_rgb(unsigned char *palette, const char *fname, unsigned int nColors);

int rnc_compressed_file (FILE *fp);

#endif
